#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#define relaylampu  12 //d6
#define relaykipas  13 //d7
#define servo 14 //d5
#define buzzer 15

Servo servoku;

const char* ssid     = "KianSantang";
const char* password = "mbuhklalen";

const char* host = "192.168.1.2";

WiFiClient client;
const int httpPort = 80;
String url;
LiquidCrystal_I2C lcd(0x3F, 16, 2);  
unsigned long timeout;
int nyala=LOW;
int mati=HIGH;
int kondisiberipakan=1;

//milis
long durasirequest = 0;
long durasitextpakan=0;
long durasitextsuhu=0;
long durasipakan  = 0;
void setup() {
  servoku.attach(servo);
  Serial.begin(9600);
  delay(10);
  lcd.begin();
  // turn on LCD backlight                      
  lcd.backlight();
   lcd.setCursor(0,0);
  lcd.print("Uyeee");
  pinMode(buzzer,OUTPUT);
  pinMode(relaylampu, OUTPUT);
  pinMode(relaykipas, OUTPUT);
  digitalWrite(relaylampu, mati);
  digitalWrite(relaykipas, mati);
  
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  
   Serial.print("kondisi Awal");
          Serial.println(kondisiberipakan);
          digitalWrite(buzzer,HIGH);
          digitalWrite(buzzer,LOW);
if (millis() - durasirequest > 500 )  
  {
    HTTPClient http;
    if (WiFi.status() == WL_CONNECTED) 
    { 
      http.begin("http://192.168.1.3/akuarium2/Status/core");
      int httpCode = http.GET();
      if (httpCode > 0) {
        char json[500];
        String payload = http.getString();
        payload.toCharArray(json,500);
        StaticJsonDocument<200> doc;
        deserializeJson(doc,json);
        const char* suhu = doc["suhu"];
        const char* sisapakan = doc["pakan"];
  
        int lampu = doc["lampu"];
        int kipas = doc["kipas"];
        int beripakan= doc["beripakan"];       
        int durasi = doc["durasi"];
       
        
  
        if (lampu==1)
        {
          digitalWrite(relaylampu,nyala);
          Serial.print("Lampu Nyala");
        }
        else if(lampu==0) 
        {
          digitalWrite(relaylampu,mati);
        }
        if (kipas==1)
        {
          digitalWrite(relaykipas,nyala);
          Serial.print("Kipas Nyala");
        }
        else if(kipas==0) 
        {
          digitalWrite(relaykipas,mati);
        }
        if (beripakan==1)
        {
          Serial.print("Sebelum kondisi ubah");
          Serial.println(kondisiberipakan);
          if (kondisiberipakan==1)
          {
            servoku.write (90);
            digitalWrite(buzzer,HIGH);
            delay (durasi);
            servoku.write (180);
            digitalWrite(buzzer,LOW);
            kondisiberipakan=0;
          }
           Serial.print("Settelah kondisi ubah");
          Serial.println(kondisiberipakan);
         }
         else if (beripakan==0)
         {
          kondisiberipakan=1;
         }
        Serial.println(payload);
        lcd.setCursor(0,0);
        lcd.print("Suhu saat ini");
        lcd.setCursor(0,1);
        lcd.print(suhu);
        delay(2000);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Sisa pakan");
        lcd.setCursor(0,1);
        lcd.print(sisapakan);
        delay(2000);
        durasirequest=millis();
        lcd.clear();
      }
      else{
       lcd.clear();
       lcd.setCursor(0,0);
       lcd.print("Not Found");
      }
      
      http.end();
    }
    else
    {
       lcd.clear();
       lcd.setCursor(0,0);
       lcd.print("Disconnected");
    }
    durasirequest=millis();
  }
}
