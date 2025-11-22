#include <Wire.h>
#include <MPU6050.h> 
#include <SPI.h>
#include <RH_RF95.h>

// MPU6050 pins and object
MPU6050 mpu;
const int SDA_PIN = 21;
const int SCL_PIN = 22;

// LoRa pins and settings
#define RFM95_CS 5
#define RFM95_RST 14
#define RFM95_INT 26
#define RF95_FREQ 433.0
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// Thresholds
const float DIR_THRESHOLD = 3.0;

// Vibration sensor
const int vibrationPin = 33;  // ✅ Vibration sensor pin

// Reset button
const int resetButtonPin = 32;  // ✅ External reset button pin

// To prevent repeating same messages
String lastMessage = "";

// DNA encoding function
String encodeToDNA(String msg) {
  String result = "";
  for (char c : msg) {
    byte b = (byte)c;
    for (int i = 6; i >= 0; i -= 2) {
      byte pair = (b >> i) & 0x03;
      result += (pair == 0) ? 'A' : (pair == 1) ? 'C' : (pair == 2) ? 'G' : 'T';
    }
  }
  return result;
}

// LoRa send function
void sendDNAEncryptedMessage(String msg) {
  if (msg != lastMessage) {
    String encoded = encodeToDNA(msg);
    Serial.print("Sending (Encoded): ");
    Serial.println(encoded);
    rf95.send((uint8_t*)encoded.c_str(), encoded.length());
    rf95.waitPacketSent();
    lastMessage = msg;
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);
  mpu.initialize();
  pinMode(vibrationPin, INPUT);
  pinMode(resetButtonPin, INPUT_PULLUP);  // ✅ Configure reset button

  if (!mpu.testConnection()) {
    Serial.println("MPU6050 connection failed!");
    while (1);
  }

  Serial.println("MPU6050 connected. Stabilizing sensor...");
  delay(2000);

  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, LOW); delay(10);
  digitalWrite(RFM95_RST, HIGH); delay(10);

  if (!rf95.init()) {
    Serial.println("LoRa init failed");
    while (1);
  }

  rf95.setFrequency(RF95_FREQ);
  rf95.setTxPower(23, false);
  Serial.println("LoRa transmitter ready.");
}

void loop() {
  // ✅ Check for external reset button press
  if (digitalRead(resetButtonPin) == LOW) {
    sendDNAEncryptedMessage("RESET");
    delay(1000);  // Debounce
    return;       // Skip rest of the loop
  }

  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);

  float x = (float)ax / 16384.0 * 10;
  float y = (float)ay / 16384.0 * 10;
  float z = (float)az / 16384.0 * 10;

  Serial.print("X: "); Serial.print(x);
  Serial.print(" | Y: "); Serial.print(y);
  Serial.print(" | Z: "); Serial.println(z);

  int vibrationValue = digitalRead(vibrationPin);

  if (vibrationValue == HIGH) {
    sendDNAEncryptedMessage("1");  // Vibration alert
  }
  else if (abs(x) < 1.0 && abs(y) < 1.0 && abs(z - 5.0) < 1.0) {
    sendDNAEncryptedMessage("Tree is safeeee");
  }
  else if (abs(x) > DIR_THRESHOLD && abs(y) < DIR_THRESHOLD) {
    sendDNAEncryptedMessage((x > 0) ? "Tree fall in West" : "Tree fall in East");
  }
  else if (abs(y) > DIR_THRESHOLD && abs(x) < DIR_THRESHOLD) {
    sendDNAEncryptedMessage((y > 0) ? "Tree fall in South" : "Tree fall in North");
  }

  delay(500);
}