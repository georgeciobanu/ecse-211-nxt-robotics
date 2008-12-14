
/* This version has only been tested in a stand-alone environment.....

Final testing will be done after integrating with the loading/ unloading code.....


QUESTION : WOULD IT BE BETTER IF I MAKE THE RAISE AND LOWER SUBROUTINES INTO TASKS

THAT WAY I COULD CONTROL THEM USING hogCpu() and releaseCpu() ( EQUIVALENT TO MUTEXES IN NXC)
*/





void RotateMotor(short motorX, int speed, unsigned int degrees){
	nMotorEncoderTarget[motorx]=degrees;
	motor[motorX]=speed;


	while(nMotorRunState[motorX]!= runStateIdle);
	//	bFloatDuringInactiveMotorPWM = false;


}



// lower and raise subroutines are based on the fact tht the scooper is fully retracted

void lower()
{

// recieve signal from the ball detection code

//wait1Msec(1000);
RotateMotor(motorA,10,160);
//return true;
}


bool raise()
{

// receive signal from the loader
wait1Msec(500);
RotateMotor(motorA,-10,90);
return true;
}


// i thought abt denoting a value of 1 for lower confirmation and 2 for raise confirmation

/*task BtSend()
{

while(true)
   {

if (lower())
     {

// send confirmation to loader a value of 1 per-say.... Causing it to raising its endeffector in sync with the scoop

      }

if ( raise())
    {

// send confirmation to loader a value of 2 per-say .... Causing it to back off and give room to the scooper bot


    }

  }

}
*/

// main was used for testing

task main()
{

// StartTask (BtSend);

lower();
raise();


}
