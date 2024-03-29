#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define DAC_PIN A0 // Use built-in DAC in pin 0

// read the input on analog pin 0:
int freq = analogRead(A7) * 50000 / 1024.0;
// read the input on analog pin 8:
float duty_cycle = analogRead(A8) / 1024.0;

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// SQUARE WAVE
// Output adjustable dual slope PWM on TCC0 with complementary outputs and dead time insertion 
void setup()
{
  analogReadResolution(10); // Set analog input resolution to max, 12-bits
  SerialUSB.begin(115200);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
}

/* 
 *  Using DAC Pin D0 (PA2) or D2 (PA10). XIAO D0 is PA2, D2 is PA10 (even port thus PMUXE)
 */
void wave_generation(int freq, float duty_cycle) {
  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN |        // Enable GCLK0
                      GCLK_CLKCTRL_GEN_GCLK0 |    // Select GCLK0 at 48MHz
                      GCLK_CLKCTRL_ID_TCC0_TCC1;  // Route GCLK0 to TCC0 and TCC1

  // Enable the port multiplexer for port pins PA10 and PA11
  PORT->Group[g_APinDescription[2].ulPort].PINCFG[g_APinDescription[2].ulPin].bit.PMUXEN = 1;
 
  // Select the port pin multiplexer switch to option F for TCC0/WO[2] and TCC0/WO[3] on 
  // port pins PA10 and PA11 respectively
//  PORT->Group[PORTA].PMUX[10 >> 1].reg = PORT_PMUX_PMUXO_F | PORT_PMUX_PMUXE_F;
  PORT->Group[g_APinDescription[2].ulPort].PMUX[g_APinDescription[2].ulPin >> 1].reg = PORT_PMUX_PMUXE_F;// | PORT_PMUX_PMUXE_F; 
  
  TCC0->WAVE.reg = TCC_WAVE_POL2 |                // Reverse the signal polarity on channel 2
                   TCC_WAVE_WAVEGEN_DSBOTTOM;     // Dual slope PWM on TCC0
  while (TCC0->SYNCBUSY.bit.WAVE);                // Wait for synchronization
  
  //  PER = 48 MHz / (2*desired_freq)
  TCC0->PER.reg = (48000000)/(2*freq);            // Set the frequency of the PWM on TCC0 to desired frequency
  while(TCC0->SYNCBUSY.bit.PER);                  // Wait for synchronization
 
  TCC0->CC[2].reg = (48000000*duty_cycle)/(2*freq);  // Output a 50% duty-cycle
  while(TCC0->SYNCBUSY.bit.CC2);                  // Wait for synchronization
  
  TCC0->CTRLA.bit.ENABLE = 1;                     // Enable TCC0 counter
  while (TCC0->SYNCBUSY.bit.ENABLE);              // Wait for synchronization
}

void loop() {
  // read the input on analog pin 0:
  int freq = analogRead(A7) * 50000 / 1024.0;
//  int freq = 20000;
  // read the input on analog pin 8:
  float duty_cycle = analogRead(A8) / 1024.0;
//  float duty_cycle = 0.5;
  
  wave_generation(freq, duty_cycle);
  
  TCC0->CTRLBSET.reg = TCC_CTRLBSET_LUPD;         // Set the Lock Update (LUPD) bit
  while (TCC0->SYNCBUSY.bit.CTRLB);               // Wait for synchroniztion

  delay(1);  // delay in between reads for stability

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
  display.print(int(duty_cycle*100));
  display.print(" ");
  display.setTextSize(2);
  display.print("%");
  
  display.display(); 

}
