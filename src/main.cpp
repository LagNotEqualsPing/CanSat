
#include <MPU9250_asukiaaa.h>
#include <Adafruit_BMP280.h>


Adafruit_BMP280 bme; // I2C
MPU9250_asukiaaa mySensor;
float aX, aY, aZ, aSqrt, gX, gY, gZ, mDirection, mX, mY, mZ;

float minX, maxX, minY, maxY, minZ, maxZ;

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

  Serial.println("gX,gY,gZ");

  minX = 1000;
  minY = 1000;
  minZ = 1000;
  maxX = -1000;
  maxY = -1000;
  maxZ = -1000;

}

int amaount;
bool canLoop = true;

float oldGX, oldGY, oldGZ;

void loop() {

  //if (mySensor.accelUpdate() == 0) {
    //aX = mySensor.accelX();
    //aY = mySensor.accelY();
    //aZ = mySensor.accelZ();
   
  //}
  
  if (amaount <= 1000) {
    if (mySensor.gyroUpdate() == 0) {
      gX = mySensor.gyroX();
      gY = mySensor.gyroY();
      gZ = mySensor.gyroZ();
      //Serial.println("\tgyroX: " + String(gX));
      //Serial.println("\tgyroY: " + String(gY));
      
    
      /*
      Serial.println("\tgX: " + String(gX));
      Serial.println("\tgY: " + String(gY));
      Serial.println("\tgZ: " + String(gZ));

      Serial.println("\told gX: " + String(oldGX));
      Serial.println("\told gY: " + String(oldGY));
      Serial.println("\told gZ: " + String(oldGZ));

      Serial.println("\tdeltaX: " + String(gX - oldGX));
      Serial.println("\tdeltaY: " + String(gY - oldGY));
      Serial.println("\tdeltaZ: " + String(gZ - oldGZ));
      */


      oldGX = gX;
      oldGY = gY;
      oldGZ = gZ;

      if (gX > maxX) {
        maxX = gX;
      }
      if (gY > maxY) {
        maxY = gY;
      }
      if (gZ > maxZ) {
        maxZ = gZ;
      }
      if (gX < minX) {
        minX = gX;
      }
      if (gY < minY) {
        minY = gY;
      }
      if (gZ < minZ) {
        minZ = gZ;
      }
      
      
      
      Serial.println(String(gX) + "," + String(gY) + "," + String(gZ));
      amaount++;
    }
  }
  else {
    if (canLoop) {
      Serial.println("maxX: " + String(maxX) + ", minX: " + String(minX));
      Serial.println("maxY: " + String(maxY) + ", minY: " + String(minY));
      Serial.println("maxZ: " + String(maxZ) + ", minZ: " + String(minZ));
      canLoop = false;
    }
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
  delay(10);
  //Serial.println(""); // Add an empty line. This text was added here from my tablet.
  }
