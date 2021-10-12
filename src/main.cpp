
//dodělat nejvyzší počet v poli, uděat větší přední kryt, vylepšit trychtýř, vyzkoušet display, přidat display do predniho krytu

#include <Arduino.h>
#include <Servo.h>
//Kniohovny pro LCD display
#include <Wire.h>
//#include <LCD.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

#define S0 4
#define S1 5
#define S2 6
#define S3 7
#define sensorOut 8
#define but1 10
#define but2 2
#define but3 3

Servo servo1;

int redFrequency;
int greenFrequency;
int blueFrequency;

int odchylka = 3;
const int pocetBarev = 4;
//Pole colors obsahuje zmerene hodnoty RGB frekfenci jednotlivych barev
int colors[pocetBarev][3] = {{60, 72, 57},  // neutral
                             {59, 67, 54},  //modra
                             {60, 69, 68},  //zelena
                             {49, 61, 62}}; //hneda

int startStop = 0;
int menu = 0;
int aktualniBarva = 1;

void stopInterrrupt()
{
  if (startStop == 1)
  {
    startStop = 0;
    menu = 3;
  }
}

void setup()
{
  //Nastavení i/o
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);
  pinMode(but1, INPUT);
  pinMode(but2, INPUT);
  servo1.attach(9);
  servo1.write(50);
  //Nastavení výstupní frekfence na doporučených 20%
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);

  lcd.begin();
  lcd.backlight();

  //Zapnutí sériového monitoru

  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(but3), stopInterrrupt, CHANGE); //preruseni pro stop
}

int mereniZapisRGB(int akualniBarva) //Funkce zmeri RGB a zapíše do pole
{
  //Čtení červené barvy, pomocí nastavní pinů S2 a S3 na LOW
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  //Zápis frekfence do proměnné redFrequency

  //Čtení zelené barvy, pomocí nastavní pinů S2 a S3 na HIGH
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  //Zápis frekfence do proměnné redFrequency

  //Čtení zelené barvy, pomocí nastavní pinů S2 na LOW a S3 na HIGH
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  //Zápis frekfence do proměnné redFrequency

  unsigned long cas = 0;
  cas = millis();

  if (startStop == 0)
  {
    delay(300);
    servo1.write(160);
    lcd.clear();
    lcd.print("   Cteni barvy ");
    lcd.print(akualniBarva);
    //delay(1000);

    do
    {
      redFrequency = pulseIn(sensorOut, LOW);
      greenFrequency = pulseIn(sensorOut, LOW);
      blueFrequency = pulseIn(sensorOut, LOW);
      int RGB[3] = {redFrequency, greenFrequency, blueFrequency};
      for (int i = 0; i < pocetBarev; i++)
        colors[aktualniBarva - 1][i] = RGB[i];
      for (int i = 0; i < 3; i++)
        Serial.print(RGB[i]);
      Serial.println("");
    } while (millis() - cas < 3000);

    servo1.write(50);
  }
  else
  {
    redFrequency = pulseIn(sensorOut, LOW);
    greenFrequency = pulseIn(sensorOut, LOW);
    blueFrequency = pulseIn(sensorOut, LOW);
    int RGBdva[3] = {redFrequency, greenFrequency, blueFrequency};
    //Konrola prave meřene barvy (pole RGB) se zmerenymi barvy v poli colors
    for (int i = 0; i < 3; i++)
      Serial.print(RGBdva[i]);
    Serial.println("");
  int pom = 0;
    int r;

   for(int r = 0;r<pocetBarev;r++){
      for (int c = 0; c < 3; c++){
        if (RGBdva[c] + (odchylka) >= colors[r][c] && RGBdva[c] - (odchylka-1) <= colors[r][c])
            return r;
        else
          r++;
    } 
   }
    return -1;
  }
}

