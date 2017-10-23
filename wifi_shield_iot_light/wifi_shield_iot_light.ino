#include <sha256.h>
#include <AESLib.h>
#include <WiFi.h>
#include <ArduinoJson.h>

#define BAUD_RATE 9600
#define USERID "scw3315"

//Encrypt Key//
uint8_t key[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31};
String sha_result = "";

const unsigned int LDR_SENSOR_1 = A0;
const unsigned int LDR_SENSOR_2 = A1;
const unsigned int LDR_SENSOR_3 = A2;

const unsigned int LED_PORT = 5;
const unsigned int LED2_PORT = 4;

//Wifi Setting//
char ssid[] = "KT_GiGA_2G_Family";     //  your network SSID (name) 1575835037
char pass[] = "ehd0134679";    // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status

unsigned long lastConnectionTime = 0;         // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 5000L; // delay between updates, in millisecondsunsigned long lastConnectionTime = 0;

String rcvbuf;
boolean getIsConnected = false;

//서버의 정보//
IPAddress hostIp(172, 30, 1, 11);
int SERVER_PORT = 8000;
// Initialize the Ethernet client object
WiFiClient client;// Initialize the Ethernet client object//서버의 정보//

void setup() {
  
  Serial.begin(BAUD_RATE);

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

  // check firmware version
  Serial.print(F("Firmware version: "));
  Serial.println(WiFi.firmwareVersion());

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

    Serial.println("------<< SHA256 Setting>>------");
    //SHA256 Encrypt//
    uint8_t *hash;
    Sha256.init();
    Sha256.print(USERID);
    hash = Sha256.result();

    for (int i=0; i<32; i++) {
      sha_result += "0123456789abcdef"[hash[i]>>4];
      sha_result += "0123456789abcdef"[hash[i]&0xf];
    }

    Serial.println(sha_result);
    Serial.println("SHA256 setting success...");
    Serial.println("-------------------------------");
    
    digitalWrite(LED_PORT, HIGH);
    delay(2000);
    digitalWrite(LED_PORT, LOW);
    delay(2000);
  }
}

void loop() {
  get_light(); //get temperature//
  Serial.println();
}
//////////////////////////
void get_light()
{
  // put your main code here, to run repeatedly:
  int ldr_sensor1_value = map(analogRead(LDR_SENSOR_1), 0, 1023, 0, 255); //mapping 수행(0,1023) -> (0, 255)//
  int ldr_sensor2_value = map(analogRead(LDR_SENSOR_2), 0, 1023, 0, 255);
  int ldr_sensor3_value = map(analogRead(LDR_SENSOR_3), 0, 1023, 0, 255);

  Serial.println(ldr_sensor1_value);
  Serial.println(ldr_sensor2_value);
  Serial.println(ldr_sensor3_value);
  
  httpRequest_Light(ldr_sensor1_value, ldr_sensor2_value, ldr_sensor3_value);  
  
  digitalWrite(LED2_PORT, HIGH);
  delay(1500);
  digitalWrite(LED2_PORT, LOW);

  delay(15000);
}
//////////////////////////
void httpRequest_Light(int ldr_sensor1_value, int ldr_sensor2_value, int ldr_sensor3_value){

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
    String user_id = USERID;

    /*//Encrypt//
    char user_id_crypto[50];
    user_id.toCharArray(user_id_crypto, 50);
    aes256_enc_single(key, user_id_crypto);
    Serial.print("encrypt: ");
    Serial.println(user_id_crypto);*/
    
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["ldr1value"] = ldr_sensor1_value;
    root["ldr2value"] = ldr_sensor2_value;
    root["ldr3value"] = ldr_sensor3_value;
    root["user_id"] = user_id;

    root.printTo(jsondata); //String으로 변환/
    Serial.println(jsondata);
    
    // send the HTTP POST request
    client.print(F("POST /lightdata"));
    client.print(F(" HTTP/1.1\r\n"));
    client.print(F("Cache-Control: no-cache\r\n"));
    client.print(F("Host: 172.30.1.11:8000\r\n"));
    client.print(F("User-Agent: Arduino\r\n"));
    client.print(F("Content-Type: application/json;charset=UTF-8\r\n"));
    client.print(F("Content-Length: "));
    client.println(jsondata.length());
    client.println();
    client.println(jsondata);
    client.print(F("\r\n\r\n"));

    /*//Decrpyt//
    aes256_dec_single(key, user_id_crypto);
    Serial.print("decrypt: ");
    Serial.println(user_id_crypto);*/

    // note the time that the connection was made
    lastConnectionTime = millis();
    getIsConnected = true;
  }
  
  else {
    // if you couldn't make a connection
    Serial.println("Connection failed");
    getIsConnected = false;
  }

  //서버로 부터 값을 받는다.//
  int headcount = 0;
 
  //No Socket available문제 해결//
  while (client.connected()) {
    if (client.available() && status == WL_CONNECTED) {
      char c = client.read();

      //String에 담아서 원하는 부분만 파싱하도록 함//
      rcvbuf += c;
      
      if(c == '\r'){
        headcount ++; //해더 정보는 생략하기 위해서 설정//

        if(headcount != 7){
          rcvbuf = "";
        }
      }

      //데이터 영역/
      if(headcount == 7){
        //JSON파싱//
        StaticJsonBuffer<200> jsonBuffer;
        JsonObject& root = jsonBuffer.parseObject(rcvbuf);
        String result = root["result"];
        
        Serial.println(result);
  
        client.stop(); //클라이언트 접속 해제//
        
        rcvbuf = "";
      }
    }
  }

  client.flush();
  client.stop();
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
