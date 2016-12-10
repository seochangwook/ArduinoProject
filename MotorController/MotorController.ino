#include <SPI.h>
#include <WiFi.h>
#include <Servo.h>

#define TEST_LED 12

char ssid[] = "Family"; //연결할 와이파이 이름//
char pass[] = "ehd35797"; //와이파이의 비밀번호/
int status = WL_IDLE_STATUS;

//입력변수//
String rcvbuf;
boolean getIsConnected = false;

unsigned long lastConnectionTime = 0;         // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 5000L; // delay between updates, in milliseconds

//서버의 정보//
IPAddress hostIp(192, 168, 0, 11);
// Initialize the Ethernet client object
WiFiClient client;// Initialize the Ethernet client object

//서보모터/
Servo motor;

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

  pinMode(TEST_LED, OUTPUT);
  motor.attach(9);
   
  // 연결에 성공했으면 연결확인 메시지와 연결된 네트워크 정보를 띄운다. 
  Serial.println("You're connected to the network");
  Serial.println("-------<network info>-------");
  
  printCurrentNet();
  printWifiData();

  Serial.println("-------<data trans>-------");
}

void loop() 
{
  //모터의 상황을 체크한다.//
  httpRequest_Boiler("boiler");
  
  Serial.println("Boiler JSON data");

  while (client.available() && status == WL_CONNECTED) {
    char c = client.read();

    //Serial.write(c);

    if(c == '1' || c == '0') //현재 전송되는 문자들에서 숫자들만 파싱//
    {
      rcvbuf += c;
    }

    if(c == '}') //JSON구조가 종료되는 부분/
    {
      Serial.println(rcvbuf);

      //숫자들의 배열에서 해당 제어값에 위치한 숫자를 파싱한다(배열의 마지막 부분)//
      int str_size = rcvbuf.length();

      Serial.print("str size: ");
      Serial.print(str_size);
      Serial.print("/");
      Serial.println(rcvbuf[str_size-1]);

      //파싱된 제어숫자를 가지고 on/off제어//
      if(rcvbuf[str_size-1] == '0')
      {
        Serial.println("motor off state");

        motor.write(10);
      }

      else if(rcvbuf[str_size-1] == '1')
      {
        Serial.println("motor on state");

        motor.write(160);
      }
      
      rcvbuf = ""; //초기화//

      break;
    }
  }
}
//////////////////
void httpRequest_Boiler(String motor_name) {
  Serial.println();

  // close any connection before send a new request
  // this will free the socket on the WiFi shield
  client.flush();
  client.stop();

  delay(1000);
  
  // if there's a successful connection
  if (client.connect(hostIp, 3000)) {
    Serial.println("Connecting...");

    //post data set//
    String postdata = "";
    
    String key_1 = "motor_name=";

    //데이터 유무에 따라 값을 넣어주거나 넣지 않는다.//
    postdata.concat(key_1);
    postdata.concat(motor_name);

    Serial.print("post data [");
    Serial.print(postdata);
    Serial.println("]");

    // send the HTTP POST request
    client.print(F("POST /service/boiler_status_get"));
    client.print(F(" HTTP/1.1\r\n"));
    client.print(F("Cache-Control: no-cache\r\n"));
    client.print(F("Host: 192.168.0.11\r\n"));
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

    digitalWrite(TEST_LED, HIGH);
    delay(1000);
    digitalWrite(TEST_LED, LOW);
    delay(1000);
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
