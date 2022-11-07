#include <TimerOne.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

unsigned long duty_cycle=512; // adjustable from 1 to 1023
unsigned long freq=20000; // default f=40 kHz

//unsigned long period=1/freq; // period (t=1/f)
unsigned long period=25; // period (t=1/f)
//volatile bool timerOutput = false;

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup()
{
  Serial.begin(115200);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  
  // set the digital pin as output:
  pinMode(10, OUTPUT);
  Timer1.initialize(period);
  Timer1.pwm(10, duty_cycle); // duty_cycle adjustable from 1 to 1023
}

void loop()
{
//  freq = 1000000/t;
  //clear display
  display.clearDisplay();

  // display frequency
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("Frequency: ");
  display.setTextSize(2);
  display.setCursor(0,10);
  display.print(freq);
  display.print(" ");
  display.setTextSize(2);
  display.print("Hz");
  
  // display duty cycle
  display.setTextSize(1);
  display.setCursor(0, 35);
  display.print("Duty Cycle: ");
  display.print(" ");
  display.setTextSize(2);
  display.setCursor(0, 45);
  display.print((duty_cycle*100)/1023);
  display.print(" ");
  display.setTextSize(2);
  display.print("%");
  
  display.display(); 
}
