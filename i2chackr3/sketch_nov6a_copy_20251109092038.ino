#include <Wire.h>
#include <math.h>

const int MPU = 0x68;
int16_t AcX, AcY, AcZ;
int AcXcal, AcYcal, AcZcal;

const int RIGHT_PIN = 13;
const int LEFT_PIN = 12;
const int UP_PIN = 11;
const int DOWN_PIN = 10; 

const int RIGHT_TEST_PIN = 2;
const int LEFT_TEST_PIN = 4;
const int UP_TEST_PIN = 6;
const int DOWN_TEST_PIN = 8;

bool tilted = false;
int lastTilt = 0;       // Last tilt integer
unsigned long lastTiltTime = 0;
const unsigned long cooldown = 500; // ms between tilt detections
const int FLAT = 0;
const int RIGHT = 1;
const int LEFT = 2;
const int UP = 3;
const int DOWN = 4;

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

  pinMode(RIGHT_TEST_PIN, OUTPUT);
  pinMode(LEFT_TEST_PIN, OUTPUT);
  pinMode(UP_TEST_PIN, OUTPUT);
  pinMode(DOWN_TEST_PIN, OUTPUT);

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
      if (lastTilt != RIGHT){
          Serial.println("Right");
          digitalWrite(RIGHT_PIN, HIGH);
          digitalWrite(RIGHT_TEST_PIN, HIGH);
          tilted = true;
          lastTiltTime = now;
          lastTilt = RIGHT;
      }
    } else if (yVal <= -900) {
      if (lastTilt != LEFT){
        Serial.println("Left");
        digitalWrite(LEFT_PIN, HIGH);
        digitalWrite(LEFT_TEST_PIN, HIGH);
        tilted = true;
        lastTiltTime = now;
        lastTilt = LEFT;
      }
    } else if (zVal >= 900) {
      if (lastTilt != DOWN){
        Serial.println("Down");
        digitalWrite(DOWN_PIN, HIGH);
        digitalWrite(DOWN_TEST_PIN, HIGH);
        tilted = true;
        lastTiltTime = now;
        lastTilt = DOWN;
      }
    } else if (zVal <= -900) {
      if (lastTilt != UP){
        Serial.println("Up");
        digitalWrite(UP_PIN, HIGH);
        digitalWrite(UP_TEST_PIN, HIGH);
        tilted = true;
        lastTiltTime = now;
        lastTilt = UP;
      }
    }
  }

  // --- Reset condition: must go back flat ---
  if (tilted) {
    if (abs(yVal) < 800 && abs(zVal) < 800) {
      tilted = false; // re-arm only when flat
        lastTilt = FLAT;
        // don't reset lastTiltTime here — let cooldown finish fully
        digitalWrite(RIGHT_PIN, LOW);
        digitalWrite(LEFT_PIN, LOW);
        digitalWrite(UP_PIN, LOW);
        digitalWrite(DOWN_PIN, LOW);
        digitalWrite(RIGHT_TEST_PIN, LOW);
        digitalWrite(LEFT_TEST_PIN, LOW);
        digitalWrite(UP_TEST_PIN, LOW);
        digitalWrite(DOWN_TEST_PIN, LOW);
    }
  }

  delay(50);
}

