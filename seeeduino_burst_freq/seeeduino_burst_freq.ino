#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define DAC_PIN A0 // Use built-in DAC in pin 0

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Declarations for base frequency, burst frequency, and duty cycle
int base_freq = 20000;
int burst_freq = 1000;
int burst_period = (1 / (double) burst_freq)*1000;          // units in microseconds
float duty_cycle = 0.5;                            // 0.0 to 1.0
float burst_ratio = 0.3;
int cycles = (int) (burst_period*1000*burst_ratio)/((1/(double)base_freq)*pow(10, 6));

// Output a burst of 6 pulses at 20 kHz, 50% duty-cycle on digital pin D2
void setup() {
//  analogReadResolution(10); // Set analog input resolution to max, 12-bits
//  SerialUSB.begin(115200);
//
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
//  delay(2000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
//
//  //clear display
//  display.clearDisplay();

  // display frequency
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("Frequency: ");
  display.setTextSize(2);
  display.setCursor(0,10);
  display.print(base_freq);
  display.print(" ");
  display.setTextSize(2);
  display.print("Hz");
//  
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
//  
  display.display(); 
  
  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN |         // Enable GCLK0
                      GCLK_CLKCTRL_GEN_GCLK0 |     // Select GCLK0 at 48MHz
                      GCLK_CLKCTRL_ID_TCC0_TCC1;   // Route GCLK0 to TCC0 and TCC1
  while (GCLK->STATUS.bit.SYNCBUSY);               // Wait for synchronization

  // Enable the port multiplexer for port pins PA10 and PA11
  PORT->Group[g_APinDescription[2].ulPort].PINCFG[g_APinDescription[2].ulPin].bit.PMUXEN = 1;
 
  // Select the port pin multiplexer switch to option F for TCC0/WO[2] and TCC0/WO[3] on 
  // port pins PA10 and PA11 respectively
//  PORT->Group[PORTA].PMUX[10 >> 1].reg = PORT_PMUX_PMUXO_F | PORT_PMUX_PMUXE_F;
  PORT->Group[g_APinDescription[2].ulPort].PMUX[g_APinDescription[2].ulPin >> 1].reg = PORT_PMUX_PMUXE_F;

  // Feed GCLK4 to TCC0 and TCC1
  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN |         // Enable GCLK4 to TCC0 and TCC1
                      GCLK_CLKCTRL_GEN_GCLK0 |     // Select GCLK4
                      GCLK_CLKCTRL_ID_TCC0_TCC1;   // Feed GCLK4 to TCC0 and TCC1
  while (GCLK->STATUS.bit.SYNCBUSY);               // Wait for synchronization

  NVIC_SetPriority(TCC0_IRQn, 0);    // Set the Nested Vector Interrupt Controller (NVIC) priority for TCC0 to 0 (highest) 
  NVIC_EnableIRQ(TCC0_IRQn);         // Connect TCC0 to Nested Vector Interrupt Controller (NVIC)

//  TCC0->WAVE.reg |= TCC_WAVE_WAVEGEN_NPWM;       // Setup normal single slope PWM on TCC0
  TCC0->WAVE.reg = TCC_WAVE_POL2 |                 // Reverse the signal polarity on channel 2
                   TCC_WAVE_WAVEGEN_DSBOTTOM;      // Dual slope PWM on TCC0
  while (TCC0->SYNCBUSY.bit.WAVE);                 // Wait for synchronization
  
  TCC0->PER.reg = (48000000)/(2*base_freq);        // Set the frequency of the PWM on TCC0 to 20 kHz 
  while (TCC0->SYNCBUSY.bit.PER);                  // Wait for synchronization 
   
  TCC0->CC[2].reg = 0;                             // Initialially set 0% duty cycle on D2 
  while (TCC0->SYNCBUSY.bit.CC2);                  // Wait for synchronization 
  
  TCC0->CTRLA.bit.ENABLE = 1;                      // Enable the TCC0 counter
  while (TCC0->SYNCBUSY.bit.ENABLE);               // Wait for synchronization 
}

void loop() {
  // Output a burst of 6 PWM pulses at 20kHz, 50% duty cycle every second
  TCC0->INTENSET.bit.OVF = 1;                     // Enable overflow (OVF) interrupts on TCC0  
  delay(burst_period);                            // Wait 1 microsecond -> burst frequency = 1 kHz 
}

void TCC0_Handler()
{
  static volatile uint32_t counter;
  
  if (TCC0->INTENSET.bit.OVF && TCC0->INTFLAG.bit.OVF)
  {
    if (counter == 0)
    {
      counter++;
      TCC0->CCB[2].reg = (48000000*duty_cycle)/(2*base_freq);    // TCC0 CCB2 - 50% duty cycle on D2 (using buffered CCBx register)
      while (TCC0->SYNCBUSY.bit.CCB2);                // Wait for synchronization     
    }
    else if (counter < cycles)                        // Have we counted up to 6?
    {
      counter++;                                      // Increment the counter and continue     
    }
    else
    {
      counter = 0;                                    // Reset the counter     
      TCC0->CCB[2].reg = 0;                           // TCC0 CCB2 - 0% duty cycle on D2 (using buffered CCBx register)
      while (TCC0->SYNCBUSY.bit.CCB2);                // Wait for synchronization  
      TCC0->INTENCLR.bit.OVF = 1;                     // Disable overflow (OVF) interrupts on TCC0     
    } 
    TCC0->INTFLAG.bit.OVF = 1;                        // Clear the interrupt flag   
  } 
}
