// PUTSCH, par Charli Dagenais Quesnel, Jérémy Havitov et Félix Poissant Baril
// Dans le cadre du cours EDM4640, "Capter et illuminer en 1D" - 25%

#include <Adafruit_NeoPixel.h>

#define ENCODER_A 2
#define ENCODER_B 3

#define LED_RED 6
#define LED_GREEN 9
#define LED_BLUE 11

#define NUMPIXELS      60

#define PersoP1 60

#define ENCODER_SW 8
#define limitP1 4
#define limitP2 4

int bn = NUMPIXELS - 1;
volatile boolean encoderMoved = false;
volatile byte encoderValue = 0;
volatile int relativeValue = 0;
int nbPerso = 0 ;
int nbPerso2 = NUMPIXELS - 1;
//PERSO1
byte rC[PersoP1];
byte gC[PersoP1];
byte bC[PersoP1];
int vie[PersoP1];
int attaque[PersoP1];
String  couleur[PersoP1];
int nbCases[PersoP1];

int positionC[PersoP1];

int pointCentral = 29;
byte r;
byte g;
byte b;
boolean estMort = false;
boolean bouton = false;
boolean changePlayer = false;
boolean stagePrepDone = false;
boolean stagePrepDone1 = false;

boolean stageFight1 = false;
boolean stageRestart = false;
byte h;
byte s;
byte v;
int switchState;
// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN            5

// How many NeoPixels are attached to the Arduino?

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {

  pixels.begin(); // This initializes the NeoPixel library.
  Serial.begin (57600);

  pinMode(ENCODER_A, INPUT);
  pinMode(ENCODER_B, INPUT);

  digitalWrite(ENCODER_A, HIGH); //turn pullup resistor on
  digitalWrite(ENCODER_B, HIGH); //turn pullup resistor on



  //call updateEncoder() when any high/low changed seen
  //on interrupt 0 (pin 2), or interrupt 1 (pin 3)
  attachInterrupt(0, updateEncoder, CHANGE);
  attachInterrupt(1, updateEncoder, CHANGE);


  pinMode(ENCODER_SW, INPUT);
  switchState = digitalRead(ENCODER_SW);
  //todo,initialise les led à mort
  h = 255;
  s = 255;
  v = 255;
  resetLed();

}
//===========================================DRAW================================================
void loop() {



  // For a set of NeoPixels the first NeoPixel is 0, second is 1, all the way up to the count of pixels minus one.
  int newSwitchState =  digitalRead(ENCODER_SW);
  if ( switchState != newSwitchState ) {
    switchState = newSwitchState;
    Serial.print("SW: ");
    Serial.println(switchState);
  }
  if ( encoderMoved ) {
    encoderMoved = false;
    //                Serial.print("Encoded: ");
    //                Serial.print(encoderValue,BIN);
    //                Serial.print(" Relative: ");
    //                Serial.print(relativeValue);
    h = relativeValue % 256;
    HSV2RGB();
    //  Serial.print("Hue: ");
    //  Serial.print(h);
    //    Serial.print(" R: ");
    //    Serial.println(r);
    //    Serial.print(" G: ");
    //    Serial.println(g);
    //    Serial.print(" B: ");
    //    Serial.println(b);
    //    Serial.println("  ");
    analogWrite(LED_RED, 255 - r);
    analogWrite(LED_GREEN, 255 - g);
    analogWrite(LED_BLUE, 255 - b);

  }

  stagePrep();
  stageFight();
  fight();
  showLed();
  millieu();

}


//======================DISPLAY===================================================



void showLed() {
  for (int i = 0; i <= NUMPIXELS; i++) {
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(positionC[i], pixels.Color(rC[i], gC[i], bC[i])); // Moderately bright green color.
    pixels.show(); // This sends the updated pixel color to the hardware.
  }
}



//========================reset================================

void resetLed() {
  nbPerso = 0;
  for (int i = 0; i <= NUMPIXELS; i++) {
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(positionC[i], pixels.Color(0, 0, 0)); // Moderately bright green color.
    millieu();
    pixels.show();
  }
}

void millieu() {
  pixels.setPixelColor(pointCentral, pixels.Color(200, 200, 200));
}
//************========================STAGEPREP==================================================================

void  stagePrep() {
  if ((stagePrepDone == false) && (stagePrepDone1 == false)) {
    if (changePlayer == false) {
      onClickPrep();
    } else if (changePlayer == true) {
      delay(100);

      onClickPrep2();

    }
  }
}


