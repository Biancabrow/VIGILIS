// C++ code
//
/*
  The circuit:
  * LCD RS pin to digital pin GPIO 23
  * LCD Enable pin to digital pin GPIO 22
  * LCD D4 pin to digital pin GPIO 21
  * LCD D5 pin to digital pin GPIO 19
  * LCD D6 pin to digital pin GPIO 18
  * LCD D7 pin to digital pin GPIO 5
  * LCD R/W pin to ground
  * LCD VSS pin to ground
  * LCD VCC pin to 5V
  * 10K resistor:
  * ends to +5V and ground
  * wiper to LCD VO pin (pin 3)
  http://www.arduino.cc/en/Tutorial/LiquidCrystal
*/

//ESP32 Wifi and UTC
#include <WiFi.h>
#define NTP_SERVER     "pool.ntp.org"
#define UTC_OFFSET     -10800
#define UTC_OFFSET_DST 0

//ESP32 Pin definitions
#include <LiquidCrystal.h>
#define interruptorAlarme		13
#define ledVermelho         12
#define ledVerde	         	14
#define ledAzul	         		27
#define piezoPin         		15
#define trigPin          		4
#define echoPin 	         	0
long duration;
long distance;
int distanciaI;
int seconds = 5;
bool alarmeAtivo = true;
LiquidCrystal lcd_1(23, 22, 21, 19, 18, 5);

void tocaAlarme() {
  ledFunc();
  while(true){
    printLocalTime();
    lerDistancia();
  	if(digitalRead(interruptorAlarme) == HIGH && abs(distanciaI-distance)>=15) {
      delay(50);
      if (digitalRead(interruptorAlarme) == HIGH && abs(distanciaI-distance)>=15) {
        noTone(piezoPin);
        alarmeAtivo = false;
        lcd_1.setCursor(0,1);
        lcd_1.print("Alarme desligado");
        return;
      }
    }
  tone(piezoPin, 500);
  delay(50);
  noTone(piezoPin);
  delay(100);
  }
}

void lerDistancia(){
  digitalWrite(trigPin,HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin,LOW);
  delayMicroseconds(2);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034/2;
}  

void ledFunc(){
  for(int i = 0; i<=255;i+=5){
    analogWrite(ledVermelho, 255-i);
    analogWrite(ledVerde, 255-i);
    analogWrite(ledAzul, 255-i);
    delay(100);
  }
}

void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    lcd_1.setCursor(0, 1);
    lcd_1.println("Connection Error");
    return;
  }
  lcd_1.setCursor(0, 0);
  lcd_1.println("Agora: ");
  lcd_1.setCursor(8, 0);
  lcd_1.println(&timeinfo, "%H:%M:%S");
}

void setup()
{
  //Wifi
  lcd_1.setCursor(0, 0);
  lcd_1.print("Conectando:     ");
  lcd_1.setCursor(0, 1);
  WiFi.begin("Wokwi-GUEST", "", 6);
  while (WiFi.status() != WL_CONNECTED){
    delay(250);
  }
  lcd_1.clear();
  lcd_1.setCursor(0, 0);
  lcd_1.print("Online");
  lcd_1.setCursor(0, 1);
  lcd_1.print("Atualizando...");
  configTime(UTC_OFFSET, UTC_OFFSET_DST, NTP_SERVER);
  delay(3000);
  lcd_1.begin(16, 2);
  printLocalTime();
  pinMode(ledVermelho,OUTPUT);
  pinMode(ledVerde,OUTPUT);
  pinMode(ledAzul,OUTPUT);
  pinMode(piezoPin,OUTPUT);
  pinMode(trigPin,OUTPUT);
  pinMode(echoPin,INPUT);
  lerDistancia();
  distanciaI = distance;
}

void loop()
{
  delay(3000);
  for(int i=0;i<=600000;i+=30000){
  printLocalTime();
    if(seconds > 0){
      lcd_1.setCursor(0,1);
      lcd_1.print("                ");
      lcd_1.setCursor(0,1);
      lcd_1.print("Alarme: ");
      lcd_1.print(seconds);
      delay(1000);
      seconds--;
    }
    if(seconds==0 && alarmeAtivo == true){
      lcd_1.setCursor(0,1);
      lcd_1.print("Alarme tocando! ");
    }
    if(seconds <= 0 && alarmeAtivo == true){
      tocaAlarme();
    }
    if(alarmeAtivo == false){
      delay(30000);
      lerDistancia();
      if(abs(distanciaI-distance)>15){
        seconds = 5;
        alarmeAtivo = true;
      }
    }
  }
}