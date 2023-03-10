#include <DHT.h>
#include <DHT_U.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <PubSubClient.h>

float temperatura_sensor1 = 0;
float humedad_sensor1 = 0;

// inicializar a la esp32 como cliente MQTT (Mosquitto)
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

// INICIALIZAR A LA PANTALLA SSD1306
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define sensorDHT1 15
#define DHTTYPE DHT11   // DHT 11


DHT dht(sensorDHT1, DHTTYPE);

// SSID / Pasword de nuestro modem de internet
const char* ssid = "INFINITUM7DR3_2.4";
const char* password = "rCtASAp94a";
// dirección broker MQTT
const char* mqtt_server = "192.168.1.66";


const int humedadPuerto = 2;
float humedadValor = 0;

const int pHPuerto = 4;
float pHValor = 0;
float valpH = 0;


static const uint8_t image_data_tite[512] = {
    0xff, 0xff, 0xff, 0xe0, 0x03, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xfc, 0x00, 0x00, 0x3f, 0xff, 0xff, 
    0xff, 0xff, 0xf0, 0x3f, 0xfc, 0x0f, 0xff, 0xff, 
    0xff, 0xff, 0x83, 0xff, 0xff, 0xc3, 0xff, 0xff, 
    0xff, 0xff, 0x0f, 0xff, 0xff, 0xf0, 0xff, 0xff, 
    0xff, 0xfc, 0x3f, 0xff, 0xff, 0xfc, 0x3f, 0xff, 
    0xff, 0xf8, 0xff, 0xff, 0xff, 0xff, 0x1f, 0xff, 
    0xff, 0xe3, 0xff, 0xff, 0xff, 0xff, 0xc7, 0xff, 
    0xff, 0xc7, 0xff, 0xff, 0xff, 0xff, 0xe7, 0xff, 
    0xff, 0x8f, 0xff, 0xff, 0xff, 0xff, 0xf7, 0xff, 
    0xff, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xfe, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x7f, 
    0xfe, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x7f, 
    0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 
    0xf9, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 
    0xf9, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x9f, 
    0xf3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xcf, 
    0xe3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc7, 
    0xe7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe7, 
    0xe7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe7, 
    0xcf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf3, 
    0xcf, 0xff, 0xfc, 0xff, 0xff, 0x3f, 0xff, 0xf3, 
    0x9f, 0xff, 0xf0, 0xff, 0xff, 0x0f, 0xff, 0xf9, 
    0x9f, 0xff, 0xc3, 0xff, 0xff, 0xc3, 0xff, 0xf9, 
    0x9f, 0xff, 0x0f, 0xff, 0xff, 0xf0, 0xff, 0xf9, 
    0x9f, 0xfc, 0x3f, 0xff, 0xff, 0xfc, 0x3f, 0xf9, 
    0xbf, 0xfc, 0xff, 0xff, 0xff, 0xff, 0x3f, 0xfc, 
    0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 
    0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 
    0x3f, 0xff, 0x3f, 0x3f, 0xfc, 0xfc, 0xff, 0xfc, 
    0x3f, 0xff, 0x1e, 0x3f, 0xfc, 0x78, 0xff, 0xfc, 
    0x3f, 0xff, 0x80, 0x7f, 0xfe, 0x01, 0xff, 0xfc, 
    0x3f, 0xff, 0x00, 0x7f, 0xfe, 0x00, 0xff, 0xfc, 
    0x3f, 0xff, 0x3e, 0x7f, 0xfe, 0x7c, 0xff, 0xfc, 
    0x3f, 0xfe, 0x3e, 0x7f, 0xfe, 0x7c, 0x7f, 0xfc, 
    0x3f, 0xfe, 0x7c, 0xff, 0xff, 0x3e, 0x7f, 0xfc, 
    0x3f, 0xfe, 0x7c, 0xfc, 0x3f, 0x3e, 0x7f, 0xfc, 
    0x3f, 0xfc, 0xfc, 0xf0, 0x0f, 0xbf, 0x3f, 0xfd, 
    0x9f, 0xfc, 0xf9, 0xe3, 0xc7, 0x9f, 0x3f, 0xf9, 
    0x9f, 0xfc, 0xf9, 0xe7, 0xe7, 0x9f, 0x3f, 0xf9, 
    0x9f, 0xfc, 0xf9, 0xc0, 0x03, 0x9f, 0x3f, 0xf9, 
    0x9f, 0xfc, 0xf9, 0xc0, 0x03, 0x9f, 0x3f, 0xf9, 
    0xcf, 0xfc, 0xf9, 0xcf, 0xf3, 0x9f, 0x3f, 0xf3, 
    0xcf, 0xfe, 0xf9, 0xcf, 0xf3, 0x9f, 0x3f, 0xf3, 
    0xe7, 0xfe, 0x79, 0xcf, 0xf3, 0x9e, 0x7f, 0xe7, 
    0xe7, 0xfe, 0x7c, 0xcf, 0xf3, 0x3e, 0x7f, 0xe7, 
    0xf3, 0xfe, 0x7c, 0xcf, 0xf3, 0x3e, 0x7f, 0xcf, 
    0xf3, 0xfe, 0x7c, 0xe7, 0xe7, 0x3e, 0x7f, 0xcf, 
    0xf9, 0xfe, 0x7e, 0x63, 0xc6, 0x7e, 0x7f, 0x9f, 
    0xf9, 0xfe, 0x7e, 0x70, 0x0e, 0x7e, 0x7f, 0x1f, 
    0xfc, 0xfc, 0xfe, 0x7c, 0x3e, 0x7f, 0x3f, 0x3f, 
    0xfe, 0x7c, 0xfe, 0x7f, 0xfe, 0x7f, 0x3e, 0x7f, 
    0xfe, 0x38, 0xfe, 0x7f, 0xfe, 0x7f, 0x1c, 0x7f, 
    0xff, 0xf9, 0xfe, 0x7f, 0xfe, 0x7f, 0x98, 0xff, 
    0xff, 0xe1, 0xfe, 0x7f, 0xff, 0x7f, 0x81, 0xff, 
    0xff, 0xe3, 0xfc, 0xff, 0xff, 0x3f, 0xc3, 0xff, 
    0xff, 0xe1, 0xfc, 0xff, 0xff, 0x3f, 0x87, 0xff, 
    0xff, 0xf8, 0xf9, 0xff, 0xff, 0x9f, 0x1f, 0xff, 
    0xff, 0xfc, 0x39, 0xff, 0xff, 0x9c, 0x3f, 0xff, 
    0xff, 0xff, 0x09, 0xff, 0xff, 0x90, 0xff, 0xff, 
    0xff, 0xff, 0x81, 0xff, 0xff, 0x83, 0xff, 0xff, 
    0xff, 0xff, 0xf0, 0x3f, 0xfc, 0x0f, 0xff, 0xff, 
    0xff, 0xff, 0xfc, 0x00, 0x00, 0x3f, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xc0, 0x07, 0xff, 0xff, 0xff
};

