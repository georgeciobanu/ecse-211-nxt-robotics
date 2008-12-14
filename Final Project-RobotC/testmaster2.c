//#include "BTlib.nxc"

#define MASTER
#include "bt.c"

#define FOXTROT 0
#define LoaderS 1
#define Curly 2

#pragma platform(NXT)

void approachBall(){
  nSyncedTurnRatio = 100;
  nSyncedMotors = synchAB;
  motor[motorA]=70;


}


bool startCapture = false;
bool stopCapture = false;

bool unloadBall=false;
bool unloadDone=false;

bool liftEff=false;
bool liftDone=false;

int ccc=0;
task communication()
{
  //	 StartTask(PrintInfo);
  clearBTMessage();

  while(true) {

  w1:  nDeltaTime = nPgmTime - nLastXmitTimeStamp;
    if (nDeltaTime < kTimeBetweenXmit || bBTBusy) {
      wait1Msec(10);
      goto w1;
    }

    clearBTMessage();

    if(startCapture){
      startCapture=false;
      dataToSend[1]=1;
    }

    if(unloadBall){
      unloadBall=false;
      dataToSend[5] = 1;
    }


    contact(true, mailbox1, mailbox11, 1);

    stopCapture=(dataReceived[1]==2);

    if(dataReceived[6]==1) {
      liftEff=true;
//      ccc++;
//      nxtDisplayTextLine(5, "SSS");
//      PlaySound(soundBeepBeep);
//      dataToSend[3]=1;
    }



    //    unloadDone=(dataReceived[6]==1);
    //SLAVE1 DATA RECEIVED

  w2:  nDeltaTime = nPgmTime - nLastXmitTimeStamp;
    if (nDeltaTime < kTimeBetweenXmit || bBTBusy) {
      wait1Msec(10);
      goto w2;
    }

    clearBTMessage();

    if(liftEff){
      liftEff=false;
      dataToSend[3]=1;
    }

    contact(true, mailbox1, mailbox11, 2);
    //SLAVE2 DATA RECEIVED


    clearBTMessage();
    wait1Msec(1);		 // Give other tasks a chance to run
  }
}


task main{
  checkBTLinkConnected();
  cCmdBTPurgeRcvBuffer();
  purgeBT(mailbox11);

  StartTask(communication);

  /*startCapture=true;
  approachBall();

  while(!stopCapture) wait1Msec(1);

  motor[motorA]=0;
  nSyncedMotors=synchNone;*/


  unloadBall=true;
  /*
  while(true){
    if(unloadDone){
      unloadDone=false;
      liftEff=true;
    }
  }*/
}
