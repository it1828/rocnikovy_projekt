#include <Arduino.h> //Arduino knihnova
#include <Servo.h> // Servo knihovna
#include <Wire.h>
#include <OneButton.h> // double click /long press button library
#include <LiquidCrystal_I2C.h> //Knihohovny pro LCD display
#include "CMBMenu.hpp" // knihovna pro multimenu


// nastaveni vsech potrebny pinu
#define S0 4
#define S1 5
#define S2 6
#define S3 7
#define sensorOut 8
#define but1 10
#define but2 2
#define but3 12 // prozatim nefunkcni tlacitko


const char g_Start_pc[] PROGMEM = {"1. START"};
const char g_Reset_pc[] PROGMEM = {"2. Reset"};
const char g_Pridat_pc[] PROGMEM = {"3. Pridat barvu"};
const char g_PridatBarvu_pc[] PROGMEM = {"Vhodte predmet"};

//Nastaveni datovych typu knohoven
LiquidCrystal_I2C lcd(0x27, 20, 4);
OneButton button(but3, false, false);
OneButton button2(but2, false, false);
OneButton button3(but1, false, false);
CMBMenu<100> g_Menu;
Servo servo1;
Servo servo2;

enum MenuFID { //IDs
  Startid,
  Resetid,
  Pridatid,
  PridatBarvuId,
};

enum KeyType { // "Funkce" tlacitek
  KeyNone, 
  KeyUp,
  KeyDown,
  KeyEnter,
  KeyExit
};

//Nastaveni promennych
int redFrequency;
int greenFrequency;
int blueFrequency;

int odchylka = 2;
const int pocetBarev = 3;
//Pole colors do ktereho se zapisou zmerene hodnoty RGB frekfenci jednotlivych barev
int colors[pocetBarev][3] = {{0, 0, 0},  
                             {0, 0, 0},
                             {0, 0, 0}};

int startStop = 0; // slouzi ke startu trizeni
int aktualniBarva = 1; 
bool layerChanged=false;
KeyType key = KeyNone;

int first = 0;
int second = 0;
int third = 0;
int others = 0;
bool clear = false;


void stopInterrrupt(){ // prerusovaci funkce pro STOP
  if (startStop == 1){
    delay(300);
    g_Menu.exit();
    startStop = 0;
    aktualniBarva = 1;
    clear = true;
  }
}

void printMenuEntry(const char* f_Info) {// "sablona" pro vypis menu
  String info_s;
  MBHelper::stringFromPgm(f_Info, info_s);
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

//Jednoduche funkce pro osetreni tlacitek
void doubleClick(){
    startStop = 1;
    return;
}

void click(){
  lcd.clear();
    key = KeyEnter;
    return;
} 
void clickup(){
  lcd.clear();
    key = KeyUp;
    return;
}
void longpress(){
    key = KeyExit;
    return;
}

void clickdown(){
    lcd.clear();
    key = KeyDown;
    return;
}



void setup(){
  //Nastavení i/o
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);
  pinMode(but1, INPUT);
  pinMode(but2, INPUT);
  pinMode(but3, INPUT);
  //Nastaveni serv
  servo1.attach(9); 
  servo1.write(50);

  servo2.attach(11);
  servo2.write(70);

  //Nastavení výstupní frekfence na doporučených 20%
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);

  //Vypis pri zapinani tridicky
  lcd.begin();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("--------------------");
  lcd.setCursor(0,1);
  lcd.print("  Arduino Tridicka");
  lcd.setCursor(0,2);
  lcd.print("--------------------");
  delay(1500);
  lcd.clear();

  //Nastaveni urovni menu (prvni parametr) a ID
  g_Menu.addNode(0, g_Start_pc , Startid);
  g_Menu.addNode(0, g_Reset_pc , Resetid);
  g_Menu.addNode(0, g_Pridat_pc , Pridatid);
  g_Menu.addNode(1, g_PridatBarvu_pc , PridatBarvuId);
  
  //Vypis menu
  const char* info;
  g_Menu.buildMenu(info);
  g_Menu.printMenu();
  printMenuEntry(info);

  //Z knihovny OneButton.h pro osetreni tlacitek
  button.attachDoubleClick(doubleClick);
  button.attachClick(click);
  button.attachLongPressStart(longpress);
  button2.attachClick(clickup);
  button3.attachClick(clickdown);

  attachInterrupt(digitalPinToInterrupt(but2), stopInterrrupt, CHANGE); //preruseni pro STOP
  Serial.begin(9600); //Zapnutí sériového monitoru
}
