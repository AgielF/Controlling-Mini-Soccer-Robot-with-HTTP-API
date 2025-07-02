// --- Sertakan Library yang Dibutuhkan ---
#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

// --- Konfigurasi Jaringan Wi-Fi (Access Point) ---
const char* ssid = "RobotESP32_AP";      // Nama jaringan Wi-Fi yang akan dibuat
const char* password = "password123";   // Kata sandi jaringan (minimal 8 karakter)

// Konfigurasi IP statis untuk Access Point
IPAddress local_IP(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

// Buat objek WebServer di port 80 (HTTP)
WebServer server(80);

// --- Konfigurasi Pin untuk ESP32 ---
const int enA = 18;
const int in1 = 19;
const int in2 = 32;
const int enB = 25;
const int in3 = 33;
const int in4 = 26;

// Pin untuk Sensor Ultrasonik
const int trigPin = 4;
const int echoPin = 16;

// Pin untuk Motor Servo
const int servoPin = 27;

// Buat objek servo
Servo myservo;

// Variabel untuk sensor
long duration;
int distance;

// Kecepatan motor (0-255)
int motorSpeed = 200;

// Nilai pulse width untuk servo (dalam mikrodetik)
const int SERVO_PULSE_MIN = 500;  // Posisi 0 derajat (melepas)
const int SERVO_PULSE_MAX = 2400; // Posisi 180 derajat
const int SERVO_PULSE_GRAB = 1450;// Posisi 90 derajat (mengambil)

// --- Fungsi Kontrol Robot (Tidak diubah) ---

/**
 * @brief Menggerakkan kedua motor untuk maju.
 */
void moveForward() {
  Serial.println("API call: /forward");
  // Motor A maju
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  analogWrite(enA, motorSpeed);

  // Motor B maju
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  analogWrite(enB, motorSpeed);
}

/**
 * @brief Menggerakkan kedua motor untuk mundur.
 */
void moveBackward() {
  Serial.println("API call: /backward");
  // Motor A mundur
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  analogWrite(enA, motorSpeed);

  // Motor B mundur
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  analogWrite(enB, motorSpeed);
}

/**
 * @brief Menghentikan kedua motor.
 */
void stopMotors() {
  Serial.println("API call: /stop");
  // Hentikan Motor A
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  analogWrite(enA, 0);

  // Hentikan Motor B
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  analogWrite(enB, 0);
}

/**
 * @brief Mengukur dan mengembalikan jarak dalam cm.
 * @return Jarak dalam sentimeter (int).
 */
int getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  return duration * 0.034 / 2;
}

// --- Fungsi Handler untuk API Endpoint ---

void handleForward() {
  moveForward();
  server.send(200, "text/plain", "OK, moving forward");
}

void handleBackward() {
  moveBackward();
  server.send(200, "text/plain", "OK, moving backward");
}

void handleStop() {
  stopMotors();
  server.send(200, "text/plain", "OK, motors stopped");
}

void handleGrab() {
  Serial.println("API call: /grab");
  myservo.writeMicroseconds(SERVO_PULSE_GRAB);
  server.send(200, "text/plain", "OK, gripper grabbing");
}

void handleRelease() {
  Serial.println("API call: /release");
  myservo.writeMicroseconds(SERVO_PULSE_MIN);
  server.send(200, "text/plain", "OK, gripper releasing");
}

/**
 * @brief Mengembalikan status sensor dalam format JSON.
 */
void handleStatus() {
  int currentDistance = getDistance();
  String json = "{\"distance\":" + String(currentDistance) + "}";
  server.send(200, "application/json", json);
}

/**
 * @brief Menangani URL yang tidak ditemukan (404).
 */
void handleNotFound() {
  // Jika root URL diakses, berikan pesan bahwa tidak ada UI.
  if (server.uri() == "/") {
    server.send(200, "text/plain", "ESP32 Robot API Server is running. No web UI.");
    return;
  }
  server.send(404, "text/plain", "404: Not Found");
}


// --- Fungsi Setup dan Loop Utama ---

void setup() {
  // Mulai komunikasi serial untuk debugging
  Serial.begin(115200);
  Serial.println("\nInisialisasi Robot ESP32...");

  // Inisialisasi pin-pin
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Inisialisasi motor servo
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  myservo.setPeriodHertz(50);
  myservo.attach(servoPin, SERVO_PULSE_MIN, SERVO_PULSE_MAX);
  myservo.writeMicroseconds(SERVO_PULSE_MIN); // Posisi awal
  delay(1000);

  // --- Setup Wi-Fi Access Point ---
  Serial.println("Mengkonfigurasi Access Point dengan IP statis...");
  if (!WiFi.softAPConfig(local_IP, gateway, subnet)) {
    Serial.println("Konfigurasi AP gagal!");
  }
  
  Serial.print("Membuat Access Point: ");
  Serial.println(ssid);
  if (!WiFi.softAP(ssid, password)) {
    Serial.println("Gagal memulai AP!");
  }

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("Alamat IP AP: ");
  Serial.println(myIP);

  // --- Daftarkan Handler untuk setiap URL ---
  // Halaman kontrol web telah dihapus.
  server.on("/forward", HTTP_GET, handleForward);
  server.on("/backward", HTTP_GET, handleBackward);
  server.on("/stop", HTTP_GET, handleStop);
  server.on("/grab", HTTP_GET, handleGrab);
  server.on("/release", HTTP_GET, handleRelease);
  server.on("/status", HTTP_GET, handleStatus);
  server.onNotFound(handleNotFound);

  // Mulai server web
  server.begin();
  Serial.println("Server HTTP telah dimulai.");
  Serial.println("Robot siap menerima perintah via API.");
}

void loop() {
  // Fungsi ini akan terus-menerus memeriksa jika ada klien HTTP yang terhubung
  server.handleClient();
}
