#include <Wire.h>
#include "Mona_ESP_lib.h"
#include <time.h>
#include <stdlib.h>


bool IR_values[5] = {false, false, false, false, false};
int threshold = 35;
int threshold_robot = 20;
// 0 -move forward , 1 - forward obstacle , 2 - right proximity , 3 - left proximity
int state, choose_side, r;
float delay_reset;

void setup(){
  Mona_ESP_init();
  state = 0;
  choose_side = 0;
  r = 0;
  delay_reset = - 5.0 / 100;
  srand(time(NULL));
}


void loop(){
  if (state == 0) {
    Set_LED(1, 0, 20, 0);
    Set_LED(2, 0, 20, 0);
    Motors_forward(100);
  }
  
  if (state == 1){
    Set_LED(1, 13, 7, 0);
    Set_LED(2, 13, 7, 0);

    if (choose_side == 1)
      Motors_spin_left(100);

    if (choose_side == 2)
      Motors_spin_right(100);
  }
  
  if (state == 2){
    Set_LED(1, 0, 20, 0);
    Set_LED(2, 20, 0, 0);
    Motors_forward(100);
  }
  
  if(state == 3){
    Set_LED(1, 20, 0, 0);
    Set_LED(2, 0, 20, 0);
    Motors_forward(100);
  }

  if (state == 4){
    Set_LED(1, 13, 7, 0);
    Set_LED(2, 13, 7, 0);
    Motors_spin_right(100);
  }

  if (state == 5){
    Set_LED(1, 13, 7, 0);
    Set_LED(2, 13, 7, 0);
    Motors_spin_left(100);
  }

  IR_values[0] = Detect_object(1,threshold);
  IR_values[1] = Detect_object(2,threshold);
  IR_values[2] = Detect_object(3,threshold);
  IR_values[3] = Detect_object(4,threshold);
  IR_values[4] = Detect_object(5,threshold);

  if (Get_IR(1) < threshold_robot){
    state = 4;
  }
  else if (Get_IR(5) < threshold_robot){
    state = 5;
  }
  else if (IR_values[1] or IR_values[2] or IR_values[3]){
    if (choose_side == 0) {
      r = rand() % 2;
      
      if (r == 0)
        choose_side = 1;
      else
        choose_side = 2;
    }
    
    delay_reset = 0;
    state = 1;
  }
  else if (IR_values[0]){ //Check for left proximity
    delay_reset = 0;
    choose_side = 2;
    state = 3;
  }
  else if (IR_values[4]){// Check for right proximity
    delay_reset = 0;
    choose_side = 1;
    state = 2;
  }
  else{ //If there are no proximities, move forward
    if (delay_reset >= 2) {
      choose_side = 0;
      delay_reset = 0;
    }

    delay_reset = delay_reset + 5.0 / 100;
    state = 0;
  }

  delay(5);
}
