
#include <MPU9250_asukiaaa.h>
#include <Adafruit_BMP280.h>


Adafruit_BMP280 bme; // I2C
MPU9250_asukiaaa mySensor;
float aX, aY, aZ, aSqrt, gX, gY, gZ, mDirection, mX, mY, mZ;

float offSetY = 3.44;

long lastTime;

void setup() {
  Serial.begin(9600);

  bme.begin();
  mySensor.beginAccel();
  mySensor.beginGyro();
  mySensor.beginMag();

  // You can set your own offset for mag values
  // mySensor.magXOffset = -50;
  // mySensor.magYOffset = -55;
  // mySensor.magZOffset = -10;


  if (!bme.begin(0x76)) {
    Serial.println("BMP280 not found! Check wiring.");
    while (1);
  }
  lastTime = millis();
}




float alpha = 0.98;  // gyro trust factor

float roll =0;
float pitch = 0;
float yaw = 0;

void loop() {

  float dt = (millis() - lastTime) / 1000.0;
  lastTime = millis();

  if (mySensor.accelUpdate() == 0) {
    aX = mySensor.accelX();
    aY = mySensor.accelY();
    aZ = mySensor.accelZ();
   
  }

  
  if (mySensor.gyroUpdate() == 0) {
    gX = mySensor.gyroX();
    gY = mySensor.gyroY();
    gZ = mySensor.gyroZ();
  
    gY = gY - offSetY;

    if (gY < 1 and gY > -1) {
      gY = 0;
    }
    
    float alpha = 0.98;
    float accelRoll  = atan2(aY, aZ) * 180.0 / PI;
    roll  = alpha * (roll  + gX * dt) + (1 - alpha) * accelRoll;

    float accelPitch = atan2(-aX, sqrt(aY * aY + aZ * aZ)) * 180.0 / PI;
    pitch = alpha * (pitch + gY * dt) + (1 - alpha) * accelPitch;
    

    //Serial.println(pitch, 5);

  }

  if (mySensor.magUpdate() == 0) {
    mX = mySensor.magX();
    mY = mySensor.magY();
    mZ = mySensor.magZ();

    float rollRad = roll * PI / 180.0;
    float pitchRad = pitch * PI / 180.0;
    float mx = mX * cos(pitchRad) + mZ * sin(pitchRad);
    float my = mX * sin(rollRad) * sin(pitchRad) + mY * cos(rollRad) - mZ * sin(rollRad) * cos(pitchRad);
    float magYaw = atan2(my, mx) * 180.0 / PI;

    yaw = alpha * (yaw + gZ * dt) + (1 - alpha) * magYaw;
    //Serial.print("\tRoll: "); Serial.print(roll, 2);
    //Serial.print("\tPitch: "); Serial.print(pitch, 2);
    Serial.print("\tYaw: "); Serial.println(yaw, 2);

  }

  
    
  /*
  
  Serial.print("\tTemperature(*C): ");
  Serial.print(bme.readTemperature());

  Serial.print("\tPressure(Inches(Hg)): ");
  Serial.print(bme.readPressure()/3377);

  */
  
  //Serial.println("-----------------------------------");

  }
