//MIPS MIPS
//Include the Mona_ESP library
#include <Wire.h>
#include "Mona_ESP_lib.h"

int verbose=1;
//Variables
bool obstacle[5] = {false, false, false, false, false};
int ir_state[5] = {false, false, false, false, false};
bool obstacle_bool = false;

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
int gap = 30;
int stop_loop = 0;

int truth_value[5];
int truth_value_old[5];

TaskHandle_t gestion_IR;
TaskHandle_t main_loop;
int lock=0;
int old_IR_state=0; //disabled
SemaphoreHandle_t sema_v;

void setup()
{
  Serial.begin(9600);
  //Initialize the MonaV2 robot
  Mona_ESP_init();
  //Initialize variables
  state = 0;
  old_state = 0;
  xTaskCreatePinnedToCore(
                    gestion_ir_code,   /* Task function. */
                    "gestion_ir",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &gestion_IR,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */                  
  delay(500); 

  //create a task that will be executed in the Task2code() function, with priority 1 and executed on core 1
  xTaskCreatePinnedToCore(
                    main_loop_code,   /* Task function. */
                    "main_loop",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &main_loop,      /* Task handle to keep track of created task */
                    1);          /* pin task to core 1 */
    delay(500); 
    sema_v = xSemaphoreCreateBinary();
}



void enable_IR()
{
    for (int i = 1; i <= 5; i++)
    {
      Enable_IR(i);
    }
    delay(1);
}


void disable_IR()
{
    for (int i = 1; i <= 5; i++)
    {
      Disable_IR(i);
    }
    delay(1);
}

void gestion_ir_code(void * pvParameters)
{
  for (;;){
    while (lock) {} //Bloque si l'autre thread est en train d'utiliser le capteur IR
    enable_IR(); 
    old_IR_state=1;
    delay(30);
    while (lock) {}
    disable_IR();
    old_IR_state=0;
    delay(30);
  }
}


void main_loop_code(void * pvParameters)
{  
  int now = millis();
  
  //--------------Motors------------------------
  //Set motors movement based on the state machine value.
  if (state == 0) { //FORWARD
    Motors_forward(forward_speed);
    Set_LED(1, 0, 20, 0);
    Set_LED(2, 0, 20, 0);
  }
  if (state == 1) { //TURN (random)
    lr_random = random(2); //0: left 1: right
    turn_delay = random(min_turn_delay, max_turn_delay); //Turn delay between min and max
    if (lr_random == 0)
    {
      Motors_spin_left(turn_speed); //Left
    }
    else
    {
      Motors_spin_right(turn_speed); //Right
    }
    Set_LED(1, 13, 7, 0);
    Set_LED(2, 13, 7, 0);
    delay(turn_delay);

  }

  if (state == 2)
  {
    stop_loop = 1;
    Set_LED(1, 20, 20, 20);
    Set_LED(2, 20, 20, 20);
    Motors_stop();
  }

  if (not(stop_loop))
  {
    //--------------IR sensors------------------------
    //Decide future state:
    //Read IR state to determine maze walls
    if (not(old_IR_state)) disable_IR(); lock=1; //Si l'ir est pas déja désactivé, désactive le et prends le lock
    
    obstacle[0] = Detect_object(1, threshold1);
    obstacle[1] = Detect_object(2, threshold2);
    obstacle[2] = Detect_object(3, threshold3);
    obstacle[3] = Detect_object(4, threshold4);
    obstacle[4] = Detect_object(5, threshold5);
    ir_state[0] = Read_IR(1);
    ir_state[1] = Read_IR(2);
    ir_state[2] = Read_IR(3);
    ir_state[3] = Read_IR(4);
    ir_state[4] = Read_IR(5);
    obstacle_bool = false;
    
    if (old_IR_state) enable_IR(); lock=0; // relache le lock pour que l'autre thread puisse continuer
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
    
    //--------------State Machine------------------------
    //Use the retrieved IR state to set the robot state
    //Check the obstacles

    if (obstacle[0] or obstacle[1] or obstacle[2] or obstacle[3] or obstacle[4])
    { //Obstacle detected
      //Wall detected
      Serial.print("obstacle detected \n");
      state = 1; //TURN
      obstacle_bool = true;
    }
    if ((ir_state[0] > min_value_IR and ir_state[0] < max_value_IR) || (ir_state[1] > min_value_IR and ir_state[1] < max_value_IR) || (ir_state[2] > min_value_IR and ir_state[2] < max_value_IR) || (ir_state[3] > min_value_IR and ir_state[3] < max_value_IR) || (ir_state[4] > min_value_IR and ir_state[4] < max_value_IR))
    {
      Serial.println("Robot ou source détecté");
      Motors_stop();
      
      //Robot detected
      int source = 1;

      
      //5 detection


      int old_time = millis();
      int new_time = millis();

      //while (new_time-old_time < 100) // teste pendant 100 ms 
      for (int i=0;i<5;i++) // 5 tests
      {
        if (i > 0)
        {
          for (int j=1;j<=5;j++)
          {
            truth_value_old[j]=truth_value[j];
          }
        }
        
       if (not(old_IR_state)) disable_IR(); lock=1;
       for (int j=1;j<=5;j++)
        {
          ir_state[j-1] = Read_IR(j);
          truth_value[j-1]=(ir_state[j-1] > min_value_IR and ir_state[j-1] < max_value_IR);
        }
        if (old_IR_state) enable_IR();lock=0;  //remet IR dans etat précedent
        if (i>0)
        {
          for (int j=1;j<=5;j++)
          {
            if (truth_value[j-1] != truth_value_old[j-1])
            {
              source=0;
            }
          }
        }
      }
      if (verbose)
      {
        new_time = millis();
        Serial.print("Temps pris par les 5 mesures, en ms, idéalement plus de 80 ms :");
        Serial.println(new_time-old_time);
      }
      if (source)
      {
        state = 2; //it was the source
        Serial.println("Source détectée");
      }

      else
      {
        state = 1; //TURN, it was a robot
        Serial.println("Robot detécté");
      }
      obstacle_bool = true;
    }
    int fin = millis();
    //Serial.println(fin - now);
    
  }
}


void loop() {}
