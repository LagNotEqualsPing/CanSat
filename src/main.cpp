
#include <MPU9250_asukiaaa.h>
#include <Adafruit_BMP280.h>


Adafruit_BMP280 bme; // I2C
MPU9250_asukiaaa mySensor;
float aX, aY, aZ, aSqrt, gX, gY, gZ, mDirection, mX, mY, mZ;

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

//test branch creation

  if (!bme.begin(0x76)) {
    Serial.println("BMP280 not found! Check wiring.");
    while (1);
  }
}


void loop() {

  if (mySensor.accelUpdate() == 0) {
    aX = mySensor.accelX();
    aY = mySensor.accelY();
    aZ = mySensor.accelZ();
   
  }
  
  if (mySensor.gyroUpdate() == 0) {
    gX = mySensor.gyroX();
    gY = mySensor.gyroY();
    gZ = mySensor.gyroZ();
    //Serial.println("\tgyroX: " + String(gX));
    //Serial.println("\tgyroY: " + String(gY));
    
  

    Serial.println("\tgX: " + String(gX));
    Serial.println("\tgY: " + String(gY));
    Serial.println("\tgZ: " + String(gZ));
    
    
  }
  

  /*
  
  Serial.print("\tTemperature(*C): ");
  Serial.print(bme.readTemperature());

  Serial.print("\tPressure(Inches(Hg)): ");
  Serial.print(bme.readPressure()/3377);

  */
  
  Serial.println("-----------------------------------");
  

  //Serial.print("\tApproxAltitude(m): ");
  //Serial.print(bme.readAltitude(1013.25)); // this should be adjusted to your local forcase
  delay(1000);
  //Serial.println(""); // Add an empty line. This text was added here from my tablet.
  }
