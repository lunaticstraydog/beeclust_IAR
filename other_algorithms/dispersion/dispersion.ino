#include <Wire.h>
#include "Mona_ESP_lib.h"


int IR_values[5] = {0};
int threshold = 35;
int threshold_robot = 20;
int state;


void setup() {
  Mona_ESP_init();
  state = 0;
}


void loop() {
  if(state == 0){
    //Start moving Forward
    Set_LED(1, 0, 20, 0);
    Set_LED(2, 0, 20, 0);
    Motors_forward(100);
  }
  
  if(state == 1){
    //Spin to the left
    Set_LED(1, 0, 20, 0);
    Set_LED(2, 20, 0, 0);
    Motors_spin_left(100);
  }
  
  if(state == 2){
    //Spin to the right
    Set_LED(1, 20, 0, 0);
    Set_LED(2, 0, 20, 0);
    Motors_spin_right(100);
  }
  
  if(state == 3){
    Set_LED(1, 0, 20, 0);
    Set_LED(2, 20, 0, 0);
    Motors_spin_left(100);
    delay(1200);
  }

  if (state == 4){
    Set_LED(1, 20, 0, 0);
    Set_LED(2, 0, 20, 0);
    Motors_spin_right(100);
    delay(1200);
  }

  IR_values[0] = Get_IR(1);
  IR_values[1] = Get_IR(2);
  IR_values[2] = Get_IR(3);
  IR_values[3] = Get_IR(4);
  IR_values[4] = Get_IR(5);

  if(IR_values[1] < threshold_robot or IR_values[2] < threshold_robot or IR_values[3] < threshold_robot or IR_values[4] < threshold_robot){
    state = 3;
  }
  else if (IR_values[0] < threshold_robot){
    state = 4;
  }
  else if(IR_values[1] > threshold or IR_values[2] > threshold or IR_values[3] > threshold){
    state = 1;
  }
  else if(IR_values[0] > threshold){
    state = 2;
  }
  else if(IR_values[4] > threshold){
    state = 1;
  }
  else{
    state = 0;
  }
  
  delay(5);
}
