//Deze code gaat samen met de code voor het bakjessysteem, de 2 arduinos communiceren met elkaar
//door 2 pinnen die een PWM signaal krijgen of een Bool signaal
//De codes zijn opgedeeld in 2 arduinos zodat het als 2 losse delen beschouwd kan worden
//dit is makkelijker met debuggen en problemen oplossen
//De noodstop werkt tijdens de delays

#include <Servo.h> //Servo library toegevoegd

//Alle servo van dit systeem worden gedeclareerd:
Servo sorteerServoK; // de sorteerservo voor de kleine knikkers
Servo doseerServoK; // de doseerservo voor de kleine knikkers
Servo sorteerServoG; // de sorteerservo voor de grote knikkers
Servo doseerServoG; // de doseerservo voor de grote knikkers
Servo stopper_1K; //stop servo 1 voor kleine knikkers
Servo stopper_2K; //stop servo 2 voor kleine knikkers
Servo knikkerServoG; //detectie servo wiel voor Grote knikkers

// Alle pinnen worden in variabelen gezet:
const uint8_t knikkerServoGPin = 11; //servo pin
const uint8_t sorteerServoGPin = 10; //servo pin
const uint8_t doseerServoGPin = 9; //servo pin

const uint8_t stopper_1KPin = 8; //servo pin
const uint8_t stopper_2KPin = 7; //servo pin
const uint8_t sorteerServoKPin = 6; //servo pin
const uint8_t doseerServoKPin = 5; //servo pin
const uint8_t knikkerKleinFSRPin = A0; //druksensor pin
const uint8_t knikkerKleinMetaalPin = 33; //stroommeter pin

const uint8_t noodstopPin = 2; //Noodstop pin

const uint8_t S0 = 44;  //kleurensensor pin
const uint8_t S1 = 30;  //kleurensensor pin
const uint8_t S2 = 31;  //kleurensensor pin
const uint8_t S3 = 32;  //kleurensensor pin
const uint8_t sensorOut = 33;  //kleurensensor pin

const uint8_t comPinOut = 25; //de pin die wordt gebruikt om te communiceren naar de andere arduino (out)
const uint8_t comPinIn1 = 40; //de pin die wordt gebruikt om te communiceren naar de andere arduino (in)
const uint8_t comPinIn2 = 41; //de pin die wordt gebruikt om te communiceren naar de andere arduino (in)

// Alle globale variabele worden hier gedefineerd:
int glas = 0; //het aantal knikkers in de buffer
int metaal = 0;
int plastic = 0;
int hout = 0;
int bakType;
bool flag = 0; //flag om de waarde te dragen of er een bakje voor de machine staat of niet
uint8_t glas_aantal; //het aantal knikkers benodigd voor een bakje
uint8_t metaal_aantal;
uint8_t plastic_aantal;
uint8_t hout_aantal;
uint8_t ticker;
volatile bool noodstopPlaatsgevonden = 0;
int redPW = 0; // Variables for Color Pulse Width Measurements
int greenPW = 0;
int bluePW = 0;
int redValue; //variabeles voor knikkerdetectie
int greenValue;
int blueValue;

//De hoeveelheid knikkers per bakje:
const int aluminium[4] = {1 , 0, 0, 0}; //Glas,Metaal,Plastic,Hout
const int doorzichtig[4] = {0, 1, 0, 0}; //Glas,Metaal,Plastic,Hout
const int PVC[4] = {1, 1, 0, 0}; //Glas,Metaal,Plastic,Hout

