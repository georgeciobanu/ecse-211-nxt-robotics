
//BLUETOOTH VARIABLES
long nLastXmitTimeStamp = nPgmTime;
long nDeltaTime					= 0;

const int kTimeBetweenXmit = 30;
const int dataSize = 8;
int nBTCmdErrorStatus;

ubyte dataReceived[dataSize];
ubyte dataToSend[dataSize];
void clearBTMessage() {
  for(int i=0;i<dataSize;i++){
    dataToSend[i]=10;
    dataReceived[i]=10;
  }
  dataToSend[dataSize-1]=0;
}


#ifdef MASTER

void waitBtIdle(byte slave) {
  while(bBTBusy) wait1Msec(1);
  while(cCmdBTCheckStatus(slave)==ioRsltCommChannelBad);
}

int timeout=0;
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
        if(timeout>1000000) {
          timeout=0;
          break;
        }
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

#else

void waitBtIdle() {
  while(bBTBusy) wait1Msec(1);
  while(cCmdBTCheckStatus(0)==ioRsltCommChannelBad);
}

void prepareDataToSend();

void readAndSend(TMailboxIDs incomingMailbox, TMailboxIDs outGoingMailbox)
{
  TFileIOResult nBTCmdErrorStatus;
  int nSizeOfMessage;

  // Check to see if a message is available

  nSizeOfMessage = cCmdMessageGetSize(incomingMailbox);
  while (nSizeOfMessage == 0)
  {
    nSizeOfMessage = cCmdMessageGetSize(incomingMailbox);
    wait1Msec(50);
  }
    if (nSizeOfMessage > dataSize)
      nSizeOfMessage = dataSize;

      waitBtIdle();
    cCmdMessageRead(dataReceived, nSizeOfMessage, incomingMailbox);
    waitBtIdle();
      if(dataReceived[0]==1) {
        prepareDataToSend();
        waitBtIdle();
        nBTCmdErrorStatus = cCmdMessageWriteToBluetooth(0, dataToSend, dataSize, outGoingMailbox);
        waitBtIdle();
      }
}
#endif


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

void purgeBT(TMailboxIDs box) {
  while(cCmdMessageGetSize(box)>0) {
    cCmdMessageRead(dataReceived, cCmdMessageGetSize(box), box);
    wait1Msec(100);
  }
}