static const uint8_t image_data_feliz[512] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xf0, 0x03, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0x80, 0x00, 0x7f, 0xff, 0xff, 
    0xff, 0xff, 0xfc, 0x0f, 0xfc, 0x0f, 0xff, 0xff, 
    0xff, 0xff, 0xf0, 0x7f, 0xff, 0x83, 0xff, 0xff, 
    0xff, 0xff, 0xe1, 0xff, 0xff, 0xe1, 0xff, 0xff, 
    0xff, 0xff, 0x87, 0xff, 0xff, 0xf8, 0x7f, 0xff, 
    0xff, 0xff, 0x1f, 0xff, 0xff, 0xfe, 0x3f, 0xff, 
    0xff, 0xfe, 0x3f, 0xff, 0xff, 0xff, 0x1f, 0xff, 
    0xff, 0xfc, 0x7f, 0xff, 0xff, 0xff, 0x8f, 0xff, 
    0xff, 0xf8, 0xff, 0xff, 0xff, 0xff, 0xc7, 0xff, 
    0xff, 0xf9, 0xff, 0xff, 0xff, 0xff, 0xe7, 0xff, 
    0xff, 0xf3, 0xff, 0xff, 0xff, 0xff, 0xf3, 0xff, 
    0xff, 0xe3, 0xff, 0xff, 0xff, 0xff, 0xf1, 0xff, 
    0xff, 0xe7, 0xff, 0xff, 0xff, 0xff, 0xf9, 0xff, 
    0xff, 0xc7, 0xff, 0xff, 0xff, 0xff, 0xf8, 0xff, 
    0xff, 0xcf, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xff, 
    0xff, 0xcf, 0xfe, 0x7f, 0xff, 0x9f, 0xfc, 0xff, 
    0xff, 0x9f, 0xfc, 0x3f, 0xff, 0x0f, 0xfe, 0x7f, 
    0xff, 0x9f, 0xfc, 0x3f, 0xff, 0x0f, 0xfe, 0x7f, 
    0xff, 0x9f, 0xfe, 0x7f, 0xff, 0x9f, 0xfe, 0x7f, 
    0xff, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 
    0xff, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 
    0xff, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 
    0xff, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 
    0xff, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 
    0xff, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 
    0xff, 0x3f, 0xff, 0xf0, 0xc3, 0xff, 0xff, 0x3f, 
    0xff, 0x3f, 0xff, 0xe0, 0x01, 0xff, 0xff, 0x3f, 
    0xff, 0x3f, 0xff, 0xe0, 0x01, 0xff, 0xff, 0x3f, 
    0xff, 0x3f, 0xff, 0xc0, 0x00, 0xff, 0xff, 0x3f, 
    0xff, 0x9f, 0xff, 0xc0, 0x00, 0xff, 0xfe, 0x7f, 
    0xff, 0x9f, 0xff, 0x80, 0x00, 0x7f, 0xfe, 0x7f, 
    0xff, 0x9f, 0xbf, 0x00, 0x00, 0x3f, 0x7e, 0x7f, 
    0xff, 0xcf, 0x80, 0x00, 0x00, 0x00, 0x7c, 0xff, 
    0xff, 0xcf, 0xc0, 0x00, 0xc0, 0x00, 0xfc, 0xff, 
    0xff, 0xc7, 0xf0, 0x01, 0xe0, 0x03, 0xf8, 0xff, 
    0xff, 0xe7, 0xf8, 0x03, 0xf0, 0x07, 0xf9, 0xff, 
    0xff, 0xe3, 0xff, 0x0f, 0xfc, 0x3f, 0xf1, 0xff, 
    0xff, 0xf3, 0xff, 0xff, 0xff, 0xff, 0xf3, 0xff, 
    0xff, 0xf9, 0xff, 0xff, 0xff, 0xff, 0xe7, 0xff, 
    0xff, 0xf8, 0xff, 0xff, 0xff, 0xff, 0xc7, 0xff, 
    0xff, 0xfc, 0x7f, 0xff, 0xff, 0xff, 0x8f, 0xff, 
    0xff, 0xfe, 0x3f, 0xff, 0xff, 0xff, 0x1f, 0xff, 
    0xff, 0xff, 0x1f, 0xff, 0xff, 0xfe, 0x3f, 0xff, 
    0xff, 0xff, 0x87, 0xff, 0xff, 0xf8, 0x7f, 0xff, 
    0xff, 0xff, 0xe1, 0xff, 0xff, 0xe1, 0xff, 0xff, 
    0xff, 0xff, 0xf0, 0x7f, 0xff, 0x83, 0xff, 0xff, 
    0xff, 0xff, 0xfc, 0x0f, 0xfc, 0x0f, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0x80, 0x00, 0x7f, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xf0, 0x03, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

