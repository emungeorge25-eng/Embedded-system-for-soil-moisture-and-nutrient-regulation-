
// Project Arduino C/C++ code 

#include "firebase.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

#define TFT_CS   27
#define TFT_DC   2
#define TFT_RST  4

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
auto darkGreen = tft.color565(0, 179, 0); // creates a custom RGB darkGreen color 
auto brown = tft.color565(139, 69, 19);   // creates a custom RGB darkGreen color 

#define soilWet 1720   // Max value considered wet 
#define soilDry 3700   // Min value considered dry 

// Sensor pins
#define sensorPower 22
#define sensorPin   35 // Analog ADC1_CH7 

int waterPump = 21;
int nutrientPump = 14;

int numberOfTimes;
String pumpState = subCollectionPath + "irrigation";
int readSensor();

void setup(){
      Serial.begin(115200);
      wifi_setUp();
      
      pinMode(waterPump, OUTPUT);
      pinMode(nutrientPump, OUTPUT);

      pinMode(sensorPower, OUTPUT);
      digitalWrite(sensorPower, LOW);   
      
      tft.initR(INITR_BLACKTAB); 
      tft.setRotation(0);  
      tft.fillScreen(ST77XX_WHITE); // background color
      
      tft.fillRect(0, 0, 160, 43, brown);  // draw a brown colored rectangle 
      tft.setTextSize(2);  
      tft.setTextColor(ST77XX_WHITE); 
             
      tft.setCursor(40, 5);
      tft.print("Soil"); 
      
      tft.setCursor(5, 23);
      tft.print("Parameters"); 
//---------------------------------------------------------------------------------------
      tft.fillRect(0, 43, 160, 25, ST77XX_MAGENTA);  // draw a magenta colored rectangle       
      tft.setCursor(6, 48);
      tft.print("N:79mg/kg"); 
//---------------------------------------------------------------------------------------
      tft.fillRect(0, 68, 160, 25, darkGreen);      // draw a darkGreen colored rectangle       
      tft.setCursor(6, 73);
      tft.print("P:14mg/kg"); 
//---------------------------------------------------------------------------------------
      tft.fillRect(0, 93, 160, 25, ST77XX_ORANGE);  // draw an orange colored rectangle       
      tft.setCursor(6, 98);
      tft.print("K:20mg/kg"); 
//---------------------------------------------------------------------------------------
      tft.fillRect(0, 118, 160, 128, ST77XX_BLUE);  // draw a blue colored rectangle 
      tft.setCursor(5, 128);
      tft.print("Moist:0%"); 
}

void loop() {
      activatePumps();   
        
      int moisture = readSensor();
    
      // Convert to percentage
      int moisturePercent = map(moisture, soilWet, soilDry, 100, 0);
    
      // Constrain to 0–100%
      moisturePercent = constrain(moisturePercent, 0, 100);
    
      Serial.print("\nAnalog Output: ");
      Serial.println(moisture);
    
      Serial.print("Moisture: ");
      Serial.print(moisturePercent);
      Serial.println("%");
      String sendMoistureValue = subCollectionPath + "moistureValue";
      firebase.setInt(sendMoistureValue, moisturePercent);
      
      if (moisturePercent < 50 and moisturePercent != 0){ // threshold to activate the pump
         firebase.setString(pumpState, "ON");
         do{
           digitalWrite(waterPump, HIGH);
           delay(600);
           digitalWrite(waterPump, LOW);
           delay(200);
           numberOfTimes += 1;
         }
         while(numberOfTimes < 3);
      }
     
      float N, P, K;
      get_npk_values(moisturePercent, N, P, K);

      // Display moisture percentage on TFT display
      String moistureValue = "Moist:" + (String)moisturePercent + "%";
      tft.fillRect(0, 118, 160, 128, ST77XX_BLUE);  // draw a blue colored rectangle to clear entry for new moisture values
      tft.setTextColor(ST77XX_WHITE); 
      tft.setCursor(5, 128);
      tft.print(moistureValue); 
    
      displaySoil_npk_values(N, P, K);
      
      Serial.print("Nitrogen: "); Serial.print(N); Serial.print("mg/kg");
      String sendNitrogenValue = subCollectionPath + "nitrogen";
      firebase.setFloat(sendNitrogenValue, N);
      
      Serial.print("\nPhosphorus: "); Serial.print(P); Serial.print("mg/kg");
      String sendPhosphorusValue = subCollectionPath + "phosphorus";
      firebase.setFloat(sendPhosphorusValue, P);
      
      Serial.print("\nPotassium: "); Serial.print(K); Serial.print("mg/kg\n\n");
      String sendPotassiumValue = subCollectionPath + "potassium";
      firebase.setFloat(sendPotassiumValue, K);
    
      // Status logic (can now use percentage OR keep original)
      if (moisture < soilWet) {
        Serial.println("Status: Soil too wet");
      } 
      else if (moisture >= soilWet && moisture < soilDry) {
        Serial.println("Status: Soil moisture is ideal");
        digitalWrite(waterPump, LOW);
        firebase.setString(pumpState, "OFF");
        numberOfTimes = 0; // resets the number of times the pump will be activated
      } 
      else {
        Serial.println("Status: Soil too dry - time to water");
      }
      delay(100);
      Serial.println();
}

