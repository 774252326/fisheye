\
//=========================HEADER=============================================================
/*
   Quad ATR Mecanum with Sabertooth
   AUTHOR: Jason Traud
   DATE: 6/8/2015
   
   This firmware demonstrates basic movement routines on a Mecanum ATR with a Sabertooth
   motor controller.
   
   Hardware: 
   
     Arduino Uno R3       (MCU-050-000)
     Programmable Mecanum (TP-095-004) 
     Sabertooth 2x25      (TE-091-225)

   Connections:    
     
     Arduino D3   -   Sabertooth S1 (Serial COM)
     Arduino D4   -   Sabertooth S2 (Emergency Stop)
     Arduino Gnd  -   Sabertooth 0V (A common ground is needed for stable communication) 
     
   DIP Switch Settings:
   
     00 11 11 (Address 128, front)
     00 10 11 (Address 129, rear)
     
   Support:    
     
     SuperDroid Robots Support Forum
       http://www.sdrobots.com/forums/   
     
     Vectoring Robot Support Page
       http://www.superdroidrobots.com/shop/custom.aspx/vectoring-robots/44/  
   
             
      
   License: CCAv3.0 Attribution-ShareAlike (http://creativecommons.org/licenses/by-sa/3.0/)
   You're free to use this code for any venture. Attribution is greatly appreciated. 
//============================================================================================
*/

// ****************************************************
// Libraries
// ****************************************************
#include <SoftwareSerial.h>
#include <Sabertooth.h>
//#include <SPI.h>  
//#include <Pixy.h>
#include <stdio.h>
//#include <Timer.h>
// *********************
// Define hardware pins
// *********************
#define sabertoothEstop 4 // This is connected to S2 on the motor controllers. 
                          // When this pin is pulled low the motors will stop. 
#define left -1
#define right 1
#define search_count_constant 3
#define lost_count_constant 6
#define tolar 11
//#define tolar 20
#define MAXrangeofA 330
#define distance 600 //unit is mm
#define back_speed_min -40

// Declaration of the software serial UART and motor controller objects
SoftwareSerial SWSerial(2, 3); // RX on pin 2 (unused), TX on pin 3 (to S1).
Sabertooth frontSaber(128, SWSerial); // Address 128, front motors, 1 = left, 2 = right
Sabertooth rearSaber(129, SWSerial);  // Address 130, rear motors, 1 = left, 2 = right
//Pixy pixy;
int x,y;
int buf_wheel = 0;
  int state = 0; // 0 = stop 1 = forward 2 = backward 3 = left = 4 = right
  int next_state = 0;
  int MAX_Speed = 70; //Range from 0 - 98
  int Object_width = 50;
  int new_s, new_d;
  int s,d;
  int accel_constant = 10;
  int Object_side = 0;
  int search = 0;
  int car_stop = 0;
  int search_finish = search_count_constant;
  int lost_count = lost_count_constant;
  int x_adjust = MAXrangeofA/160;
//******************************************************************************
// Sets up our serial com, hardware pins, SPI bus for ethernet shield and motor controller.
// RETURNS: Nothing
//******************************************************************************

//==============================communicate with Pi==========================================
String input;
char info[32];
char *a,*b,*c;
float fa,fb,fc;
int ina,inb,inc;
unsigned long times;
//================TIMER====================

//Timer t;

void setup() 
{
  
  delay(1000);           // Short delay to allow the motor controllers
                         // to power up before we attempt to send it commands.
                         // If you try to talk to them before the finish booting
                         // they can lock up and be unresponsive
  
  Serial.begin(19200);    // Serial for the debug output
  Serial.setTimeout(5);
  SWSerial.begin(9600);  // Serial for the motor controllers
  
  frontSaber.autobaud(); // Allows the motor controllers to detect the baud rate
  rearSaber.autobaud();  // we're using and adjust appropriately
  
  // Initialize GPIO inputs and outputs
  pinMode(sabertoothEstop, OUTPUT);
 
//  allStop();    // Make sure all motors are stopped for safety
//  pixy.init();
  Serial.println("Start");
  
}

