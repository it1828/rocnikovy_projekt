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

int odchylka = 3;
const int pocetBarev = 3;
//Pole colors do ktereho se zapisou zmerene hodnoty RGB frekfenci jednotlivych barev
int colors[pocetBarev][3] = {{0, 0, 0},  
                             {0, 0, 0},
                             {0, 0, 0}};

int startStop = 0; // slouzi ke startu trizeni
int aktualniBarva = 1; 
bool layerChanged=false;
KeyType key = KeyNone;

void stopInterrrupt(){ // prerusovaci funkce pro STOP
  if (startStop == 1){
    delay(300);
    g_Menu.exit();
    startStop = 0;
    aktualniBarva = 1;
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
    key = KeyEnter;
    return;
}
void clickup(){
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
  servo2.write(50);

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

//---------------------------------------------------------------------------------------------//

int mereniZapisRGB(int akualniBarva){ //Funkce zmeri RGB a zapíše do pole
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

  unsigned long cas = 0;
  cas = millis();

  if (startStop == 0){
    //Pri stopu tridicky (zapis barvy do pole colors)
    delay(300);
    servo1.write(160);
    lcd.clear();
    do{
      redFrequency = pulseIn(sensorOut, LOW);
      greenFrequency = pulseIn(sensorOut, LOW);
      blueFrequency = pulseIn(sensorOut, LOW);
      int RGB[3] = {redFrequency, greenFrequency, blueFrequency};
      for (int i = 0; i <= pocetBarev; i++)
        colors[aktualniBarva - 1][i] = RGB[i];
      for (int i = 0; i < 3; i++)
        Serial.print(RGB[i]);
      Serial.println("");
    } while (millis() - cas < 2000);
    servo1.write(50);
  }
  else{
    //Pri startu tridicky (rozlisovani barev)
    redFrequency = pulseIn(sensorOut, LOW);
    greenFrequency = pulseIn(sensorOut, LOW);
    blueFrequency = pulseIn(sensorOut, LOW);
    int RGBdva[3] = {redFrequency, greenFrequency, blueFrequency};
    
    //Vypis RGB ze sensoru
    for (int i = 0; i < 3; i++)
      Serial.print(RGBdva[i]);
    Serial.println("");

  //Konrola prave meřene barvy (pole RGB) se zmerenymi barvy v poli colors
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

void vypisBarvu(){ //Funkce kontroluje a vypise barvu

  

  servo1.write(160); //prvni poloha serva pro čteni barvy
  unsigned long time = 0;
  time = millis();
  int help[pocetBarev] = {0, 0, 0}; // pomocne pole pro zaznamenavani vysledku mereni jednotlivych barev

  do{ //začátek měření; přičtení hodnoty na místo barvy v poli podle toho ktera barva se vyskytuje nejcasteji
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

  //Pokud namerene hodnoty se temer vůbec neschoduji - jina barva
  int minimum = 20;
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
  switch (result){
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

void doplnNulyDoPole(){ //Funkce doplni nuly do pole colors pro zapsani mene nez 3 barev
  for (int r = aktualniBarva; aktualniBarva <= pocetBarev; aktualniBarva++){
    for (int c = 0; c < 3; c++){
      colors[aktualniBarva-1][c] = 0;
    }
  }
  aktualniBarva = 1;
  return;
}
//Kontrolni vypis pole colors
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

//Funkce vyvolane stiskem tlacitek
void Pridat(){
  if(startStop == 0){
    delay(200);
    lcd.clear();
    Serial.println(aktualniBarva);
    mereniZapisRGB(aktualniBarva);
    if(aktualniBarva == 3){
      
      doplnNulyDoPole();
      startStop = 1;
      //aktualniBarva = 1;
    }
    aktualniBarva++;
  }
}
void Start(){
  delay(300);
  lcd.clear();
  doplnNulyDoPole();
 startStop = 1;
}

void Reset(){
  
}

void loop(){

  if (startStop == 0){ // Pri stopu
    int fid = 0;
    const char* info;
    //Definovani funkce tlacitka
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
    //Vypis menu na display
    fid = g_Menu.getInfo(info);
    printMenuEntry(info);
  //Pri stisku Enter spust funkci odpovidajiciho ID
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
  key = KeyNone; //Defaultni nastaveni tlacitka na None
  }
  else{//Start trizeni
    vypisPole();
    vypisBarvu();
    servo2.write(10);
  }
  button.tick(); //Funkce tick() kontroluje stav tlacitek
  button2.tick();
  button3.tick();
  delay(10);
}