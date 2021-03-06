#include "DHT.h"
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <Arduino.h>
#include <Ds1302.h>


#define DHTPIN 7     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
#define RELE 9
#define BUTTON 6
#define ANALOG A0
#define TIME_A 8   //pulsacion corta
#define TIME_B 35  // pulsacion larga

const static char* WeekDays[] =
{
    "Lun", //1
    "Mar", //2
    "Mie", //3
    "Jue", //4
    "Vie", //5
    "Sab", //6
    "Dom"  //7
};

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
Ds1302 rtc(A3, A5, A4);

float sensorValue = 0;
int counter = 0;
int counter_b = 0;
bool button;

//configuraciones

int modo = 0;
int valorTemperatura = 0;
int tiempoEncendido = 0;
int tiempoApagado = 0;

// sensores

int h;
float t;

void setup() {
  lcd.begin(16, 2);
  dht.begin();
  pinMode(RELE, OUTPUT);
  pinMode(BUTTON, INPUT);
  digitalWrite(RELE, HIGH);
  modo = EEPROM.read(1);
  EEPROM.get(2, valorTemperatura);
  tiempoEncendido = EEPROM.read(5);
  tiempoApagado = EEPROM.read(6);

  Ds1302::DateTime now;
  rtc.getDateTime(&now);
  rtc.init();
  delay(2000);

  leer_temperatura();
}

void loop() {
  counter = counter + 100;
  delay(100); //100

  sensorValue = analogRead(ANALOG);
  button = digitalRead(BUTTON);
  lcd.setCursor(0, 1);
  lcd.print("M:");
  if(modo == 0)
  {
    lcd.print("OFF     ");
  }
  else if (modo == 1)
  {
    lcd.print("Arriba  ");
  }
  else if (modo == 2)
  {
    lcd.print("Abajo   ");
  }
  lcd.setCursor(10,1);
  lcd.print("V:");
  lcd.print(valorTemperatura);
  lcd.print("C ");
  

  if (button == 1)
  {
    while(1){
      delay(100); //100
      button = digitalRead(BUTTON);
      counter_b++;
      if (button == 0 || counter_b == TIME_B)
      {
        if (counter_b == TIME_B)
        {
          counter_b = 0;
          modo_menu();
        }
        else
        {
          counter_b = 0;
          modo_apagado();
        }
      }
    }
  }
  else
  {
    counter_b = 0;
  }

  if (counter >= 2100)
  {
    leer_temperatura();
    counter = 0;
  }

  while(modo == 1 && t > valorTemperatura)
  {
    button = digitalRead(BUTTON);
    lcd.setCursor(0, 1);
    lcd.print("!!!  ALARMA  !!!");
    delay(tiempoEncendido*1000);
    digitalWrite(RELE, LOW);
    delay(tiempoApagado*1000);
    digitalWrite(RELE, HIGH);
    leer_temperatura();
    if (button == 1)
    {
      while(1){
        delay(100); //100
        button = digitalRead(BUTTON);
        counter_b++;
        if (button == 0 || counter_b == TIME_B)
        {
          if (counter_b == TIME_B)
          {
            counter_b = 0;
            modo_menu();
          }
          else
          {
            counter_b = 0;
            modo_apagado();
          }
        }
      }
    }
  }
    while(modo == 2 && t < valorTemperatura)
  {
    button = digitalRead(BUTTON);
    lcd.setCursor(0, 1);
    lcd.print("!!!  ALARMA  !!!");
    delay(tiempoEncendido*1000);
    digitalWrite(RELE, LOW);
    delay(tiempoApagado*1000);
    digitalWrite(RELE, HIGH);
    leer_temperatura();
    if (button == 1)
    {
      while(1){
        delay(100); //100
        button = digitalRead(BUTTON);
        counter_b++;
        if (button == 0 || counter_b == TIME_B)
        {
          if (counter_b == TIME_B)
          {
            counter_b = 0;
            modo_menu();
          }
          else
          {
            counter_b = 0;
            modo_apagado();
          }
        }
      }
    }
  }
}

void leer_temperatura()
{
  
  Ds1302::DateTime now;
  rtc.getDateTime(&now);
  
  lcd.setCursor(0, 0);
  h = dht.readHumidity();
  t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    lcd.setCursor(0, 0);
    lcd.print("Falla de sensor");
    //Serial.println("Failed to read from DHT sensor!");
    return;
  }
  lcd.print("T:");
  lcd.print(t);
  lcd.print("C  ");
  lcd.setCursor(10, 0);
  lcd.print(now.hour);
  lcd.print(":");
  lcd.setCursor(13, 0);
  if (now.minute < 10) lcd.print("0");
  lcd.print(now.minute);

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.println(" ");
  if(now.hour >= 12 && now.dow >= 6)
  {
    lcd.clear();
    lcd.print("No se Trabaja");
    while(1)
    {
      Ds1302::DateTime now;
      rtc.getDateTime(&now);
      lcd.setCursor(0, 1);
      lcd.print(now.hour);
      lcd.print(":");
      lcd.setCursor(3, 1);
      if (now.minute < 10) lcd.print("0");
      lcd.print(now.minute);
      lcd.print(":");
      lcd.setCursor(6, 1);
      if (now.second < 10) lcd.print("0");
      lcd.print(now.second);
      lcd.setCursor(13, 1);
      lcd.print(WeekDays[now.dow - 1]);
      delay(1000);
      if(now.hour <= 12 && now.dow == 1)
      {
        lcd.clear();
        break;
      }
    }    
  }
}

