# SAFEMINE-Fall-Detection-Vitals-Monitoring-System

An IoT-based wearable system that provides long-range fall detection and real-time biometric monitoring. Data is transmitted via LoRa to a local receiver and synced to a Firebase Realtime Database for live web monitoring.

## 🚀 Key Features
* **Real-Time Biometrics:** Continuous monitoring of heart rate and SpO2.
* **Fall Detection Algorithm:** Multi-stage motion and orientation tracking prevents false alarms and triggers instant alerts upon detecting a true fall.
* **Long-Range Transmission:** Operates on a 433 MHz LoRa link, allowing use without direct Wi-Fi access on the wearable.
* **Live Web Dashboard:** A centralized Firebase-powered interface for monitoring data and alert statuses in real-time.

---

## 🛠️ System Architecture

### Hardware Components
* **Sensor Node (Wearable):** ESP8266 microcontroller, MPU6050 (Accelerometer/Gyroscope), MAX30100/MAX30102 (Pulse Oximeter), LoRa SX1278 transceiver.
* **Gateway Node (Receiver):** ESP32 microcontroller, LoRa SX1278 transceiver (connected to Wi-Fi).
* **Cloud & Web:** Firebase Realtime Database, HTML/JS Web Dashboard.

### How It Works
1. The **Sensor Node** continuously reads biometric and spatial data. 
2. Data is sent via **LoRa** to the **Gateway Node**.
3. The Gateway Node pushes the structured data to the **Firebase Realtime Database**.
4. The **Web Dashboard** listens for database changes and instantly updates the UI.

*(Drag and drop your block diagram or schematic image here)*

---

## 🚨 Fall Detection Logic

The system utilizes the MPU6050's 6-axis IMU (accelerometer and gyroscope) to continuously calculate the magnitude of the acceleration vector. A fall is registered only when a specific sequence of three events occurs within a brief time window, minimizing false positives from normal daily movements:

1. **Free-fall (Lower Threshold):** The system continuously calculates the total acceleration vector ($A = \sqrt{a_x^2 + a_y^2 + a_z^2}$). When a fall begins, the acceleration momentarily drops toward 0g (typically crossing below a ~0.4g threshold), indicating weightlessness.
2. **Impact (Upper Threshold):** Immediately following the free-fall phase, the accelerometer detects a sharp, sudden spike in acceleration (e.g., > 2.5g) as the body impacts the ground. 
3. **Immobility (Orientation & Stillness):** Following the impact spike, the gyroscope monitors for a lack of angular momentum. If the sensor remains relatively flat and motionless for several seconds, the system concludes the user has not recovered and triggers the final **Fall Detected** alert.

---

## 💻 Firebase Web Dashboard

The web interface is designed to provide immediate visibility into the user's status. It updates dynamically without requiring a page refresh, displaying flashing alerts if a fall is triggered or if biometrics drop out of safe ranges.

<img width="1903" height="906" alt="image" src="https://github.com/user-attachments/assets/26cfa6e3-3fc1-412a-968d-7a48d7771290" />
<img width="1917" height="893" alt="image" src="https://github.com/user-attachments/assets/bbe4db5d-ea13-4567-bc18-412d766aa530" />


---

## ⚙️ Setup and Installation

### 1. Hardware Configuration
* Flash the sensor node code (`Transmitter_code.ino`) to the ESP8266.
* Flash the gateway node code (`Receiver_code.ino`) to the ESP32.
* Ensure both LoRa modules are tuned to the same frequency (433 MHz) and sync word.

### 2. Web Dashboard Setup
* Open `index.html`.
* Replace the `firebaseConfig` object with your own Firebase project credentials.
* **Security Note:** Ensure your Firebase Security Rules restrict write access to authenticated hardware/admins only.

### 3. Running the Project
Simply host the web files (e.g., using GitHub Pages or Firebase Hosting) or open `index.html` locally in a browser to view the live data.
