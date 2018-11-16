//Receive//
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(13, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available()){
    char receive_msg = Serial.read();
    Serial.println(receive_msg);
    
    if(receive_msg == 'A'){
      digitalWrite(13, HIGH);
    } else if(receive_msg == 'B'){
      digitalWrite(13, LOW);
    }
  }
}
