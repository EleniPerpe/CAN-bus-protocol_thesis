#include <Arduino_CAN.h>
#include <LiquidCrystal.h>

int Contrast=75;
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
float temperature = 0;

static uint32_t const CAN_ID = 0x100;

byte degreeSymbol[8] = {
  B00111,
  B00101,
  B00111,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};

void setup()
{
  analogWrite(6,Contrast);
  lcd.begin(16, 2);
  lcd.clear();
  Serial.begin(115200);
  lcd.createChar(0, degreeSymbol);
  while (!Serial) { }

  if (!CAN.begin(CanBitRate::BR_125k))
  {
    Serial.println("CAN.begin(...) failed.");
    for (;;) {}
  }
  else {
    Serial.println("CAN bus init success!");
  }
}

void loop()
{
  if (CAN.available())
  {
    CanMsg const msg = CAN.read();

    temperature = atof((char*)msg.data);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temperature:");
    lcd.setCursor(0, 1);
    lcd.print(temperature);
    lcd.setCursor(6, 1);
    lcd.write(byte(0));
    lcd.setCursor(7, 1);
    lcd.print("C");

    if(temperature > 28){
      
      uint8_t const msg_data[] = {1,0,0,0,0,0,0,0};
      CanMsg tx_msg(CAN_ID, sizeof(msg_data), msg_data);
      Serial.println(temperature);
      Serial.println(tx_msg);
      if (int const rc = CAN.write(tx_msg); rc < 0)
      {
        Serial.print  ("CAN.write(...) failed with error code ");
        Serial.println(rc);
        for (;;) { }
      }
    }
    else {
      uint8_t const msg_data[] = {0,0,0,0,0,0,0,0};
      CanMsg tx_msg(CAN_ID, sizeof(msg_data), msg_data);
      Serial.println(temperature);
      Serial.println(tx_msg);
      if (int const rc = CAN.write(tx_msg); rc < 0)
      {
        Serial.print  ("CAN.write(...) failed with error code ");
        Serial.println(rc);
        for (;;) { }
      }
    }
  }
}
