//#pragma config(Sensor, S1,     ,                    sensorLightActive)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#define BALLMIN 25


//global variables
int ballnumber=0; //ballno
int startLoader=2;//when 1 is sent from the master, CaptureBall is called, at the end, startLoader is set back to 2


void RotateMotor(short motorX, int speed, unsigned int degrees){
  nMotorEncoderTarget[motorx]=degrees;
  motor[motorX]=speed;


  while(nMotorRunState[motorX]!= runStateIdle);
  //	bFloatDuringInactiveMotorPWM = false;


}



//////////////
//Puts the ball inside the mill thingy
////////////////////

void initprepBall(int ballno){

  if(ballno != 3){

    RotateMotor(motorC, 30, 90);
    wait1Msec(1500);}

}


//Checks that the ball has indeed been loaded
bool prepBall(){

  RotateMotor(motorC, -15, 50);

  wait1Msec(2000);

  if(SensorValue(S1) >= BALLMIN){
    RotateMotor(motorC, 15, 50);
    return true;}

  else
  {RotateMotor(motorC, 15, 50);
    return false;
  }
}



//LOWER THE END EFFECTOR

void lowerEff(){

  RotateMotor(motorA, 30, 118);

}


void raiseEff(){
  RotateMotor(motorA,-30, 118);
}



void syncRaise(){

  RotateMotor(motorA,-10, 118);

}

/////////////////
//LOAD FUNCTION
//////////////

void Loader(){

  //  i is a counter that is incremented each time the loader function is called, by the ball detection code */

  if (ballnumber<4) {
    ballnumber++;

    switch(ballnumber)
    {

    case 1: // load the first ball
      {

        //Wait(1000);  // loader waits 1 second

        RotateMotor(motorC,30,360);
        wait1Msec(200);
        break;

      }

    case 2:
      {
        //wait1Msec(1000);  // loader wait1Msecs 1 second

        RotateMotor(motorC,40,360);
        wait1Msec(150);
        break;

      }

    case 3:
      {

        //wait1Msec(1000);  // loader wait1Msecs 1 second

        RotateMotor(motorC,30,110);
        wait1Msec(100);
        break;
      }

    }
}}



//////////////////////////////////////
//unload function.
/////////////////////////////////

void unload(int ballno){

  initprepBall(ballno);

  while(!prepBall()){
    prepBall();    //prepball to put ball in correct location
  }

  wait1Msec(1500);

  RotateMotor(motorC, -30, 90);   //eject

  ballnumber--;

  wait1Msec(1000);
  lowerEff();
}





void captureBall(){


  while(true){
    if(SensorValue[S4] >= BALLMIN){
      Loader();
      startLoader=2;
      //Change variable to notify capture
      break;}

  }


}


//Communication task
task sending(){





}

void receive(){


  //

  // Skip to the last message received

  //

  while (bQueuedMsgAvailable())

  {

    word temp;

    ClearMessage();   // We�re ready to process the next message

    temp = message;   // Obtain the next message

}


if (message == 0)

{

   // No message is available to process

    wait1Msec(5);



}

// A message is ready to be processed


startLoader = message;// the value of �S1� from the remote NXT
ClearMessage();
int remoteSensor2 = messageParm[1];   // the value of �S2� from the remote NXT . . . user code to process the message. It may have many delays.



}




task main{
SensorType[S4] = sensorLightActive;
SensorMode[S4] = modePercentage;
//StartTask(receive);
while(true){
	receive();
if(ballnumber <3){


if(startLoader == 1)
captureBall();

}}

}