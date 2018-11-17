//Receive//
#define TEST_LED 8

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(TEST_LED, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available()){
    char receive_msg = Serial.read();
    Serial.println(receive_msg);
    
    if(receive_msg == 'A'){
      digitalWrite(TEST_LED, HIGH);
    } else if(receive_msg == 'B'){
      digitalWrite(TEST_LED, LOW);
    }
  }
}
