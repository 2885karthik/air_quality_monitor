# 🌫️ Real-Time Air Quality Monitoring using ESP32 and AWS DynamoDB

This project monitors air quality using the MQ135 gas sensor connected to an ESP32 board. It sends live data to AWS DynamoDB via AWS IoT Core using HTTP protocol. The system helps track air pollution levels and can be extended for smart city, industrial, or health-based monitoring solutions.

---

## 🔧 Components Used

- **ESP32 Microcontroller**
- **MQ135 Gas Sensor**
- **Wi-Fi Module (onboard ESP32)**
- **AWS IoT Core**
- **AWS DynamoDB (NoSQL database)**

---

## ⚙️ Features

- Real-time air quality monitoring
- Live data transmission to AWS DynamoDB using HTTP
- Timestamped data logging
- Cloud-based data storage (scalable & reliable)
- Ready for dashboard or alert system integration

---

## 📊 DynamoDB Table Format

| Timestamp           | AQI Value |  
|---------------------|-----------|  
| 2025-03-31 10:30:00 | 135       |  
| 2025-03-31 10:45:00 | 148       |  

> *The data is stored in DynamoDB with the timestamp as a primary key, allowing scalable real-time tracking of AQI values.*

---

## 💻 Code Overview (Arduino C++)

```cpp
void loop() {
  int aqi = analogRead(A0); // Read air quality from MQ135
  sendToDynamoDB(aqi);      // Custom function to send data using HTTP
  delay(30000);             // Send every 30 seconds
}
