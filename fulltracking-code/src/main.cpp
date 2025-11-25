// MPU-6050 Short Example Sketch
// By Arduino User JohnChi
// August 17, 2014
// Public Domain
#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>

const int MPU_addr = 0x68; // I2C address of the MPU-6050
int Sensordata[7];
const char *ssid = "yourssid";
const char *password = "yourpasswd";

NetworkServer server(80);

void wifi_begin()
{
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void readSensor()
{
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 14, true); // request a total of 14 registers
  for (int i = 0; i < 7; i++)
  {
    Sensordata[i] = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L) ... 0x48 (GYRO_ZOUT_H) & 0x49 (GYRO_ZOUT_L)
  }
}

void print_data(Print &out)
{
  String dataString[7] = {"AcX", "AcY", "AcZ", "Tmp", "GyX", "GyY", "GyZ"};
  for (int i = 0; i < 7 && i != 3; i++)
  {
    out.printf("%S = ", dataString[i]);
    out.print(Sensordata[i]);
    if (i < 6)
    {
      out.print(" | ");
    }
    else
    {
      out.println();
    }
  }
  // Temperature
  out.print("Tmp=");
  out.print(Sensordata[3] / 340.00 + 36.53); // equation for temperature in degrees C from datasheet
}
void setup()
{
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0);    // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  Serial.begin(115200);
  wifi_begin();
}
void loop()
{
  NetworkClinet client = server.accept();

  if (client)
  {
    Serial.println("New Client");
    while (client.connected())
    {
      print_data(client);
    }
  }
  readSensor();
  print_data(Serial);
  delay(333);
}