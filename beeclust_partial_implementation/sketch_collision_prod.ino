//MIPS MIPS
//Include the Mona_ESP library
#include <Wire.h>
#include "Mona_ESP_lib.h"

//Variables
bool obstacle[5] = {false, false, false, false, false};
int ir_state[5] = {false, false, false, false, false};
bool obstacle_bool=false;

//Threshold value used to determine a detection on the IR sensors.
//Reduce the value for a earlier detection, increase it if there
//false detections.
int threshold1 = 50;
int threshold2 = 50;
int threshold3 = 50;
int threshold4 = threshold2;
int threshold5 = threshold1;
//State Machine Variable
// 0 -move forward , 1 - forward obstacle , 2 - right proximity , 3 - left proximity
int state, old_state, lr_random, turn_delay;
//Min and Max values for the robot détection
int min_value_IR = 0; //TODO : determine the value
int max_value_IR = 230; //TODO : determine the value
//Speed
int forward_speed = 150;
int turn_speed = 100;
//Min and Max values for the turn delay
int min_turn_delay = 500;
int max_turn_delay = 1500;

void setup()
{
  Serial.begin(9600);
  //Initialize the MonaV2 robot
  Mona_ESP_init();
  //Initialize variables
  state=0;
  old_state=0;
}


void loop(){
  for (int i=1;i<=5;i++)
  {
    Enable_IR(i);
  }
  delay(100);
  //--------------Motors------------------------
  //Set motors movement based on the state machine value.
  if(state == 0){ //FORWARD
    //Motors_forward(forward_speed);
  }
  if(state == 1){ //TURN (random)
    lr_random=random(2); //0: left 1: right
    turn_delay=random(min_turn_delay,max_turn_delay); //Turn delay between min and max
    if(lr_random==0)
    {
      //Motors_spin_left(turn_speed); //Left
    }
    else
    {
      //Motors_spin_right(turn_speed); //Right 
    }
    delay(turn_delay);
  }
  

  //--------------IR sensors------------------------
  //Decide future state:
  //Read IR state to determine maze walls
   for (int i=1;i<=5;i++)
    {
      Disable_IR(i);
    }
  delay(1);
 
  obstacle[0] = Detect_object(1,threshold1);
  obstacle[1] = Detect_object(2,threshold2);
  obstacle[2] = Detect_object(3,threshold3);
  obstacle[3] = Detect_object(4,threshold4);
  obstacle[4] = Detect_object(5,threshold5);
  ir_state[0]=Read_IR(1);
  ir_state[1]=Read_IR(2);
  ir_state[2]=Read_IR(3);
  ir_state[3]=Read_IR(4);
  ir_state[4]=Read_IR(5);
  obstacle_bool=false;
  //Print for debug
/*
  Serial.print("Read_IR");
  Serial.print(Read_IR(1));
  Serial.print("\n");
  Serial.print(Read_IR(2));
  Serial.print("\n");
  Serial.print(Read_IR(3));
  Serial.print("\n");
  Serial.print(Read_IR(4));
  Serial.print("\n");
  Serial.print(Read_IR(5));
  Serial.print("\n");
  */
  for (int i=1;i<=5;i++)
  {
    Enable_IR(i);
  }
  
  //--------------State Machine------------------------
  //Use the retrieved IR state to set the robot state
  //Check the obstacles

  if(obstacle[0] or obstacle[1] or obstacle[2] or obstacle[3] or obstacle[4]) 
  { //Obstacle detected 
      //Wall detected
    Serial.print("obstacle detected \n");
    state=1; //TURN
    obstacle_bool=true;
  }

   if((ir_state[1]>min_value_IR and ir_state[1]<max_value_IR) || (ir_state[2]>min_value_IR and ir_state[2]<max_value_IR) || (ir_state[3]>min_value_IR and ir_state[3]<max_value_IR) || (ir_state[4]>min_value_IR and ir_state[4]<max_value_IR) || (ir_state[0]>min_value_IR and ir_state[0]<max_value_IR))
   { //Robot detected
      //TODO adapt min_value_IR and max_value_IR
      delay(1000);  //TODO : adapt the delay with the intensity of the light
      Serial.print("Robot detected \n");
      obstacle_bool=true;
      state=1; //TURN
   }

  if (!obstacle_bool)
  { //No obstacle detected
    state=0; //FORWARD
  }
  delay(5);
}
