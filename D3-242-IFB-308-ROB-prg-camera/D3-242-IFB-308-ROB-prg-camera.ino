#include <WiFi.h>
#include <WebServer.h>
#include <esp32cam.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

// Ganti dengan nama SSID dan password WiFi Anda
const char* ssid = "RobotESP32_AP";
const char* password = "password123";

// Alamat IP ESP32 utama dan endpoint spesifik untuk mengirim status
// PERBAIKAN: Mengirim ke endpoint spesifik, bukan root
const char* esp32_main_server = "http://192.168.4.1/update_status";

// Alamat IP statis yang ingin kita tetapkan
IPAddress local_IP(192, 168, 4, 3);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

WebServer server(80);
static auto hiRes = esp32cam::Resolution::find(640, 480);

// PERBAIKAN: Menghapus keyword 'extern' yang tidak perlu
bool isBallDetected = false;


void kirimStatusBola(bool terdeteksi) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(esp32_main_server); // Menggunakan URL dengan endpoint
    http.addHeader("Content-Type", "application/json");

    JSONVar jsonData;
    jsonData["bola_terdeteksi"] = terdeteksi;
    jsonData["sumber"] = "ESP32_Pendeteksi_Bola";
    String jsonPayload = JSON.stringify(jsonData);
    
    Serial.println("===================================");
    Serial.println("Mengirim status ke server tujuan...");
    Serial.print("Payload: ");
    Serial.println(jsonPayload);

    int httpCode = http.POST(jsonPayload);

    if (httpCode > 0) {
      Serial.printf("Kode balasan dari server tujuan: %d\n", httpCode);
      String response = http.getString();
      Serial.print("Respons server tujuan: ");
      Serial.println(response);
    } else {
      Serial.printf("Gagal mengirim POST ke server tujuan, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  } else {
    Serial.println("Koneksi Wi-Fi terputus, tidak dapat mengirim status.");
  }
}

void handleData() {
  String value = "N/A";
  bool statusBerubah = false;

  if (server.hasArg("value")) {
    value = server.arg("value");
    Serial.println("Data diterima dari Python: " + value);
    bool statusSebelumnya = isBallDetected;

    if (value == "3") {
      isBallDetected = false;
    } else {
      isBallDetected = true;
    }

    if (isBallDetected != statusSebelumnya) {
      statusBerubah = true;
      Serial.printf("Status bola berubah menjadi: %s\n", isBallDetected ? "TERDETEKSI" : "TIDAK TERDETEKSI");
    }
  } else {
    Serial.println("Permintaan diterima tanpa argumen 'value'. Tidak ada perubahan status.");
  }
  
  if (statusBerubah) {
    kirimStatusBola(isBallDetected);
  }

  server.send(200, "text/plain", "OK. Status diterima."); 
}

void serveJpg() {
  auto frame = esp32cam::capture();
  if (frame == nullptr) {
    Serial.println("CAPTURE FAIL");
    server.send(503, "", "");
    return;
  }
  server.setContentLength(frame->size());
  server.send(200, "image/jpeg");
  WiFiClient client = server.client();
  frame->writeTo(client);
}

void handleJpg() {
  if (!esp32cam::Camera.changeResolution(hiRes)) {
    Serial.println("SET-HI-RES FAIL");
  }
  serveJpg();
}

void setup() {
  // PERBAIKAN: Menggunakan baud rate yang lebih cepat
  Serial.begin(115200);
  Serial.println();

  {
    using namespace esp32cam;
    Config cfg;
    cfg.setPins(pins::AiThinker);
    cfg.setResolution(hiRes);
    cfg.setBufferCount(2);
    cfg.setJpeg(90);
    bool ok = Camera.begin(cfg);
    Serial.println(ok ? "CAMERA OK" : "CAMERA FAIL");
  }

  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/data", handleData);
  server.on("/jpg", handleJpg);
  server.begin();
  
  // PERBAIKAN: Kirim status awal saat pertama kali menyala
  Serial.println("Mengirim status awal ke server utama...");
  kirimStatusBola(isBallDetected);
}

void loop() {
  // Logika re-koneksi ini sudah OK untuk proyek ini
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected. Reconnecting...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("\nWiFi reconnected");
  }
  server.handleClient();
}