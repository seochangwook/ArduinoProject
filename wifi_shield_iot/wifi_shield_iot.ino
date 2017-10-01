#include <DHT11.h> //온습도 라이브러리/
#include <WiFi.h>
#include <ArduinoJson.h>

//temperature sensor//
#define TEMP_SENSOR 6

const unsigned int LED_PORT = 8;
const unsigned int LED2_PORT = 5;

DHT11 dht11(TEMP_SENSOR); //use temperature sensor library

//Wifi Setting//
char ssid[] = "KT_GiGA_2G_Family";     //  your network SSID (name) 1575835037
char pass[] = "ehd0134679";    // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status

unsigned long lastConnectionTime = 0;         // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 5000L; // delay between updates, in millisecondsunsigned long lastConnectionTime = 0;

boolean getIsConnected = false;

//서버의 정보//
IPAddress hostIp(172, 30, 1, 31);
int SERVER_PORT = 8000;
// Initialize the Ethernet client object
WiFiClient client;// Initialize the Ethernet client object//서버의 정보//

void setup() {
  // initialize serial:
  Serial.begin(9600);

  pinMode(LED_PORT, OUTPUT);
  pinMode(LED2_PORT, OUTPUT);

  // attempt to connect using WPA2 encryption:
  Serial.println("Attempting to connect to WPA network...");
  status = WiFi.begin(ssid, pass);

  // 현재 아두이노에 연결된 실드를 확인
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present"); 
    // 와이파이 실드가 아닐 경우
    while(true);
  } else{
    Serial.println("wifi shield is load...");
  }
  
  // if you're not connected, stop here:
  if ( status != WL_CONNECTED) { 
    Serial.println("Couldn't get a wifi connection");
    while(true);
  } 
  // if you are connected, print out info about the connection:
  else {
    Serial.println("Connected to network");

    Serial.println("------------<network info>-----------");
    printWifiData();
    printCurrentNet();
    Serial.println("-------------------------------------");

    digitalWrite(LED_PORT, HIGH);
    delay(2000);
    digitalWrite(LED_PORT, LOW);
  }
}

void loop() {
  get_temperature(); //get temperature//

  client.flush();
  client.stop();
}
//////////////////////////
void get_temperature()
{
  int err;
  float temp, humi; 
  
  if((err=dht11.read(humi, temp))==0) //온도, 습도 읽어와서 표시 
  {  
    Serial.print("humi :");
    Serial.print(humi);
    Serial.print(" / ");
    Serial.print(temp);
    Serial.println();

    httpRequest_Temp_Humi((int)temp, (int)humi);  

    digitalWrite(LED2_PORT, HIGH);
    delay(1500);
    digitalWrite(LED2_PORT, LOW);
  }

  else                               
  {    
    Serial.println();    
    Serial.print("Error No :");   
    Serial.print(err); 
    Serial.println();     
  } 

  delay(10000);
}
//////////////////////////
void httpRequest_Temp_Humi(int temp, int humi) {
  Serial.println();

  // close any connection before send a new request
  // this will free the socket on the WiFi shield
  //client.stop();

  delay(1000);
  
  // if there's a successful connection
  if (client.connect(hostIp, SERVER_PORT)) {
    Serial.println("Connecting...");

    //POST Data Set//
    String jsondata = "";
    
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["tempvalue"] = "10";
    root["humivalue"] = "20";

    root.printTo(jsondata); //String으로 변환/
    Serial.println(jsondata);
    
    // send the HTTP POST request
    client.print(F("POST /ajaxtest"));
    client.print(F(" HTTP/1.1\r\n"));
    client.print(F("Cache-Control: no-cache\r\n"));
    client.print(F("Host: 172.30.1.31:8000\r\n"));
    client.print(F("User-Agent: Arduino\r\n"));
    client.print(F("Content-Type: application/json\r\n"));
    client.print(F("Content-Length: "));
    client.println(jsondata.length());
    client.println();
    client.println(jsondata);
    client.print(F("\r\n\r\n"));
    
    // note the time that the connection was made
    lastConnectionTime = millis();
    getIsConnected = true;
  }
  
  else {
    // if you couldn't make a connection
    Serial.println("Connection failed");
    getIsConnected = false;
  }
}
//////////////////////////
void printWifiData() {
  // WI-FI 실드의 IP를 출력한다.
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.println(ip);
  
  // MAC어드레스를 출력한다.
  byte mac[6];  
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  Serial.print(mac[5],HEX);
  Serial.print(":");
  Serial.print(mac[4],HEX);
  Serial.print(":");
  Serial.print(mac[3],HEX);
  Serial.print(":");
  Serial.print(mac[2],HEX);
  Serial.print(":");
  Serial.print(mac[1],HEX);
  Serial.print(":");
  Serial.println(mac[0],HEX);
}

void printCurrentNet() {
  // 접속하려는 네트워크의 SSID를 출력한다.
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // 접속하려는 router의 MAC 주소를 출력한다.
  byte bssid[6];
  WiFi.BSSID(bssid);    
  Serial.print("BSSID: ");
  Serial.print(bssid[5],HEX);
  Serial.print(":");
  Serial.print(bssid[4],HEX);
  Serial.print(":");
  Serial.print(bssid[3],HEX);
  Serial.print(":");
  Serial.print(bssid[2],HEX);
  Serial.print(":");
  Serial.print(bssid[1],HEX);
  Serial.print(":");
  Serial.println(bssid[0],HEX);

  // 수신 신호 강도를 출력한다.
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // 암호화 타입을 출력한다.
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption,HEX);
  Serial.println();
}
