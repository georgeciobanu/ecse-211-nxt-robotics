#ifndef _CONTROL_
#define _CONTROL_

#include "odometry.c"

#define MAX_TRIES 20 //41
#define degPerMM 2.2
#define degPerDeg 3.1//2.967

#define LEFT_MOTOR	motorA
#define RIGHT_MOTOR motorB

//Go forward a specified number of milimeters
int GoForward(float distMM, int power){
	//Save Theta - for later
	nSyncedMotors = synchNone;
//	bFloatDuringInactiveMotorPWM = false;
	nMotorEncoderTarget[motorA]=distMM*degPerMM;
	nSyncedMotors = synchAB;
	nSyncedTurnRatio = 100; //not 100% because of symmetry issues at low speed
	motor[motorA]=power;


	while(nMotorRunState[motorA]!= runStateIdle);
	nSyncedMotors = synchNone;
	//bFloatDuringInactiveMotorPWM = true;
	return 0;
	//If Theta changed, restore
}

int StartForward(int power) {

/*
	nMotorPIDSpeedCtrl[motorA] = nMotorPIDSpeedCtrl[motorB] = mtrSpeedReg; // enables motor speed regulation
	motor[motorA] = motor[motorB] = power;
	*/

	//Save Theta - for later
	nSyncedMotors = synchNone;
//	bFloatDuringInactiveMotorPWM = false;
	nSyncedMotors = synchAB;
	nSyncedTurnRatio = 100; //not 100% because of symmetry issues at low speed
	motor[motorA]=power;

	return 0;
}

int StartBackward(int power) {

/*
	nMotorPIDSpeedCtrl[motorA] = nMotorPIDSpeedCtrl[motorB] = mtrSpeedReg; // enables motor speed regulation
	motor[motorA] = motor[motorB] = power;
	*/

	//Save Theta - for later
	nSyncedMotors = synchNone;
//	bFloatDuringInactiveMotorPWM = false;
	nSyncedMotors = synchAB;
	nSyncedTurnRatio = 100; //not 100% because of symmetry issues at low speed
	motor[motorA]=-power;

	return 0;
}


int StopMotors() {
	//nMotorPIDSpeedCtrl[motorA] = nMotorPIDSpeedCtrl[motorB] = mtrNoReg;    // disables motor speed regulation
	nSyncedMotors = synchNone;
	motor[motorA] = motor[motorB] = 0;
//	bFloatDuringInactiveMotorPWM = false;
	return 0;
}


//Move backwards a specified number of milimeters
int GoBackwards(long distMM, int power) {
	nSyncedMotors = synchNone;
//	bFloatDuringInactiveMotorPWM=false;
	nMotorEncoderTarget[motorA]=distMM*degPerMM;
	nSyncedMotors = synchAB;
	nSyncedTurnRatio = 100; //not 100% because of symmetry issues at low speed
	motor[motorA]=-power;


	while(nMotorRunState[motorA]!= runStateIdle);
	nSyncedMotors = synchNone;
//	bFloatDuringInactiveMotorPWM=true;
	return 0;
}



//Rotate, relative to the current position, a specified number of degrees
//Negative values mean rotation to the right, positive values to the left
int RotateHelper(float radian, int power) {
	long initial, final ;
//TODO: degPerDeg should depend on power
	if (radian < 0) {
	//	bFloatDuringInactiveMotorPWM = false;
		//nMotorEncoderTarget[motorA]=57.3*(radian) * degPerDeg; //Should this be -=??
		//nSyncedMotors = synchAB;
		//nSyncedTurnRatio = -100; //not 100% because of symmetry issues at low speed

		initial = nMotorEncoder[motorB];
		final = initial + rad2deg(radian)/degPerDeg;

		motor[motorB]=-power;
		motor[motorA] = power;

		while(nMotorEncoder[motorB] > final );
		motor[motorB] = 0;
		motor[motorA]= 0;


//		bFloatDuringInactiveMotorPWM = true;
	}
	else{
	//	bFloatDuringInactiveMotorPWM = false;
		initial = nMotorEncoder[motorB];
		final = initial + rad2deg(radian)/degPerDeg  ;
		motor[motorB]=power;
		motor[motorA] = -power;

		while(nMotorEncoder[motorB] < final);
		motor[motorB] = 0;
		motor[motorA]= 0;

//		bFloatDuringInactiveMotorPWM = true;
	}
	return 0;
}



//Rotates to the absolute heading in radians
int GoToHeading(float NewHeading, int power) {
	float diff;
	NewHeading = NormalizeHeading(NewHeading);

	int tries = 0;
	diff = CircleDistance(NewHeading, theta);
	while(abs(diff)>0.02) { //0.002 for Scooper

		if (tries > 1)
			RotateHelper(diff, 27); //Use low power to converge
		else
			RotateHelper (diff, power);

		tries++;

		//Just in case the algorith does not converge
		if (tries > MAX_TRIES)
			return 0;

		diff = CircleDistance(NewHeading, theta);
		wait10Msec(10);  //Wait a little to converge. Not sure this is needed
	}
	return 0;
}


//Rotate the robot EXACTLY by that many degrees (controlled)
int Rotate(float radian, int power){
	float newHeading = theta+radian;
	newHeading = NormalizeHeading(newHeading);

	GoToHeading(newHeading, power);
	return 0;
}


//Similar to the RotateMotor Function in NXC, used for the Loader and Scooper Motor
//THINGS TO CHECK ASAP:argument "string motorX", right or wrong
//						1. If used to rotate BACKWARDS, does it cause a problem?
//						2. Is nMotorEncoder[motorX] in Degrees????
int RotateMotor(short motorX, int speed, int degrees){

	nMotorEncoder[motorX] = 0;
	while(nMotorEncoder[motorX] < degrees ) {
		motor[motorX] = speed;
	}
	motor[motorX] = 0;

	return 0;
}


int RotateCW(int power) {
	nSyncedMotors = synchAB;
	nSyncedTurnRatio = -100;
	motor[motorA]=power;
}

int RotateCCW(int power) {
	nSyncedMotors = synchBA;
	nSyncedTurnRatio = -100;
	motor[motorB]=power;
	return 0;
}

int StopRotate() {
	motor[motorA]=motor[motorB]=0;
//	bFloatDuringInactiveMotorPWM = false;
	nSyncedMotors = synchNone;
	return 0;
}






//Move to the absolute position given by X and Y
//Very nice function to use if a lot is known about the map
void GoToXY(long xMM, long yMM, int power)
{
float dx = xMM-x;
float dy = yMM-y;
float dxs = dx*dx;
float dys = dy*dy;
dys += dxs;
float distance = sqrt(dys);
float tempuraSauceInAWeirdSpoon;
tempuraSauceInAWeirdSpoon =  dx/distance;
float heading = acos(tempuraSauceInAWeirdSpoon);
GoToHeading(heading, power);
GoForward(distance, power);
}

/*
TODO: check odometry for FW and Rotation - 2 different constants
			fix GoToHeading - currently only moves one way - maybe CircleDistance is wrong?


		Things that crash the compiler:
			-invalid number of params passed to function
			-things like float a = functionCall();
*/
/*task main(){


	StartTask(UpdateValues);
	wait10Msec(50);
	//GoToHeading(2*PI/3, 60);
//	GoForward(90, 30);
Rotate(PI/2, 60);
//GoForward(300,// 60);

	while (true)
		wait10Msec(10000);
}*/


#endif
