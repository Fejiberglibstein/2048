#include <Wire.h>
#include <math.h>

const int MPU = 0x68;
int16_t AcX, AcY, AcZ;
int AcXcal, AcYcal, AcZcal;

const int RIGHT_PIN = 2;
const int LEFT_PIN = 4;
const int UP_PIN = 6;
const int DOWN_PIN = 8; 

bool tilted = false;
unsigned long lastTiltTime = 0;
const unsigned long cooldown = 500; // ms between tilt detections

void setup() {
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);  // power management register
  Wire.write(0);     // wake up the MPU-6050
  Wire.endTransmission(true);
  Serial.begin(9600);

  // calibration offsets
  AcXcal = 15800;
  AcYcal = -100;
  AcZcal = -1200;

  pinMode(RIGHT_PIN, OUTPUT);
  pinMode(LEFT_PIN, OUTPUT);
  pinMode(UP_PIN, OUTPUT);
  pinMode(DOWN_PIN, OUTPUT);

}

void loop() {
  // Read MPU data as before...
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true);
  AcX = Wire.read() << 8 | Wire.read();
  AcY = Wire.read() << 8 | Wire.read();
  AcZ = Wire.read() << 8 | Wire.read();

  int xVal = (AcX + AcXcal) / 10;
  int yVal = (AcY + AcYcal) / 10;
  int zVal = (AcZ + AcZcal) / 10;

  unsigned long now = millis();

  // --- Tilt Detection ---
  if (!tilted && (now - lastTiltTime >= cooldown)) {
    if (yVal >= 900) {
      Serial.println("Right");
      digitalWrite(RIGHT_PIN, HIGH);
      tilted = true;
      lastTiltTime = now;
    } else if (yVal <= -900) {
      Serial.println("Left");
      digitalWrite(LEFT_PIN, HIGH);
      tilted = true;
      lastTiltTime = now;
    } else if (zVal >= 900) {
      Serial.println("Down");
      digitalWrite(DOWN_PIN, HIGH);
      tilted = true;
      lastTiltTime = now;
    } else if (zVal <= -900) {
      Serial.println("Up");
      digitalWrite(UP_PIN, HIGH);
      tilted = true;
      lastTiltTime = now;
    }
  }

  // --- Reset condition: must go back flat ---
  if (tilted) {
    if (abs(yVal) < 800 && abs(zVal) < 800) {
      tilted = false; // re-arm only when flat
        // don't reset lastTiltTime here — let cooldown finish fully
        digitalWrite(RIGHT_PIN, LOW);
        digitalWrite(LEFT_PIN, LOW);
        digitalWrite(UP_PIN, LOW);
        digitalWrite(DOWN_PIN, LOW);
    }
  }

  delay(50);
}

