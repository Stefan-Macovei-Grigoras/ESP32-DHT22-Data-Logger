#include <WiFi.h>
#include <WebSocketsClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

// WiFi Credentials
const char* ssid = "DIGI-xzV9";
const char* password = "cEUhqZd2xL";

// WebSocket Server Details
const char* websocketServer = "192.168.1.179"; // Change to your AWS WebSocket Server
const uint16_t websocketPort = 3000;  
const char* websocketPath = "/ws";  

WebSocketsClient webSocket;

// DHT22 Configuration
#define DHTPIN 33  // Change if using another pin
#define DHTTYPE DHT22  
DHT dht(DHTPIN, DHTTYPE);

unsigned long lastMessageTime = 0; // Timer for sending data

// Function to read and send sensor data
void sendSensorData() {
    float temperature = dht.readTemperature();  // Read temperature (°C)
    float humidity = dht.readHumidity();        // Read humidity (%)

    if (isnan(temperature) || isnan(humidity)) {
        Serial.println("❌ Failed to read from DHT22 sensor! Check connections.");
        return;
    }

    // JSON formatted data
    String jsonData = "{\"temperature\":" + String(temperature) + ", \"humidity\":" + String(humidity) + "}";
    
    webSocket.sendTXT(jsonData); // Send JSON data to WebSocket server
    Serial.println("📤 Sent: " + jsonData);
}

// WebSocket event handler
void webSocketEvent(WStype_t type, uint8_t *payload, size_t length) {
    switch (type) {
        case WStype_CONNECTED:
            Serial.println("✅ Connected to WebSocket Server!");
            webSocket.sendTXT("{\"status\":\"ESP32 Connected\"}");
            break;
        case WStype_DISCONNECTED:
            Serial.println("❌ Disconnected from WebSocket Server!");
            break;
        case WStype_TEXT:
            Serial.printf("📩 Received: %s\n", payload);
            break;
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("\n🔄 Connecting to WiFi...");

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\n✅ WiFi Connected!");
    Serial.print("📶 ESP32 IP Address: ");
    Serial.println(WiFi.localIP());

    // Start DHT sensor
    dht.begin();
    delay(5000);  // Give sensor time to stabilize

    // Connect to WebSocket Server
    webSocket.begin(websocketServer, websocketPort, websocketPath);
    webSocket.onEvent(webSocketEvent);
    webSocket.setReconnectInterval(5000); // Try to reconnect every 5 seconds
}

void loop() {
    webSocket.loop(); // Maintain WebSocket connection

    // Send sensor data every 5 seconds
    if (millis() - lastMessageTime > 5000) {
        lastMessageTime = millis();
        sendSensorData();
    }
}
