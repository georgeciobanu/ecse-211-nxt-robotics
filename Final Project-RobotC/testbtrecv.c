

int remoteSensor1=-1;
int remoteSensor2=-1;
int remoteSensor3=-1;



task main(){
  while (true)

  {

    //
    // Skip to the last message received
    //

    while (bQueuedMsgAvailable())

    {

      word temp;

      ClearMessage();   // We’re ready to process the next message

      temp = message;   // Obtain the next message

    }


    if (message == 0)

    {

      // No message is available to process

      wait1Msec(5);

      continue;

    }

    // A message is ready to be processed


     remoteSensor1 = message;          // the value of ‘S1’ from the remote NXT

    remoteSensor2 = messageParm[1];   // the value of ‘S2’ from the remote NXT . . . user code to process the message. It may have many delays.
     remoteSensor3 = messageParm[2];



  }
}
