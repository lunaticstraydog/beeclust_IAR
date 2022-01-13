#include <Wire.h>
#include "Mona_ESP_lib.h"
#include <time.h>
#include <stdlib.h>


int IR_values[5] = {0};
int IR_init[5] = {0};
int state;
int threshold = 35;
int threshold_down = 10;
int gap = 30;
int low_light = 110;
int up_light = 190;
bool detected;
bool tdetected[5] = {true};
int turn;
float delay_reset = 0;

void setup() {
  Mona_ESP_init();
  state = 0;
  srand(time(NULL));
  turn = rand() % 5;
}

void loop() {

  delay_reset = delay_reset + 5.0 / 100;
  
  if (state == 0) {
    for (int i = 0; i < 5; i++) {
      IR_values[i] = Get_IR(i + 1);
    }

    if (IR_values[1] <= threshold and IR_values[2] <= threshold and IR_values[3] <= threshold and IR_values[0] <= threshold and IR_values[4] <= threshold and IR_values[1] > threshold_down and IR_values[2] > threshold_down and IR_values[3] > threshold_down and IR_values[0] > threshold_down and IR_values[4] > threshold_down) {
      Set_LED(1, 0, 20, 0);
      Set_LED(2, 0, 20, 0);
      Motors_forward(100);
    }
    else {
      state = 1;
      Set_LED(1, 13, 7, 0);
      Set_LED(2, 13, 7, 0);
      Motors_stop();
    }
  }
  else if (state == 1) {
    delay(50);
    int now = millis();
    int end_time = millis();

    detected = false;
    
    for (int i = 0; i < 5; i++) {
      IR_init[i] = Read_IR(i + 1);
      tdetected[i] = true;
    }
    
    while ((end_time - now) < 200) {
      for (int i = 0; i < 5; i++) {
        IR_values[i] = Read_IR(i + 1);
      }

      for (int i = 0; i < 5; i++) {
        if (abs(IR_values[i] - IR_init[i]) > gap) {
          tdetected[i] = false;
        } else if (not (low_light <= IR_values[i] and IR_values[i] <= up_light)){
          tdetected[i] = false;
        }
      }

      for (int i = 0; i < 5; i++) {
        if (tdetected[i]) {
          detected = true;
          break;
        }
      }

      if (detected) {
        break;
      }

      end_time = millis();
    }
    
    if (not detected) {
      state = 3;
    } else {
      state = 2;
    }
  }
  else if (state == 2) {
    Motors_spin_left(100);
    delay(1200);
    Motors_stop();
    Set_LED(1, 20, 20, 20);
    Set_LED(2, 20, 20, 20);

    for (int i = 0; i < 5; i++) {
      Enable_IR(i + 1);
    }

    state = 4;
  }
  else if (state == 3) {
    for (int i = 0; i < 5; i++) {
      IR_values[i] = Get_IR(i + 1);
    }

    if (IR_values[2] > threshold or IR_values[2] <= threshold_down) {
      turn = rand() % 2;

      if (turn == 0) {
        while ((IR_values[2] > threshold or IR_values[2] <= threshold_down) or (IR_values[1] > threshold or IR_values[1] <= threshold_down or IR_values[0] <= threshold_down) or (IR_values[3] > threshold or IR_values[3] <= threshold_down or IR_values[4] <= threshold_down)) {
          Set_LED(1, 20, 0, 0);
          Set_LED(2, 0, 20, 0);
          Motors_spin_right(100);
          
          for (int i = 0; i < 5; i++) {
            IR_values[i] = Get_IR(i + 1);
          }
        }
      } else {
        while ((IR_values[2] > threshold or IR_values[2] <= threshold_down) or (IR_values[1] > threshold or IR_values[1] <= threshold_down or IR_values[0] <= threshold_down) or (IR_values[3] > threshold or IR_values[3] <= threshold_down or IR_values[4] <= threshold_down)) {
          Set_LED(1, 0, 20, 0);
          Set_LED(2, 20, 0, 0);
          Motors_spin_left(100);

          for (int i = 0; i < 5; i++) {
            IR_values[i] = Get_IR(i + 1);
          }
        }  
      }

      Set_LED(1, 13, 7, 0);
      Set_LED(2, 13, 7, 0);
      Motors_stop();
    }
    else if (IR_values[1] > threshold or IR_values[1] <= threshold_down or IR_values[0] <= threshold_down) {
      Set_LED(1, 20, 0, 0);
      Set_LED(2, 0, 20, 0);
      Motors_spin_right(100);
    }
    else if (IR_values[3] > threshold or IR_values[3] <= threshold_down or IR_values[4] <= threshold_down) {
      Set_LED(1, 0, 20, 0);
      Set_LED(2, 20, 0, 0);
      Motors_spin_left(100);
    }
    else if (IR_values[0] > threshold and not(IR_values[4] > threshold)) {
      Set_LED(1, 0, 0, 20);
      Set_LED(2, 0, 0, 20);
      
      if (delay_reset >= 3) {
        turn = rand() % 2;
        delay_reset = 0;
      }
        
      if (turn == 0) {
        Set_LED(1, 20, 0, 0);
        Set_LED(2, 0, 20, 0);
        Motors_spin_right(100);
      } else {
        //Set_LED(1, 0, 20, 0);
        //Set_LED(2, 0, 20, 0);
        Motors_forward(100);
      }
    }
    else if (IR_values[4] > threshold and not(IR_values[0] > threshold)) {
      Set_LED(1, 0, 0, 20);
      Set_LED(2, 0, 0, 20);
      
      if (delay_reset >= 3) {
        turn = rand() % 2;
        delay_reset = 0;
      }

      if (turn == 0) {
        Set_LED(1, 0, 20, 0);
        Set_LED(2, 20, 0, 0);
        Motors_spin_left(100);
      } else {
        //Set_LED(1, 0, 20, 0);
        //Set_LED(2, 0, 20, 0);
        Motors_forward(100);
      }
    }
    
    else {
      state = 0;
    }
  } 
  else if (state == 4) {
    return ;
  }

  delay(5);

}