void onClickPrep() {
  //On assigne la première led à la position 0 après le bang du boutton
  if (nbPerso == 0) {
    if (switchState == 1) {
      //buton nous assure qu'il va pas prendre deux couleurs d'affiler
      bouton = true;
      nbPerso = nbPerso + 1;
      if (bouton == true) {
        //switchState est le recepteur
        constructionLed(r, g, b, 0);
        bouton = false;
        changePlayer = true;

      }
    }
  }
  // si on est au deuxième bouton, réassigne l'indexe de position et de réfférence en conséquence
  else if ((nbPerso <= limitP1) && (nbPerso > 0)) {

    if (switchState == 1) {
      bouton = true;
      if (bouton == true) {
        nbPerso = nbPerso + 1;

        //retablie les index incluant le 0 à la position 0
        constructionLed(r, g, b, nbPerso - 1);
        bouton = false;
        delay(200);

        changePlayer = true;
      }
    }
  }
  else if ((nbPerso >= limitP1)) {
    if (switchState == 1) {
      bouton = true;
      if (bouton == true) {
        nbPerso = nbPerso + 1;
        Serial.print("nbPerso::");
        Serial.println(nbPerso);
        bouton = false;
        delay(200);

        stagePrepDone = true;
      }
    }
  }
  else  if  (nbPerso == 6) {
    while ( (nbPerso < pointCentral) && ( nbPerso > limitP1)) {
      constructionLed(0, 0, 0, nbPerso);
    }
    changePlayer = true;
  }
}
//=======================PREP_PLAYER2===========================================================
void onClickPrep2() {

  //On assigne la première led à la position 0 après le bang du boutton
  if (nbPerso2 == ( NUMPIXELS - 1)) {
    if (switchState == 1) {
      //buton nous assure qu'il va pas prendre deux couleurs d'affiler
      bouton = true;
      nbPerso2 = nbPerso2 - 1;
      if (bouton == true) {
        //switchState est le recepteur
        constructionLed(r, g, b, NUMPIXELS - 1);

        bouton = false;
        changePlayer = false;
        Serial.print("nbPerso2:0:");
        Serial.println(NUMPIXELS);
      }
    }
  }
  // si on est au deuxième bouton, réassigne l'indexe de position et de réfférence en conséquence
  else if ((nbPerso2 >= ( NUMPIXELS - 1) - limitP2) && (nbPerso2 < ( NUMPIXELS - 1))) {

    if (switchState == 1) {
      bouton = true;
      if (bouton == true) {
        nbPerso2 = nbPerso2 - 1;
        //retablie les index incluant le 0 à la position 0
        constructionLed(r, g, b, (nbPerso2 + 1));
        bouton = false;
        Serial.print("nbPerso2:1:");
        Serial.println(nbPerso2);
        changePlayer = false;
        delay(200);


        //        3erial.print("nbPerso");
        //        Serial.println(nbPerso);
      }
    }


  }
  else if ((nbPerso2 <= NUMPIXELS - (limitP2 + 1))) {

    if (switchState == 1) {
      bouton = true;
      if (bouton == true) {
        nbPerso2 = nbPerso2 - 1;

        bouton = false;
        delay(100);


        changePlayer = false;

        stagePrepDone1 = true;
        Serial.print("stagePrepDone1::");


      }
    }
  }
  else  if  (nbPerso2 == NUMPIXELS - (nbPerso2 + 2)) {
    delay(100);
    while ( (nbPerso2 > pointCentral) && ( nbPerso2 > NUMPIXELS - limitP2)) {
      constructionLed(0, 0, 0, nbPerso2--);
    }

    changePlayer = false;
  }


}



//======================CONSTRUCTION_LED_PLAYER1===================================================
void constructionLed(byte r, byte g, byte b, int nbPerso) {

  positionC[nbPerso] = nbPerso;
  // BLEU
  if ((h <= 177 ) && (h  >= 137)) {

    rC[nbPerso] = 0;
    gC[nbPerso] = 0;
    bC[nbPerso] = 250;
    couleur[nbPerso] = "BLEU";
    vie[nbPerso] = 5 ;
    attaque[nbPerso] = 2;
    nbCases[nbPerso] = 2;

  }
  // CYAN
  else if ((h <= 136 ) && (h  >= 96)) {

    rC[nbPerso] = 0;
    gC[nbPerso] = 250;
    bC[nbPerso] = 100;
    couleur[nbPerso] = "CYAN";
    vie[nbPerso] = 4;
    attaque[nbPerso] = 1;
    nbCases[nbPerso] = 4;
  }
  // VERT
  else if ((h <= 95 ) && (h  >= 56)) {

    rC[nbPerso] = 0;
    gC[nbPerso] = 255;
    bC[nbPerso] = 0;
    couleur[nbPerso] = "VERT";
    vie[nbPerso] = 7;
    attaque[nbPerso] = 2;
    nbCases[nbPerso] = 1;


  }
  // JAUNE
  else if ((h <= 55 ) && (h  >= 16)) {

    rC[nbPerso] = 255;
    gC[nbPerso] = 255;
    bC[nbPerso] = 0;
    couleur[nbPerso] = "JAUNE";
    vie[nbPerso] = 12   ;
    attaque[nbPerso] = 1;
    nbCases[nbPerso] = 1;


  }
  // ROUGE
  else if ((h <= 15) || (h  >= 253)) {
    rC[nbPerso] = 255;
    gC[nbPerso] = 0;
    bC[nbPerso] = 0;
    couleur[nbPerso] = "ROUGE";
    vie[nbPerso] = 3;
    attaque[nbPerso] = 1;
    nbCases[nbPerso] = 3;


  }
  // MAUVE
  else if ((h <= 252 ) && (h  >= 178)) {
    rC[nbPerso] = 255;
    gC[nbPerso] = 0;
    bC[nbPerso] = 255;
    couleur[nbPerso] = "MAUVE";
    vie[nbPerso] = 4;
    attaque[nbPerso] = 5;
    nbCases[nbPerso] = 3;

  }
  //    positionC[nbPerso] = nbPerso; //    rC[nbPerso] = r; //    gC[nbPerso] = g;//    bC[nbPerso] = b;//assigne position et couleur par position
}


