#include <WiFi.h>
#include <ArduinoJson.h>

#define BAUD_RATE 9600
#define USERID "scw3315" //암호화를 하여야 하나 우노의 성능이 부족하여 일반 평문으로 전송//
#define PIR_INPUT_PIN 2

const unsigned int LED_PORT = 5;
const unsigned int LED2_PORT = 4;

char ssid[] = "KT_GiGA_2G_Family";     //  your network SSID (name) 
char pass[] = "ehd0134679";    // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status

unsigned long lastConnectionTime = 0;         // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 5000L; // delay between updates, in millisecondsunsigned long lastConnectionTime = 0;

String rcvbuf;
boolean getIsConnected = false;

int val = 0;

//모션인식관련 클래스//
class PassiveInfraredSensor
{
  int input_pin;
  
public:
  PassiveInfraredSensor(const int input_pin) //생성자//
  {
    this->input_pin = input_pin;

    pinMode(this->input_pin, INPUT);
  }

  const bool motion_detected() const
  {
    if(digitalRead(this->input_pin) == HIGH)
    {
      return true;
    }

    else
    {
      return false;
    }
  }
};

PassiveInfraredSensor pir(PIR_INPUT_PIN);

//서버의 정보//
IPAddress hostIp(172, 30, 1, 19);
int SERVER_PORT = 8000;
// Initialize the Ethernet client object
WiFiClient client;// Initialize the Ethernet client object//서버의 정보//

void setup() {
  // initialize serial:
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

    digitalWrite(LED_PORT, HIGH);
    delay(1000);
    digitalWrite(LED_PORT, LOW);

    Serial.println("--------------------------");
    printWifiData();
    Serial.println("--------------------------");
    printCurrentNet();

    Serial.println("-------------------------------------");

    /*Serial.println("------<< SHA256 Setting>>------");
    //SHA256 Encrypt//
    uint8_t *hash;
    Sha1.init();
    Sha1.print(USERID);
    hash = Sha1.result();

    for (int i=0; i<20; i++) {
      sha_result += "0123456789abcdef"[hash[i]>>4];
      sha_result += "0123456789abcdef"[hash[i]&0xf];
    }

    Serial.println(USERID);
    Serial.println(sha_result);
    Serial.println("SHA256 setting success...");
    Serial.println("-------------------------------");
    
    digitalWrite(LED_PORT, HIGH);
    delay(2000);
    digitalWrite(LED_PORT, LOW);
    delay(2000);*/
  }
}

void loop() {
  if(pir.motion_detected())
  {
    val = 1;
    httpRequest_Motiondata();
    Serial.println();
  }

  else
  {
    val = 0;
    httpRequest_Motiondata();
    Serial.println();
  }

  delay(5000);
}
//////////////////////////
void httpRequest_Motiondata(){

  Serial.println();
  Serial.println("motion data request");

  // close any connection before send a new request
  // this will free the socket on the WiFi shield
  //client.stop();

  delay(1000);
  
  // if there's a successful connection
  if (client.connect(hostIp, SERVER_PORT)) {
    Serial.println("Connecting...");

    //POST Data Set//
    String jsondata = "";

    /*//Encrypt//
    char user_id_crypto[50];
    user_id.toCharArray(user_id_crypto, 50);
    aes256_enc_single(key, user_id_crypto);
    Serial.print("encrypt: ");
    Serial.println(user_id_crypto);*/
    
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["user_id"] = USERID;
    root["motionvalue"] = val;

    root.printTo(jsondata); //String으로 변환/
    Serial.println(jsondata);
    
    // send the HTTP POST request
    client.print(F("POST /motiondata"));
    client.print(F(" HTTP/1.1\r\n"));
    client.print(F("Cache-Control: no-cache\r\n"));
    client.print(F("Host: 172.30.1.19:8000\r\n"));
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
    
        if(headcount != 13){
          rcvbuf = "";
        }
      }

      //데이터 영역/
      if(headcount == 13){
        //JSON파싱//
        StaticJsonBuffer<200> jsonBuffer;
        JsonObject& root = jsonBuffer.parseObject(rcvbuf);
        String result = root["result"];

        Serial.println(result);

        digitalWrite(LED2_PORT, HIGH);
        delay(1000);
        digitalWrite(LED2_PORT, LOW);
  
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
