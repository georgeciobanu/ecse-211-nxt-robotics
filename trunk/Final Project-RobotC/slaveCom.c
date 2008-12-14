long nLastXmitTimeStamp = nPgmTime;
long nDeltaTime					= 0;

const int kTimeBetweenXmit = 100;
const int dataSize = 5;
int nBTCmdErrorStatus;

ubyte dataReceived[dataSize];
ubyte dataToSend[dataSize];
ubyte ack[1];
ubyte count=1;

void prepareDataToSend();

void waitBtIdle() {
  while(bBTBusy) wait1Msec(1);
  while(cCmdBTCheckStatus(0)==ioRsltCommChannelBad);
}

bool readAndSend(TMailboxIDs incomingMailbox, TMailboxIDs outGoingMailbox)
{
  TFileIOResult nBTCmdErrorStatus;
  int nSizeOfMessage;

    // Check to see if a message is available

//  waitBtIdle();
    nSizeOfMessage = cCmdMessageGetSize(incomingMailbox);
    while (nSizeOfMessage == 0)
    {
      nSizeOfMessage = cCmdMessageGetSize(incomingMailbox);
      wait1Msec(50);
    }

//      PlaySound(soundLowBuzz);
      if (nSizeOfMessage > dataSize)
        nSizeOfMessage = dataSize;
      waitBtIdle();
      nBTCmdErrorStatus = cCmdMessageRead(dataReceived, nSizeOfMessage, incomingMailbox);
	    waitBtIdle();

      if(dataReceived[0]==1) {
//	  TFileIOResult res = cCmdBTCheckStatus(0);
	        prepareDataToSend();
	        waitBtIdle();
          nBTCmdErrorStatus = cCmdMessageWriteToBluetooth(0, dataToSend, dataSize, outGoingMailbox);
    	    waitBtIdle();
          count++;
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

bool send2=false;

void prepareDataToSend() {
  if(send2) {
    send2=false;
    dataToSend[2]=1;
  }
}

void purgeBT(TMailboxIDs box) {
  while(cCmdMessageGetSize(box)>0) {
    cCmdMessageRead(dataReceived, cCmdMessageGetSize(box), box);
    wait1Msec(100);
  }
//  while(nxtReadRawBluetooth(dataReceived, dataSize)!=0);
}

task main()
{
  checkBTLinkConnected();
  cCmdBTPurgeRcvBuffer();

  purgeBT(mailbox1);
  clearSendMessage();
  while(true) {
  	clearBuffers();
    readAndSend(mailbox1, mailbox11);
  	if(dataReceived[1]==1) {
  		send2=true;
  		PlaySound(soundFastUpwardTones);
  	}
    clearBuffers();
  }
}
