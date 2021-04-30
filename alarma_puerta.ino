const int Sensor=2;
const int Alarma=12;
const int LED=13;
int counter;
int s;

void setup() {
  pinMode(LED, OUTPUT);
  pinMode(Sensor, INPUT);
  pinMode(Alarma, OUTPUT);
  counter=0;
}

void loop() {
  s=digitalRead(Sensor);
  if(s==LOW){
    digitalWrite(LED,HIGH);
    delay(500);
    digitalWrite(LED,LOW);
    delay(500);
    counter++;
    while(counter>=10 && s==LOW){
      digitalWrite(Alarma, HIGH);
      delay(1000);
      digitalWrite(Alarma, LOW);
      delay(3000);
      digitalRead(Sensor);
      s=digitalRead(Sensor);
    }
  }
  else{
    counter=0;
  }
}
