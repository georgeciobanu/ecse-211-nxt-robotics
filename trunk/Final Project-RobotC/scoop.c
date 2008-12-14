
/* This version has only been tested in a stand-alone environment.....

Final testing will be done after integrating with the loading/ unloading code.....


QUESTION : WOULD IT BE BETTER IF I MAKE THE RAISE AND LOWER SUBROUTINES INTO TASKS

THAT WAY I COULD CONTROL THEM USING hogCpu() and releaseCpu() ( EQUIVALENT TO MUTEXES IN NXC)
*/





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


// i thought abt denoting a value of 1 for lower confirmation and 2 for raise confirmation

/*task BtSend()
{

While(true)
{

if (lower())
{

// send confirmation to loader a value of 1 per-say.... Causing it to raising its endeffector in sync with the scoop

}

If ( raise())
{

// send confirmation to loader a value of 2 per-say .... Causing it to back off and give room to the scooper bot


}

}

}
*/


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
  //StartTask (BtSend());
  repeat(10)
  {

  lower();
  raise();
  dropoff();
  initial();
}
  //dropoff();
  //wait1Msec(1000);
  //lower();
  //lower();
  //raise();


}
