#include<Servo.h>

//사용될 LED정의//
const unsigned int LED_1_PIN = 13;
const unsigned int LED_2_PIN = 12;
const unsigned int LED_3_PIN = 11;
const unsigned int LED_4_PIN = 10;
const unsigned int LED_5_PIN = 9;

//Array관련 변수//
int LED_ARRAY[] = {LED_1_PIN,
                   LED_2_PIN,
                   LED_3_PIN,
                   LED_4_PIN,
                   LED_5_PIN
}; 
int array_size = sizeof(LED_ARRAY) / sizeof(int);

//가변저항(on/off switch역할)//
const unsigned int SWITCH = A0;

void setup() {
  Serial.begin(9600);
  
  // put your setup code here, to run once:
  pinMode(LED_1_PIN, OUTPUT);
  pinMode(LED_2_PIN, OUTPUT);
  pinMode(LED_3_PIN, OUTPUT);
  pinMode(LED_4_PIN, OUTPUT);
  pinMode(LED_5_PIN, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  int switch_value = analogRead(SWITCH);
  int array_number = rand()%array_size; //LED선택 번호//

  //PhotentionMeter의 값의 유무에 따라 LED를 제어//
  if(switch_value >= 200)
  {
    digitalWrite(LED_ARRAY[array_number], HIGH);
    delay(500);

    digitalWrite(LED_ARRAY[array_number], LOW);
    delay(500);
  }

  else //일정수치 이하로 내려가면 모든 LED off//
  { 
    digitalWrite(LED_1_PIN, LOW);
    digitalWrite(LED_2_PIN, LOW);
    digitalWrite(LED_3_PIN, LOW);
    digitalWrite(LED_4_PIN, LOW);
    digitalWrite(LED_5_PIN, LOW);
  }
}