void setup() {
  Serial.begin(9600);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  Serial.println(F("DHTxx test!"));
  pinMode(humedadPuerto, INPUT);
  dht.begin();

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  
}


// ***** VOID SETUP_WIFI() ***********

void setup_wifi(){
  delay(10);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);
  WiFi.begin(ssid,password);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");  
  }
  Serial.println("");
  Serial.println("WiFi Conectado");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void callback(char* topic, byte* message, unsigned int length){
  Serial.print("Mensaje recibido en topic");
  Serial.print(topic);
  Serial.print(", Message: ");
  String messageTemp;
  for(int i = 0;i < length; i++){
   Serial.print((char)message[i]);
   messageTemp += (char)message[i]; 
  }
  Serial.println();
}

// *************** VOID RECONNECT ***********

void reconnect(){
  while(!client.connected()){
    Serial.print("Intentando conexion MQTT...");
    if(client.connect("ESP8266Client")){
      Serial.println("conectado");
    } else{
      Serial.print("Fallo, rc = ");
      Serial.print(client.state());
      Serial.println(" Intentelo de nuevo en 5s ");
      delay(5000);  
    }
  }
}



void loop() {
  delay(2000);
  if(!client.connected()){
    reconnect();
  }
  client.loop();
  long now = millis();
  if(now - lastMsg > 100){
    lastMsg = now;
  // LECTURAS DE SENSORES
 temperatura_sensor1 = dht.readTemperature();
 humedad_sensor1 = dht.readHumidity();

  humedadValor = analogRead(humedadPuerto);
  pHValor = analogRead(pHPuerto);
  if(humedadValor >= 3200){
      display.clearDisplay();
      display.drawBitmap(0, 0,image_data_tite, 64, 64, 1);
      display.display(); 
  } else {
      display.clearDisplay();
      display.drawBitmap(0, 0, image_data_feliz, 64, 64, 1);
      display.display(); 
  }
  valpH = float(pHValor) / 1023 * 5.0;
  // IMPRESIONES A PANTALLA
  Serial.println("Humedad: ");
  Serial.println(humedad_sensor1);
  char humString1[8];
  dtostrf(humedad_sensor1, 1, 2, humString1);
  client.publish("esp32/humedad", humString1);
  
  Serial.println("Temperatura: ");
  Serial.println(temperatura_sensor1);
  char tempString[8];
  dtostrf(temperatura_sensor1, 1, 2, tempString);
  client.publish("esp32/temperatura", tempString);
  
  Serial.println("Humedad Suelo: ");
  Serial.println(humedadValor);
  char humsString[8];
  dtostrf(humedadValor, 1, 2, humsString);
  client.publish("esp32/hsuelo", humsString);
  
  Serial.println("pH Valor: ");
  Serial.print(valpH);
  char valpHString[8];
  dtostrf(valpH, 1, 2, valpHString);
  client.publish("esp32/pH", valpHString);
  
  }
}
