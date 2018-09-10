/* TX - 2
 * RX - 3
 */

#include<SoftwareSerial.h>

//블루투스 연동을 위한 소프트웨어 시리얼 구현//
SoftwareSerial BTSerial(3,2); //시리얼 정의 시 아두이노에 연결된 핀 번호와 크로스 되게 해준다.//

//LED 및 센서 정의//
const unsigned int LED_GREEN = 7;
const unsigned int POTENTIAL = A0;

int sensor_value = 0;
 
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  BTSerial.begin(9600); //블루투스 통신 시작//

  pinMode(LED_GREEN, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  sensor_value = analogRead(POTENTIAL);

  if(BTSerial.available()){
    //Serial.write(BTSerial.read());

    int value = BTSerial.read() - '0';

    if(value == 1){
      digitalWrite(LED_GREEN, HIGH);
    } else if(value == 0){
      digitalWrite(LED_GREEN, LOW);
    }
  }

  if(sensor_value > 200){
    BTSerial.write('a');
  }

  delay(1000);
}
