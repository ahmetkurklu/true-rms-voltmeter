#include "U8glib.h"
#include <math.h>
#define VREF 5.0
#define ANALOG_PIN 0

#define BUTTON_PIN1 12
#define BUTTON_PIN2 8
#define BUTTON_PIN3 5

float Req;  
float R1;
float R2=1000.0;
int flag =0;

//CONFIGURATION
U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_FAST); 

void setup() {
  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  u8g.setColorIndex(255);// white
  u8g.setFont(u8g_font_unifont);
  analogReference(DEFAULT);
  pinMode(BUTTON_PIN1,INPUT);
  pinMode(BUTTON_PIN2,INPUT);
  pinMode(BUTTON_PIN3,INPUT);
  
  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  
  noInterrupts(); //Désactivation des interruptions
  TCCR1A = 0;//Remise a 0 des bits de réglage TCCR1A
  TCCR1B = 0;//Remise a 0 des bits de réglage TCCR1B

  TCNT1 = 49536;// Prérempli le timer 65536-16MHz/1/1KHz
  TCCR1B |=(1<<CS10);//Prescaler de 1
  TIMSK1 |= (1 << TOIE1);//Activation des interruptions sur overflow
  interrupts();// Activation des interruptions



}
  ISR(TIMER1_OVF_vect) //Service d'interruption sur overflow
  {  
    flag = 1;
    TCNT1 = 49536;// Prérempli le timer (etat inital)
  }

void loop() {

  //Déclaration des variables  
  static unsigned short nbr_Aquisition =0 ;
  static float resultatMoy = 0;
  static float resultatEff = 0;
  static float mesure =0;
  static char str1[11];
  static char str2[11];
  static char str3[1];

  //TEST des boutons pour les calibres
  if(digitalRead(BUTTON_PIN1)== HIGH){
    R1 = 180.0;//Calibre 3V
    str3[0]='1';//Valeur a afficher a côté de calibre 
  }
  if(digitalRead(BUTTON_PIN2)== HIGH){
    R1 = 11000.0;//Calibre 30V
    str3[0]='2';//Valeur a afficher a côté de calibre 

  }
  if(digitalRead(BUTTON_PIN3)== HIGH){
    R1 =39000.0;//Calibre,100V
    str3[0]='3';//Valeur a afficher a côté de calibre 
  }

  Req = (R1+R2)/R2;
  
  //Une interruption a eu lieu le flag est a 1
  if(flag ==1){
    if(nbr_Aquisition<1000){
      //Aquisition de Vnum et conversion en Vs 
       mesure = analogRead(ANALOG_PIN)*(VREF/1023.0); 
       //On applique les coefficients 
       mesure = ((mesure - VREF/2)*Req);
       //Ajout du resultat pour la moyenne et la valeur efficace
       resultatMoy = resultatMoy+ mesure;
       resultatEff = resultatEff+(mesure*mesure);    
       nbr_Aquisition ++;
    }
    else{

      //Calcule de la valeur efficace et de la valeur moyenne
      resultatMoy = resultatMoy/nbr_Aquisition;
      resultatEff = sqrt(resultatEff/nbr_Aquisition);

      //Conversion des resultat en chaine de caractere 
      dtostrf(resultatMoy,5,5,str1);
      dtostrf(resultatEff,5,5,str2);

       //Affichage 
      u8g.firstPage(); 
    do {
      u8g.drawStr( 0, 10, "Valeur moyenne:");
      u8g.drawStr( 0, 25, str1);
      //u8g.drawStr(55,25,"V");
      u8g.drawStr( 0, 40, "Valeur eff:");
      u8g.drawStr( 0, 55, str2);
      //u8g.drawStr(55,55,"V");
      u8g.drawStr(95, 55, "Cal");
      u8g.drawStr(118, 55, str3);
    } while( u8g.nextPage());
    
      nbr_Aquisition = 0; 
    }
  }
  flag =0;//Remise a 0 du flag 
    
}