//==============================================stageFight======================================
/********************************************************************************************************/
//================================------------------------------------------------------==========
void  stageFight() {
  if ((stagePrepDone == true )) {

    attackP1();

  }
}

//======================MOVE_LED_PLAYER1===================================================

void attackP1() {

  if ((stagePrepDone == true) && (stageFight1 == false)) {

    for (int j = limitP1 ; j >= 0; j--) {
      for (int i = positionC[j]; i < pointCentral - (j); i++) {
        showLed();
        positionC[j] = i;
        positionC[j + 1] =    positionC[j] - 1;
        positionC[j + 2] =    positionC[j + 1] - 1;
        positionC[j + 3] =    positionC[j + 2] - 1;
        positionC[j + 4] =    positionC[j + 3] - 1;
        positionC[j + 5] =    positionC[j + 4] - 1;
      }
    }
    for (int j = ( NUMPIXELS - 1) - limitP2 ; j <= NUMPIXELS - 1; j++) {
      for (int i = positionC[j]; i > pointCentral + (( NUMPIXELS - 1) - (j)); i--) {
        showLed();
        positionC[j] = i;
        positionC[j - 1] =    positionC[j] + 1;
        positionC[j - 2] =    positionC[j - 1] + 1;
        positionC[j - 3] =    positionC[j - 2] + 1;
        positionC[j - 4] =    positionC[j - 3] + 1;
        positionC[j - 5] =    positionC[j - 4] + 1;
      }
    }
    stageFight1 = true;
  }

}
/**********************************************************************************/
//===  ===========================**FIGHT**=====================================
void fight() {
  if (stageFight1 == true) {
    for (int i = 0; i <= limitP1; i++) {

      comparaison(i);



      delay(100);
    }
  }
}
//===  ===========================**COMPARAISON**=====================================
void comparaison(int i) {
  if (( couleur[i] == "ROUGE") && (couleur[bn - i] == "ROUGE")) {
    if (estMort == false) {

      delay(100);
      attaque[bn - i] = attaque[bn - i] + attaque[bn - i] ;
      delay(100);
      attaque[i] = attaque[i] + attaque[i] ;
      vie[i] = vie[i] - attaque[bn - i] ;
      vie[(bn - i)] = vie[(bn - i)] - attaque[i] ;
      vie[i - 1] = vie[i - 1] - attaque[bn - i] ;
      vie[(bn - i) - 1] = vie[(bn - i) - 1] - attaque[i] ;
      vie[i - 2] = vie[i - 1] - attaque[bn - i] ;
      vie[(bn - i) - 2] = vie[(bn - i) - 2] - attaque[i] ;
      delay(100);
    }

    isDead(i);
    Serial.println("viei::");
    Serial.println(vie[i]);
    Serial.println("vieBi::");
    Serial.println(vie[(bn - i) - i]);

  } else if (( couleur[i] == "ROUGE") && (couleur[bn - i] == "BLEU")) {
    if (estMort == false) {
      attaque[bn - i] = attaque[bn - i] + attaque[bn - i] ;
      delay(100);
      attaque[i] = attaque[i] + attaque[i] ;
      vie[(bn - i)] = vie[(bn - i)] - attaque[i] ;
      vie[(bn - i) - 1] = vie[(bn - i) - 1] - attaque[i] ;
      vie[(bn - i) - 2] = vie[(bn - i) - 2] - attaque[i] ;
      vie[i] = vie[i] - attaque[bn - i] ;
      vie[i - 1] = vie[i - 1] - attaque[bn - i] ;
      delay(100);
    }
    isDead(i);


  } else if (( couleur[i] == "ROUGE") && (couleur[bn - i] == "VERT")) {
    if (estMort == false) {
      attaque[bn - i] = attaque[bn - i] + attaque[bn - i] ;
      attaque[i] = attaque[i] + attaque[i] ;
      vie[(bn - i)] = vie[(bn - i)] - attaque[i] ;
      vie[(bn - i) - 1] = vie[(bn - i) - 1] - attaque[i] ;
      vie[(bn - i) - 2] = vie[(bn - i) - 2] - attaque[i] ;

      vie[i] = vie[i] - attaque[bn - i] ;
      delay(100);
    }
    isDead(i);



  } else if (( couleur[i] == "ROUGE") && (couleur[bn - i] == "MAUVE")) {
    if (estMort == false) {
      attaque[bn - i] = attaque[bn - i] + attaque[bn - i] ;
      attaque[i] = attaque[i] + attaque[i] ;
      vie[(bn - i)] = vie[(bn - i)] - attaque[i] ;
      vie[i] = vie[i] - attaque[bn - i] ;
      vie[(bn - i) - 1] = vie[(bn - i) - 1] - attaque[i] ;
      vie[i - 1] = vie[i] - attaque[bn - i] ;
      delay(100);
      rC[(bn - i)] = 100;
      gC[(bn - i)] = 100;
      bC[(bn - i)] = 100;
      delay(100);
      rC[(bn - i)] = 200;
      gC[(bn - i)] = 200;
      bC[(bn - i)] = 200;

      vie[(bn - i)] = 0 ;
      delay(100);
    }
    isDead(i);
    isDead(bn - i);


  } else if (( couleur[i] == "ROUGE") && (couleur[bn - i] == "JAUNE")) {
    if (estMort == false) {
      attaque[bn - i] = attaque[bn - i] + attaque[bn - i] ;
      attaque[i] = attaque[i] + attaque[i] ;
      vie[(bn - i)] = vie[(bn - i)] - attaque[i] ;
      vie[(bn - i) - 1] = vie[(bn - i) - 1] - attaque[i] ;
      vie[(bn - i) - 2] = vie[(bn - i) - 2] - attaque[i] ;
      vie[i - 1] = vie[i] - attaque[bn - i] ;
      delay(100);
    }
    isDead(i);

  } else if (( couleur[i] == "ROUGE") && (couleur[bn - i] == "CYAN")) {
    if (estMort == false) {
      attaque[bn - i] = attaque[bn - i] + attaque[bn - i] ;
      attaque[i] = attaque[i] + attaque[i] ;
      vie[(bn - i)] = vie[(bn - i)] - attaque[i] ;
      vie[(bn - i) - 1] = vie[(bn - i) - 1] - attaque[i] ;
      vie[(bn - i) - 2] = vie[(bn - i) - 2] - attaque[i] ;
      vie[i ] = vie[i] - attaque[bn - i] ;
      vie[i - 1] = vie[i - 1] - attaque[bn - i] ;
      vie[i - 2] = vie[i - 2] - attaque[bn - i] ;


      delay(100);
    }
    isDead(i);

  } /*/else if(( couleur[i]=="BLEU")&&(couleur[bn-i]=="ROUGE")){
         if(estMort == false){
       attaque[bn - i] =attaque[bn - i] +attaque[bn - i] ;
                         delay(100);
       attaque[i] =attaque[i] +attaque[i] ;
      vie[(bn - i)] = vie[(bn - i)] - attaque[i] ;
      vie[(bn - i) - 1] = vie[(bn - i) - 1] - attaque[i] ;
        vie[i] = vie[i] - attaque[bn - i] ;
      vie[i-1] = vie[i-1] - attaque[bn - i] ;
      vie[i-2] = vie[i-2] - attaque[bn - i] ;
                  delay(100);
                  }
      isDead(i);

    }else if(( couleur[i]=="BLEU")&&(couleur[bn-i]=="BLEU")){
           if(estMort == false){

       attaque[bn - i] =attaque[bn - i] +attaque[bn - i] ;
    delay(100);
       attaque[i] =attaque[i] +attaque[i] ;
      vie[(bn - i)] = vie[(bn - i)] - attaque[i] ;
      vie[(bn - i) - 1] = vie[(bn - i) - 1] - attaque[i];
      vie[i] = vie[i] - attaque[bn - i] ;
      vie[i-1] = vie[i-1] - attaque[bn - i] ;
                  delay(100);
                  }
      isDead(i);

    }else if(( couleur[i]=="BLEU")&&(couleur[bn-i]=="VERT")){
           if(estMort == false){

       attaque[bn - i] =attaque[bn - i] +attaque[bn - i] ;
    delay(100);
       attaque[i] =attaque[i] +attaque[i] ;
      vie[(bn - i)] = vie[(bn - i)] - attaque[i] ;
      vie[(bn - i) - 1] = vie[(bn - i) - 1] - attaque[i];
      vie[i] = vie[i] - attaque[bn - i] ;
                  delay(100);
                  }
      isDead(i);

    }else if(( couleur[i]=="BLEU")&&(couleur[bn-i]=="MAUVE")){
           if(estMort == false){
       attaque[bn - i] =attaque[bn - i] +attaque[bn - i] ;
    delay(100);
       attaque[i] =attaque[i] +attaque[i] ;
      vie[(bn - i)] = vie[(bn - i)] - attaque[i] ;
      vie[(bn - i) - 1] = vie[(bn - i) - 1] - attaque[i];
      vie[i] = vie[i] - attaque[bn - i] ;
                    delay(100);
                   vie[(bn - i)] =0;
                   }
      isDead(i);

    }else if(( couleur[i]=="BLEU")&&(couleur[bn-i]=="JAUNE")){
           if(estMort == false){
       attaque[bn - i] =attaque[bn - i] +attaque[bn - i] ;
    delay(100);
       attaque[i] =attaque[i] +attaque[i] ;
      vie[(bn - i)] = vie[(bn - i)] - attaque[i] ;
      vie[(bn - i) - 1] = vie[(bn - i) - 1] - attaque[i];
      vie[i] = vie[i] - attaque[bn - i] ;
                  delay(100);
                  }
      isDead(i);

    }else if(( couleur[i]=="BLEU")&&(couleur[bn-i]=="CYAN")){
           if(estMort == false){

       attaque[bn - i] =attaque[bn - i] +attaque[bn - i] ;
    delay(100);
       attaque[i] =attaque[i] +attaque[i] ;
      vie[(bn - i)] = vie[(bn - i)] - attaque[i] ;
      vie[(bn - i) - 1] = vie[(bn - i) - 1] - attaque[i];
      vie[i] = vie[i] - attaque[bn - i] ;
            vie[i-1] = vie[i-1] - attaque[bn - i] ;
                vie[i-2] = vie[i-2] - attaque[bn - i] ;
                   vie[i-3] = vie[i-3] - attaque[bn - i] ;
                  delay(100);
                  }
                  isDead(i);


    }/*/else if (( couleur[i] == "VERT") && (couleur[bn - i] == "ROUGE")) {
    if (estMort == false) {
      attaque[bn - i] = attaque[bn - i] + attaque[bn - i] ;
      attaque[i] = attaque[i] + attaque[i] ;
      vie[(bn - i)] = vie[(bn - i)] - attaque[i] ;
      vie[i ] = vie[i] - attaque[bn - i] ;
      vie[i - 1] = vie[i - 1] - attaque[bn - i] ;
      vie[i - 2] = vie[i - 2] - attaque[bn - i] ;
    }
    isDead(i);

  } else if (( couleur[i] == "VERT") && (couleur[bn - i] == "BLEU")) {
    if (estMort == false) {
      attaque[bn - i] = attaque[bn - i] + attaque[bn - i] ;
      attaque[i] = attaque[i] + attaque[i] ;
      vie[(bn - i)] = vie[(bn - i)] - attaque[i] ;
      vie[i ] = vie[i] - attaque[bn - i] ;
      vie[i - 1] = vie[i - 1] - attaque[bn - i] ;
      delay(100);
    }
    isDead(i);

  } else if (( couleur[i] == "VERT") && (couleur[bn - i] == "VERT")) {
    if (estMort == false) {
      attaque[bn - i] = attaque[bn - i] + attaque[bn - i] ;
      attaque[i] = attaque[i] + attaque[i] ;
      vie[(bn - i)] = vie[(bn - i)] - attaque[i] ;
      vie[i ] = vie[i] - attaque[bn - i] ;
      delay(100);
    }
    isDead(i);

  } else if (( couleur[i] == "VERT") && (couleur[bn - i] == "MAUVE")) {
    if (estMort == false) {
      attaque[bn - i] = attaque[bn - i] + attaque[bn - i] ;
      attaque[i] = attaque[i] + attaque[i] ;
      vie[(bn - i)] = vie[(bn - i)] - attaque[i] ;
      vie[i ] = vie[i] - attaque[bn - i] ;
      delay(100);
      vie[(bn - i)] = 0;
      delay(100);
    }
    isDead(i);

  } else if (( couleur[i] == "VERT") && (couleur[bn - i] == "JAUNE")) {
    if (estMort == false) {
      attaque[bn - i] = attaque[bn - i] + attaque[bn - i] ;
      attaque[i] = attaque[i] + attaque[i] ;
      vie[(bn - i)] = vie[(bn - i)] - attaque[i] ;
      vie[i ] = vie[i] - attaque[bn - i] ;
      delay(100);
    }
    isDead(i);

  } else if (( couleur[i] == "VERT") && (couleur[bn - i] == "CYAN")) {
    if (estMort == false) {
      attaque[bn - i] = attaque[bn - i] + attaque[bn - i] ;
      attaque[i] = attaque[i] + attaque[i] ;
      vie[(bn - i)] = vie[(bn - i)] - attaque[i] ;
      vie[i ] = vie[i] - attaque[bn - i] ;
      vie[i - 1] = vie[i - 1] - attaque[bn - i] ;
      vie[i - 2 ] = vie[i - 2] - attaque[bn - i] ;
      vie[i - 3 ] = vie[i - 3] - attaque[bn - i] ;
      delay(100);
    }
    isDead(i);

  }/*/else if(( couleur[i]=="MAUVE")&&(couleur[bn-i]=="ROUGE")){
         if(estMort == false){
       attaque[bn - i] =attaque[bn - i] +attaque[bn - i] ;
       attaque[i] =attaque[i] +attaque[i] ;
      vie[(bn - i)] = vie[(bn - i)] - attaque[i] ;
       vie[i ] = vie[i] - attaque[bn - i] ;
              vie[i-1] = vie[i-1] - attaque[bn - i] ;
       vie[i-2 ] = vie[i-2] - attaque[bn - i] ;
       delay(100);
       vie[i ] = 0;
        delay(100);
        }
      isDead(i);

    }else if(( couleur[i]=="MAUVE")&&(couleur[bn-i]=="BLEU")){
           if(estMort == false){
       attaque[bn - i] =attaque[bn - i] +attaque[bn - i] ;
       attaque[i] =attaque[i] +attaque[i] ;
        vie[(bn - i)] = vie[(bn - i)] - attaque[i] ;
       vie[i ] = vie[i] - attaque[bn - i] ;
       vie[i-1] = vie[i-1] - attaque[bn - i] ;
       delay(100);
       vie[i ] = 0;
        delay(100);
        }
        isDead(i);

    }else if(( couleur[i]=="MAUVE")&&(couleur[bn-i]=="VERT")){
           if(estMort == false){
       attaque[bn - i] =attaque[bn - i] +attaque[bn - i] ;
       attaque[i] =attaque[i] +attaque[i] ;
        vie[(bn - i)] = vie[(bn - i)] - attaque[i] ;
       vie[i ] = vie[i] - attaque[bn - i] ;
       delay(100);
  vie[i ] = 0;
        delay(100);
        }
      isDead(i);

    }else if(( couleur[i]=="MAUVE")&&(couleur[bn-i]=="MAUVE")){
           if(estMort == false){
       attaque[bn - i] =attaque[bn - i] +attaque[bn - i] ;
       attaque[i] =attaque[i] +attaque[i] ;
        vie[(bn - i)] = vie[(bn - i)] - attaque[i] ;
       vie[i ] = vie[i] - attaque[bn - i] ;
       delay(100);
  vie[i ] = 0;
        delay(100);
  vie[bn-i] = 0;
        delay(100);
        }
      isDead(i);

    }else if(( couleur[i]=="MAUVE")&&(couleur[bn-i]=="JAUNE")){
           if(estMort == false){
       attaque[bn - i] =attaque[bn - i] +attaque[bn - i] ;
       attaque[i] =attaque[i] +attaque[i] ;
       vie[(bn - i)] = vie[(bn - i)] - attaque[i] ;
       vie[i ] = vie[i] - attaque[bn - i] ;
       delay(100);
  vie[i ] = 0;
        delay(100);
        }
      isDead(i);

    }else if(( couleur[i]=="MAUVE")&&(couleur[bn-i]=="CYAN")){
           if(estMort == false){
       attaque[bn - i] =attaque[bn - i] +attaque[bn - i] ;
       attaque[i] =attaque[i] +attaque[i] ;
        vie[(bn - i)] = vie[(bn - i)] - attaque[i] ;
       vie[i ] = vie[i] - attaque[bn - i] ;
              vie[i -1] = vie[i-1] - attaque[bn - i] ;
       vie[i-2] = vie[i-2] - attaque[bn - i] ;
       vie[i-3 ] = vie[i-3] - attaque[bn - i] ;

       delay(100);
  vie[i ] = 0;
        delay(100);
        }
      isDead(i);

    }/*/else if (( couleur[i] == "JAUNE") && (couleur[bn - i] == "ROUGE")) {
    if (estMort == false) {
      attaque[bn - i] = attaque[bn - i] + attaque[bn - i] ;
      attaque[i] = attaque[i] + attaque[i] ;
      vie[(bn - i)] = vie[(bn - i)] - attaque[i] ;
      vie[i ] = vie[i] - attaque[bn - i] ;
      delay(100);
    }
    isDead(i);

  } else if (( couleur[i] == "JAUNE") && (couleur[bn - i] == "BLEU")) {
    if (estMort == false) {
      attaque[bn - i] = attaque[bn - i] + attaque[bn - i] ;
      attaque[i] = attaque[i] + attaque[i] ;
      vie[(bn - i)] = vie[(bn - i)] - attaque[i] ;
      vie[i ] = vie[i] - attaque[bn - i] ;
      vie[i - 1 ] = vie[i - 1] - attaque[bn - i] ;

      delay(100);
    }
    isDead(i);

  } else if (( couleur[i] == "JAUNE") && (couleur[bn - i] == "VERT")) {
    if (estMort == false) {
      attaque[bn - i] = attaque[bn - i] + attaque[bn - i] ;
      attaque[i] = attaque[i] + attaque[i] ;
      vie[(bn - i)] = vie[(bn - i)] - attaque[i] ;
      vie[i ] = vie[i] - attaque[bn - i] ;
      delay(100);
    }
    isDead(i);

  } else if (( couleur[i] == "JAUNE") && (couleur[bn - i] == "MAUVE")) {
    if (estMort == false) {
      attaque[bn - i] = attaque[bn - i] + attaque[bn - i] ;
      attaque[i] = attaque[i] + attaque[i] ;
      vie[(bn - i)] = vie[(bn - i)] - attaque[i] ;
      vie[i ] = vie[i] - attaque[bn - i] ;
      delay(100);
      vie[(bn - i)] = 0;
      delay(100);
    }
    isDead(i);

  } else if (( couleur[i] == "JAUNE") && (couleur[bn - i] == "JAUNE")) {
    if (estMort == false) {
      attaque[bn - i] = attaque[bn - i] + attaque[bn - i] ;
      attaque[i] = attaque[i] + attaque[i] ;
      vie[(bn - i)] = vie[(bn - i)] - attaque[i] ;
      vie[i ] = vie[i] - attaque[bn - i] ;
      delay(100);
    }
    isDead(i);

  } else if (( couleur[i] == "JAUNE") && (couleur[bn - i] == "CYAN")) {
    if (estMort == false) {
      attaque[bn - i] = attaque[bn - i] + attaque[bn - i] ;
      attaque[i] = attaque[i] + attaque[i] ;
      vie[(bn - i)] = vie[(bn - i)] - attaque[i] ;
      vie[i ] = vie[i] - attaque[bn - i] ;
      vie[i - 1 ] = vie[i - 1] - attaque[bn - i] ;
      vie[i - 2] = vie[i - 2] - attaque[bn - i] ;
      vie[i - 3] = vie[i - 3] - attaque[bn - i] ;
      delay(100);
    }
    isDead(i);

  } else if (( couleur[i] == "CYAN") && (couleur[bn - i] == "ROUGE")) {
    if (estMort == false) {
      attaque[bn - i] = attaque[bn - i] + attaque[bn - i] ;
      attaque[i] = attaque[i] + attaque[i] ;
      vie[(bn - i)] = vie[(bn - i)] - attaque[i] ;
      vie[(bn - i) - 1] = vie[(bn - i) - 1] - attaque[i] ;
      vie[(bn - i) - 2] = vie[(bn - i) - 2] - attaque[i] ;
      vie[(bn - i) - 3] = vie[(bn - i) - 3] - attaque[i] ;
      vie[i ] = vie[i] - attaque[bn - i] ;
      vie[i - 1 ] = vie[i - 1] - attaque[bn - i] ;
      vie[i - 2] = vie[i - 2] - attaque[bn - i] ;

      delay(100);
    }
    isDead(i);

  } else if (( couleur[i] == "CYAN") && (couleur[bn - i] == "BLEU")) {
    if (estMort == false) {
      attaque[bn - i] = attaque[bn - i] + attaque[bn - i] ;
      attaque[i] = attaque[i] + attaque[i] ;
      vie[(bn - i)] = vie[(bn - i)] - attaque[i] ;
      vie[(bn - i) - 1] = vie[(bn - i) - 1] - attaque[i] ;
      vie[(bn - i) - 2] = vie[(bn - i) - 2] - attaque[i] ;
      vie[(bn - i) - 3] = vie[(bn - i) - 3] - attaque[i] ;
      vie[i ] = vie[i] - attaque[bn - i] ;
      vie[i - 1 ] = vie[i - 1] - attaque[bn - i] ;
      delay(100);
    }
    isDead(i);

  } else if (( couleur[i] == "CYAN") && (couleur[bn - i] == "VERT")) {
    if (estMort == false) {
      attaque[bn - i] = attaque[bn - i] + attaque[bn - i] ;
      attaque[i] = attaque[i] + attaque[i] ;
      vie[(bn - i)] = vie[(bn - i)] - attaque[i] ;
      vie[(bn - i) - 1] = vie[(bn - i) - 1] - attaque[i] ;
      vie[(bn - i) - 2] = vie[(bn - i) - 2] - attaque[i] ;
      vie[(bn - i) - 3] = vie[(bn - i) - 3] - attaque[i] ;
      vie[i ] = vie[i] - attaque[bn - i] ;
      delay(100);
    }
    isDead(i);

  } else if (( couleur[i] == "CYAN") && (couleur[bn - i] == "MAUVE")) {
    if (estMort == false) {
      attaque[bn - i] = attaque[bn - i] + attaque[bn - i] ;
      attaque[i] = attaque[i] + attaque[i] ;
      vie[(bn - i)] = vie[(bn - i)] - attaque[i] ;
      vie[(bn - i) - 1] = vie[(bn - i) - 1] - attaque[i] ;
      vie[(bn - i) - 2] = vie[(bn - i) - 2] - attaque[i] ;
      vie[(bn - i) - 3] = vie[(bn - i) - 3] - attaque[i] ;
      vie[i ] = vie[i] - attaque[bn - i] ;
      delay(100);
      vie[(bn - i)] = 0;
      delay(100);
    }
    isDead(i);

  } else if (( couleur[i] == "CYAN") && (couleur[bn - i] == "JAUNE")) {
    if (estMort == false) {
      attaque[bn - i] = attaque[bn - i] + attaque[bn - i] ;
      attaque[i] = attaque[i] + attaque[i] ;
      vie[(bn - i)] = vie[(bn - i)] - attaque[i] ;
      vie[(bn - i) - 1] = vie[(bn - i) - 1] - attaque[i] ;
      vie[(bn - i) - 2] = vie[(bn - i) - 2] - attaque[i] ;
      vie[(bn - i) - 3] = vie[(bn - i) - 3] - attaque[i] ;
      vie[i ] = vie[i] - attaque[bn - i] ;
      delay(100);
    }
    isDead(i);

  } else {
    if (estMort == false) {
      attaque[bn - i] = attaque[bn - i] + attaque[bn - i] ;
      attaque[i] = attaque[i] + attaque[i] ;
      vie[(bn - i)] = vie[(bn - i)] - attaque[i] ;
      vie[(bn - i) - 1] = vie[(bn - i) - 1] - attaque[i] ;
      vie[(bn - i) - 2] = vie[(bn - i) - 2] - attaque[i] ;
      vie[(bn - i) - 3] = vie[(bn - i) - 3] - attaque[i] ;
      vie[i ] = vie[i] - attaque[bn - i] ;
      vie[i - 1 ] = vie[i - 1] - attaque[bn - i] ;
      vie[i - 2] = vie[i - 2] - attaque[bn - i] ;
      vie[i - 3 ] = vie[i - 3] - attaque[bn - i] ;

      delay(100);
    }
    isDead(i);

  }
}