//Callibratie Grote knikker systeem:
//Kleurensensor hout:
const int Hout_RedLaag = 40;
const int Hout_RedHoog = 70;
const int Hout_GreenLaag = 30;
const int Hout_GreenHoog = 60;
const int Hout_BlueLaag = 30;
const int Hout_BlueHoog = 50;
//Kleurensensor Plastic:
const int Plastic_RedLaag = 100;
const int Plastic_RedHoog = 130;
const int Plastic_GreenLaag = 110;
const int Plastic_GreenHoog = 140;
const int Plastic_BlueLaag = 100;
const int Plastic_BlueHoog = 130;
//Kleurensensor standaard:
const int redMin = 28; // Red minimum value
const int redMax = 178; // Red maximum value
const int greenMin = 32; // Green minimum value
const int greenMax = 199; // Green maximum value
const int blueMin = 28; // Blue minimum value
const int blueMax = 160; // Blue maximum value
//Servo variables:
const int knikkerServo_Begin = 18;
const int knikkerServo_Sensor = 80;
const int knikkerServo_Eind = 144;
const int sorteerServoGhoekPlastic = 69; //de buffer voor plastic
const int sorteerServoGhoekHout = 0; //de buffer voor hout
const int doseerServoGhoekHout = 180;
const int doseerServoGhoekPlastic = 0;
const int doseerServoGhoekMidden = 92;

//Callibratie kleine knikker systeem:
const int stopper_1Khoekopen = 105; //de eerste servo
const int stopper_1Khoekdicht = 180;
const int stopper_2Khoekopen = 105; //de tweede servo
const int stopper_2Khoekdicht = 180;
const int sorteerServoKhoekGlas = 102; //de buffer voor glazen
const int sorteerServoKhoekMetaal = 55; //de buffer voor metalen
const int doseerServoKhoekMetaal = 180;
const int doseerServoKhoekGlas = 0;
const int doseerServoKhoekMidden = 84;
const int FSR_grensWaarde = 5; //grenswaarde voor de FSR


void setup() {
  //De setup:
  Serial.begin(9600); // Serial Communication is starting with 9600 of baudrate speed

  //De interupt voor de noodstop:
  attachInterrupt (digitalPinToInterrupt (noodstopPin), noodstop, LOW); 
  
  //Alle pinmodes worden hier gedefineerd:
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);
  
  pinMode(noodstop, INPUT);
  pinMode(comPinOut, OUTPUT);
  pinMode(comPinIn1, INPUT);
  pinMode(comPinIn2, INPUT);
  pinMode(knikkerServoGPin, OUTPUT);
  pinMode(sorteerServoGPin, OUTPUT);
  pinMode(doseerServoGPin, OUTPUT);
  
  pinMode(stopper_1KPin, OUTPUT);
  pinMode(stopper_2KPin, OUTPUT);
  pinMode(sorteerServoKPin, OUTPUT);
  pinMode(doseerServoKPin, OUTPUT);  
  pinMode(knikkerKleinFSRPin, INPUT);
  pinMode(knikkerKleinMetaalPin, INPUT);
  
  //Alle Servos worden verbinden met de juiste pin:
  knikkerServoG.attach(knikkerServoGPin);
  sorteerServoG.attach(sorteerServoGPin);
  doseerServoG.attach(doseerServoGPin);
  
  stopper_1K.attach(stopper_1KPin);
  stopper_2K.attach(stopper_2KPin);
  sorteerServoK.attach(sorteerServoKPin);
  doseerServoK.attach(doseerServoKPin);
  
  // Set Frequency scaling to 20%
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);

  //Alle actuatoren worden naar de beginpositie gebracht:
  digitalWrite(comPinOut,LOW); //comm wordt naar de beginpositie gebracht
  
  //alle servo's naar de beginpositie:
  knikkerServoG.write(knikkerServo_Begin);
  sorteerServoG.write(sorteerServoGhoekHout);
  
  stopper_1K.write(stopper_1Khoekdicht);
  stopper_2K.write(stopper_2Khoekdicht);
  sorteerServoK.write(sorteerServoKhoekGlas);
  
  Serial.println("Setup Complete.");
}

void loop() {
  //de main code:
  knikker_Groot(); //meten grote knikkers boven en sorteren
  knikker_Klein(); //meten kleine knikkers onder en sorteren
  flag = bakjes(); //meten bakjes gedeelte en registreren of er een bakje staat
  if (check() == HIGH && flag == HIGH ) { //checken of er genoeg knikkers in de buffer zitten en of er een bakje staat
    doseer(); //doseren knikkers
    digitalWrite(comPinOut,HIGH); //communiceren dat het bakje vol is
  }
} 

