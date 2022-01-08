#include <Wire.h>
#include "Mona_ESP_lib.h"


int IR_values[5] = {0};
int threshold = 35;
int threshold_robot = 10;
int threshold_light = 200;
float limit_time = 1;
int state;
bool state_end, back_on;
float test_time, detect_time;
bool detect_robot[5] = {false};


void setup() {
  Mona_ESP_init();
  state = 0;
  test_time = 0;
  state_end = false;
  back_on = true;
}


void loop() {
  if (state_end) {
    return ;
  }
  
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

  if (state == 4){
    Set_LED(1, 13, 7, 0);
    Set_LED(2, 13, 7, 0);
    
    if (back_on) {
      Motors_backward(100);
      delay(200);
      back_on = false;
    }
    
    Motors_stop();
    test_time = test_time + 5.0 / 100;
    IR_values[0] = Read_IR(1);
    IR_values[1] = Read_IR(2);
    IR_values[2] = Read_IR(3);
    IR_values[3] = Read_IR(4);
    IR_values[4] = Read_IR(5);
    
    if (IR_values[0] < threshold_light or IR_values[1] < threshold_light or IR_values[2] < threshold_light or IR_values[3] < threshold_light or IR_values[4] < threshold_light){
      Set_LED(1, 13, 7, 0);
      Set_LED(2, 13, 7, 0);
      Motors_stop();
      state = 5;
    }
   }

   if (state == 5) {
    for (int i = 0; i < 5; i++) {
      detect_robot[i] = false;
    }
    
    detect_time = 0;

    while(detect_time < 1) {
      IR_values[0] = Read_IR(1);
      IR_values[1] = Read_IR(2);
      IR_values[2] = Read_IR(3);
      IR_values[3] = Read_IR(4);
      IR_values[4] = Read_IR(5);
      detect_time = detect_time + 5.0 / 100;
      
      for (int i = 0; i < 5; i++) {
        if (IR_values[i] >= threshold_light)
          detect_robot[i] = true;
      }

      delay(5);
    }

    for (int i = 0; i < 5; i++) {
      if (not detect_robot[i]){
        state = 3;
        state_end = true;
        break;  
      }
    }
   }

   if(state == 3){
    Motors_spin_left(100);
    delay(1200);
    Motors_stop();
    Set_LED(1, 20, 20, 20);
    Set_LED(2, 20, 20, 20);

    for (int i = 1; i <= 5; i++) {
      Enable_IR(i);
    }

    return ;
  }

  IR_values[0] = Get_IR(1);
  IR_values[1] = Get_IR(2);
  IR_values[2] = Get_IR(3);
  IR_values[3] = Get_IR(4);
  IR_values[4] = Get_IR(5);

  if(IR_values[0] < threshold_robot or IR_values[1] < threshold_robot or IR_values[2] < threshold_robot or IR_values[3] < threshold_robot or IR_values[4] < threshold_robot){
      state = 4;
  }
  else if(IR_values[1] > threshold or IR_values[2] > threshold or IR_values[3] > threshold){
    if (test_time < limit_time) {
      state = 4;
    }
    else{
      back_on = true;
      state = 1;
    }
  }
  else if(IR_values[0] > threshold){
    if (test_time < limit_time) {
      state = 4;
    }
    else{
      back_on = true;
      state = 2;
    }
  }
  else if(IR_values[4] > threshold){
    if (test_time < limit_time) {
      state = 4;
    }
    else{
      back_on = true;
      state = 1;
    }
  }
  else{
    test_time = 0;
    state = 0;
  }
  
  delay(5);
}
