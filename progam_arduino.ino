#include <DHT.h>
#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; //MAC Address

#define DHTPIN 2
#define DHTTYPE DHT22

DHT dht(DHTPIN,DHTTYPE);

float DataKelembaban;
float DataSuhu;

int period = 30000; //interval simpan data ke db
unsigned long time_now = 0;

unsigned long byteCount = 0;
bool printWebData = true;  // set to false for better speed measurement


char server[] = "192.168.10.10";
IPAddress ip(192,168,10,5); //arduino IP
EthernetClient client; 

void setup() {
  Serial.begin(115200);
  dht.begin();
  lcd.begin();

  //intro gak penting
  lcd.setCursor(0,0);
  lcd.print("   Temperature");
  lcd.setCursor(0,1);
  lcd.print("     Monitor");
  
  Ethernet.begin(mac, ip);
  Serial.print("Local IP: ");
  Serial.println(Ethernet.localIP());
  delay(2000);
  lcd.clear();
}

void loop(){
  DataKelembaban = dht.readHumidity();
  DataSuhu = dht.readTemperature(); 
  String print_temp = "Temp: ";  print_temp += String(DataSuhu); print_temp += " C";
  String print_humd = "Humd: ";  print_humd += String(DataKelembaban); print_humd += " %";
  lcd.setCursor(0,0);  lcd.print(print_temp);
  lcd.setCursor(0,1);  lcd.print(print_humd);
  if(millis() >= time_now + period){
    time_now += period;
    SendtoDB();
  } 
  int len = client.available();
  if (len > 0) {
        byte buffer[80];
        if (len > 80) len = 80;
        client.read(buffer, len);
        if (printWebData) {
          Serial.write(buffer, len); // show in the serial monitor (slows some boards)
        }
        byteCount = byteCount + len;
  } 
  
}

//insert data ke DB via injeksi control.php
void SendtoDB(){
   if (client.connect(server, 80)) {
    Serial.println("");
    Serial.println("connected");
    // Make a HTTP request:
    Serial.print("GET /arduino_mysql/control.php?dataKelembaban=");
    Serial.print(DataKelembaban);
    Serial.print("&dataSuhu=");
    Serial.println(DataSuhu);
    Serial.println("");
    
    client.print("GET /arduino_mysql/control.php?dataKelembaban=");     //YOUR URL
    client.print(DataKelembaban);
    client.print("&dataSuhu=");
    client.print(DataSuhu);
    client.print(" ");      //SPACE BEFORE HTTP/1.1
    client.print("HTTP/1.1");
    client.println();
    client.println("Host: 192.168.10.5");
    client.println("Connection: close");
    client.println();
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
 }
