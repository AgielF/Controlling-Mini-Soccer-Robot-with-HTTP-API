# Robot Controller: DC Motor & Servo



A project for controlling a robot with DC motors and servos. This robot has two operational modes: **manual control** via a user interface and **automatic mode**, which uses a camera to detect a ball and a distance sensor to measure proximity.

---

## 🚀 Features

* **Manual Mode:** Directly control the robot's movement (DC motors) and peripherals (servos) using a keyboard or other input device.
* **Automatic Mode:** The robot automatically detects and moves towards a ball using camera vision (OpenCV with contour detection).
* **Obstacle Avoidance:** An integrated distance sensor helps the robot to stop or change its path when it gets too close to an object.
* **Real-time Vision:** Live camera feed to monitor the robot's environment and detection process.

---

## 🛠️ Hardware Requirements

* Robot chassis with 2 DC motors
* Motor driver (e.g., L298N)
* Microcontroller (e.g., Arduino, ESP32)
* Servo motor(s)
* ESP32CAM
* Distance sensor (e.g., Ultrasonic HC-SR04)
* Power supply (e.g., battery pack)
* Jumper wires

---

## 📦 Software & Library Requirements

* **Python 3.x**
* **OpenCV:** For image processing and ball detection.
    ```bash
    pip install opencv-python
    ```
* **NumPy:** For numerical operations with image data.
    ```bash
    pip install numpy
    ```
* **PySerial:** To communicate with the microcontroller.
    ```bash
    pip install cvzone
    ```
* **PySerial:** To communicate with the microcontroller.
    ```bash
    pip install request
    ```
* **Arduino IDE:** To program the microcontroller.

---

## ⚙️ Installation & Setup

1.  **Clone the repository:**
    ```bash
    git clone [https://github.com/AgielF/Controlling-Mini-Soccer-Robot-with-HTTP-API.git](https://github.com/AgielF/Controlling-Mini-Soccer-Robot-with-HTTP-API.git)
    ```
2.  **Hardware Connection:**
    * Connect the DC motors and power supply to the motor driver.
      const int enA = 18;
      const int in1 = 19;
      const int in2 = 32;
      const int enB = 25;
      const int in3 = 33;
      const int in4 = 26;
      const int trigPin = 4;
      const int echoPin = 16;
      const int servoPin = 27;
    * Connect the motor driver, servos, and distance sensor to the microcontroller's pins.
    * Connect the ESP32CAM to computer and upload the D3-242-IFB-308-ROB-prg-camera.ino
3.  **Upload Microcontroller Code:**
    * Open the `.ino` file in the `arduino/` directory with the Arduino IDE.
    * Select the correct board and port.
    * Upload the sketch to your microcontroller.
4.  **Install Python libraries:**
    * Navigate to the project directory and run the command in the [Software & Library Requirements](#-software--library-requirements) section.

---

## ▶️ How to Use

### Manual Mode

In this mode, you have direct control over the robot.

1.  Run the manual control script:
    ```bash
    python D3-242-IFB-308-ROB-prg-ui.py
    ```
2.  Use the following keys to control the robot:
    * **Maju:** Move forward
    * **Mundur:** Move backward
    * **Berhenti:** Stop
    * **Grab:** Grab servo
    * **release:** release servo
    * **Manual:** Manual mode
    * **Auto:** Auto mode


## 🤝 Contributing

Contributions, issues, and feature requests are welcome! Feel free to check the [issues page](https://github.com/your-username/your-repository-name/issues).

---

## 📜 License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details.
