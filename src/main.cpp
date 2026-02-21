
#include <MPU9250_asukiaaa.h>
#include <Adafruit_BMP280.h>
#include <Wire.h>
#include <HardwareSerial.h>


Adafruit_BMP280 bme; // I2C
MPU9250_asukiaaa mySensor;
HardwareSerial LoRaSerial(1); //esp32-nek ez kell(elmeletileg)
float aX, aY, aZ, aSqrt, gX, gY, gZ, mDirection, mX, mY, mZ;

float offSetY = 3.44;

long lastTime;

float alpha = 0.98;  // gyro trust factor

#define TXD_PIN 2
#define RXD_PIN 15

#define SCL 22
#define SDA 23

float roll =0;
float pitch = 0;
float yaw = 0;

void sendATCommand(String command) {
  LoRaSerial.println(command); //\r\n\r\n\r\n"
  delay(100);
  String Message = "";
  while (LoRaSerial.available()) {
    Message += (char) LoRaSerial.read();
  }
  Serial.println(Message);
}

void sendBTCommand(String command) {
  LoRaSerial.println(command); //\r\n\r\n\r\n"
  Serial.println(command + "\r\n");
  delay(2000);
  String Message = "";
  while (LoRaSerial.available()) {
    Message += (char) LoRaSerial.read();
  }
  if (Message != ""){
  Serial.println(Message);}
  else {Serial.println("no response");}
}

void clearLoRaBuffer() {
  while (LoRaSerial.available()) {
    LoRaSerial.read();
  }
}

void setupLoRa(){
  //radio set sync
  delay(1500);
  clearLoRaBuffer();
  delay(2000);
  //sendBTCommand("sys get ver");
  sendBTCommand("sys reset");
  delay(1000);
  //sendBTCommand("mac pause");
  //delay(1000);
  sendBTCommand("radio set freq 863000000"); //868000000
  sendBTCommand("radio set mod lora");
  sendBTCommand("radio set pwr 20"); // POWER 20dbm (jobb lenne tobb ) akarmi lehet a foldi aallomason
  sendBTCommand("radio set sync a1"); // sync word (same for reciver and transmiter!!!)
  sendBTCommand("radio set bw 125"); // same for transmitter and reciver!!
  sendBTCommand("radio set sf sf7"); // same for tx rx sf8
}

void setup() {
  Serial.begin(115200);
  Serial.println("starting");
  Wire.begin(SDA, SCL);
  bme.begin(0x76);
  /*
  if (!bme.begin(0x76)) {
    Serial.println("BMP280 not found! Check wiring.");
    while (1);
  }
  */
  mySensor.beginAccel();
  mySensor.beginGyro();
  mySensor.beginMag();

  // You can set your own offset for mag values
  // mySensor.magXOffset = -50;
  // mySensor.magYOffset = -55;
  // mySensor.magZOffset = -10;

  
  
  
  lastTime = millis();
  //Serial.println("debug");
  LoRaSerial.begin(115200, SERIAL_8N1, RXD_PIN, TXD_PIN);
  //Serial.println(LoRaSerial.available());
  //Serial.println("calling setupLoRa");
  setupLoRa();
}

struct gyroscopeData
{
  float roll, pitch, yaw;
};


gyroscopeData gyroscope() {
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
    

  }

  //Serial.print("\tRoll: "); Serial.print(roll, 2);
  //Serial.print("\tPitch: "); Serial.print(pitch, 2);
  //Serial.print("\tYaw: "); Serial.println(yaw, 2);
  return {roll, pitch, yaw};

}


float readTMP117() {
  Wire.beginTransmission(0x48);
  Wire.write(0x00);
  Wire.endTransmission();

  Wire.requestFrom(0x48, 2);

  int raw = Wire.read() << 8;
  raw |= Wire.read();

  return raw * 0.0078125;
}


void loop() {

  gyroscopeData gyroData = gyroscope();
  float TMP117_data = readTMP117();
  //Serial.print("\tRoll: "); Serial.print(gyroData.roll, 2);
  //Serial.print("\tPitch: "); Serial.print(gyroData.pitch, 2);
  //Serial.print("\tYaw: "); Serial.println(gyroData.yaw, 2);
  //Serial.println(TMP117_data);
  /*
  
  Serial.print("\tTemperature(*C): ");
  Serial.print(bme.readTemperature());

  Serial.print("\tPressure(Inches(Hg)): ");
  Serial.print(bme.readPressure()/3377);
  */
  //pressure,temperature1,temperature2,aX,aY,aZ,gX,gY,gZ,mX,mY,mZ,roll,pitch,yaw
  String message = String(bme.readPressure()) + "a" + String(bme.readTemperature()) + "a" + String(TMP117_data) + "a" + String(mySensor.accelX())+ "a" + String(mySensor.accelY())+ "a" + String(mySensor.accelZ())+ "a" + String(mySensor.gyroX())+ "a" + String(mySensor.gyroY())+ "a" + String(mySensor.gyroZ())+ "a" + String(mySensor.magX())+ "a" + String(mySensor.magY())+ "a" + String(mySensor.magZ())+ "a" + String(gyroData.roll)+ "a" + String(gyroData.pitch)+ "a" + String(gyroData.yaw) + "a" + String(millis());

  message.replace(".", "c");
  message.replace("-", "b");
  sendATCommand("radio tx " + message + " 1"); //radio_rx a14c5230
  Serial.println("radio tx " + message + " 1");
  //Serial.println("sent " + TMP117_data_string);
  delay(100);
  
  //Serial.println("-----------------------------------");

  }
