
/* This version has only been tested in a stand-alone environment.....

Final testing will be done after integrating with the loading/ unloading code.....


QUESTION : WOULD IT BE BETTER IF I MAKE THE RAISE AND LOWER SUBROUTINES INTO TASKS

THAT WAY I COULD CONTROL THEM USING hogCpu() and releaseCpu() ( EQUIVALENT TO MUTEXES IN NXC)
*/


//BLUETOOTH VARIABLES
long nLastXmitTimeStamp = nPgmTime;
long nDeltaTime					= 0;

const int kTimeBetweenXmit = 30;
const int kMaxSizeOfMessage = 8;
int nBTCmdErrorStatus;

ubyte dataReceived[kMaxSizeOfMessage];
ubyte dataToSend[kMaxSizeOfMessage];


void RotateMotor(short motorX, int power, int degrees){
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
  RotateMotor(motorA,30,60);
  //return true;
}


void raise()
{

  // receive signal from the loader
  wait1Msec(500);
  RotateMotor(motorA,-50,60);
  //return true;
}


void clearBTMessage() {
	for(int i=0;i<kMaxSizeOfMessage;i++){
	  dataToSend[i]=10;
	  dataReceived[i]=10;}

	  dataToSend[kMaxSizeOfMessage-1]=0;
}

bool readAndSend(TMailboxIDs incomingMailbox, TMailboxIDs outGoingMailbox)
{
  TFileIOResult nBTCmdErrorStatus;
  int nSizeOfMessage;

    // Check to see if a message is available

    nSizeOfMessage = cCmdMessageGetSize(incomingMailbox);
    if (nSizeOfMessage == 0)
    {
      wait1Msec(50);
    } else {
      while(bBTBusy);
      //PlaySound(soundLowBuzz);
      if (nSizeOfMessage > kMaxSizeOfMessage)
        nSizeOfMessage = kMaxSizeOfMessage;

      nBTCmdErrorStatus = cCmdMessageRead(dataReceived, nSizeOfMessage, incomingMailbox);
	    while(bBTBusy) wait1Msec(1);

        if(dataReceived[0]==1) {

TFileIOResult res = cCmdBTCheckStatus(0);
          nBTCmdErrorStatus = cCmdMessageWriteToBluetooth(0, dataToSend, kMaxSizeOfMessage, outGoingMailbox);
    	    while(bBTBusy) wait1Msec(1);


        }


    }

  return false;
}


void checkBTLinkConnected()
{
  if (nBTCurrentStreamIndex >= 0)
    return;

  PlaySound(soundLowBuzz);
  PlaySound(soundLowBuzz);
  eraseDisplay();
  nxtDisplayCenteredTextLine(3, "BT not");
  nxtDisplayCenteredTextLine(4, "Connected");
  wait1Msec(3000);
  StopAllTasks();
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

task communication()
{
  checkBTLinkConnected();
  bNxtLCDStatusDisplay = true;
  while(true) {
   // wait1Msec(50);
    //Process data before sending
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


memset(dataReceived, 0, kMaxSizeOfMessage);
    readAndSend(mailbox1, mailbox11);

    //process data received
    captureBall=(dataReceived[1]==1);
    liftEff=(dataReceived[3]==1);
	  unloadBall=(dataReceived[5]==1);

    clearBTMessage();
  }
}


void initial()
{
  wait1Msec(1000);
  RotateMotor(motorA,40,40)
}
void dropoff(){
  wait1Msec(500);
  RotateMotor(motorA,-80,60);
}

// main was used for testing
task main
{
  wait1Msec(500);
StartTask(communication);
lower();
while(true){
if(liftEff){
	liftEff=false;
	raise();
	wait1Msec(1000);
	dropoff();
}
}


}
