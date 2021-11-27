# Rocnikovy projekt
# Barevná třídička

Třídička bude rozlišovat zakladní barvy. Výpis aktuální a počet v jednotrlivých barvách bude zobrazen na displayi. 
Pomocí tří talčitek bude možno scrolovat v menu, zapinat a vypinat trizeni.
V menu bude funkce, která bude uožnovat pridani vlastnich barev(RGB).

Protože barevný sensor je citlivý na světlo musel jsem vybrat jako vývojovou desku Aruino, protože umožnuje výstup 5V což je dostačující pro nasvícení daného obejktu. ESP8266 umožnuje pouze 3V, při který ledky na snímači nesvicí dostatečně.



**Využitý hardware**

    Barevný sensor - TCS3200

    LCD Display 20x4 s I2C převodníkem

    Arduino Uno

    Micro servo SG90 2x


**VS Code s PlatformIO**

![0](https://user-images.githubusercontent.com/46921138/132726285-e1a74359-74cd-4ade-a001-82f35b558eb8.PNG)



![image](https://user-images.githubusercontent.com/46921138/132726252-2c386ec9-8e58-42e5-b147-92cae0655a90.png)

**Vypis na display**


![image](https://user-images.githubusercontent.com/46921138/132727269-ed70d03b-1296-4654-b436-a610cd1cceb9.png)

**Přidání skluzavky a vytvoření základního menu na display ovladaného pomocí tlačítek**


![image](https://user-images.githubusercontent.com/46921138/136070701-7533c755-00e1-41c4-8492-8c618ffacbd7.png)


Tridička ted dokáže přidávat dynamicky barvy do pole, ale největší problém je, že sensorn není dostatečně citlivý pro rozlišení všech barev, proto funguje spolehlivěji na více odlišných barvách

Jako další krok bude napájení tří tlačítek na plošný spoj a zabudování do předního krytu třídičky, to platí také pro display, který bude na předním krytu.

**Prozatimní schéma**

![image](https://user-images.githubusercontent.com/46921138/136985554-44aac8ef-4c2b-46b6-ae5e-bbe128b27295.png)

**Schéma na tlačítkový modul**

![image](https://user-images.githubusercontent.com/46921138/143688075-ba766b70-ccea-4caf-97b1-a8dc0d98b70c.png)


**Update krabičky o vestavěný display**

![ddd](https://user-images.githubusercontent.com/46921138/137468195-80edbd1b-2200-4eb3-b5c1-1b592651908e.jpg)

Menu reference: 

https://github.com/MajicDesigns/MD_Menu/blob/main/examples/Menu_LCD-Shield/Menu_LCD-Shield.ino
https://sudonull.com/post/98657-Multilevel-menu-for-Arduino-and-not-only

**Počitadlo**

![image](https://user-images.githubusercontent.com/46921138/141691790-5f0083a7-949d-4b61-ba48-ae81a32a9c56.png)


**Konečné schéma v programu KiCad**
![image](https://user-images.githubusercontent.com/46921138/143688105-7e740e57-aed2-4f39-903c-35d74ecb7f92.png)