void vypisBarvu()
{
  servo1.write(160); //prvni poloha serva pro čteni barvičky

  unsigned long time = 0;
  time = millis();
  int help[pocetBarev] = {0, 0, 0, 0}; // pomocne pole pro zaznamenavani vysledku mereni jednotlivych barev

  do
  { //začátek měření; přičtení hodnoty na místo barvy v poli podle toho ktera barva se vyskytuje nejcasteji
    int color;
    color = mereniZapisRGB(aktualniBarva);
    if (color == 0)
      help[0]++;
    if (color == 1)
      help[1]++;
    if (color == 2)
      help[2]++;
    if (color == 3)
      help[3]++;
  } while (millis() - time < 3000); //mereni po dobu 2000 milisekund

  //vybrani nejvetsiho cisla (nejcastejsi barvy) v poli
  int result = 0;
  int i = 0;
  int max = 0;
  for (; i < pocetBarev; i++)
  {/*
    if(help[i] < 15){
      result = -1;
      break;
    }
      */
    if (help[i] > max)
    {
      max = help[i];
      result = i;
    }
  }
  

  //vypis pole help
  for (int i = 0; i < pocetBarev; i++)
  {
    Serial.print(help[i]);
    Serial.print(" - ");
  }
  Serial.println("");

  //Podminka switch rozhoduje finalni barvu
  switch (result)
  {
  case 0:
    lcd.print("Barva 1");
    break;

  case 1:
    lcd.print("Barva 2");
    break;

  case 2:
    lcd.print("Barva 3");
    break;

  case 3:
    lcd.print("Barva 4");
    break;
  default:
    lcd.print("Nezadana barva");
  }
  servo1.write(50); //druha poloha serva1
  delay(2000);
  return;
}

void doplnNulyDoPole()
{
  for (int r = aktualniBarva; aktualniBarva < pocetBarev + 1; aktualniBarva++)
  {
    for (int c = 0; c < 3; c++)
    {
      colors[aktualniBarva - 1][c] = 0;
    }
  }
  aktualniBarva = 1;
  return;
}

void changeMenu()
{

  int val1 = digitalRead(but1);
  int val2 = digitalRead(but2);
  int val3 = digitalRead(but3);

  if (val2 == HIGH)
  { //posuv menu
    delay(300);
    menu++;
  }

  if (menu == 3)
    menu = 0;

  if (val1 == HIGH && menu == 0)
  { //Start
    startStop = 1;
    menu = 3;
  }
  if (val1 == HIGH && menu == 5)
  { //stisk prdani barvy
    mereniZapisRGB(aktualniBarva);
    aktualniBarva++;
    menu = 5;
  }

  if (val1 == HIGH && menu == 4)
  { //stisk prdani barvy
    mereniZapisRGB(aktualniBarva);
    aktualniBarva++;
    menu = 5;
  }

  if (val1 == HIGH && menu == 2)
  {
    delay(200);
    menu = 4;
  }

  if (val3 == HIGH && menu == 5)
  {
    delay(300);
    startStop = 1;
    doplnNulyDoPole();
  }

  if (aktualniBarva == 5)
  {
    startStop = 1;
    doplnNulyDoPole();
  }

  switch (menu)
  {
  case 0:
    lcd.print("        MENU");
    lcd.setCursor(0, 1);
    lcd.print(">START");
    lcd.setCursor(0, 2);
    lcd.print(" Reset");
    lcd.setCursor(0, 3);
    lcd.print(" Pridat barvy");
    delay(50);
    break;

  case 1:
    lcd.print("        MENU");
    lcd.setCursor(0, 1);
    lcd.print(" START");
    lcd.setCursor(0, 2);
    lcd.print(">Reset");
    lcd.setCursor(0, 3);
    lcd.print(" Pridat barvy");
    delay(50);
    break;

  case 2:
    lcd.print("        MENU");
    lcd.setCursor(0, 1);
    lcd.print(" START");
    lcd.setCursor(0, 2);
    lcd.print(" Reset");
    lcd.setCursor(0, 3);
    lcd.print(">Pridat barvy");
    delay(50);
    break;
  case 4:
    lcd.print("   Pridani barvy");
    lcd.setCursor(0, 2);
    lcd.print("Vhod objekt + ENTER");

    delay(50);
    break;

  case 5:
    lcd.print("  Pridani barvy ");
    lcd.print(aktualniBarva);
    lcd.setCursor(0, 2);
    lcd.print("ENTER - pridat");
    lcd.setCursor(0, 3);
    lcd.print("STOP - tridit");

    delay(70);
    break;

  default:
    break;
  }
  //lcd.clear();
}
void vypisPole()
{
  for (int r = 0; r < pocetBarev; r++)
  {
    for (int c = 0; c < 3; c++)
    {
      Serial.print(colors[r][c]);
      Serial.print("-");
    }
    Serial.println("-");
  }
  Serial.println("---");
  delay(1000);
}

void loop()
{

  if (startStop == 0)
    changeMenu();
  else
  {
    vypisPole();
    vypisBarvu();
  }

  lcd.clear();
}
