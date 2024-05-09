#include "DHT.h"
#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
#include <MFRC522DriverPinSimple.h>
#include <MFRC522Debug.h>
#include <LiquidCrystal_I2C.h>
#include <Stepper.h>

class LiquidCrystal_I2C lcd(0x27, 16, 2);
class MFRC522DriverPinSimple sda_pin(53);
class MFRC522DriverSPI driver {sda_pin};
class MFRC522 mfrc522 {driver};
const uint16_t STEP_PER_REVOLUTION {2048U}; 
const uint16_t STEP_STOP {0U}; 
const uint16_t STEP_PER_STOP {0U}; 
const uint8_t TEMPER_HUMID = {A0};
const uint8_t CDS_SENSOR = {A4};
const uint8_t RGB_R = {A1};
const uint8_t RGB_G = {A2};
const uint8_t RGB_B = {A3};
const String MASTER_CARD_UID{String("31C15826")};
class Stepper stepping(STEP_PER_REVOLUTION, 24, 25, 26, 27); //1.3.2.4
class DHT dht(TEMPER_HUMID, 11); //객체 생성

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200UL);
  dht.begin();
  stepping.setSpeed(13);
  pinMode(TEMPER_HUMID, INPUT);
  pinMode(CDS_SENSOR, INPUT);
  pinMode(RGB_R, OUTPUT);
  pinMode(RGB_G, OUTPUT);
  pinMode(RGB_B, OUTPUT);
  mfrc522.PCD_Init();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("USER");
  lcd.setCursor(0, 1);
  lcd.print("CERTIFICATION");
}

void loop() 
{
  // put your main code here, to run repeatedly:
  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;
  MFRC522Debug::PICC_DumpToSerial(mfrc522, Serial, &(mfrc522.uid));
  String tagID = ""; //빈 문자열

  for (uint8_t i {0u}; i < 4; ++i)
  {
    tagID += String(mfrc522.uid.uidByte[i], HEX);
  }

  tagID.toUpperCase(); // 소문자를 대문자로 변환.
  mfrc522.PICC_HaltA();

  if (tagID == MASTER_CARD_UID)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WELCOME");
    analogWrite(RGB_G, 255);
    delay(3000); 
    analogWrite(RGB_G, 0);
    lcd.setCursor(0, 0);
    lcd.print("CERTIFICATION");
    lcd.setCursor(0, 1);
    lcd.print("COMPLETE");
    

    while(dht.read())
  {
    const float temperature {dht.readTemperature()}; //readTemperature -> 메소드
    const float humidity {dht.readHumidity()}; //readHumidity -> 메소드
    float cdsValue = analogRead(CDS_SENSOR);
    cdsValue = map(cdsValue, 242, 955, 255, 0);
    const String sending_data {String(temperature) + "," + String(humidity) + "," + String(cdsValue)};
   

    Serial.println(sending_data);
    if(Serial.available())
    {
      const String in_comming_data {Serial.readStringUntil('\n')};

      if(in_comming_data.equals("RGB_ON"))
      {
        analogWrite(RGB_R, 255);
        analogWrite(RGB_G, 255);
        analogWrite(RGB_B, 255);
      } 
      else if(in_comming_data.equals("RGB_OFF"))
      {
        analogWrite(RGB_R, 0);
        analogWrite(RGB_G, 0);
        analogWrite(RGB_B, 0);
      } 
      else if(in_comming_data.equals("Motor_ON"))
      {
        stepping.step(STEP_PER_REVOLUTION);
      }
      else if(in_comming_data.equals("Motor_OFF"))
      {
        stepping.step(STEP_STOP);
      }
   }
  delay(500UL);
}
  }
  else if(tagID != MASTER_CARD_UID)
  {
    lcd.setCursor(0, 0);
    lcd.print("FAIL");
    lcd.setCursor(0, 1);
    lcd.print("NOT CORRECT CARD");
    analogWrite(RGB_R, 255);
    delay(3000);
    analogWrite(RGB_R, 0);
    lcd.setCursor(0, 0);
    lcd.print("USER");
    lcd.setCursor(0, 1);
    lcd.print("CERTIFICATION");
  }
}
