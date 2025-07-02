import tkinter as tk
from tkinter import messagebox
from Object_Detect import ObjectDetection 

# URL dari ESP32-CAM dan file HSV (Konfigurasi tidak diubah)
esp32cam_url = "http://192.168.4.3/jpg"
data_url = "http://192.168.4.3/data"
hsv_file = 'D7-IFB308-ROB-UAS-prg-hsv.txt'

# Membuat instance ObjectDetection (Tidak diubah)
object_detection = ObjectDetection(esp32cam_url, data_url, hsv_file)

# --- Definisi Aksi untuk Setiap Tombol ---

def maju_action():
    object_detection.maju()
    messagebox.showinfo("Status", "Robot Maju")

def stop_action():
    # [FIX] Memanggil nama fungsi BARU yang sudah diperbaiki di Object_Detect.py
    object_detection.stop_motor() 
    messagebox.showinfo("Status", "Robot Berhenti")

def backward_action():
    object_detection.mundur()
    messagebox.showinfo("Status", "Robot Mundur")

def grab_action():
    object_detection.ambil()
    messagebox.showinfo("Status","Mengambil object")

def release_action():
    object_detection.lepas()
    messagebox.showinfo("Status","Melepas object")

# --- Pembuatan Jendela GUI (Tidak diubah) ---
root = tk.Tk()
root.title("Remote")
root.geometry("300x300") # Tinggi sedikit ditambahkan agar semua tombol pas

title_label = tk.Label(root, text="Pengendali Robot", font=("Arial", 16))
title_label.pack(pady=10)


# Tombol LIHAT KAMERA
camera_button = tk.Button(root, text="Lihat Kamera", command=lambda: object_detection.start_detection(), height=1, width=15)
camera_button.pack(pady=5)

# Tombol LIHAT KAMERA
stop_camera_button = tk.Button(root, text="Stop Kamera", command=lambda: object_detection.stop_detection(), height=1, width=15)
stop_camera_button.pack(pady=5)


# Tombol mode manual
auto_mode_button = tk.Button(root, text="Mode Auto", command=lambda: object_detection.mode_auto(), height=1, width=15)
auto_mode_button.pack(pady=5)


# Tombol mode manual
manual_mode_button = tk.Button(root, text="Mode Manual", command=lambda: object_detection.mode_manual(), height=1, width=15)
manual_mode_button.pack(pady=5)



# Tombol MAJU
start_button = tk.Button(root, text="Maju", command=maju_action, height=1, width=10)
start_button.pack(pady=5)

# Tombol MUNDUR
back_button = tk.Button(root, text="Mundur", command=backward_action, height=1, width=10)
back_button.pack(pady=5)

# Tombol STOP
stop_button = tk.Button(root, text="Berhenti", command=stop_action, height=1, width=10)
stop_button.pack(pady=5)

# Tombol AMBIL
grab_button = tk.Button(root, text="Ambil", command=grab_action, height=1, width=10)
grab_button.pack(pady=5)

# Tombol LEPAS
release_button = tk.Button(root, text="Lepas", command=release_action, height=1, width=10)
release_button.pack(pady=5)

root.mainloop()





# # ===== D7-IFB308-ROB-UAS-prg-ui.py (REVISI) =====

# import tkinter as tk
# from tkinter import messagebox
# from Object_Detect import ObjectDetection 

# # --- KONFIGURASI ---
# # [DIUBAH] Hanya butuh IP Address dan path file HSV
# # GANTI IP INI DENGAN YANG MUNCUL DI SERIAL MONITOR ANDA
# ESP32_IP = "192.168.4.1" # Contoh, ganti dengan IP Anda
# hsv_file = 'D7-IFB308-ROB-UAS-prg-hsv.txt'

# # [DIUBAH] Membuat instance ObjectDetection dengan cara baru
# object_detection = ObjectDetection(ESP32_IP, hsv_file)

# # --- Definisi Aksi untuk Setiap Tombol ---

# # [BARU] Aksi untuk tombol kamera
# def start_camera_action():
#     object_detection.start_detection()
#     messagebox.showinfo("Status", "Kamera dan Deteksi Dimulai")

# def stop_camera_action():
#     object_detection.stop_detection()
#     messagebox.showinfo("Status", "Kamera dan Deteksi Dihentikan")

# # Aksi untuk tombol gerak (tidak berubah)
# def maju_action():
#     object_detection.maju()

# def stop_action():
#     object_detection.stop_motor() 

# def backward_action():
#     object_detection.mundur()

# def grab_action():
#     object_detection.ambil()

# def release_action():
#     object_detection.lepas()

# # [BARU] Fungsi yang dipanggil saat jendela GUI ditutup
# def on_closing():
#     print("Menutup aplikasi...")
#     object_detection.stop_detection() # Hentikan thread deteksi
#     object_detection.close() # Lepaskan resource kamera
#     root.destroy() # Hancurkan jendela GUI

# # --- Pembuatan Jendela GUI ---
# root = tk.Tk()
# root.title("Kontrol Robot")
# root.geometry("300x400") # Tinggikan untuk tombol baru

# title_label = tk.Label(root, text="Pengendali Robot", font=("Arial", 16))
# title_label.pack(pady=10)

# # [BARU] Tombol Kontrol Kamera
# start_cam_button = tk.Button(root, text="Start Camera", command=start_camera_action, height=2, width=15, bg="lightgreen")
# start_cam_button.pack(pady=5)

# stop_cam_button = tk.Button(root, text="Stop Camera", command=stop_camera_action, height=2, width=15, bg="salmon")
# stop_cam_button.pack(pady=5)

# # Tombol Gerak (tidak berubah)
# start_button = tk.Button(root, text="Maju", command=maju_action, height=1, width=10)
# start_button.pack(pady=5)

# back_button = tk.Button(root, text="Mundur", command=backward_action, height=1, width=10)
# back_button.pack(pady=5)

# stop_button = tk.Button(root, text="Berhenti", command=stop_action, height=1, width=10)
# stop_button.pack(pady=5)

# grab_button = tk.Button(root, text="Ambil", command=grab_action, height=1, width=10)
# grab_button.pack(pady=5)

# release_button = tk.Button(root, text="Lepas", command=release_action, height=1, width=10)
# release_button.pack(pady=5)

# # [BARU] Mengatur aksi saat tombol 'X' (close) pada jendela ditekan
# root.protocol("WM_DELETE_WINDOW", on_closing)

# root.mainloop()