//******************************************************************************
// This is our main program loop and is wrapped in an implied while(true) loop.
// This sample code demonstrates example movement for the robot and alternates
// through the motions by calling the commandMotors function. 
// 
// Description of the type of movement and the power level sent is printed to
// the serial console at a baud rate of 9600
// 
// RETURNS: Nothing
//******************************************************************************
void loop() 
{

  int x = 0; //range 0 to 319

//==============================prevent over flow====================================
    if(fa > 32767) fa = 32767;
    else if(fa < -32768) fa = -32768;
    if(fb > 32767) fb = 32767;
    else if(fb < -32768) fb = -32768;
    if(fc > 32767) fc = 32767;
    else if(fc < -32768) fc = -1;
//==============================convert to int ========================================
    ina = fa;
    inb = fb;
    inc = fc;
    
int speeda=25;
int speedb=10;
if(inc>0)
{
  if(inc>distance)
{
  d=d*0.6+speeda*0.4;
}
else
{
  d=d*0.6;
}

if(ina>inc*0.12)
{
  s=speedb;
}
else
{
  if(ina>-inc*0.12)
  s=0;
  else
  s=-speedb;
}
}
else
{
  d=0;
  s=0;
}

  
            
  // ****************  update the speed  ******************* //

        commandMotors(127 + d ,127,127 + s,1);

        delay(50);
		//delay(50);
}
//******************************************************************************
// Sets the speed of all motor controllers to zero and sets all ESTOPs
// RETURNS: NONE
//******************************************************************************

//================================read data=====================================

void serialEvent(){
  if(Serial.available() > 0){
    
    times = millis() - times;
    
   input = Serial.readString();
   input.toCharArray(info,32);
    
  //  int ib=Serial.read();
 //   int ib2=Serial.read();
    
 //   Serial.print("parse: ");
 //   Serial.print(ib);
 //       Serial.print("parse2: ");
 //   Serial.print(ib2);
    
    Serial.print("Time : ");
    Serial.print(times);
    times = millis();
    Serial.println("ms");
    
    Serial.println(input);
     Serial.print(d);
      Serial.print("\t");
      Serial.print(s);
    
    a = strtok(info,",");
    b = strtok(NULL,",");
    c = strtok(NULL,",");
    fa = atof(a);
    fb = atof(b);
    fc = atof(c);
    //Serial.flush();
  }Serial.flush();
  
}

int detect(){
    if(inc >= 0)
    return 1;
    else return 0;
}

void printdata(){
  noInterrupts();
  if(Serial.available() <= 0) {
    Serial.println(a[0]);
    Serial.print("a = ");
    Serial.println(ina);
    Serial.println(fa);
    Serial.print("b = ");
    Serial.println(inb);
    Serial.println(fb);
    Serial.print("c = ");
    Serial.println(inc);
    Serial.println(fc);
    interrupts();
  }
}


//================================finash read data ===================================
void demo()
{
String check="HI"; 
int duration, l=0, x=0, time_diff;

  check = Serial.readString() ;


//forward
 while ( (check == "" && (l<180)) && (check != "STOP"))
  {
             
             
              check = Serial.readString() ;
              Serial.println(l);
              Serial.println(check);
             
              
              commandMotors(160, 127, 127, 1);
              /*delay(4000);
              commandMotors(95, 127, 127, 1);
              delay(4000); 
              commandMotors(127, 127, 127, 1); 
              Serial.println(x);*/

            l = l + 10;
              
  }

    delay(3000);

 while ( (check == "" && (x<1800)) && (check != "STOP"))
  {
             
             
              check = Serial.readString() ;
              Serial.println(x);
              Serial.println(check);
             
              
              commandMotors(95, 127, 127, 1);
              /*delay(4000);
              commandMotors(95, 127, 127, 1);
              delay(4000); 
              commandMotors(127, 127, 127, 1); 
              Serial.println(x);*/

            x = x + 100;
              
  } 
 
}
  //move backward

void allStop()
{
  digitalWrite(sabertoothEstop, LOW);
  
  frontSaber.motor(1, 0);
  frontSaber.motor(2, 0);
  rearSaber.motor(1, 0);
  rearSaber.motor(2, 0);
}