void modo_menu()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Configuracion:");
  bool acceso = 0;
  while (1)
  {
    delay(100);
    sensorValue = analogRead(ANALOG);
    button = digitalRead(BUTTON);
    if (sensorValue >= 0 && sensorValue < 205)
    {
      lcd.setCursor(0, 1);
      lcd.print("Modo:");
      lcd.setCursor(5, 1);
      if (modo == 0) {
        lcd.print("Apagado    ");
      }
      else if (modo == 1) {
        lcd.print("Superior   ");
      }
      else if (modo == 2) {
        lcd.print("inferior   ");
      }
      if (acceso == 1)
      {
        acceso = 0;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Modo de alarma:");
        while (1)
        {
          sensorValue = analogRead(ANALOG);
          button = digitalRead(BUTTON);
          delay(100);
          if (sensorValue >= 0 && sensorValue < 341)
          {
            lcd.setCursor(0, 1);
            lcd.print("Apagado         ");
            modo = 0;
          }
          else if (sensorValue >= 342 && sensorValue < 682)
          {
            lcd.setCursor(0, 1);
            lcd.print("Superior a valor");
            modo = 1;
          }
          else
          {
            lcd.setCursor(0, 1);
            lcd.print("Inferior a valor");
            modo = 2;
          }
          if (acceso == 1)
          {
            acceso = 0;
            lcd.clear();
            EEPROM.write(1, modo);
            return;
          }

          // Boton
          if (button == 1)
          {
            counter_b++;
            if (counter_b == TIME_A)
            {
              counter_b = 0;
              acceso = 1;
            }
          }
          else
          {
            counter_b = 0;
          }
        }
      }
    }
    else if (sensorValue > 206 && sensorValue < 410)
    {
      lcd.setCursor(0, 1);
      lcd.print("Valor:");
      lcd.setCursor(6, 1);
      lcd.print(valorTemperatura);
      lcd.print("C       ");
      if (acceso == 1)
      {
        acceso = 0;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Valor:");
        while (1)
        {
          sensorValue = analogRead(ANALOG);
          button = digitalRead(BUTTON);
          delay(100);
          lcd.setCursor(0, 1);
          valorTemperatura = map(sensorValue, 0, 1022, -10, 40);
          lcd.print("          ");
          lcd.setCursor(0, 1);
          lcd.print(valorTemperatura);
          lcd.print(" C           ");
          if (acceso == 1)
          {
            acceso = 0;
            lcd.clear();
            EEPROM.put(2, valorTemperatura);
            return;
          }

          // Boton
          if (button == 1)
          {
            counter_b++;
            if (counter_b == TIME_A)
            {
              counter_b = 0;
              acceso = 1;
            }
          }
          else
          {
            counter_b = 0;
          }
        }
      }
    }
    else if (sensorValue > 411 && sensorValue < 615)
    {
      lcd.setCursor(0, 1);
      lcd.print("T.Encendido:");
      lcd.setCursor(12, 1);
      lcd.print(tiempoEncendido);
      lcd.print("s   ");
      if (acceso == 1)
      {
        acceso = 0;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("T.Encendido:");
        while (1)
        {
          sensorValue = analogRead(ANALOG);
          button = digitalRead(BUTTON);
          delay(100);
          lcd.setCursor(0, 1);
          tiempoEncendido = map(sensorValue, 0, 1022, 0, 10);
          lcd.print("          ");
          lcd.setCursor(0, 1);
          lcd.print(tiempoEncendido);
          lcd.print(" s          ");
          if (acceso == 1)
          {
            acceso = 0;
            lcd.clear();
            EEPROM.write(5, tiempoEncendido);
            return;
          }

          // Boton
          if (button == 1)
          {
            counter_b++;
            if (counter_b == TIME_A)
            {
              counter_b = 0;
              acceso = 1;
            }
          }
          else
          {
            counter_b = 0;
          }
        }
      }
    }
    else if (sensorValue > 616 && sensorValue < 820)
    {
      lcd.setCursor(0, 1);
      lcd.print("T.Apagado:");
      lcd.setCursor(10, 1);
      lcd.print(tiempoApagado);
      lcd.print("s    ");
      if (acceso == 1)
      {
        acceso = 0;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("T.Apagado:");
        while (1)
        {
          sensorValue = analogRead(ANALOG);
          button = digitalRead(BUTTON);
          delay(100);
          lcd.setCursor(0, 1);
          tiempoApagado = map(sensorValue, 0, 1022, 1, 10);
          lcd.print("          ");
          lcd.setCursor(0, 1);
          lcd.print(tiempoApagado);
          lcd.print(" s        ");
          if (acceso == 1)
          {
            acceso = 0;
            lcd.clear();
            EEPROM.write(6, tiempoApagado);
            return;
          }

          // Boton
          if (button == 1)
          {
            counter_b++;
            if (counter_b == TIME_A)
            {
              counter_b = 0;
              acceso = 1;
            }
          }
          else
          {
            counter_b = 0;
          }
        }
      }
    }
    else if (sensorValue > 821 && sensorValue <= 1025)
    {
      lcd.setCursor(0, 1);
      lcd.print("Salir           ");
      if (acceso == 1)
      {
        acceso = 0;
        lcd.clear();
        return;
      }
    }

    // Boton
    if (button == 1)
    {
      counter_b++;
      if (counter_b == TIME_A)
      {
        counter_b = 0;
        acceso = 1;
      }
    }
    else
    {
      counter_b = 0;
    }
  }
}

void modo_apagado()
{
    while(1)
  {
    counter = counter + 100;
    delay(100);//100
    sensorValue = analogRead(ANALOG);
    button = digitalRead(BUTTON);
    lcd.setCursor(0, 1);
    lcd.print("!!! Apagado !!!");
    if (button == 1)
    {
      while(1){
        button = digitalRead(BUTTON);
        if (button == 0){loop();}
      }
    }
    if (counter >= 2100)
    {
      leer_temperatura();
      counter = 0;
    }
  }
}
