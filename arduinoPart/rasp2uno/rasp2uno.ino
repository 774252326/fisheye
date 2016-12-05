/*
//=========================HEADER=============================================================
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

int s,d;
float xf,yf,zf;
unsigned long times;
//******************************************************************************
// Sets the speed of all motor controllers to zero and sets all ESTOPs
// RETURNS: NONE
//******************************************************************************

//================================read data=====================================

void serialEvent(){
    if(Serial.available() > 0){
        String input= Serial.readString();
        str2xyz(input,xf,yf,zf);

        times = millis() - times;
        Serial.print("Time : ");
        Serial.print(times);
        times = millis();
        Serial.println("ms");

        Serial.println(input);
        Serial.print(d);
        Serial.print("\t");
        Serial.print(s);
    }Serial.flush();
}


void setup() 
{
    Serial.begin(19200);    // Serial for the debug output
    Serial.setTimeout(5);

    setupMotors();

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

    xyz2speed(d,s,xf,yf,zf,0.5,0.7);

    // ****************  update the speed  ******************* //

    commandMotors(127 + d, 127, 127 + s, 1);

    delay(50);
    //delay(50);
}











