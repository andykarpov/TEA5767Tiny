#include <TinyWireM.h>
#include <TEA5767Tiny.h>
#include <Button.h>
#include <EEPROM.h>
#include <Led.h>

TEA5767Tiny Radio;
double stored_frequency;
int search_mode = 0;
int search_direction;
unsigned long current_time, last_written;

Button btn_forward(3, PULLUP);
Button btn_backward(4, PULLUP);
Led led(1);

boolean state = false;

double load_frequency() {
  int a = EEPROM.read(0);
  int b = EEPROM.read(1);
  if (a > 0) {
    return a + ((float) b/100);
  }
  return 0;
}

void store_frequency(double f) {
  int a = (int) f;
  int b = (f - a) * 100;
  EEPROM.write(0, a);
  EEPROM.write(1, b); 
}

void setup() {
  Radio.init();
  stored_frequency = load_frequency();
  if (stored_frequency >= 88.5 && stored_frequency <= 108.00) {
    Radio.set_frequency(stored_frequency);
  } else {
    Radio.set_frequency(105.4);  
  }
   
  led.on();
}

void loop() {
  
  unsigned char buf[5];
  double current_freq;
  
  current_time = millis();
  
  if (Radio.read_status(buf) == 1) {
    current_freq =  floor (Radio.frequency_available (buf) / 100000 + .5) / 10;
    if (current_freq != stored_frequency && current_time - last_written > 10000 && search_mode == 0) {
      stored_frequency = current_freq;
      store_frequency(stored_frequency);
      last_written = current_time;
    }
  }
  
  if (search_mode == 1) {
      led.off();
      if (Radio.process_search (buf, search_direction) == 1) {
          search_mode = 0;
          led.on();
      }
  }
  
  if (btn_forward.isPressed()) {
    search_mode = 1;
    search_direction = TEA5767_SEARCH_DIR_UP;
    Radio.search_up(buf);
    delay(300);
  }
  
  if (btn_backward.isPressed()) {
    search_mode = 1;
    search_direction = TEA5767_SEARCH_DIR_DOWN;
    Radio.search_down(buf);
    delay(300);
  } 
  //delay(20); 
  delay(50);
}
