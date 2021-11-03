#include <Arduino.h>
#include <Servo.h>
//Knihohovny pro LCD display
#include <Wire.h>

#include <LiquidCrystal_I2C.h>
#include "CMBMenu.hpp"

LiquidCrystal_I2C lcd(0x27, 20, 4);

#define S0 4
#define S1 5
#define S2 6
#define S3 7
#define sensorOut 8
#define but1 10
#define but2 2
#define but3 3

const char g_Start_pc[] PROGMEM = {"1. START"};
const char g_Reset_pc[] PROGMEM = {"2. Reset"};
const char g_Pridat_pc[] PROGMEM = {"3. Pridat barvu"};
const char g_PridatBarvu_pc[] PROGMEM = {"Vhodte predmet"};

enum MenuFID { //IDs
  Startid,
  Resetid,
  Pridatid,
  PridatBarvuId,
};


enum KeyType {
  KeyNone, 
  KeyUp,
  KeyDown,
  KeyEnter,
  KeyExit
};

CMBMenu<100> g_Menu;

Servo servo1;

int redFrequency;
int greenFrequency;
int blueFrequency;

int odchylka = 3;
const int pocetBarev = 3;
//Pole colors obsahuje zmerene hodnoty RGB frekfenci jednotlivych barev
int colors[pocetBarev][3] = {{0, 0, 0},  // neutral
                             {0, 0, 0},  //modra
                             {0, 0, 0}}; //hneda

int startStop = 0;
int menu = 0;
int aktualniBarva = 1;

void stopInterrrupt()
{
  
  if (startStop == 1){
    delay(300);
    g_Menu.exit();
    startStop = 0;
    aktualniBarva = 1;
  }
}

void printMenuEntry(const char* f_Info)
{
  Serial.println("print funkce");
  String info_s;
  MBHelper::stringFromPgm(f_Info, info_s);
  //lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("--------------------");
  lcd.setCursor(20/info_s.length()+1, 1);
  lcd.print(info_s);
   lcd.setCursor(0, 2);
  lcd.print("--------------------");
  lcd.setCursor(4, 3);

  lcd.print((char)127);
  lcd.print(" ");
  lcd.print((char)126);
  lcd.print("  ");
  lcd.print("Enter/Exit");
  
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
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("--------------------");
  lcd.setCursor(0,1);
  lcd.print("  Arduino Tridicka");
  lcd.setCursor(0,2);
  lcd.print("--------------------");
  delay(500);
  lcd.clear();

  g_Menu.addNode(0, g_Start_pc , Startid);
  g_Menu.addNode(0, g_Reset_pc , Resetid);
  g_Menu.addNode(0, g_Pridat_pc , Pridatid);
  g_Menu.addNode(1, g_PridatBarvu_pc , PridatBarvuId);

  const char* info;
  g_Menu.buildMenu(info);
  g_Menu.printMenu();
  printMenuEntry(info);

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

  if (startStop == 0){
    
    delay(300);
    servo1.write(160);
    lcd.clear();
    do{
      redFrequency = pulseIn(sensorOut, LOW);
      greenFrequency = pulseIn(sensorOut, LOW);
      blueFrequency = pulseIn(sensorOut, LOW);
      int RGB[3] = {redFrequency, greenFrequency, blueFrequency};
      for (int i = 0; i < pocetBarev; i++)
        colors[aktualniBarva - 1][i] = RGB[i];
      for (int i = 0; i < 3; i++)
        Serial.print(RGB[i]);
      Serial.println("");
    } while (millis() - cas < 2000);

    servo1.write(50);
  }
  else{
    redFrequency = pulseIn(sensorOut, LOW);
    greenFrequency = pulseIn(sensorOut, LOW);
    blueFrequency = pulseIn(sensorOut, LOW);
    int RGBdva[3] = {redFrequency, greenFrequency, blueFrequency};
    //Konrola prave meřene barvy (pole RGB) se zmerenymi barvy v poli colors
    
    for (int i = 0; i < 3; i++)
      Serial.print(RGBdva[i]);
    Serial.println("");


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
  int help[pocetBarev] = {0, 0, 0}; // pomocne pole pro zaznamenavani vysledku mereni jednotlivych barev

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
  } while (millis() - time < 2000); //mereni po dobu 2000 milisekund

  //vybrani nejvetsiho cisla (nejcastejsi barvy) v poli
  int result = 0;
  int i = 0;
  int max = 0;
  for (; i < pocetBarev; i++){
    if (help[i] > max){
      max = help[i];
      result = i;
    }
  }

  int minimum = 7;
  if(help[0] <= minimum && help[1] <= minimum && help[2] <= minimum)
      result = -1;

  //vypis pole help
  for (int i = 0; i < pocetBarev; i++){
    Serial.print(help[i]);
    Serial.print(" - ");
  }
  Serial.println("");

  lcd.clear();
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

  default:
    lcd.print("Jina barva");
  }
  servo1.write(50); //druha poloha serva1
  delay(2000);
  lcd.clear();
  return;
}