//******************************************************************************
// Processes all motor commands. 
// The function expects three values; 
//    yAxis is our forward and back movement
//    xAxis is our left and right movement
//    spin is our third access
//    mode is the type of movement we're sending (tank versus vectoring)
//
// To understand the input of this function you need to envision a grid with 0,0
// in the bottom left, 127,127 in the center and 255,255 in the top right. The location
// of the data point passed into this function relative to the 127,127 center point
// is the direction that the robot is commanded to vector
// 
// RETURNS: NONE
//******************************************************************************
void commandMotors(float yAxis, int xAxis, int spin, int mode)
{
  // Initialize our local variables
  float leftFrontPower = 0;
  int leftRearPower = 0;
  float rightFrontPower = 0; 
  int rightRearPower = 0;
  int maxMotorPower = 0;
  double tempScale = 0;
  int x =0; //compensation variable
  int y=0;

  
  
  // Motor Constants 
  int motorValueMax = 255;  
  int motorValueMin = 0;
  int motorZero = 127;
  
  // Bound our incoming data to a safe and expected range
  if (yAxis > motorValueMax) { yAxis = motorValueMax; }
  else  if (yAxis < motorValueMin) { yAxis = motorValueMin; }  
  if (xAxis > motorValueMax) { xAxis = motorValueMax; }
  else  if (xAxis < motorValueMin) { xAxis = motorValueMin; }  
  if (spin > motorValueMax) { spin = motorValueMax; }
  else  if (spin < motorValueMin) { spin = motorValueMin; }
    
  // Shift incoming data to straddle 0
  yAxis = yAxis - 127;
  xAxis = xAxis - 127;
  spin = spin - 127;
  
  // A mode value of 1 passed into this function changes the motor mixing to
  // vectoring mode
  if (mode == 1)
  {    
    // *************************
    // Front and Back Motion
    // *************************
    leftFrontPower = leftFrontPower + yAxis;
    leftRearPower = leftRearPower + yAxis;
    rightFrontPower = rightFrontPower + yAxis;
    rightRearPower = rightRearPower + yAxis;
    
    // *************************
    // Left and Right Motion
    // *************************
/*
    if (xAxis > 127)
    {
      xAxis;
      }
    else if (xAxis < 127)
    {
      x == 20;
      }*/

if (xAxis > 155) { x = -2; }
  else  {x = 0; }  
  

if (xAxis < 100) { y = 2; }
  else  {y = 0; }
      
    leftFrontPower = leftFrontPower + (xAxis + y );
    leftRearPower = leftRearPower - (xAxis + y);
    rightFrontPower = rightFrontPower - (xAxis + x);
    rightRearPower = rightRearPower + (xAxis + x);
    
    // *************************
    // Spin
    // *************************
    leftFrontPower = leftFrontPower + spin;
    leftRearPower = leftRearPower + spin;
    rightFrontPower = rightFrontPower - spin;
    rightRearPower = rightRearPower - spin;
  
    // After our mixing above our motor powers are most likely going to exceed
    // our maximum values. We need to find our maximum and scale everything down
    // to values that our motor controller can understand 
    maxMotorPower = max(abs(leftFrontPower), abs(leftRearPower));
    maxMotorPower = max(maxMotorPower, abs(rightFrontPower));
    maxMotorPower = max(maxMotorPower, abs(rightRearPower)); 
    
    // Scale down by the maximum value if we exceed 127
    if (maxMotorPower > 127)
    {
     tempScale = (double)127 / (double)maxMotorPower;
     leftFrontPower = tempScale * (double)leftFrontPower;
     leftRearPower = tempScale * (double)leftRearPower;
     rightFrontPower = tempScale * (double)rightFrontPower;
     rightRearPower = tempScale * (double)rightRearPower;
    }
    
    // Cleans up our output data
    leftFrontPower = boundAndDeadband(leftFrontPower);
    leftRearPower = boundAndDeadband(leftRearPower);
    rightFrontPower = boundAndDeadband(rightFrontPower);
    rightRearPower = boundAndDeadband(rightRearPower);
    
    // Raises the ESTOP lines before commanding the motors
    digitalWrite(sabertoothEstop, HIGH);
    
    // Applies our calculated and bounded values to our drive motor controllers
    frontSaber.motor(1, rightFrontPower);
    frontSaber.motor(2, leftFrontPower);
    rearSaber.motor(1, rightRearPower);
    rearSaber.motor(2, leftRearPower);   
  }
  
  // If the mode value is not "1" then we are in tank mode
  else 
  {
    // Applies our calculated and bounded values to our drive motor controllers
    frontSaber.drive(yAxis);
    frontSaber.turn(xAxis);
    
    rearSaber.drive(yAxis);
    rearSaber.turn(xAxis);
  }
}

//******************************************************************************
// Cleans up our values for the motor controllers 
// The motor controllers only accept a value range of -127 to 127. We also apply
// a deadband so the robot doesn't drift when idle
// 
// RETURNS: Cleaned up value
//******************************************************************************
int boundAndDeadband (int inputValue) 
{
  if (inputValue < -127)  { inputValue = -127; }
  if (inputValue > 127)   { inputValue = 127; }
  if ((inputValue < 5) && (inputValue > -5)) { inputValue = 0; }

  return inputValue; 
}
