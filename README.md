# SAFEMINE-Fall-Detection-Vitals-Monitoring-System
# Wearable Biometric & Fall Detection System 

An IoT-based wearable system that provides long-range fall detection and real-time biometric monitoring. Data is transmitted via LoRa to a local receiver and synced to a Firebase Realtime Database for live web monitoring.

## 🚀 Key Features
* **Real-Time Biometrics:** Continuous monitoring of heart rate and SpO2.
* **Fall Detection:** Motion and orientation tracking triggers instant alerts upon detecting a fall.
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

## 💻 Firebase Web Dashboard

The web interface is designed to provide immediate visibility into the user's status. It updates dynamically without requiring a page refresh. 

*(<img width="1908" height="903" alt="image" src="https://github.com/user-attachments/assets/394caa42-f1f7-4b5b-9094-53d7f24ed32b" />
)*

---

## ⚙️ Setup and Installation

### 1. Hardware Configuration
* Flash the sensor node code (`sensor_node.cpp`) to the ESP8266.
* Flash the gateway node code (`gateway_node.cpp`) to the ESP32.
* Ensure both LoRa modules are tuned to the same frequency and sync word.

### 2. Web Dashboard Setup
* Open `index.html`.
* Replace the `firebaseConfig` object with your own Firebase project credentials.
* **Security Note:** Ensure your Firebase Security Rules restrict write access to authenticated hardware/admins only.

### 3. Running the Project
Simply host the web files (e.g., using GitHub Pages or Firebase Hosting) or open `index.html` locally in a browser to view the live data.
