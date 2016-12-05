// ****************************************************
// Libraries
// ****************************************************
#include <SoftwareSerial.h>
#include <Sabertooth.h>
#include <stdio.h>
// *********************
// Define hardware pins
// *********************
#define sabertoothEstop 4 // This is connected to S2 on the motor controllers. 
// When this pin is pulled low the motors will stop.

// Declaration of the software serial UART and motor controller objects
SoftwareSerial SWSerial(2, 3); // RX on pin 2 (unused), TX on pin 3 (to S1).
Sabertooth frontSaber(128, SWSerial); // Address 128, front motors, 1 = left, 2 = right
Sabertooth rearSaber(129, SWSerial);  // Address 130, rear motors, 1 = left, 2 = right
//******************************************************************************
// Sets up our serial com, hardware pins, SPI bus for ethernet shield and motor controller.
// RETURNS: Nothing
//******************************************************************************


void setupMotors()
{

    delay(1000);           // Short delay to allow the motor controllers
    // to power up before we attempt to send it commands.
    // If you try to talk to them before the finish booting
    // they can lock up and be unresponsive

    SWSerial.begin(9600);  // Serial for the motor controllers

    frontSaber.autobaud(); // Allows the motor controllers to detect the baud rate
    rearSaber.autobaud();  // we're using and adjust appropriately

    // Initialize GPIO inputs and outputs
    pinMode(sabertoothEstop, OUTPUT);

    //  allStop();    // Make sure all motors are stopped for safety
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
