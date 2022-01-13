#include <Wire.h>
#include "Mona_ESP_lib.h"


int state;
float init_IR[5] = {0};
float IR_value[5] = {0};
int eps = 2;


void setup(){
  Mona_ESP_init();
  state = 0;
  delay(3000);
  
  for (int i = 1; i <= 5; i++){
    init_IR[i - 1] = Get_IR(i);
  }
}


void loop(){
  if(state == 0){
    Set_LED(1, 0, 20, 0);
    Set_LED(2, 0, 20, 0);
    Motors_forward(100);
  }
  
  if(state == 1){
    Set_LED(1, 20, 0, 0);
    Set_LED(2, 0, 20, 0);
    Motors_spin_right(100);
  }
  
  if(state == 2){
    Set_LED(1, 20, 0, 0);
    Set_LED(2, 20, 0, 0);
    Motors_stop();
  }
  
  if(state == 3){
    Set_LED(1, 0, 20, 0);
    Set_LED(2, 20, 0, 0);
    Motors_spin_left(100);
  }

  for (int i = 1; i <= 5; i++){
    IR_value[i - 1] = Get_IR(i);
  }

  if (abs(IR_value[1] - init_IR[1]) < eps and abs(IR_value[2] - init_IR[2]) < eps and abs(IR_value[3] - init_IR[3]) < eps) {
    state = 2; 
  }
  else if(IR_value[3] < init_IR[3] and IR_value[1] >= init_IR[1]){
    state = 3;
  }
  else if(IR_value[1] < init_IR[1] and IR_value[3] >= init_IR[3]){
    state = 1;
  }
  else if(IR_value[1] >= init_IR[1] and IR_value[2] >= init_IR[2] and IR_value[3] >= init_IR[3]){
    state = 2;
  }
  else{
    state = 0;
  }

  delay(5);
}