void noodstop(){
  //de noodstop functie die vast staat aan een interrupt pin:
  bool noodstopStatus = digitalRead(noodstopPin);
  if(noodstopStatus == LOW){
    noodstopPlaatsgevonden = 1; //variabele om te weten of er in het programma eventueel metingen opnieuw gedaan moeten worden
    Serial.println("Noodstop geactiveerd");
  }
  while(noodstopStatus == LOW){
    noodstopStatus = digitalRead(noodstopPin); //loop om de arduino te freezen
  }
  if(noodstopPlaatsgevonden == 1){
    //alle voorheen geactiveerde actuatoren weer aanzetten
    Serial.println("Noodstop gedeactiveerd");
  }
}

//de functie voor het sorteren van de grote knikkers:
void knikker_Groot() {
  // wiel draaien naar beginpositie en naar de sensor:
  Serial.println("meten grote knikkers");
  knikkerServoG.write(knikkerServo_Begin);
  delay(1000);
    for(int hoek = knikkerServo_Begin; hoek <= knikkerServo_Sensor; hoek++){ 
      knikkerServoG.write(hoek); //per stap van 1 graden wordt de servo aangestuurd
      delay(3.7);
    }
  delay(1000);
  
  //kleurensensor uitlezen:
  delay(100);// Delay to stabilize sensor
  redPW = getRedPW();  // Read Red value
  redValue = map(redPW, redMin, redMax, 255, 0);  // Map to value from 0-255
  delay(100); // Delay to stabilize sensor
  greenPW = getGreenPW();  // Read Green value
  greenValue = map(greenPW, greenMin, greenMax, 255, 0);  // Map to value from 0-255
  delay(100); // Delay to stabilize sensor
  bluePW = getBluePW();  // Read Blue value
  blueValue = map(bluePW, blueMin, blueMax, 255, 0);  // Map to value from 0-255

  // hout knikker:
  if (redValue > Hout_RedLaag && redValue < Hout_RedHoog && greenValue > Hout_GreenLaag && greenValue < Hout_GreenHoog && blueValue > Hout_BlueLaag && blueValue < Hout_BlueHoog) {
    Serial.println(" hout bij de grote knikkers");
    sorteerServoG.write(sorteerServoGhoekHout); //sturen naar de hout buffer
    for(int hoek = knikkerServo_Sensor; hoek <= knikkerServo_Eind; hoek++){ 
      knikkerServoG.write(hoek); //per stap van 1 graden wordt de servo aangestuurd
      delay(3.7);
    }
    hout++;
    delay(2000);
    for(int hoek = knikkerServo_Eind; hoek >= knikkerServo_Begin; hoek--){ 
      knikkerServoG.write(hoek); //per stap van 1 graden wordt de servo aangestuurd
      delay(3.7);
    }
  }
  
  // plastic knikker:
  if (redValue > Plastic_RedLaag && redValue < Plastic_RedHoog && greenValue > Plastic_GreenLaag && greenValue < Plastic_GreenHoog && blueValue > Plastic_BlueLaag && blueValue < Plastic_BlueHoog) {
    Serial.println(" plastic bij de grote knikkers");
    sorteerServoG.write(sorteerServoGhoekPlastic); //sturen naar de plastic buffer
    for(int hoek = knikkerServo_Sensor; hoek <= knikkerServo_Eind; hoek++){ 
      knikkerServoG.write(hoek); //per stap van 1 graden wordt de servo aangestuurd
      delay(3.7);
    }
    plastic++;
    delay(2000);
    for(int hoek = knikkerServo_Eind; hoek >= knikkerServo_Begin; hoek--){ 
      knikkerServoG.write(hoek); //per stap van 1 graden wordt de servo aangestuurd
      delay(3.7);
    }
  }

  //geen knikker:
  else {
    Serial.println(" geen knikker bij de grote knikkers");
    for(int hoek = knikkerServo_Sensor; hoek >= knikkerServo_Begin; hoek--){ 
      knikkerServoG.write(hoek); //per stap van 1 graden wordt de servo aangestuurd
      delay(3.7);
    }
  }
  // Print output to Serial Monitor
  Serial.print("Red = ");
  Serial.print(redValue);
  Serial.print(" - Green = ");
  Serial.print(greenValue);
  Serial.print(" - Blue = ");
  Serial.println(blueValue);
}