void doplnNulyDoPole(){
  for (int r = aktualniBarva; aktualniBarva < pocetBarev + 1; aktualniBarva++){
    for (int c = 0; c < 3; c++){
      colors[aktualniBarva - 1][c] = 0;
    }
  }
  aktualniBarva = 1;
  return;
}

void vypisPole(){
  for (int r = 0; r < pocetBarev; r++){
    for (int c = 0; c < 3; c++){
      Serial.print(colors[r][c]);
      Serial.print("-");
    }
    Serial.println("-");
  }
  Serial.println("---");
  delay(1000);
}


KeyType getKey(){
  int val1 = digitalRead(but1);
  int val2 = digitalRead(but2);
  int val3 = digitalRead(but3);
  KeyType key = KeyNone;

  if(val2 == HIGH){
    Serial.println("val222");
    delay(200);
    lcd.clear();
    key = KeyUp;

  }
    
  if(val1 == HIGH){
    Serial.println("val111");
    delay(200);
    lcd.clear();
    key = KeyEnter;
    
  }

  /*
  if(val3 == 1){
    Serial.println("val333");
     delay(200);
    lcd.clear();
    key = KeyEnter;
  }*/
  g_Menu.printMenu();
  return key;
}

void Pridat(){
  if(startStop == 0){
        delay(200);
    lcd.clear();
    Serial.println(aktualniBarva);
    if(aktualniBarva == 4){
      startStop = 1;
      doplnNulyDoPole();
      aktualniBarva = 1;
    }else{
     
      if(aktualniBarva == 3){
        startStop = 1;
        doplnNulyDoPole();
        
    }else{
      mereniZapisRGB(aktualniBarva);
      aktualniBarva++;
    }
     
  }
  }

 
}
void Start(){
  delay(300);
  lcd.clear();
 startStop = 1;
}

void Reset(){
  
}
void loop(){

  if (startStop == 0){
    int fid = 0;
    const char* info;
    bool layerChanged=false;

    KeyType key = getKey();
    switch(key) {
      case KeyExit:
        g_Menu.exit();
        break;
      case KeyEnter:
        g_Menu.enter(layerChanged);
        break;
      case KeyUp:
        g_Menu.right();
        break;
      case KeyDown:
        g_Menu.left();
        break;
      default:
        break;
    }

    fid = g_Menu.getInfo(info);
    printMenuEntry(info);
  
  if ((KeyEnter == key) && (!layerChanged)) {
    switch (fid) {
      case Startid:
        Start();
        break;
      case Resetid:
        Reset();
        break;
      case PridatBarvuId:
        Pridat();
        break;
      default:
        break;
    }
  }
  }
  else
  {
    vypisPole();
    vypisBarvu();
  }
}