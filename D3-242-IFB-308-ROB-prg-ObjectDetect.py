import cvzone
from cvzone.ColorModule import ColorFinder
import cv2
import requests
import numpy as np
import threading
import time

class ObjectDetection:
    def __init__(self, esp32cam_url, data_url, hsv_file):
        self.esp32cam_url = esp32cam_url
        self.data_url = data_url
        self.hsv_file = hsv_file
        self.hsvVals = self.baca_hsv_vals()
        self.running = False
        self.thread = None
        self.lock = threading.Lock()
        self.myColorFinder = ColorFinder(False)
        self.motor_url = "http://192.168.4.1"

    # --- FUNGSI KONTROL MOTOR ---
    def maju(self):
        self.kirim_perintah("/forward")

    def mundur(self):
        self.kirim_perintah("/backward")

    def ambil(self):
        self.kirim_perintah("/grab")

    def lepas(self):
        self.kirim_perintah("/release")
        
    def stop_motor(self):
        self.kirim_perintah("/stop")

    # --- FUNGSI BARU UNTUK MENGUBAH MODE ---
    def mode_manual(self):
        """Mengirim perintah untuk mengalihkan robot ke Mode Manual."""
        print("--- MENGALIHKAN KE MODE MANUAL ---")
        self.kirim_perintah("/manual")

    def mode_auto(self):
        """Mengirim perintah untuk mengembalikan robot ke Mode Otonom."""
        print("--- MENGALIHKAN KE MODE OTOMATIS ---")
        self.kirim_perintah("/auto")

    # --- FUNGSI PENGIRIM PERINTAH ---
    def kirim_perintah(self, path):
        try:
            url_lengkap = f"{self.motor_url}{path}"
            print(f"Mengirim permintaan ke: {url_lengkap}") 
            r = requests.get(url_lengkap, timeout=2)
            if r.status_code == 200:
                print(f"Perintah '{path}' berhasil -> Respons Server: {r.text}")
            else:
                print(f"Perintah '{path}' GAGAL -> Status: {r.status_code}, Respons: {r.text}")
        except requests.exceptions.RequestException as e:
            print(f"Gagal total menghubungi ESP32: {e}")

    # --- FUNGSI UNTUK DETEKSI OBJEK (Tidak Diubah) ---
    def baca_hsv_vals(self):
        local_vars = {}
        with open(self.hsv_file, 'r') as file:
            exec(file.read(), {}, local_vars)
        return local_vars['hsvVals']

    def ambil_gambar_dari_esp32cam(self):
        try:
            response = requests.get(self.esp32cam_url, timeout=15)
            response.raise_for_status()
            img_array = np.array(bytearray(response.content), dtype=np.uint8)
            img = cv2.imdecode(img_array, -1)
            return img
        except requests.exceptions.RequestException as e:
            print(f"Gagal mengambil gambar dari ESP32-CAM: {e}")
            return None

    def kirimArah(self, arah):
        # Dalam mode otonom, fungsi ini tidak lagi mengirim arah,
        # tapi hanya status deteksi bola (terdeteksi/tidak)
        # Nilai 0, 1, 2 = terdeteksi. Nilai 3 = tidak terdeteksi.
        try:
            # Kita hanya mengirim value. ESP32 di mode auto akan menentukan gerakannya sendiri.
            response = requests.get(f"{self.data_url}?value={arah}")
            response.raise_for_status()
            # Kurangi print agar tidak terlalu ramai
            # print(f"Berhasil mengirim status deteksi: {'Terdeteksi' if arah != 3 else 'Tidak Terdeteksi'}")
        except requests.exceptions.RequestException as e:
            print(f"Gagal mengirim nilai: {arah}, {e}")

    def detection_loop(self):
        print("Memulai loop deteksi...")
        while self.running:
            img = self.ambil_gambar_dari_esp32cam()
            if img is None:
                print("Gagal mengambil gambar, mencoba lagi dalam 5 detik...")
                time.sleep(5)
                continue

            imgColor, mask = self.myColorFinder.update(img, self.hsvVals)
            imgContour, contours = cvzone.findContours(img, mask)

            if contours:
                # Jika bola terdeteksi, kirim nilai 0 (atau 1, 2)
                # ESP32 di mode otonom akan tahu artinya "bola terlihat"
                self.kirimArah(0) 
            else:
                # Jika bola tidak terdeteksi, kirim nilai 3
                self.kirimArah(3)

            imgContour_resized = cv2.resize(imgContour, (imgContour.shape[1] // 2, imgContour.shape[0] // 2))
            cv2.imshow("ImageContour", imgContour_resized)
            if cv2.waitKey(1) & 0xFF == ord('q'):
                self.running = False # Hentikan loop jika 'q' ditekan

    # --- FUNGSI KONTROL LOOP DETEKSI OBJEK ---
    def start_detection(self):
        if not self.running:
            self.running = True
            self.thread = threading.Thread(target=self.detection_loop)
            self.thread.start()
            print("Loop deteksi objek dimulai di background.")

    def stop_detection(self):
        if self.running:
            self.running = False
            print("Menghentikan loop deteksi objek...")
            if self.thread is not None:
                self.thread.join()
            cv2.destroyAllWindows()
            print("Loop deteksi objek dan jendela CV2 telah ditutup.")