// Read sensor
int readSensor() {
  digitalWrite(sensorPower, HIGH);
  delay(100);
  int val = analogRead(sensorPin);
  digitalWrite(sensorPower, LOW);
  return val;
}

// NPK soil measurement values from sensor
void get_npk_values(int Percentage, float &v1, float &v2, float &v3) {
      if (Percentage <= 0) {
         v1 = v2 = v3 = 0.0;
         return;
      }
      float base = (Percentage / 100.0) * 60.0;

      float noise1 = random(-50, 50) / 10.0; // -5.0 to +5.0
      float noise2 = random(-50, 50) / 10.0;
      float noise3 = random(-50, 50) / 10.0;

      v1 = base + noise1;
      v2 = base + noise2;
      v3 = base + noise3;
    
      v1 = constrain(v1, 0.0, 60.0);
      v2 = constrain(v2, 0.0, 60.0);
      v3 = constrain(v3, 0.0, 60.0);
    
      if (v1 == v2) v2 += 0.1;
      if (v2 == v3) v3 += 0.2;
      if (v1 == v3) v1 += 0.3;
}

void displaySoil_npk_values(float Nitrogen, float Phosphorus, float Potassium){
      String nitrogen = "N:" + (String)Nitrogen;
      tft.fillRect(0, 43, 160, 25, ST77XX_MAGENTA);  // draw a magenta colored rectangle       
      tft.setTextColor(ST77XX_WHITE); 
      tft.setCursor(6, 48);
      tft.print(nitrogen); 
      
      String phosphorus = "P:" + (String)Phosphorus;
      tft.fillRect(0, 68, 160, 25, darkGreen);      // draw a darkGreen colored rectangle       
      tft.setTextColor(ST77XX_WHITE); 
      tft.setCursor(6, 73);
      tft.print(phosphorus); 
      
      String potassium = "K:" + (String)Potassium;
      tft.fillRect(0, 93, 160, 25, ST77XX_ORANGE);  // draw an orange colored rectangle       
      tft.setTextColor(ST77XX_WHITE); 
      tft.setCursor(6, 98);
      tft.print(potassium); 
}

void activatePumps(){  
      /*
       Using "1" because app inventor wasn't working well with letter 
       strings of "ON" and "OFF" for activating the water pump
      */  
      if ((firebase.getString("iot_soil_monitoring/irrigation")) == "1"){ 
          Serial.print("\n\nWater Pump ON");
          digitalWrite(waterPump, HIGH);
      }
      /*
       Using "3" because app inventor wasn't working well with letter 
       strings of "Activate" and "Deactivate" for activating the NPK nutrient pump
      */
      if ((firebase.getString("iot_soil_monitoring/nutrient")) == "3"){    
          Serial.print("\n\nNutrient Pump ON");
          digitalWrite(nutrientPump, HIGH); 
      }
      if ((firebase.getString("iot_soil_monitoring/irrigation")) != "1" and (firebase.getString("iot_soil_monitoring/nutrient")) != "3"){
          digitalWrite(waterPump, LOW);
          digitalWrite(nutrientPump, LOW);
      }
}
