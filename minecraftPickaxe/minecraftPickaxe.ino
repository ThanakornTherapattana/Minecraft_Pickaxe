#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_MPU6050.h>
#include <HijelHID_BLEMouse.h>


float accX, accY, accZ, prevX, prevY, prevZ;
float shake_mag;

bool isShake = false;
bool patternShake = false;

float shake_Threshold = 50;
int count_Threshold = 5;

unsigned long lastShakeTime = 0;
unsigned long shakeGap = 200;
unsigned long resetGap = 250;

int shakeCount = 0;

Adafruit_MPU6050 mpu;
HijelBLEMouse mouse("minecraftPickaxe");

void get_value() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  accX = a.acceleration.x;
  accY = a.acceleration.y;
  accZ = a.acceleration.z;
}

void check_shake() {


  float del_x = abs(accX - prevX);
  float del_y = abs(accY - prevY);
  float del_z = abs(accZ - prevZ);

  shake_mag = del_x + del_y + del_z;

  unsigned long now = millis();

  if (shake_mag >= shake_Threshold) {
    if (now - lastShakeTime > shakeGap) {
      isShake = true;
      shakeCount++;
      lastShakeTime = now;
    }
    if(shakeCount>count_Threshold){
      patternShake = true;
      isShake = false;
    }
  }

  if (now - lastShakeTime > resetGap) {
    shakeCount = 0;
    isShake = false;
    patternShake = false;

  }

  prevX = accX;
  prevY = accY;
  prevZ = accZ;
}

void press_mouse(){
  if(isShake){
    mouse.click(MouseButton::Left);
  }else if(patternShake){
    mouse.press(MouseButton::Left);
  }else{
    mouse.release(MouseButton::Left);
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin(21, 22);

  if (!mpu.begin()) {
    Serial.println("MPU6050 not found");
    while (1);
  }

  Serial.println("MPU6050 connected");

  mouse.begin();

  while (!mouse.isPaired()) {
    Serial.println("waiting for mouse to connect");
    delay(500);
    
  }
}

void loop() {
  get_value();
  check_shake();
  press_mouse();

  Serial.print("Shake: ");
  Serial.print(isShake);

  Serial.print("pattern: ");
  Serial.print(patternShake);

  Serial.print(" Magnitude: ");
  Serial.print(shake_mag);

  Serial.print(" Count: ");
  Serial.println(shakeCount);

  delay(50);
}