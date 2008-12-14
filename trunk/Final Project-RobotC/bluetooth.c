#define GOTOCUP               1
#define LOWER_SCOOP           2
#define LIFT_SCOOP            3
#define UNLOAD_BALLS          4
#define GO_TO_WAITING_STATE   5
#define CONFIRMATION          6


bool done ;
word msg=0, valpar2=0;
word msgParams [3]= {0,0,0};
bool btsend(par1,par2,par3)
{
  wait1Msec(10);

  while (bBTBusy) wait1Msec(1);   // w1aits in 1 sec progressions if channel is clear or not

  sendMessageWithParm(par1,par2,par3);

  // will return true if it sends else no
  return true;
}

//Patrick Diez suggested that their shud be atleast 30 ms delay in sending and recieving to account for latency issues
bool btreceive()
{

 // while something is available
  while (!bQueuedMsgAvailable() );
/*
  {

    word temp;

    ClearMessage();   // clearing msg but could be tht the first msg is accidentaly deleted send garbage first time

    temp = message;   // Obtain the next message

}


if (message == 0) // if nothing in que

{

   // No message is available to process

    wait1Msec(5);


}*/

// A message is ready to be processed

msgParams[0] = messageParm[0];
msgParams[1] = messageParm[1];
msgParams[2] = messageParm[2];
//msg= message;          // the value of ‘S1’ from the remote NXT

//valpar2 = messageParm[1];  // the value of ‘S2’ from the remote NXT . . . user code to process the message. It may have many delays.


}
