//#include "BTlib.nxc"
#pragma platform(NXT)


long nLastXmitTimeStamp = nPgmTime;
long nDeltaTime         = 0;

const int kTimeBetweenXmit = 100;
const int dataSize = 5;


ubyte dataToSend[dataSize];
ubyte dataReceived[dataSize];

TFileIOResult nBTCmdErrorStatus;

int timeout=0;

void waitBtIdle(byte slave) {
  while(bBTBusy) wait1Msec(1);
  while(cCmdBTCheckStatus(slave)==ioRsltCommChannelBad);
}

void contact(bool waitForReply, TMailboxIDs outgoingMailbox, TMailboxIDs incomingMailbox, byte slave) {
  if(waitForReply) dataToSend[0]=1;
  // For NXT-G compatability, last byte of message must be zero because of string messsages.

  waitBtIdle(slave);
  nBTCmdErrorStatus = cCmdMessageWriteToBluetooth(slave, dataToSend, dataSize, outgoingMailbox);
	waitBtIdle(slave);

  switch (nBTCmdErrorStatus)
  {
  case ioRsltSuccess:
  case ioRsltCommPending:
    nLastXmitTimeStamp = nPgmTime;
	  if(waitForReply) {
	    int nSizeOfMessage = cCmdMessageGetSize(incomingMailbox);
	    while(nSizeOfMessage==0) {
	      nSizeOfMessage = cCmdMessageGetSize(incomingMailbox);
	      timeout++;
	      if(timeout>1000000) break;
	    }

	    if (nSizeOfMessage > dataSize)
	      nSizeOfMessage = dataSize;

      waitBtIdle(slave);
	    nBTCmdErrorStatus = cCmdMessageRead(dataReceived, nSizeOfMessage, incomingMailbox);
      waitBtIdle(slave);
	  }
    break;

  case ioRsltCommChannelBad:
  default:
    break;
  }

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

void clearReceiveMessage() {
  for(int i=0;i<dataSize;i++) {
    dataReceived[i]=255;
  }
}

void clearSendMessage(){
  for(int i=0;i<dataSize;i++)
	  dataToSend[i]=255;
	dataToSend[dataSize-1]=0;
}

inline void clearBuffers() {
  clearReceiveMessage();
  clearSendMessage();
}

void purgeBT(TMailboxIDs box) {
  while(cCmdMessageGetSize(box)>0) {
    cCmdMessageRead(dataReceived, cCmdMessageGetSize(box), box);
    wait1Msec(100);
  }
//  while(nxtReadRawBluetooth(dataReceived, dataSize)!=0);
}


ubyte cc=255;
bool send1=true;
task main()
{
  checkBTLinkConnected();
  cCmdBTPurgeRcvBuffer();

  purgeBT(mailbox11);

  clearBuffers();

  while(true) {
  w1:  nDeltaTime = nPgmTime - nLastXmitTimeStamp;
    if (nDeltaTime < kTimeBetweenXmit || bBTBusy) {
      wait1Msec(10);
      goto w1;
    }

	clearBuffers();
	if(send1) {
		send1=false;
		dataToSend[1]=1;
	}
    contact(true, mailbox1, mailbox11, 1);
	if(dataReceived[2]==1) {
		PlaySound(soundBeepBeep);
	}
    clearBuffers();
	/*
  w2:  nDeltaTime = nPgmTime - nLastXmitTimeStamp;
    if (nDeltaTime < kTimeBetweenXmit || bBTBusy) {
      wait1Msec(10);
      goto w2;
    }

    contact(true, mailbox1, mailbox11, 2);
    clearBuffers();
	dataToSend[4] = dataReceived[4];
	*/
    wait1Msec(1);		 // Give other tasks a chance to run
  }
}
