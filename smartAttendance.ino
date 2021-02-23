#include <rdm6300.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
const char* ssid = "RFID";
const char* password = "123123123";

#define RDM6300_RX_PIN D4
#define READ_LED_PIN D7
#define GREEN_LED_PIN
#define RED_LED_PIN
//RDM-D4
String RFID[] = {"6055068","6065871","6073768","5958666","5958741"};
String NAME[] = {"General","Major","Brigadier","Captain","Lieutenant",};

Rdm6300 rdm6300;

void setup() {

  Serial.begin(115200);
  pinMode(READ_LED_PIN, OUTPUT);
  digitalWrite(READ_LED_PIN, LOW);
  Wire.begin(D2, D1);
  lcd.init();
  lcd.backlight();
  lcd.home();

  rdm6300.begin(RDM6300_RX_PIN); // start serial to RFID reader

  Serial.println();
  lcd.print("WiFi Connecting...");
  Serial.print("Wifi connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  Serial.println();
  Serial.print("Connecting");
  lcd.setCursor(0, 1);
  while ( WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    lcd.print(".");
  }

  Serial.println();
  lcd.setCursor(0,0);
  Serial.println("WiFi Connected to ");
  lcd.setCursor(0,1);
  Serial.println(ssid);
  Serial.println(" Successfully!");
  delay(1000);
  lcd.setCursor(0, 0);
  lcd.print("WiFi Connected");
  Serial.print("NodeMCU IP Address : ");
  Serial.println(WiFi.localIP());

}


void loop()
{

  if ((WiFi.status() == WL_CONNECTED))
  {

label:
    lcd.clear();
    Serial.println("Place Your ID:");
    lcd.setCursor(0, 0);
    lcd.print("Place your ID:");
    delay(1000);
    String tag;
    int check = 0;
    int flag[5] = {0, 0,0,0,0};

    if (rdm6300.update()) //if non-zero tag_id, update() returns true- a new tag is near!
    {
      tag = String(rdm6300.get_tag_id());
      Serial.println(tag);
      for (int i = 0; i < 5; i++)
      {
        if (RFID[i] == tag)
        {
          digitalWrite(READ_LED_PIN, HIGH);
          delay(1000);
          digitalWrite(READ_LED_PIN, LOW);
          check = 1;
          Serial.println("Welcome " + NAME[i]+" ");
          lcd.setCursor(0, 0);
          lcd.println("");
          lcd.println("Welcome " + NAME[i]+"   ");
          delay(2500);
          flag[i] = 1;
        }
        if (flag[i] == 1)
        {
          Serial.println("Attendance already Counted");
          lcd.setCursor(0, 1);
          
          lcd.println("AtendanceCounted");
          delay(3000);

          rdm6300.end();
          rdm6300.begin(RDM6300_RX_PIN);
          goto label;
        }
      }
      if (check == 0) {
        tone(READ_LED_PIN, 261);
        noTone(READ_LED_PIN);
        Serial.println("Invalid RFID");
        digitalWrite(READ_LED_PIN, HIGH);
        delay(5000);

        
        lcd.setCursor(0,0);
        lcd.println("Invalid RFID");
        lcd.setCursor(0,1);
        lcd.println("Contact Official");
        delay(2000);
        digitalWrite(READ_LED_PIN, LOW);

        rdm6300.end();
        rdm6300.begin(RDM6300_RX_PIN);

        delay(2000);
      }
    }
  }

  delay(1000);
}
