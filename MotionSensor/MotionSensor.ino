#define PIR_INPUT_PIN 7
#define LED 12
#define BAUD_RATE 9600

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
///////////////////
PassiveInfraredSensor pir(PIR_INPUT_PIN);
///////////////////
void setup() {
  // put your setup code here, to run once:
  Serial.begin(BAUD_RATE);

  pinMode(LED, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(pir.motion_detected())
  {
    Serial.println("1");

    digitalWrite(LED, HIGH);
    delay(200);
    digitalWrite(LED, LOW);
    delay(200);
  }

  else
  {
    Serial.println("0");

    digitalWrite(LED, LOW);
  }

  delay(1000);
}
