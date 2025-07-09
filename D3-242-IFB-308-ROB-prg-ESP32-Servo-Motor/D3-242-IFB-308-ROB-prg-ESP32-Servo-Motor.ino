// --- Sertakan Library yang Dibutuhkan ---
#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>
#include <Arduino_JSON.h> // --- BARU ---: Library untuk parsing JSON

// --- Konfigurasi Jaringan Wi-Fi (Access Point) ---
const char* ssid = "RobotESP32_AP";
const char* password = "password123";

// Konfigurasi IP statis untuk Access Point
IPAddress local_IP(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

WebServer server(80);

// --- Konfigurasi Pin ---
// (Tidak ada perubahan di sini)
const int enA = 18;
const int in1 = 19;
const int in2 = 32;
const int enB = 25;
const int in3 = 33;
const int in4 = 26;
const int trigPin = 4;
const int echoPin = 16;
const int servoPin = 27;

Servo myservo;

// --- Variabel Global ---
// (Tidak ada perubahan di sini)
long duration;
int distance;
int motorSpeed = 200;
const int SERVO_PULSE_MIN = 500;
const int SERVO_PULSE_MAX = 2400;
const int SERVO_PULSE_GRAB = 1450;

// --- BARU ---: Variabel untuk status dari kamera dan mode kontrol
bool isBallDetectedByCam = false; // Status terakhir yang diterima dari ESP32-CAM
bool isAutonomousMode = true;     // Robot mulai dalam mode otonom


// --- BARU ---: Konstanta untuk logika otonom
const int GRAB_DISTANCE_THRESHOLD = 15; // Jarak (cm) untuk mulai mengambil bola
const int SAFE_DISTANCE_THRESHOLD = 20; // Jarak aman minimum di depan


// --- BARU ---: Fungsi helper untuk gripper
void grabBall() {
  Serial.println("Action: Grabbing ball");
  myservo.writeMicroseconds(SERVO_PULSE_GRAB);
}

void releaseBall() {
  Serial.println("Action: Releasing ball");
  myservo.writeMicroseconds(SERVO_PULSE_MIN);
}


// --- BARU ---: Fungsi yang berisi semua logika cerdas untuk mode otonom
void runAutonomousLogic() {
  // Pertama, selalu ukur jarak saat ini
  int currentDistance = getDistance();
  Serial.print("Dist: ");
  Serial.print(currentDistance);
  Serial.print("cm, BallSeen: ");
  Serial.print(isBallDetectedByCam);
  Serial.print(" -> ");

  // LOGIKA 1: Bola terdeteksi dan sudah sangat dekat -> Ambil bola!
  if (isBallDetectedByCam && currentDistance < GRAB_DISTANCE_THRESHOLD) {
    Serial.println("Logic: Ball is close. Stopping and grabbing.");
    stopMotors();
    grabBall();
    // Setelah mengambil bola, kita nonaktifkan mode otonom agar robot diam
    // menunggu perintah selanjutnya (misal: kembali ke base, dll)
    isAutonomousMode = false;
    Serial.println("Autonomous mode disabled after grabbing ball.");
  }
  // LOGIKA 2: Bola terdeteksi tapi masih jauh -> Dekati bola
  else if (isBallDetectedByCam) {
    Serial.println("Logic: Ball detected. Moving forward to approach.");
    moveForward();
  }
  // LOGIKA 3: Bola TIDAK terdeteksi TAPI jalanan di depan aman -> Cari bola
  else if (!isBallDetectedByCam && currentDistance > SAFE_DISTANCE_THRESHOLD) {
    Serial.println("Logic: No ball, path is clear. Moving forward to search.");
    moveForward();
  }
  // LOGIKA 4: Kondisi lainnya (bola tidak terdeteksi & ada halangan, dll) -> Berhenti
  else {
    Serial.println("Logic: Obstacle ahead or no ball. Stopping.");
    stopMotors();
  }
}


// --- Fungsi Kontrol Robot (Tidak diubah) ---
void moveForward() {
  Serial.println("Moving forward...");
  digitalWrite(in1, HIGH); digitalWrite(in2, LOW); analogWrite(enA, motorSpeed);
  digitalWrite(in3, HIGH); digitalWrite(in4,LOW); analogWrite(enB, motorSpeed);
}

void moveKanan() {
  Serial.println("Moving forward...");
  digitalWrite(in1, HIGH); digitalWrite(in2, LOW); analogWrite(enA, motorSpeed);
  digitalWrite(in3, LOW); digitalWrite(in4, HIGH); analogWrite(enB, motorSpeed);
}
void moveKiri() {
  Serial.println("Moving forward...");
  digitalWrite(in1, LOW); digitalWrite(in2, HIGH); analogWrite(enA, motorSpeed);
  digitalWrite(in3, HIGH); digitalWrite(in4, LOW); analogWrite(enB, motorSpeed);
}
void moveBackward() {
  Serial.println("Moving backward...");
  digitalWrite(in1, LOW); digitalWrite(in2, HIGH); analogWrite(enA, motorSpeed);
  digitalWrite(in3, LOW); digitalWrite(in4, HIGH); analogWrite(enB, motorSpeed);
}

void stopMotors() {
  Serial.println("Stopping motors...");
  digitalWrite(in1, LOW); digitalWrite(in2, LOW); analogWrite(enA, 0);
  digitalWrite(in3, LOW); digitalWrite(in4, LOW); analogWrite(enB, 0);
}

int getDistance() {
  digitalWrite(trigPin, LOW); delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  return duration * 0.034 / 2;
}

// --- Fungsi Handler untuk API Endpoint ---

// --- MODIFIKASI ---: Fungsi manual sekarang menonaktifkan mode otonom
void handleForward() {
  isAutonomousMode = false; // Beralih ke mode manual
  moveForward();
  server.send(200, "text/plain", "OK, moving forward (Manual Mode)");
}

void handleBackward() {
  isAutonomousMode = false;
  moveBackward();
  server.send(200, "text/plain", "OK, moving backward (Manual Mode)");
}

void handleStop() {
  isAutonomousMode = false;
  stopMotors();
  server.send(200, "text/plain", "OK, motors stopped (Manual Mode)");
}

void handleGrab() {
  isAutonomousMode = false;
  Serial.println("API call: /grab");
  myservo.writeMicroseconds(SERVO_PULSE_GRAB);
  server.send(200, "text/plain", "OK, gripper grabbing (Manual Mode)");
}

void handleRelease() {
  isAutonomousMode = false;
  Serial.println("API call: /release");
  myservo.writeMicroseconds(SERVO_PULSE_MIN);
  server.send(200, "text/plain", "OK, gripper releasing (Manual Mode)");
}

void handleStatus() {
  int currentDistance = getDistance();
  // --- MODIFIKASI ---: Tambahkan status mode dan deteksi kamera
  String json = "{\"distance\":" + String(currentDistance) +
                ",\"is_autonomous\":" + String(isAutonomousMode) +
                ",\"ball_detected_by_cam\":" + String(isBallDetectedByCam) + "}";
  server.send(200, "application/json", json);
}

// --- BARU ---: Handler untuk menerima update status dari ESP32-CAM
void handleUpdateStatus() {
  if (server.hasArg("plain") == false) {
    server.send(400, "text/plain", "Bad Request: No body");
    return;
  }
  
  String body = server.arg("plain");
  Serial.println("===================================");
  Serial.print("Menerima POST request dari ESP32-CAM. Body: ");
  Serial.println(body);
  
  JSONVar jsonData = JSON.parse(body);

  if (JSON.typeof(jsonData) == "undefined") {
    Serial.println("Parsing JSON gagal!");
    server.send(400, "application/json", "{\"status\":\"error\", \"message\":\"Invalid JSON\"}");
    return;
  }

  // Ambil nilai "bola_terdeteksi" dari JSON
  if (jsonData.hasOwnProperty("bola_terdeteksi")) {
    isBallDetectedByCam = jsonData["bola_terdeteksi"];
    Serial.print("Status bola diperbarui menjadi: ");
    Serial.println(isBallDetectedByCam ? "TERDETEKSI" : "TIDAK TERDETEKSI");
    server.send(200, "application/json", "{\"status\":\"ok\"}");
  } else {
    server.send(400, "application/json", "{\"status\":\"error\", \"message\":\"Missing key: bola_terdeteksi\"}");
  }
}

// --- BARU ---: Handler untuk mengontrol mode
void handleAutoMode() {
  isAutonomousMode = true;
  Serial.println("Mode beralih ke: OTOMATIS");
  server.send(200, "text/plain", "OK, switching to Autonomous Mode.");
}

void handleManualMode() {
  isAutonomousMode = false;
  stopMotors(); // Hentikan motor saat beralih ke manual untuk keamanan
  Serial.println("Mode beralih ke: MANUAL");
  server.send(200, "text/plain", "OK, switching to Manual Mode. Motors stopped.");
}

void handleNotFound() {
  if (server.uri() == "/") {
    server.send(200, "text/plain", "ESP32 Robot API Server is running. No web UI.");
    return;
  }
  server.send(404, "text/plain", "404: Not Found");
}

// --- Fungsi Setup dan Loop Utama ---
void setup() {
  Serial.begin(115200);
  Serial.println("\nInisialisasi Robot ESP32...");

  pinMode(in1, OUTPUT); pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT); pinMode(in4, OUTPUT);
  pinMode(enA, OUTPUT); pinMode(enB, OUTPUT);
  pinMode(trigPin, OUTPUT); pinMode(echoPin, INPUT);
  
  ESP32PWM::allocateTimer(0); ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2); ESP32PWM::allocateTimer(3);
  myservo.setPeriodHertz(50);
  myservo.attach(servoPin, SERVO_PULSE_MIN, SERVO_PULSE_MAX);
  myservo.writeMicroseconds(SERVO_PULSE_MIN);
  delay(1000);

  Serial.println("Mengkonfigurasi Access Point...");
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("Alamat IP AP: ");
  Serial.println(myIP);

  // --- MODIFIKASI ---: Daftarkan handler-handler baru
  server.on("/forward", HTTP_GET, handleForward);
  server.on("/backward", HTTP_GET, handleBackward);
  server.on("/stop", HTTP_GET, handleStop);
  server.on("/grab", HTTP_GET, handleGrab);
  server.on("/release", HTTP_GET, handleRelease);
  server.on("/status", HTTP_GET, handleStatus);
  
  // --- BARU ---: Handler untuk menerima data dan kontrol mode
  server.on("/update_status", HTTP_POST, handleUpdateStatus);
  server.on("/auto", HTTP_GET, handleAutoMode);
  server.on("/manual", HTTP_GET, handleManualMode);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("Server HTTP telah dimulai.");
  Serial.println("Robot siap menerima perintah.");
  Serial.println("Mode awal: OTOMATIS");
}

void loop() {
  // Selalu proses permintaan klien yang masuk
  server.handleClient();

  // --- MODIFIKASI ---: Logika otonom berdasarkan status dari kamera
   if (isAutonomousMode) {
    runAutonomousLogic();
  }
  // Jika dalam mode manual, robot tidak melakukan apa-apa kecuali ada perintah API.
  
  // Beri sedikit jeda agar tidak membanjiri serial dan motor
  delay(100); 
}
