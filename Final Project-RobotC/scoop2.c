
/* This version has only been tested in a stand-alone environment.....

Final testing will be done after integrating with the loading/ unloading code.....


QUESTION : WOULD IT BE BETTER IF I MAKE THE RAISE AND LOWER SUBROUTINES INTO TASKS

THAT WAY I COULD CONTROL THEM USING hogCpu() and releaseCpu() ( EQUIVALENT TO MUTEXES IN NXC)
*/

#include "bt.c"
#include "control.c"

void RotateMotorEx(short motorX, int power, int degrees){
  // 	bFloatDuringInactiveMotorPWM = false;
  /*	nMotorEncoderTarget[motorX]=degrees;
  motor[motorX]=speed;


  while(nMotorRunState[motorX]!= runStateIdle);
  */

  long initial;
  initial = nMotorEncoder[motorX];
  motor[motorX] = power;
  if (power >= 0)
    while (nMotorEncoder[motorX] < initial + degrees);//Too many degrees!
  else
    while (nMotorEncoder[motorX] > initial - degrees);

  motor[motorX]=0;
}



// lower and raise subroutines are based on the fact tht the scooper is fully retracted

void lower()
{

  // recieve signal from the ball detection code

  wait1Msec(1000);
  RotateMotorEx(motorC,30,60);
  //return true;
}

void dropoff();
void raise()
{

  // receive signal from the loader
  wait1Msec(500);
  RotateMotorEx(motorC,-50,60);
  //return true;
}

//booleans for capture ball
bool captureBall=false;
bool stopCapture=false;

//boolean for rendez-vous
bool maximumBall=false;

//booleans for lifting
bool liftEff=false;
bool liftDone=false;

//booleans for unloading
bool unloadBall = false;
bool unloadDone = false;

void prepareDataToSend() {
    if(stopCapture) {
      stopCapture=false;
      dataToSend[1] =2;
    }

    if(maximumBall){
      dataToSend[2] = 1;
    }

    if(liftDone){
      liftDone=false;
      dataToSend[4] =1;
    }

    if(unloadDone){
      unloadDone=false;
      dataToSend[6] = 1;
    }
}
int cou=0;
task communication()
{
  while(true) {
    wait1Msec(50);

    clearBTMessage();
    //Process data before sending
    readAndSend(mailbox1, mailbox11);

    //process data received
    captureBall=(dataReceived[1]==1);
    liftEff=(dataReceived[3]==1);
        if(liftEff){
      liftEff=false;
      cou++;
      raise();
      wait1Msec(1000);
      dropoff();
    }

    unloadBall=(dataReceived[5]==1);

    clearBTMessage();
  }
}


void initial()
{
  wait1Msec(1000);
  RotateMotorEx(motorC,40,40)
}
void dropoff(){
  wait1Msec(500);
  RotateMotorEx(motorC,-30,60);
}

// main was used for testing
task main
{
  //checkBTLinkConnected();
  //cCmdBTPurgeRcvBuffer();
  //purgeBT(mailbox1);
  //wait1Msec(500);
  //StartTask(communication);
  lower();
  wait1Msec(15000);
  raise();
  wait1Msec(1000);
  RotateHelper(deg2rad(90), 30);
  dropoff();

  /*
  while(true){
    if(liftEff){
      liftEff=false;
      raise();
      wait1Msec(1000);
      dropoff();
    }
  }
  */
}
