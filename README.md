# Robot Controller: DC Motor & Servo

![Robot Banner](https://via.placeholder.com/800x200.png?text=Robot+Controller+Project)

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
* USB camera
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
    pip install pyserial
    ```
* **Arduino IDE:** To program the microcontroller.

---

## ⚙️ Installation & Setup

1.  **Clone the repository:**
    ```bash
    git clone [https://github.com/your-username/your-repository-name.git](https://github.com/your-username/your-repository-name.git)
    ```
2.  **Hardware Connection:**
    * Connect the DC motors and power supply to the motor driver.
    * Connect the motor driver, servos, and distance sensor to the microcontroller's pins.
    * Connect the USB camera to your computer or single-board computer (e.g., Raspberry Pi).
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
    python manual_control.py
    ```
2.  Use the following keys to control the robot:
    * **W:** Move forward
    * **S:** Move backward
    * **A:** Turn left
    * **D:** Turn right
    * **Q / E:** Control servo 1
    * **Z / C:** Control servo 2
    * **Spacebar:** Stop

### Automatic Mode

In this mode, the robot will automatically search for and move towards a ball.

1.  Make sure the camera and distance sensor are properly connected.
2.  Run the automatic mode script:
    ```bash
    python automatic_mode.py
    ```
3.  The robot will now start detecting a ball (of a pre-defined color) in its field of view.
4.  It will move towards the detected ball while maintaining a safe distance from other objects using the distance sensor. The robot will stop if an object is too close.

---

## 🤝 Contributing

Contributions, issues, and feature requests are welcome! Feel free to check the [issues page](https://github.com/your-username/your-repository-name/issues).

---

## 📜 License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details.
