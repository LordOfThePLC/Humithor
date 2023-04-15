//Digital Inputs
#define DI_OPERATIONMODE 12  //Digitaleingang Betriebsartenwahlschalter 0 = Dauerbetrieb 1 = Intervallbetrieb

//Digital Outputs
#define DO_FAN_CIRCULATOR 3  //Digitalausgang Lüfter Umluft
#define DO_FAN_HUMIDIFIER 6  //Digitalausgang Befeuchter

//LCD Pins and Config
#define LCD_RESET 4   //Resetpin LCD
#define LCD_ENABLE 7  //Enablepin LCD
#define LCD_D4 9      //D4-Pin LCD
#define LCD_D5 8      //D5-Pin LCD
#define LCD_D6 5      //D6-Pin LCD
#define LCD_D7 2      //D7-Pin LCD

#define LCD_NROFROWS 2      //LCD Zeilenanzahl
#define LCD_NROFCOLUMNS 16  //LCD Spaltenanzahl

//Sensor-Config
#define HYT_ADDR 0x28  //I2C Adresse

//Humidity Limits
#define HUMIDITY_LOWERLIMIT 67  //Untergrenze Luftfeuchtigkeit
#define HUMIDITY_UPPERLIMIT 70  //Obergrenze Luftfeuchtigkeit

//States
#define INIT 0       //Initialstate
#define IDLE 1       //Leerlauf
#define CIRCULATE 2  //Umluft
int STATE = INIT;

#define WAITTIME_IDLE 30       //Wartezeit Leerlauf
#define WAITTIME_CIRCULATE 10  //Ablaufzeit Umluft