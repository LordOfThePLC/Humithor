//Digital Inputs
#define DI_OPERATIONMODE 12   //Digitaleingang Betriebsartenwahlschalter 0 = Dauerbetrieb 1 = Intervallbetrieb
#define DI_ENCODER_BUTTON 55  //Button Rotaryencoder
#define DI_ENCODER_A 56       //Spur A Rotaryencoder
#define DI_ENCODER_B 57       //Spur B Rotaryencoder

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

//States Fan
#define INIT 0       //Initialstate
#define IDLE 1       //Leerlauf
#define CIRCULATE 2  //Umluft
int STATE = INIT;

//Waittimes
#define WAITTIME_IDLE 30            //Wartezeit Leerlauf
#define WAITTIME_CIRCULATE 10       //Ablaufzeit Umluft
#define WAITTIME_ACIVATESETTINGS 3  //Haltezeit Button für Einstellungen
#define REFRESHTIME_SENSOR 500      //Refresh-Zeit der Sensorwerte in Millisekunden

//States LCD
#define INIT_LCD 0    //Init LCD-Anzeige
#define MAINSCREEN 1  //Hauptanzeige
#define SETTINGS 2    //Einstellungen
int STATE_LCD = MAINSCREEN;