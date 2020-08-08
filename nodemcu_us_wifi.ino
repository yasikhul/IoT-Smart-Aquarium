#include <ESP8266WiFi.h>
#include <DallasTemperature.h>
#include <OneWire.h>

#define ONE_WIRE_BUS 5  //D2
#define TRIGGER_PIN  4  //D1
#define ECHO_PIN     0  //D

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);


const char* ssid     = "KianSantang";
const char* password = "mbuhklalen";

const char* host = "akuariumpintar.000webhostapp.com";

WiFiClient client;
const int httpPort = 80;
String url;
long duration, distance;
double suhu;
unsigned long timeout;
  
void setup() {
  Serial.begin(9600);
  delay(10);
  
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
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
void baca_suhu()
{
 sensors.requestTemperatures();                
  Serial.println("Temperature is: ");
  suhu=sensors.getTempCByIndex(0);
  Serial.println(suhu);
}

void baca_jarak(){
  digitalWrite(TRIGGER_PIN, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10); // Added this line
  digitalWrite(TRIGGER_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = (duration/2) / 29.1;
  Serial.print(distance);
  Serial.println(" cm");
}

void loop() {
  Serial.print("baca jarak ");
  baca_jarak();
  baca_suhu();
  
  Serial.print("connecting to ");
  Serial.println(host);

  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    //return;
  }

// We now create a URI for the request
  url = "/simpan/update?suhu=";
   url+=suhu;
  url+="&pakan=";
  url += distance;
  
  
  Serial.print("Requesting URL: ");
  Serial.println(url);

// This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }

// Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  Serial.println();
  Serial.println("closing connection");
  Serial.println();
  delay(5000);
}