void isDead(int i) {
  if (vie[i] <= 0) {
    estMort = true;
    if ( estMort == true) {
      rC[i] = 0;
      gC[i] = 0;
      bC[i] = 0;
      positionC[i + 1]--;
      estMort = false;
      Serial.print("vie1::");
      Serial.println(i);

    }

  }

  if (vie[bn - i] <= 0) {
    Serial.print("vies2hp::");
    Serial.println(vie[bn - i]);
    estMort = true;
    if (estMort == true) {
      rC[bn - i] = 0;
      gC[bn - i] = 0;
      bC[bn - i] = 0;
      positionC[(bn - (i + 1))] --;
      estMort = false;
      Serial.print("vie2::");
      Serial.println(bn - i); 
    }

  }
}


/**********************************************************************************/
//*************************CALIBRATIONCAPASITORLED*************************************************************************************


void updateEncoder() {

  encoderMoved = true;

  byte MSB = digitalRead(ENCODER_A); //MSB = most significant bit
  byte LSB = digitalRead(ENCODER_B); //LSB = least significant bit

  byte newEncodedValue = (MSB << 1) | LSB; //converting the 2 pin value to single number
  byte old_vs_new  = (encoderValue << 2) | newEncodedValue; //adding it to the previous encoded value

  if ( old_vs_new == B1101 ||  old_vs_new == B0100 ||  old_vs_new == B0010 ||  old_vs_new == B1011) relativeValue ++;
  if ( old_vs_new == B1110 ||  old_vs_new == B0111 ||  old_vs_new == B0001 ||  old_vs_new == B1000) relativeValue --;

  encoderValue = newEncodedValue; //store this value for next time
}


/* HSV to RGB conversion function with only integer
  math : values are between 0 and 255 */
void HSV2RGB() {
  byte region;
  unsigned int fpart, p, q, t;

  if (s == 0) {
    /* color is grayscale */
    r = g = b = v;
    return;
  }

  /* make hue 0-5 */
  region = h / 43;   // 4
  /* find remainder part, make it from 0-255 */
  fpart = (h - (region * 43)) * 6; // 120

  /* calculate temp vars, doing integer multiplication */
  p = (v * (255 - s)) >> 8; // 0
  q = (v * (255 - ((s * fpart) >> 8))) >> 8; // 255 * 120
  t = (v * (255 - ((s * (255 - fpart)) >> 8))) >> 8;
  /* assign temp vars based on color cone region */
  switch (region) {
    case 0:
      r = v; g = t; b = p; break;
    case 1:
      r = q; g = v; b = p; break;
    case 2:
      r = p; g = v; b = t; break;
    case 3:
      r = p; g = q; b = v; break;
    case 4:
      r = t; g = p; b = v; break;
    default:
      r = v; g = p; b = q; break;
  }
}