// Function to read Red Pulse Widths bij de grote knikkers
int getRedPW() {
  // Set sensor to read Red only
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  // Define integer to represent Pulse Width
  int PW;
  // Read the output Pulse Width
  PW = pulseIn(sensorOut, LOW);
  // Return the value
  return PW;
}

// Function to read Green Pulse Widths bij de grote knikkers
int getGreenPW() {
  // Set sensor to read Green only
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  // Define integer to represent Pulse Width
  int PW;
  // Read the output Pulse Width
  PW = pulseIn(sensorOut, LOW);
  // Return the value
  return PW;
}

// Function to read Blue Pulse Widths bij de grote knikkers
int getBluePW() {
  // Set sensor to read Blue only
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  // Define integer to represent Pulse Width
  int PW;
  // Read the output Pulse Width
  PW = pulseIn(sensorOut, LOW);
  // Return the value
  return PW;
}

//de functie voor het sorteren van de kleine knikkers:
void knikker_Klein() {
  //Poortje openen en sluiten, om de knikkers te doseren:
  delay(500);
  Serial.println("meten kleine knikkers");
  stopper_2K.write(stopper_2Khoekdicht); //servo 2 sluiten
  delay(500);
  stopper_1K.write(stopper_1Khoekopen); //eerste poortje open
  delay(1000);
  stopper_1K.write(stopper_1Khoekdicht); //eerste poortje dicht
  delay(500);

  //het 2e poortje wordt geopend en er wordt gekeken welke knikker het is:
    bool sensor;
    int FSR;
    sorteerServoK.write(sorteerServoKhoekGlas);//standaard naar de galzen buffer sturen
    delay(200);
    stopper_2K.write(stopper_2Khoekopen); //servo 2 openen
    unsigned long Tijd1 = millis();
    unsigned long Tijd2;

    do{
      // loopje herhalen totdat een bepaalde tijd is bereikt
      sensor = digitalRead(knikkerKleinMetaalPin);
      FSR = analogRead(knikkerKleinFSRPin);
      Tijd2 = millis();
    }while(Tijd2-Tijd1 <= 3000 && sensor == LOW && FSR <= FSR_grensWaarde);

    if(sensor == LOW && FSR > FSR_grensWaarde){
      //als de sensor laag is was het een glazen knikker
      glas++;
      Serial.println("Glazen knikker gesorteerd bij de kleine knikkers");
    }
    else if(sensor == HIGH){
      //als de sensor hoog is was het een metalen knikker
      sorteerServoK.write(sorteerServoKhoekMetaal);//naar de metalen buffer sturen
      metaal++;
      Serial.println("Metalen knikker gesorteerd bij de kleine knikkers");      
    }
    else if(sensor == LOW && FSR <= FSR_grensWaarde){
      Serial.println("Geen knikker gesorteerd bij de kleine knikkers");
    }
  }

// kijken of er genoeg knikkers in de buffers zitten om de bakjes te vullen:
int check() {
  switch (bakType) { //glas,metaal,plastic.hout
    case 1:
      if (glas >= aluminium[0] && metaal >= aluminium[1] && plastic >= aluminium[2] && hout >= aluminium[3]) {
        return 1;
      }
      else {
        return 0;
      }
    case 2:
      if (glas >= doorzichtig[0] && metaal >= doorzichtig[1] && plastic >= doorzichtig[2] && hout >= doorzichtig[4]) {
        return 1;
      }
      else {
        return 0;
      }
    case 3:
      if (glas >= PVC[0] && metaal >= PVC[1] && plastic >= PVC[2] && hout >= PVC[3]) {
        return 1;
      }
      else {
        return 0;
      }
  }
}

