#define BLYNK_TEMPLATE_ID "TMPL6c5ml3ZsE"
#define BLYNK_TEMPLATE_NAME "Smart Expressway Management"
#define BLYNK_AUTH_TOKEN "OcNUrpKTsJ3SOiFsWoKm1W4q78GeNaoZ"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <SPI.h>
#include <MFRC522.h>

// Pins
#define SS_PIN   5
#define RST_PIN  21
#define LED_PIN  2
#define irSensor1 33
#define irSensor2 32
#define buzzerPin 25

MFRC522 mfrc522(SS_PIN, RST_PIN);

// WiFi credentials
char ssid[] = "Hasan";
char pass[] = "00000000";

// Distance between IR sensors (in cm)
float distanceBetweenSensors = 10.0;

unsigned long firstDetectionTime = 0;
unsigned long secondDetectionTime = 0;
bool firstDetected = false;
bool secondDetected = false;

float speed = 0;
bool buzzerManual = false;

// Dummy car info
String carInfo1 = "Model: Toyota Prius   |\nColor: White   |\nPlate: AB-1234";
String carInfo2 = "Model: Honda Civic    |\nColor: Black   |\nPlate: XY-5678";
bool toggleCar = false;

// For detecting changes in IR sensor state
int lastIR1 = HIGH;
int lastIR2 = HIGH;

// Read buzzer control button from Blynk
BLYNK_WRITE(V1) {
  int buttonState = param.asInt();
  buzzerManual = buttonState;
}

void setup() {
  Serial.begin(115200);
  Serial.println("Booting...");

  // SPI and RFID setup
  SPI.begin(14, 12, 13, SS_PIN);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(RST_PIN, LOW);
  delay(100);
  digitalWrite(RST_PIN, HIGH);
  delay(100);
  mfrc522.PCD_Init();
  mfrc522.PCD_SetRegisterBitMask(mfrc522.RFCfgReg, (0x07 << 4));
  Serial.println("RFID Ready...");

  // IR and buzzer setup
  pinMode(irSensor1, INPUT_PULLUP);
  pinMode(irSensor2, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);

  // WiFi
  WiFi.begin(ssid, pass);
  Serial.print("Connecting to WiFi");
  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 20) {
    delay(500);
    Serial.print(".");
    retries++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi Connected");
  } else {
    Serial.println("\nWiFi NOT connected! Running in offline mode.");
  }

  Blynk.config(BLYNK_AUTH_TOKEN);
  Blynk.connect();
}

void loop() {
  Blynk.run();

  int currentIR1 = digitalRead(irSensor1);
  int currentIR2 = digitalRead(irSensor2);

  // Log only if sensor state changed
  if (currentIR1 != lastIR1 || currentIR2 != lastIR2) {
    Serial.print("IR1: "); Serial.print(currentIR1);
    Serial.print(" | IR2: "); Serial.println(currentIR2);
    lastIR1 = currentIR1;
    lastIR2 = currentIR2;
  }

  // Detect car passing
  if (!firstDetected && currentIR1 == LOW) {
    firstDetectionTime = millis();
    firstDetected = true;
    Serial.println("First IR detected");
    delay(50);
  }

  if (firstDetected && !secondDetected && currentIR2 == LOW) {
    secondDetectionTime = millis();
    secondDetected = true;
    Serial.println("Second IR detected");

    float timeDifference = (secondDetectionTime - firstDetectionTime) / 1000.0;
    if (timeDifference > 0.05 && timeDifference < 10.0) {
      speed = distanceBetweenSensors / timeDifference;
      Serial.print("Speed: "); Serial.print(speed); Serial.println(" cm/s");

      if (Blynk.connected()) Blynk.virtualWrite(V0, speed);

      if (speed > 80) {
        digitalWrite(buzzerPin, HIGH);
      } else {
        digitalWrite(buzzerPin, buzzerManual ? HIGH : LOW);
      }
    } else {
      Serial.println("Invalid timing detected.");
    }

    // Reset
    firstDetected = false;
    secondDetected = false;
    firstDetectionTime = 0;
    secondDetectionTime = 0;
  }

  // RFID Reading
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    digitalWrite(LED_PIN, HIGH);
    Serial.print("UID:");
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      Serial.print(mfrc522.uid.uidByte[i], HEX);
    }
    Serial.println();
    digitalWrite(LED_PIN, LOW);

    // Send dummy car info
    if (toggleCar) {
      if (Blynk.connected()) Blynk.virtualWrite(V2, carInfo1);
    } else {
      if (Blynk.connected()) Blynk.virtualWrite(V2, carInfo2);
    }
    toggleCar = !toggleCar;

    delay(200);
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
  }

    // Manual buzzer control (if no overspeeding)
  if (!firstDetected && !secondDetected && speed <= 60) {
    digitalWrite(buzzerPin, buzzerManual ? HIGH : LOW);
  }



  delay(50); // Reduce CPU load and make monitor easier to read
}
