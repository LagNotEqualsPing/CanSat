#include <MPU9250_asukiaaa.h>
#include <Adafruit_BMP280.h>
#include <Wire.h>
#include <HardwareSerial.h>
#include <FS.h>
#include <SD_MMC.h>
#include <esp32cam.h>
#include <SparkFun_u-blox_GNSS_v3.h>


Adafruit_BMP280 bme;
MPU9250_asukiaaa mySensor;
HardwareSerial LoRaSerial(0);
SFE_UBLOX_GNSS gps;
float aX, aY, aZ, aSqrt, gX, gY, gZ, mDirection, mX, mY, mZ;

float offSetY = 3.44;

long lastTime;

float alpha = 0.98;  // gyro trust factor


#define SCL 1
#define SDA 13


#define RX 0 
#define TX 3

float roll =0;
float pitch = 0;
float yaw = 0;

void sendATCommand(String command) {
  LoRaSerial.println(command);
  delay(100);
  String Message = "";
  while (LoRaSerial.available()) {
    Message += (char) LoRaSerial.read();
  }
  Serial.println(Message);
}

void sendBTCommand(String command) {
  LoRaSerial.println(command);
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
  delay(1500);
  clearLoRaBuffer();
  delay(2000);
  sendBTCommand("sys get ver");
  sendBTCommand("sys reset");
  delay(1000);
  sendBTCommand("radio set freq 863000000"); //868000000
  sendBTCommand("radio set mod lora");
  sendBTCommand("radio set pwr 20");
  sendBTCommand("radio set sync a1");
  sendBTCommand("radio set bw 125");
  sendBTCommand("radio set sf sf7");
}

const auto RES = esp32cam::Resolution::find(640,480); //1600, 1200

void init_cam() {
    using namespace esp32cam;
    Config cfg;
    cfg.setPins(pins::AiThinker);
    cfg.setResolution(RES);
    cfg.setJpeg(80);
    Serial.printf("Heap: %u\n", ESP.getFreeHeap());
    Serial.printf("PSRAM found: %s\n", psramFound() ? "YES" : "NO");
    Serial.printf("PSRAM size: %u\n", ESP.getPsramSize());
    bool ok = Camera.begin(cfg);
    Serial.println(ok ? "CAMERA OK" : "CAMERA FAIL");
    if (!ok) {
      String error_message = "e3";
      sendATCommand("radio tx " + error_message + " 1"); 
    }
}

void take_picture() {
    static int cnt = 0;
    auto frame = esp32cam::capture();
    if (frame == nullptr) {
        Serial.println("Capture failed.");
        String error_message = "e1";
        sendATCommand("radio tx " + error_message + " 1");
        return;
    }
    String path = "/img" + String(cnt++) + ".jpg";
    File file = SD_MMC.open(path.c_str(), FILE_WRITE);
    if (!file) {
        Serial.println("Failed to open file.");
        String error_message = "e2";
        sendATCommand("radio tx " + error_message + " 1");
        return;
    }
    frame->writeTo(file);
    file.close();
}

void write_to_SD_card_overwrite(String message) {
    String path = "/data.csv";
    File file = SD_MMC.open(path, FILE_WRITE);

    if (!file) {
        Serial.println("failed to open file");
        return;
    }
    if (file.println(message)) {
        Serial.println("File written.");
    }
    else {
        Serial.println("Write failed.");
    }
    file.close();
}

void append_to_file(String message) {
    String path = "/data.csv";
    File file = SD_MMC.open(path, FILE_APPEND);

    if (!file) {
        Serial.println("failed to open file");
        return;
    }
    if (file.println(message)) {
        Serial.println("File written.");
    }
    else {
        Serial.println("Write failed.");
    }
    file.close();
}

void setup() {
  // Serial.begin(115200);
  // Serial.println("starting");
  
  lastTime = millis();
  LoRaSerial.begin(115200, SERIAL_8N1, RX, TX);
  setupLoRa();

  if (!SD_MMC.begin("/sdcard", true)) {
        // Serial.println("SD card mount failed.");
        return;
  }

  write_to_SD_card_overwrite("pressure,temperature1,temperature2,aX,aY,aZ,gX,gY,gZ,mX,mY,mZ,roll,pitch,yaw,time,latitude,longitude,gpsaltitude,SIV");

  Wire.begin(SDA, SCL);
  // bme.begin(0x76);
  
  if (!bme.begin(0x76)) {
    // Serial.println("BMP280 not found! Check wiring.");
    sendATCommand("radio tx e7 1");
  }
  
  mySensor.beginAccel();
  mySensor.beginGyro();
  mySensor.beginMag();

  // mySensor.magXOffset = -50;
  // mySensor.magYOffset = -55;
  // mySensor.magZOffset = -10;

  gps.begin();
  gps.setNavigationFrequency(1);
  init_cam();
  delay(1000);
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

int c = 0;

void loop() {
  gyroscopeData gyroData = gyroscope();
  float TMP117_data = readTMP117();
  
  //pressure,temperature1,temperature2,aX,aY,aZ,gX,gY,gZ,mX,mY,mZ,roll,pitch,yaw,time,Latitude,Longitude,gpsAltitude,SIV
  String message = String(bme.readPressure()) + "," + String(bme.readTemperature()) + "," + String(TMP117_data) + "," + String(mySensor.accelX())+ "," + String(mySensor.accelY())+ "," + String(mySensor.accelZ())+ "," + String(mySensor.gyroX())+ "," + String(mySensor.gyroY())+ "," + String(mySensor.gyroZ())+ "," + String(mySensor.magX())+ "," + String(mySensor.magY())+ "," + String(mySensor.magZ())+ "," + String(gyroData.roll)+ "," + String(gyroData.pitch)+ "," + String(gyroData.yaw) + "," + String(millis()) + "," + String(gps.getLatitude()) + "," + String(gps.getLongitude()) + "," + String(gps.getAltitude()) + "," + String(gps.getSIV()) + "," + String(bme.readAltitude());
  append_to_file(message);
  message.replace(",", "a");
  message.replace(".", "c");
  message.replace("-", "b");
  sendATCommand("radio tx " + message + " 1");

  if (c > 9) {
    take_picture();
    c = 0;
  }
  else {
    c++;
  }

  delay(100);
}