//hier wordt gekeken en doorgevoerd welk bakje er voor de machine staat:
int bakjes() {
 bool soortBakje1 = digitalRead(comPinIn1); //lezen wat de andere arduino stuurt
 bool soortBakje2 = digitalRead(comPinIn2); //lezen wat de andere arduino stuurt
    if ( soortBakje1 == 1 && soortBakje2 == 1) {
      Serial.println(" Doorzichtig bakje !! ");
      bakType = 1;
      return 1; //terugsturen naar loop dat er een bakje is
    }
    else if (soortBakje1 == 1 && soortBakje2 == 0) {
      Serial.println(" PVC bakje !! ");
      bakType = 2;
      return 1; //terugsturen naar loop dat er een bakje is
    }
    else if ( soortBakje1 == 0 && soortBakje2 == 1) {
      Serial.println(" aluminium bakje !! ");
      bakType = 3;
      return 1; //terugsturen naar loop dat er een bakje is
    } 
    else if (soortBakje1 == 0 && soortBakje2 == 0){
      Serial.println(" Geen bakje ");
      digitalWrite(comPinOut,LOW); //communiceren dat er een nieuw bakje nodig is
      return 0; //terugsturen naar loop dat er geen bakje is
  }
}

//Hier worden alle knikkers naar het bakje gesorteerd:
void doseer() {
  switch (bakType) { //verwerken welke knikkers er in moeten vanuit de varibelen
    case 1:
      glas_aantal = aluminium[0];
      metaal_aantal = aluminium[1];
      plastic_aantal = aluminium[2];
      hout_aantal = aluminium[3];
      break;
    case 2:
      glas_aantal = doorzichtig[0];
      metaal_aantal = doorzichtig[1];
      plastic_aantal = doorzichtig[2];
      hout_aantal = doorzichtig[3];
      break;
    case 3:
      glas_aantal = PVC[0];
      metaal_aantal = PVC[1];
      plastic_aantal = PVC[2];
      hout_aantal = PVC[3];
      break;
  }
  //hier wordt per knikkersoort gedoseerd:
  Serial.println("Bakje aan het vullen");
  ticker = 1;
  knikkerCheck: //een goto om als er genoeg knikkers per soort gedoseerd zijn opnieuw door alle if statements te lopen
  //glazen knikkers:
  if (glas_aantal = !0 && ticker == 1) {
    doseerServoK.write(doseerServoKhoekGlas);
    delay(1000);
    doseerServoK.write(doseerServoKhoekMidden);
    delay(1000);
    glas_aantal--;
    glas--;
    if (glas_aantal == 0) {
      ticker = 2; //als er niet meer van deze soort knikkers nodig is wordt de volgende knikker gedaan
    }
    goto knikkerCheck;
  }
  //metalen knikkers:
  if (metaal_aantal = !0 && ticker == 2) {
    doseerServoK.write(doseerServoKhoekMetaal);
    delay(1000);
    doseerServoK.write(doseerServoKhoekMidden);
    delay(1000);
    metaal_aantal--;
    metaal--;
    if (metaal_aantal == 0)
      ticker = 3; //als er niet meer van deze soort knikkers nodig is wordt de volgende knikker gedaan
    goto knikkerCheck;
  }
  //plastic knikkers:
  if (plastic_aantal = !0 && ticker == 3) {
    doseerServoG.write(doseerServoGhoekPlastic);
    delay(1000);
    doseerServoG.write(doseerServoGhoekMidden);
    delay(1000);
    plastic_aantal--;
    plastic--;
    if (plastic_aantal == 0)
      ticker = 4; //als er niet meer van deze soort knikkers nodig is wordt de volgende knikker gedaan
    goto knikkerCheck;
  }
  //houten knikkers:
  if (hout_aantal = !0 && ticker == 4) {
    doseerServoG.write(doseerServoGhoekHout);
    delay(1000);
    doseerServoG.write(doseerServoGhoekMidden);
    hout_aantal--;
    hout--;
    if (hout_aantal == 0)
      ticker = 0; //als er niet meer van deze soort knikkers nodig is wordt de volgende knikker gedaan
    goto knikkerCheck;
  }
  delay(2000); //delay om de knikker genoeg tijd te geven om naar het bakje te rollen
  digitalWrite(comPinOut,HIGH); //communiceren met andere arduino dat het bakje vol is
  Serial.println("Bakje gevuld");
} 
//wh24 groep 3
