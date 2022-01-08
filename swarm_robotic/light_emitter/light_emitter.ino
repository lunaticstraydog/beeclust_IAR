#include <Wire.h>
#include "Mona_ESP_lib.h"


void setup() {
  Mona_ESP_init();
  delay(3000);
  
  for (int i = 1; i <= 5; i++){
    Enable_IR(i);  
  }

  Set_LED(1, 50, 50, 50);
  Set_LED(2, 50, 50, 50);

}


void loop() {
}
