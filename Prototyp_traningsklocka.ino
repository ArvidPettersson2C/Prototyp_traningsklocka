#define USE_ARDUINO_INTERRUPTS true
#include <PulseSensorPlayground.h>     //inkluderar bibliotek för pulssensor
#include <U8glib.h>                    //inkluderar bibliotek för Oledskärmen
#include <Wire.h>
#include <RtcDS3231.h>                  //inkluderar bibliotek för RtcModulen

//  Variables
const int PulseWire = 0;      // PulseSensor kopplad till pin 0
const int buttonGreen = 3;    // grön Knapp är kopplad till pin 3
const int buttonYellow = 4;  // gul Knapp är kopplad till pin 4
const int buttonRed = 5;     // röd Knapp är kopplad till pin 5

bool State_pulse = false;     //ge pulssensorn tillståndet false (inte på)
bool State_stopwatch = false; //ge stoppuret tillståndet false (inte på)
bool State_clock = false;     //ge klockan tillståndet false (inte på)

RtcDS3231<TwoWire> Rtc(Wire);
U8GLIB_SSD1306_128X64 oled(U8G_I2C_OPT_NO_ACK);

//skapa variabler för tid
int hour;
int minute;
int second;
int elapsedHours;
int elapsedMinutes;
int elapsedSeconds;
int startTime;
int timeNow;
int elapsedTime;
int Threshold = 750;           // threshold-värde för pulssesnorn
// default värde på 550

PulseSensorPlayground pulseSensor;


void setup() {
  oled.setFont(u8g_font_helvB10);     //setup för skärmen
  Wire.begin();

  // setup för klockan 
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  Rtc.SetDateTime(compiled); 
  
  Serial.begin(9600);
  
  // sätt pinmode                   
  pinMode(buttonGreen, INPUT_PULLUP);   
  pinMode(buttonYellow, INPUT_PULLUP);  
  pinMode(buttonRed, INPUT_PULLUP);     

  //setup för pulsesensor
  pulseSensor.analogInput(PulseWire);
  pulseSensor.setThreshold(Threshold);


  
}
void loop() {
  updateRTC(); //uppdatera kolckan


  Serial.println(analogRead(A0));

  //När grön knapp trycks ned så ska skärmen visa användarens puls 
  if (digitalRead(buttonGreen) == LOW) { 
    delay(50); // Debounce delay
    State_pulse = true;       
    State_stopwatch = false;  
    State_clock = false;      
    while ((State_pulse = true) && (digitalRead(buttonRed) == HIGH) && (digitalRead(buttonYellow) == HIGH)) {
      int myBPM = pulseSensor.getBeatsPerMinute();  
      updateOled(String(myBPM));
    }
  }
  //När gul knapp trycks ned så ska skärmen visa tiden
  if (digitalRead(buttonYellow) == LOW) { //tidvisningen
    delay(50); // Debounce delay
    State_pulse = false;
    State_stopwatch = false;
    State_clock = true;
    while ((State_clock = true) && (digitalRead(buttonRed) == HIGH) && (digitalRead(buttonGreen) == HIGH)) {
      Serial.println("The yellow button is pressed");
      updateRTC();
      updateOled(String(hour) + ";" + String(minute) + ";" + String(second));

    }
  }
  // när röd knapp trycks ned så ska skärmen visa stoppuret
  if (digitalRead(buttonRed) == LOW) { //stoppuret
    delay(50); // Debounce delay
    State_pulse = false;
    State_stopwatch = false;
    State_clock = true;
    timeStart();
    while ((State_clock = true) && (digitalRead(buttonYellow) == HIGH) && (digitalRead(buttonGreen) == HIGH)) {
      Serial.println("The red button is pressed");
      updateRTCtimer();
      timeElapsed();
      updateOled(String(elapsedHours) + ";" + String(elapsedMinutes) + ";" + String(elapsedSeconds));
    }
    timeElapsed();
  }
}

// Funktionen uppdaterar stoppuret, den sätts igäng när den röda knappen trycks ned
void timeStart() {
  RtcDateTime now = Rtc.GetDateTime();
  startTime = now.Hour() * 3600 + now.Minute() * 60 + now.Second();
}
// funktionen kollar tiden just nu
void updateRTCtimer() {
  RtcDateTime now = Rtc.GetDateTime();
  timeNow = now.Hour() * 3600 + now.Minute() * 60 + now.Second();
}
// funktion för att visa tiden som gått, tiden räknar upp
void timeElapsed() {
  elapsedTime = timeNow - startTime;

  //konverterar tiden som gått till timmar, minuter och sekunder
  elapsedHours = floor(elapsedTime / 3600);
  elapsedMinutes = floor((elapsedTime % 3600) / 60);
  elapsedSeconds = elapsedTime % 60;
}
//funktion för att hämta klockslag från rtc modulen
void updateRTC() {
  RtcDateTime now = Rtc.GetDateTime();
  hour = now.Hour();
  minute = now.Minute();
  second = now.Second();
}
//funktion för att lägga till text på skärmen samt hur den texten ska synas
void updateOled(String text) {

  oled.firstPage();
  do {
    oled.drawStr (20, 40, text.c_str());
  } while (oled.nextPage());
}
