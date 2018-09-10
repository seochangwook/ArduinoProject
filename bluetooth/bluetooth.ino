/* TX - 2
 * RX - 3
 */

#include<SoftwareSerial.h>

//블루투스 연동을 위한 소프트웨어 시리얼 구현//
SoftwareSerial BTSerial(3,2); //시리얼 정의 시 아두이노에 연결된 핀 번호와 크로스 되게 해준다.//
 
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  BTSerial.begin(9600); //블루투스 통신 시작//
}

void loop() {
  // put your main code here, to run repeatedly:
  if(BTSerial.available()){
    Serial.write(BTSerial.read());
  }

  if(Serial.available()){
    BTSerial.write(Serial.read());
  }
}
