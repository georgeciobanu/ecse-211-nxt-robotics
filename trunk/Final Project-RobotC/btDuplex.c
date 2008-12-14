
//BLUETOOTH VARIABLES
long nLastXmitTimeStamp = nPgmTime;
long nDeltaTime					= 0;

const int kTimeBetweenXmit = 30;
const int dataSize = 2;
int nBTCmdErrorStatus;

ubyte dataReceived[dataSize];
ubyte dataToSend[dataSize];
void clearBTMessage() {
	dataReceived[0]=dataReceived[1]=0;
	dataToSend[0]=dataToSend[1]=0;
}


#ifdef MASTER
inline void waitBtIdle() {
  while(bBTBusy) wait1Msec(1);
  while(cCmdBTCheckStatus(1)==ioRsltCommChannelBad);
}
void btsend(ubyte data) {
	waitBtIdle();
	dataToSend[0]=data;
	cCmdMessageWriteToBluetooth(dataToSend, dataSize, mailbox1);
	waitBtIdle();
}

ubyte btreceive() {
	int nSizeOfMessage = cCmdMessageGetSize(mailbox11);
	while(nSizeOfMessage==0) nSizeOfMessage = cCmdMessageGetSize(mailbox11);
	waitBtIdle();
	nCmdMessageRead(dataReceived, dataSize, mailbox11);
	waitBtIdle();
	return dataReceived[0];
}
#else
inline void waitBtIdle() {
  while(bBTBusy) wait1Msec(1);
  while(cCmdBTCheckStatus(0)==ioRsltCommChannelBad);
}
void btsend(ubyte data, TMailboxIDs outgoingMailbox) {
	waitBtIdle();
	dataToSend[0]=data;
	cCmdMessageWriteToBluetooth(dataToSend, dataSize, mailbox11);
	waitBtIdle();
}

ubyte btreceive(TMailboxIDs incomingMailbox) {
	int nSizeOfMessage = cCmdMessageGetSize(mailbox1);
	while(nSizeOfMessage==0) nSizeOfMessage = cCmdMessageGetSize(mailbox1);
	waitBtIdle();
	nCmdMessageRead(dataReceived, dataSize, mailbox1);
	waitBtIdle();
	return dataReceived[0];
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

void init() {
	purgeBT(mailbox1);
	purgeBT(mailbox11);
	clearBTMessage();
}