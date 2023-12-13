#define USE_ARDUINO_INTERRUPTS true
#include <PulseSensorPlayground.h>     //inkluderar bibliotek för pulssensor
#include <U8glib.h>                    //inkluderar bibliotek för Oledskärmen
#include <Wire.h>
#include <RtcDS3231.h>                  //inkluderar bibliotek för RtcModulen

//  Variables
const int PulseWire = 0;      // PulseSensor PURPLE WIRE connected to ANALOG PIN 0
const int buttonGreen = 3;    // grön Knapp är kopplad till pin 3
const int buttonYellow = 4;  // gul Knapp är kopplad till pin 4
const int buttonRed = 5;     // röd Knapp är kopplad till pin 5

bool State_pulse = false;     //ge pulssensorn tillståndet false (inte på)
bool State_stopwatch = false; //ge stoppuret tillståndet false (inte på)
bool State_clock = false;     //ge klockan tillståndet false (inte på)

RtcDS3231<TwoWire> Rtc(Wire);
U8GLIB_SSD1306_128X64 oled(U8G_I2C_OPT_NO_ACK);
int hour; //skapa variabler för tid
int minute;
int second;
int elapsedHours;
int elapsedMinutes;
int elapsedSeconds;
int startTime;
int timeNow;
int elapsedTime;
int Threshold = 750;           // Use the "Gettting Started Project" to fine-tune Threshold Value beyond default setting.
// Otherwise leave the default "550" value.

PulseSensorPlayground pulseSensor;  


void setup() {
  oled.setFont(u8g_font_helvB10);
  Wire.begin();
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  Rtc.SetDateTime(compiled);
  Serial.begin(9600);                // initialize serial
  pinMode(buttonGreen, INPUT_PULLUP); // set arduino pin to input pull-up mode
  pinMode(buttonYellow, INPUT_PULLUP); // set arduino pin to input pull-up mode
  pinMode(buttonRed, INPUT_PULLUP); // set arduino pin to input pull-up mode

  
  pulseSensor.analogInput(PulseWire);
  pulseSensor.setThreshold(Threshold);

  
  if (pulseSensor.begin()) {
    Serial.println("We created a pulseSensor Object !");  //för att sakerstalla att pulssensorn är igang
  }
}
void loop() {
  updateRTC();


  Serial.println(analogRead(A0));

  if (digitalRead(buttonGreen) == LOW) { //pulsmätaren
    delay(50); // Debounce delay

    Serial.println("The green button is pressed");
    State_pulse = true;
    State_stopwatch = false;
    State_clock = false;
    while ((State_pulse = true) && (digitalRead(buttonRed) == HIGH) && (digitalRead(buttonYellow) == HIGH)) {
      // Constantly test to see if "a beat happened".
      
      int myBPM = pulseSensor.getBeatsPerMinute();  // Calls function on our pulseSensor object that returns BPM as an "int".
      
      updateOled(String(myBPM));
    }
  }
  // delay(200);
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

// Main function that updates the clock with each passing second since the tiltswitch was tilted.

void timeStart() {
  RtcDateTime now = Rtc.GetDateTime();
  startTime = now.Hour() * 3600 + now.Minute() * 60 + now.Second();
}
// function for checking the time right now
void updateRTCtimer() {
  RtcDateTime now = Rtc.GetDateTime();
  timeNow = now.Hour() * 3600 + now.Minute() * 60 + now.Second();
}
// help function for timer, and startFan functions that calculates the difference from now since the start of the timer
void timeElapsed() {
  elapsedTime = timeNow - startTime;

  //Converts the elapsed time into hours, minutes, and seconds
  elapsedHours = floor(elapsedTime / 3600);
  elapsedMinutes = floor((elapsedTime % 3600) / 60);
  elapsedSeconds = elapsedTime % 60;
}
void updateRTC() {
  RtcDateTime now = Rtc.GetDateTime();
  hour = now.Hour();
  minute = now.Minute();
  second = now.Second();
}
void updateOled(String text) {
  
  oled.firstPage();
  do {
    oled.drawStr (20, 40, text.c_str());
  } while (oled.nextPage());
}
