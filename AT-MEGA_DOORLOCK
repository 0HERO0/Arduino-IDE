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
const uint16_t STEP_PER_REVOLUTION {512U}; 
class Stepper stepping(STEP_PER_REVOLUTION, 8, 10, 9, 11); 
const String MASTER_CARD_UID{String("31C15826")};
const uint8_t BUZZER {12};
const uint8_t RED {4};
const uint8_t GREEN {5};

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(115200UL);
  stepping.setSpeed(13);
  pinMode(GREEN, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(BUZZER,OUTPUT);
  mfrc522.PCD_Init();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("PLEASE CARD KEY");
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
    stepping.step(-STEP_PER_REVOLUTION);
    analogWrite(GREEN, 255);
    delay(1000); 

    tone(BUZZER, 30);
    delay(500);
    noTone(BUZZER);
    analogWrite(GREEN, 0);
    lcd.clear();
  }

   if (tagID != MASTER_CARD_UID)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("FAIL");
    lcd.setCursor(0, 1);
    lcd.print("NOT CORRECT CARD");
    analogWrite(RED, 255);
    delay(2000);

    analogWrite(RED, 0);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("PLEASE CARD KEY");
  }
}
