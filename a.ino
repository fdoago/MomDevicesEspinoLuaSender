#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_LSM9DS0.h>
#include <Adafruit_Sensor.h>  // not used in this demo but required!
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#define USE_SERIAL Serial


// i2c
Adafruit_LSM9DS0 lsm = Adafruit_LSM9DS0();

// agregado recientemente //
uint8_t MAC_array[6];
char MAC_char[18];

ESP8266WiFiMulti WiFiMulti;

void setupSensor()
{
  // 1.) Set the accelerometer range
  //lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_2G);
  //lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_4G);
  //lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_6G);
  //lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_8G);
  lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_16G);
  

  // 3.) Setup the gyroscope
  //lsm.setupGyro(lsm.LSM9DS0_GYROSCALE_245DPS);
  //lsm.setupGyro(lsm.LSM9DS0_GYROSCALE_500DPS);
  lsm.setupGyro(lsm.LSM9DS0_GYROSCALE_2000DPS);
}

void setup() {
  
    USE_SERIAL.begin(115200);
    // USE_SERIAL.setDebugOutput(true);
    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }
   // WiFiMulti.addAP("OPEN", "aabbccdd123");
    WiFiMulti.addAP("INFINITUM375A_2.4", "7110270614");

    //Obtenemos la MAC //
    WiFi.macAddress(MAC_array);
      for (int i = 0; i < sizeof(MAC_array); ++i){
        sprintf(MAC_char,"%s%02x:",MAC_char,MAC_array[i]);
      }
    
    // Try to initialise and warn if we couldn't detect the chip
    if (!lsm.begin())
    {
      Serial.println("Oops ... unable to initialize the LSM9DS0. Check your wiring!");
      while (1);
    }
    Serial.println("Found LSM9DS0 9DOF");
    Serial.println("");
    Serial.println("");
      
    // Memory pool for JSON object tree.
    //
    // Inside the brackets, 200 is the size of the pool in bytes,
    // If the JSON object is more complex, you need to increase that value.
    StaticJsonBuffer<200> jsonBuffer;
    char json[] = "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";
    JsonObject& root = jsonBuffer.parseObject(json);
    if (!root.success()) {
      Serial.println("parseObject() failed");
      return;
    }
    
    // Fetch values.
    //
    // Most of the time, you can rely on the implicit casts.
    // In other case, you can do root["time"].as<long>();
    const char* sensor = root["sensor"];
    long time = root["time"];
    double latitude = root["data"][0];
    double longitude = root["data"][1];
    
    // Print values.
    Serial.println(sensor);
    Serial.println(time);
    Serial.println(latitude, 6);
    Serial.println(longitude, 6);
}

void loop() {
    if((WiFiMulti.run() == WL_CONNECTED)) {
        // Se lee la info del sensor
        lsm.read();
        Serial.print("Acelerometro X: "); Serial.print((int)lsm.accelData.x); Serial.print(" ");
        Serial.print("Y: "); Serial.print((int)lsm.accelData.y);       Serial.print(" ");
        Serial.print("Z: "); Serial.println((int)lsm.accelData.z);     Serial.print(" ");
        Serial.print("Giroscopio X: "); Serial.print((int)lsm.gyroData.x);   Serial.print(" ");
        Serial.print("Y: "); Serial.print((int)lsm.gyroData.y);        Serial.print(" ");
        Serial.print("Z: "); Serial.println((int)lsm.gyroData.z);      Serial.print(" ");
        Serial.print ("MAC "); Serial.println(MAC_char);   Serial.println(" ");


//Condicion para verificar si esta acostado hacia la derecha o izquiera//
     //if ((int)lsm.accelData.y < -14000 )
     if ((int)lsm.accelData.y > 14000 )
     {
      Serial.print ("Acostado Derecha"); Serial.println("");
     }
      //else if ((int)lsm.accelData.y > 14000 )
      else if ((int)lsm.accelData.y < -14000 )
     {
         Serial.print ("Acostado Izquierda"); Serial.println("");
      }
      
// Cindicion para verificar si esta sentado (parado) o parado de manos //

  //if ((int)lsm.accelData.x > 13000 )
  if ((int)lsm.accelData.x < -14000 )
     {
      Serial.print ("Sentado o parado "); Serial.println("");
     }
      //else if ((int)lsm.accelData.x < -14000 )
      else if ((int)lsm.accelData.x > 13000 )
     {
         Serial.print ("Parado de manos"); Serial.println("");
      }

// Cindicion para verificar si esta acostado boca arriba o boca abajo //
 if ((int)lsm.accelData.z > 13000 )
     {
      Serial.print ("Acostado boca arriba "); Serial.println("");
     }
      else if ((int)lsm.accelData.z < -14000 )
     {
          Serial.print ("Acostado boca abajo"); Serial.println("");
          HTTPClient http;
          Serial.print("[HTTP] begin...\n");
          http.begin("http://vacaciones-todo-incluido.com/api/v1/giroescopio/set/");
          Serial.print("[HTTP] POST...\n");
          http.addHeader("Content-Type", "application/x-www-form-urlencoded");
          String dataPost = "mac=" + (String)MAC_char + "&giroescopio_x=" + (String)lsm.gyroData.x + "&giroescopio_y=" + (String)lsm.gyroData.y + "&giroescopio_z=" + (String)lsm.gyroData.z + "&acelerometro_x=" + (String)lsm.accelData.x + "&acelerometro_y=" + (String)lsm.accelData.y + "&acelerometro_z=" + (String)lsm.accelData.z  + "&status=1" + "&active=1";
          int httpCode = http.POST(dataPost);
          if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            Serial.printf("[HTTP] POST... code: %d\n", httpCode);
          
            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                Serial.println(payload);
            }
          } else {
            Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
          }
          http.end();
     }
    }
    delay(5000);
}

void getMac(){

}
