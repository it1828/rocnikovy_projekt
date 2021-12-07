#include "setup.h"
//---------------------------------------------------------------------------------------------//


int mereniZapisRGB(int akualniBarva){ //Funkce zmeri RGB a zapíše do pole
  unsigned long cas = 0;
  cas = millis();

  if (startStop == 0){  //Pri zastavení tridicky (zapis barvy do pole colors)
    delay(300);
    servo1.write(160);
    lcd.clear();
    do{
      //Čtení červené barvy, pomocí nastavní pinů S2 a S3 na LOW
      digitalWrite(S2, LOW);
      digitalWrite(S3, LOW);
      //Zápis frekfence do proměnné redFrequency
      redFrequency = pulseIn(sensorOut, LOW);

      //Čtení zelené barvy, pomocí nastavní pinů S2 a S3 na HIGH
      digitalWrite(S2, HIGH);
      digitalWrite(S3, HIGH);
      //Zápis frekfence do proměnné greenFrequency
      greenFrequency = pulseIn(sensorOut, LOW);

      //Čtení zelené barvy, pomocí nastavní pinů S2 na LOW a S3 na HIGH
      digitalWrite(S2, LOW);
      digitalWrite(S3, HIGH);
      //Zápis frekfence do proměnné blueFrequency
      blueFrequency = pulseIn(sensorOut, LOW);

      int RGB[] = {redFrequency, greenFrequency, blueFrequency};

    
      for (int i = 0; i <= pocetBarev; i++)
        colors[aktualniBarva - 1][i] = RGB[i];
      
        for (int i = 0; i < 3; i++)
        Serial.print(RGB[i]);
      Serial.println("");
     
    } while (millis() - cas < 1500);
    delay(1000);
    servo1.write(50);
  }
  else{//Pri startu tridicky (rozlisovani barev)
   
      //Čtení červené barvy, pomocí nastavní pinů S2 a S3 na LOW
      digitalWrite(S2, LOW);
      digitalWrite(S3, LOW);
      //Zápis frekfence do proměnné redFrequency
      redFrequency = pulseIn(sensorOut, LOW);

      //Čtení zelené barvy, pomocí nastavní pinů S2 a S3 na HIGH
      digitalWrite(S2, HIGH);
      digitalWrite(S3, HIGH);
      //Zápis frekfence do proměnné greenFrequency
      greenFrequency = pulseIn(sensorOut, LOW);

      //Čtení zelené barvy, pomocí nastavní pinů S2 na LOW a S3 na HIGH
      digitalWrite(S2, LOW);
      digitalWrite(S3, HIGH);
      //Zápis frekfence do proměnné blueFrequency
      blueFrequency = pulseIn(sensorOut, LOW);

      int RGB[] = {redFrequency, greenFrequency, blueFrequency};
      //---------------------------------
  /*
    for (int i = 0; i < 3; i++)
      Serial.print(RGB[i]);
   Serial.println("");
*/
  int pom = 0; //pomocná proměnná pro kontrolu jednotlivych složek RGB
  //Konrola právě měřené barvy (pole RGB) se změřenými barvy v poli colors
   for(int r = 0;r<pocetBarev;r++){
      for (int c = 0; c < 3; c++){
        if (RGB[c] + (odchylka) >= colors[r][c] && RGB[c] - (odchylka) <= colors[r][c]){
          pom++;
          if(pom == 3)
            return r;
        }        
       /* else{
         r++;
        }  */       
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

  do{ //Začátek měření; přičtení hodnoty na místo barvy v poli podle toho ktera barva se vyskytuje nejcasteji
    int color;
    color = mereniZapisRGB(aktualniBarva);
    if (color == 0)
      help[0]++;
    if (color == 1)
      help[1]++;
    if (color == 2)
      help[2]++;
  } while (millis() - time < 2500); //mereni po dobu 2000 milisekund

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
  int minimum = 300;
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
    servo2.write(0);
    first++;
    break;

  case 1:
    servo2.write(23);
    second++;
    break;

  case 2:
    servo2.write(46);
    third++;
    break;

  default:
    servo2.write(70);
    others++;

  }
  String vypis = String(first) + " | " + String(second) + " | " + String(third) + " | " + String(others);
  String vypis2 = "1.  2.  3.  NOK";
  lcd.setCursor(0,0);
  lcd.print("--------------------");
  lcd.setCursor(3,1);
  lcd.print(vypis);
  lcd.setCursor(0,2);
  lcd.print("--------------------");
  lcd.setCursor(3,3);
  lcd.print(vypis2);

  servo1.write(50); //druha poloha serva1
  delay(2000);
  //lcd.clear();
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
  if(colors[0][0] != 0){
    delay(300);
    lcd.clear();
    doplnNulyDoPole();
    startStop = 1;
  }else{
    lcd.clear();  
    lcd.setCursor(0,1);
    lcd.print("Prazdne pole barev!");
    delay(2000);
    lcd.clear();  
    }
    
}

void Reset(){
  lcd.clear();
  first = 0;
  second = 0;
  third = 0;
  others = 0;
  aktualniBarva = 1;
  delay(750);
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
  if(clear){
    lcd.clear();
    clear = false;
      }
      servo2.write(70);
  }
  else{//Start trizeni
    vypisPole();
    vypisBarvu();
  }
  button.tick(); //Funkce tick() kontroluje stav tlacitek
  button2.tick();
  button3.tick();
  delay(10);
}