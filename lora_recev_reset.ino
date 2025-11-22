#include <SPI.h>
#include <LoRa.h>

#define SS   5
#define RST  14
#define DIO0 2

int vibrationCount = 0;

int yellowLED = 25;
int redLED = 26;
int buzzer = 27;

void setup() {
  Serial.begin(115200);

  pinMode(yellowLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(buzzer, OUTPUT);

  digitalWrite(yellowLED, LOW);
  digitalWrite(redLED, LOW);
  digitalWrite(buzzer, LOW);

  LoRa.setPins(SS, RST, DIO0);

  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa failed!");
    while (1);
  }

  Serial.println("Receiver Ready...");
}

void loop() {

  int packetSize = LoRa.parsePacket();
  if (!packetSize) return;

  String msg = "";
  while (LoRa.available()) msg += (char)LoRa.read();

  Serial.print("RX: ");
  Serial.println(msg);

  // ------------------ RESET HANDLER ------------------
  if (msg == "RESET") {
    handleReset();
    return;
  }

  // ------------------ VIBRATION ------------------
  if (msg == "VIBRATION") {
    vibrationCount++;

    Serial.println("Vibration Count = " + String(vibrationCount));

    if (vibrationCount <= 5) {
      blinkYellow();
    }

    return;
  }

  // ------------------ TILT AFTER VIBRATION ------------------
  if (vibrationCount > 5 && msg.startsWith("TILT_")) {
    emergencyAlarm();
    return;
  }
}

// =====================================================
// FUNCTIONS
// =====================================================

void blinkYellow() {
  digitalWrite(yellowLED, HIGH);
  delay(150);
  digitalWrite(yellowLED, LOW);
  delay(150);
}

void emergencyAlarm() {
  Serial.println("TILT AFTER VIBRATION → EMERGENCY!");

  for (int i = 0; i < 10; i++) {
    digitalWrite(redLED, HIGH);
    digitalWrite(buzzer, HIGH);
    delay(150);
    digitalWrite(redLED, LOW);
    digitalWrite(buzzer, LOW);
    delay(150);
  }
}

void handleReset() {
  Serial.println("SYSTEM RESET RECEIVED");

  // Reset vibration counter
  vibrationCount = 0;

  // Turn off all indicators
  digitalWrite(yellowLED, LOW);
  digitalWrite(redLED, LOW);
  digitalWrite(buzzer, LOW);

  // Give visual confirmation flash
  digitalWrite(yellowLED, HIGH); delay(150);
  digitalWrite(yellowLED, LOW);  delay(150);
  digitalWrite(yellowLED, HIGH); delay(150);
  digitalWrite(yellowLED, LOW);

  Serial.println("Receiver Reset Complete");
}
