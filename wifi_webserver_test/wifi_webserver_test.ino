#include <SPI.h>
#include <WiFi.h>

#define FIRE_SENSOR  2
#define WARING_LED 12

char ssid[] = "Family"; //연결할 와이파이 이름//
char pass[] = "ehd35797"; //와이파이의 비밀번호/
int status = WL_IDLE_STATUS;

//입력변수//
char trans_str;
String rcvbuf;
boolean getIsConnected = false;

//trans data//
String transdata = "";

unsigned long lastConnectionTime = 0;         // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 5000L; // delay between updates, in milliseconds

//서버의 정보//
IPAddress hostIp(192, 168, 0, 10);
// Initialize the Ethernet client object
WiFiClient client;// Initialize the Ethernet client object

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); 
  while (!Serial) {
    ; // 시리얼 포트에 연결될 때까지 기다림(아두이노 레오나르도만 쓰면 됨)
  }
  
  // 현재 아두이노에 연결된 실드를 확인
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present"); 
    // 와이파이 실드가 아닐 경우
    while(true);
  } 
  
 // 와이파이에 연결 시도
  while ( status != WL_CONNECTED) { // 연결이 될 때 까지 반복
    Serial.print("Attempting to CONNECT to WPA SSID: ");
    Serial.println(ssid);
    // WPA/WAP2 네트워크에 연결 (시간이 필요)
    status = WiFi.begin(ssid, pass);

    delay(10000);
  }

  pinMode(FIRE_SENSOR, INPUT); //센서활성화//
  pinMode(WARING_LED, OUTPUT);
   
  // 연결에 성공했으면 연결확인 메시지와 연결된 네트워크 정보를 띄운다. 
  Serial.println("You're connected to the network");
  Serial.println("-------<network info>-------");
  printCurrentNet();
  printWifiData();

  Serial.println("-------<data trans>-------");
}

void loop() 
{
  // put your main code here, to run repeatedly:
  int fire_sensor_val = digitalRead(FIRE_SENSOR);

  Serial.println(fire_sensor_val);

  if(fire_sensor_val == 0) //현재 불이 감지되었다는 의미//
  {
    //서버로 데이터를 전송한다.(웹 통신 시 POST지향)//
    httpRequest_Fire();

    digitalWrite(WARING_LED, HIGH);
    delay(2000);
    digitalWrite(WARING_LED, LOW);
    delay(500);
  }
  
  delay(1000);
  
  /*if(Serial.available())
  {
    char data = Serial.read();
    
    if(data == '\n') //Enter key - Line Feed//
    {
      Serial.print("Trans data [");
      Serial.print(transdata);
      Serial.println("] Please wait...");

      httpRequest_GET(); //GET method//
      httpRequest_POST(); //POST method//

      transdata = ""; //init string//
    }

    else
    {
      //string concatination//
      transdata.concat(data);
    }
  }*/

  //서버로 부터 값을 받는다.//
  while (client.available()) {
    char c = client.read();
    if ( c != NULL ) {
      if (rcvbuf.length() > 20)
        rcvbuf = "";
      rcvbuf += c;
      Serial.write(c);
    }
  }

  rcvbuf = ""; //값을 다시 초기화//
}
//////////////////
// this method makes a HTTP connection to the server
void httpRequest_GET() {
  Serial.println();

  // close any connection before send a new request
  // this will free the socket on the WiFi shield
  client.stop();

  delay(1000);
  
  // if there's a successful connection
  if (client.connect(hostIp, 8080)) {
    Serial.println("Connecting...");

    // send the HTTP GET request
    client.print(F("GET /DummyServer_Blog/TestArduino/testserver.jsp"));
    client.print("?");
    
    //데이터 유무에 따라 값을 넣어주거나 넣지 않는다.//
    client.print("test_str=");
    client.print(transdata);
    
    client.print(F(" HTTP/1.1\r\n"));
    client.print(F("Host: 192.168.0.9\r\n"));
    client.print(F("User-Agent: Arduino\r\n"));
    client.print(F("\r\n\r\n"));
    client.println();
    
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
//////////////////
void httpRequest_POST() {
  Serial.println();

  // close any connection before send a new request
  // this will free the socket on the WiFi shield
  //client.stop();

  delay(1000);
  
  // if there's a successful connection
  if (client.connect(hostIp, 8080)) {
    Serial.println("Connecting...");

    //post data set//
    String postdata = "";
    String key_1 = "test_str=";

    //데이터 유무에 따라 값을 넣어주거나 넣지 않는다.//
    postdata.concat(key_1);
    postdata.concat(transdata);

    Serial.print("post data [");
    Serial.print(postdata);
    Serial.println("]");

    // send the HTTP POST request
    client.print(F("POST /DummyServer_Blog/TestArduino/testserver.jsp"));
    client.print(F(" HTTP/1.1\r\n"));
    client.print(F("Cache-Control: no-cache\r\n"));
    client.print(F("Host: 192.168.0.9\r\n"));
    client.print(F("User-Agent: Arduino\r\n"));
    client.print(F("Content-Type: application/x-www-form-urlencoded\r\n"));
    client.print(F("Content-Length: "));
    client.println(postdata.length());
    client.println();
    client.println(postdata);
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
//////////////////
void httpRequest_Fire() {
  Serial.println();

  // close any connection before send a new request
  // this will free the socket on the WiFi shield
  //client.stop();

  delay(1000);
  
  // if there's a successful connection
  if (client.connect(hostIp, 8080)) {
    Serial.println("Connecting...");

    //post data set//
    String postdata = "";
    String key_1 = "test_str=";

    //데이터 유무에 따라 값을 넣어주거나 넣지 않는다.//
    postdata.concat(key_1);
    postdata.concat("fire!!");

    Serial.print("post data [");
    Serial.print(postdata);
    Serial.println("]");

    // send the HTTP POST request
    client.print(F("POST /DummyServer_Blog/TestArduino/testserver.jsp"));
    client.print(F(" HTTP/1.1\r\n"));
    client.print(F("Cache-Control: no-cache\r\n"));
    client.print(F("Host: 192.168.0.9\r\n"));
    client.print(F("User-Agent: Arduino\r\n"));
    client.print(F("Content-Type: application/x-www-form-urlencoded\r\n"));
    client.print(F("Content-Length: "));
    client.println(postdata.length());
    client.println();
    client.println(postdata);
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
//////////////////
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
