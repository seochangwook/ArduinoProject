#define BAUD_RATE 9600

const unsigned int LDR_SENSOR_1 = A0;
const unsigned int LDR_SENSOR_2 = A1;

int ldr_sensor1_value = 0;
int ldr_sensor2_value = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(BAUD_RATE);
}

void loop() {
  // put your main code here, to run repeatedly:
  ldr_sensor1_value = map(analogRead(LDR_SENSOR_1), 0, 1023, 0, 255); //mapping 수행(0,1023) -> (0, 255)//
  ldr_sensor2_value = map(analogRead(LDR_SENSOR_2), 0, 1023, 0, 255);
   
  Serial.println(ldr_sensor1_value);
  Serial.println(ldr_sensor2_value);

  delay(1000);
}
