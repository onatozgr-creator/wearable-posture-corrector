#include <Wire.h>

const int MOTOR_PIN  = 9;
const int MPU_ADDR   = 0x68;

float currentAngle   = 0.0;
float referenceAngle = 0.0;
const float THRESHOLD = 15.0;
const float ALPHA     = 0.15;

void setup() {
  pinMode(MOTOR_PIN, OUTPUT);
  digitalWrite(MOTOR_PIN, LOW);

  Serial.begin(9600);
  delay(100);

  Wire.begin();
  Wire.setClock(100000);
  delay(200);

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission(true);
  delay(100);
  Serial.println("--- 2 saniyede  Kalibre Ediliyor ---");
  delay(2500);

  float angleSum = 0;
  for (int i = 0; i < 20; i++) {
    angleSum += getSensorAngle();
    delay(25);
  }
  referenceAngle = angleSum / 20.0;
  currentAngle   = referenceAngle;

  Serial.print("Referans Aci = ");
  Serial.print(referenceAngle, 1);
  Serial.println(" deg <<<\n");

  for (int i = 0; i < 2; i++) {
    digitalWrite(MOTOR_PIN, HIGH);
    delay(120);
    digitalWrite(MOTOR_PIN, LOW);
    delay(120);
  }
}

void loop() {
  float rawAngle = getSensorAngle();
  currentAngle = (ALPHA * rawAngle) + ((1.0 - ALPHA) * currentAngle);

  float diff = abs(currentAngle - referenceAngle);

  if (diff >= THRESHOLD) {
    digitalWrite(MOTOR_PIN, HIGH);
  } else {
    digitalWrite(MOTOR_PIN, LOW);
  }

  Serial.print("Guncel: ");
  Serial.print(currentAngle, 1);
  Serial.print(" Referans: ");
  Serial.print(referenceAngle, 1);
  Serial.print(" Fark: ");
  Serial.print(diff, 1);
  if (diff >= THRESHOLD) {
    Serial.print("dik dur");
  }
  Serial.println();

  delay(40);
}

float getSensorAngle() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  if (Wire.endTransmission(false) != 0) {
    return currentAngle;
  }

  byte count = Wire.requestFrom((uint8_t)MPU_ADDR, (uint8_t)6);

  if (count == 6) {
    uint8_t xh = Wire.read(); uint8_t xl = Wire.read();
    uint8_t yh = Wire.read(); uint8_t yl = Wire.read();
    uint8_t zh = Wire.read(); uint8_t zl = Wire.read();

    int16_t ax = (int16_t)((xh << 8) | xl);
    int16_t ay = (int16_t)((yh << 8) | yl);
    int16_t az = (int16_t)((zh << 8) | zl);

    float pitch = atan2((float)ax, sqrt((float)ay * ay + (float)az * az)) * 180.0 / 3.141592;
    return pitch;
  }

  return currentAngle;
}
