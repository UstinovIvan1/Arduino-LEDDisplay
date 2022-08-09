#include <TimerOne.h>
#include "Adafruit_NeoPixel.h"

#define leds_numb 72  //кількість світлодіодів на стрічці    
#define degree 10     //Розмір сегменту (градусів)     
#define k 0.3        //коефіцієнт згладжування
#define b 1.4     //коефіцієнт підбору (підбирається для досягнення найкращого результату)
#define START_POS 240 //положення датчика Холла відносно вертикального положення (в градусах)
#define NUM_FRAMES 3    //к-ть зображень
#define number_frame 50  //к-ть обертів для зміни зображення

#define PIN 7  //пін, до якого підключена стрічка         
   
#include "3images.h"


Adafruit_NeoPixel strip = Adafruit_NeoPixel(leds_numb, PIN, NEO_RGB + NEO_KHZ800);

uint16_t num1, num2;
byte n_segm = 360 / degree;
byte start_pos = (float)start_pos / 360 * n_segm;
byte this_frame = 0, count_frame;
volatile uint32_t new_period, period,timer;
volatile boolean timer_bool, hall_bool;

void setup() {

  strip.begin();
  strip.setBrightness(50);
  attachInterrupt(0, hall, RISING);
  Timer1.initialize();
Timer1.attachInterrupt(timer_interrupt);
  Timer1.stop();
  strip.clear();
  strip.show();
}

void timer_interrupt() 
{
  timer_bool = true;
}

void hall() 
{   
  if (micros() - timer > 50000) 
  {     
    new_period = micros() - timer;   
    timer = micros();
    hall_bool = true;

  }
}
void led(byte i, int counter, int this_pix) {
  switch (this_frame) {
    case 0: strip.setPixelColor(i,pgm_read_dword(&(kadr1[counter][this_pix])));
      break;
    case 1: strip.setPixelColor(i,pgm_read_dword(&(kadr2[counter][this_pix])));
      break;
          case 2: strip.setPixelColor(i,pgm_read_dword(&(kadr3[counter][this_pix])));
      break;
  }

}
void loop() {
  if (hall_bool) 
  {
    hall_bool = false;
    num1 = 0 + start_pos;
    num2 = n_segm / 2 + start_pos;
    if (num2 > n_segm) num2 = num2 - n_segm;
    
    count_frame++;
    if (count_frame >= number_frame) {
      count_frame = 0;
      this_frame = (this_frame + 1) % NUM_FRAMES;
    }
    period = (float)new_period * k + (float)period * ((float)1 - k);
    Timer1.setPeriod(period / n_segm * b);
    Timer1.restart();
  }

  if (timer_bool) 
  {
    timer_bool = false;
    set_led();
  }
}
void set_led() {

  strip.clear();

  byte this_pix = 0;
  for (int i = leds_numb / 2 - 1; i >= 0; i--) {
    led(i, num1, this_pix);
    this_pix++;
  }
  this_pix = 0;
  for (int i = leds_numb / 2 ; i < leds_numb; i++) {
    led(i, num2, this_pix);
    this_pix++;
  }

  num1++;   
  num2++;  
  if (num2 > n_segm) num2 = 0;
  if (num1 > n_segm) num1 = 0;

  strip.show();
}
