
#include <MPU9250_asukiaaa.h>
#include <Adafruit_BMP280.h>


Adafruit_BMP280 bme; // I2C
MPU9250_asukiaaa mySensor;
float aX, aY, aZ, aSqrt, gX, gY, gZ, mDirection, mX, mY, mZ;

float offSetY = 3.44;

float tStart = millis();

void setup() {
  Serial.begin(9600);

  bme.begin();
  //mySensor.beginAccel();
  mySensor.beginGyro();
  //mySensor.beginMag();

  // You can set your own offset for mag values
  // mySensor.magXOffset = -50;
  // mySensor.magYOffset = -55;
  // mySensor.magZOffset = -10;

//test branch creation

  if (!bme.begin(0x76)) {
    Serial.println("BMP280 not found! Check wiring.");
    while (1);
  }
  tStart = millis();
}


float currentY = 0;
int i;

float oldTime = 0;

float alpha = 0.98;  // gyro trust factor

float roll =0;

void loop() {

  //if (mySensor.accelUpdate() == 0) {
    //aX = mySensor.accelX();
    //aY = mySensor.accelY();
    //aZ = mySensor.accelZ();
   
  //}

  
  if (mySensor.gyroUpdate() == 0) {
    gX = mySensor.gyroX();
    gY = mySensor.gyroY();
    gZ = mySensor.gyroZ();
    //Serial.println("\tgyroX: " + String(gX));
    //Serial.println("\tgyroY: " + String(gY));
  
    gY = gY - offSetY;

    if (gY < 1 and gY > -1) {
      gY = 0;
    }
    
    roll = roll + (millis() - tStart) /1000 * gY;
    //Serial.println((millis() - tStart)/1000, 3);
    tStart = millis();
    //Serial.println(tStart);
    Serial.println(roll, 5);

    /*if (gY < 1 and gY > -1) {
      gY = 0;
    }

    currentY += gY;

    while (currentY > 360) {
      currentY -= 360;
    }
    while (currentY < -360) {
      currentY += 360;
    }
    i++;
    if (i >= 1000) {
      Serial.println(currentY);
      i = 0;
    }*/
  }
  
    
  
  

  /*
  
  Serial.print("\tTemperature(*C): ");
  Serial.print(bme.readTemperature());

  Serial.print("\tPressure(Inches(Hg)): ");
  Serial.print(bme.readPressure()/3377);

  */
  
  //Serial.println("-----------------------------------");
  

  //Serial.print("\tApproxAltitude(m): ");
  //Serial.print(bme.readAltitude(1013.25)); // this should be adjusted to your local forcase
  //delay();
  //Serial.println(""); // Add an empty line. This text was added here from my tablet.
  }
