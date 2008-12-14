
#include "control.c"

//These two can be OR'ed into one variable and returned
#define OBSTACLE_PRESENT 1
#define BALL_PRESENT 2

#define BALL_FOUND 1
#define WALL 2
#define ARRIVED_AT_TARGET 3
#define FATAL_ERROR -1

#define STEER_AWAY -6

#define RIGHT 1
#define LEFT 2


//#define LEFT_MOTOR motorA - Defined in control.c
//#define RIGHT_MOTOR motorB


#define SMALL_ERROR_CM 5 //5 cm
#define LARGE_ERROR_CM 30 //15 cm, to avoid obstacles

#define OBSTACLE_RANGE_CM 40	//This can be much larger
#define BALL_RANGE_CM 40 //Ball is not reliably detected further than this
//Wwe should be able to define a custom range for some functions
float test_ang;
float distToTarget = 90000;
float zzzzzzdist = -1;
bool passedTargetVar = false;

float getAngle(float x1, float y1, float x2, float y2) {
	float a=atan((y2-y1)/(x2-x1));
	if(x2<x1) a+=PI;
	if(a<0) a+=2*PI;

	return a;
}




//Prices subject to change
float safetyMargin(float distance){
	//	return deg2rad(45);


	if (distance > 35)
		return deg2rad(35);
	if (distance > 25)
		return deg2rad(50);
	if (distance > 15)
		return deg2rad(55);
	if (distance > 5)
		return deg2rad(65);
	return deg2rad(80);
}

bool obstacleInFront(float range, int ignoreBalls){
	if (getDistance(OBSTACLE_SENSOR, 2) <= range){	//If we see the ball, there might be an obstacle behind it
			return true;
	}

	//While it is true that the obstacle may be a wall, this is an expensive check and
	//thus must be performed explicitly by the caller, if needed.
	//Most of the time it is not important.

	return false;
}

float euclidDistance(float x1, float y1, float x2, float y2){
	return sqrt( (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) );
}

bool arrived(float targetX, float targetY){
	//	return false;
	distToTarget = euclidDistance(x,y,targetX, targetY);
	//Clear way, just make sure you don't get fooled by roundoff errors
	if (distToTarget   <= SMALL_ERROR_CM * 10)
		return true;

	//We are assuming this is NOT the bowl
	//We need to make sure it is not a ball...not crucial
	//what if we are looking at the cup?
	if (obstacleInFront(OBSTACLE_RANGE_CM, true) && distToTarget <= LARGE_ERROR_CM*10)
		return true;

	//are we there yet??
	return false;
}

float findOpening(int direction, bool ignoreBalls)
{
	//TODO: THIS SHOULD BE PID. Also any code that needs to run at low power under load

	//We need a Watchdog task that looks for signs of trouble, the best indicator being the compass not moving
	//while the robot is supposed to move.
	//It should also look at the light sensor, US sensor(s), as well as wheels moving.

	//OR circle distance > 135
	float initial = theta;
	bool Blocked = false;


	//Try right, at most 90 deg
	RotateCW(20);
	while (
		obstacleInFront(OBSTACLE_RANGE_CM+10,true) &&
	(abs(CircleDistance(initial, theta)) < deg2rad(90) ))
	{
		/*if (!ignoreBalls)
		if (getDistance(BALL_SENSOR, 2) < BALL_RANGE_CM - 10)
		return -1; //We found a ball*/
	}
	StopRotate();

	if (!obstacleInFront(OBSTACLE_RANGE_CM+10, true))
		return RIGHT;


	//Try left 90 degrees
	initial = theta;
	RotateCCW(20);
	while (
		obstacleInFront(OBSTACLE_RANGE_CM,true) &&
	abs(CircleDistance(initial, theta)) < deg2rad(270)
	);

	StopRotate();

	//RotateHelper(deg2rad(7), 30);

	if (!obstacleInFront(OBSTACLE_RANGE_CM,true))
		return LEFT;

	//We are desperate. Is there ANY way out?
	initial = theta;
	RotateCW(20);
	while (
		obstacleInFront(OBSTACLE_RANGE_CM,true) &&
	(abs(CircleDistance(initial, theta)) < degreesToRadians(359) ));
	StopRotate();

	if (!obstacleInFront(OBSTACLE_RANGE_CM, true))
		return LEFT;
	//If yes, rotate in the indicated direction until either an opening is found or
	//we have rotated 135 degrees or so (MAX_DEGREES)
	//If opening not found, go back to initial and search left up to 135 degrees
	//If still not found, go to initial + 135 to the first direction and look for
	//and opening in the next 360-135-135=90 degrees. If nothing found, we are enclosed.

	//Note: this might also discover balls!
	//While rotating, if the ball flag is set, report we found a ball, and it's heading (the first occurence).

	return FATAL_ERROR;
}


int sweepForBalls(int direction, float rads){
	float initial = theta;

	if (direction == RIGHT)
		RotateCW(15);
	else RotateCCW(15);

	float distToBall = 0;
	while ( CircleDistance(theta, initial) < rads ){
		if (getDistance(OBSTACLE_SENSOR, 2) < OBSTACLE_RANGE_CM)
			return STEER_AWAY;

		distToBall= getDistance(BALL_SENSOR, 2);
		if ( distToBall < BALL_RANGE_CM ) //
			if ( getDistance (OBSTACLE_SENSOR, 2) > BALL_RANGE_CM + 30){
			StopRotate();
			return 1;
		}
	}

	StopRotate();

	//	GoToHeading(initial, 30);

	return 0;
}


//Find the edge of the ball, where direction indicates the edge to be found (LEFT or RIGHT)
float findBallEdge(int dir, float precision){
	if (dir == LEFT)
		RotateCCW(7);
	else
		RotateCW(7);

	while (SensorValue[BALL_SENSOR] < BALL_RANGE_CM);
	StopRotate();

	return theta;
}


//Find ball heading
float FindBallHeading(){
	//this has to take into consideration the fact that there might be an obstacle in continuation to the ball
	//or just after it, or close to it
	//So it needs to probe both sensors, to make sure no obstacles are close to it
	//As well as check around the ball

	//Save initial position
	float initial = theta;

	float R, L;

	R = findBallEdge(RIGHT, 10);

	while(SensorValue[BALL_SENSOR] > BALL_RANGE_CM);
	RotateCCW(7);
	RotateHelper(deg2rad(2), 20);
	StopRotate();

	L = findBallEdge(LEFT, 10);

	//Note: L+R / 2 does not work for L=359 and R = 4
	float toAdd = CircleDistance(L,R);
	//	float angle = L + abs(toAdd)/2;
	test_ang = L + abs(toAdd)/2;
	return test_ang;
	//return angle;
}

int navigate (float xTarget, float yTarget, bool ignoreBalls, bool isCupLocation, long timeout){
	//Returns reason for returning
	//	BALL
	//	ARRIVED_AT_TARGET
	//	FATAL_ERROR

	float targetHeading = -1;

	float curDist, prevDist;

	while (!arrived(xTarget, yTarget) ){

		if (ignoreBalls){

			//Orient towards target
			targetHeading = getAngle(x,y,xTarget,yTarget);

			//Rotate towards the target - How do we know we will not bump into something?
			GoToHeading(targetHeading, 30);

			tempX = tempY = 0;
			passedTargetVar = false;
			StartForward(30);
			while ( !obstacleInFront(OBSTACLE_RANGE_CM, ignoreBalls) &&
				!arrived(xTarget,yTarget) &&
			!passedTargetVar)
			{
				prevDist = euclidDistance(xTarget, yTarget, tempX, tempY);
				curDist = euclidDistance(xTarget, yTarget, x, y);
				passedTargetVar = prevDist < curDist;

				if (passedTargetVar == true)
				{
					StopMotors();
					passedTargetVar = true;
					break;
				}
			}
			//GoBackwards a little. If not arrived, rotate towards

			StopMotors();

			if (arrived(xTarget,yTarget))
				return ARRIVED_AT_TARGET;

			if (passedTargetVar){
				passedTargetVar = false;
				continue;				//Reorient towards the target
			}

			//Obstacle found
			float edgeDistance = -1;
			edgeDistance = 	getDistance(OBSTACLE_SENSOR, 3);

			int rotDirection = findOpening(RIGHT, ignoreBalls);

			if (rotDirection == RIGHT)
				RotateHelper( -safetyMargin(edgeDistance), 30 ); //This should look at the sign/direction!
			else
				RotateHelper( safetyMargin(edgeDistance), 30 ); //This should look at the sign/direction!

			//Opening found. Go through it
			//GoForward(edgeDistance * 5, 40);	//Move forward 50% the distance, without checking

			float startX = x, startY = y;
			StartForward(30);
			//TODO: the distance should be less than 255 !!!
			while ( //Move until you passed the obstacle ~ 18 cm +
				!obstacleInFront(OBSTACLE_RANGE_CM, ignoreBalls)
			&& !arrived(xTarget,yTarget)
			&& (euclidDistance(startX, startY, x, y) < 600)
			);


			StopMotors();

		}

		else //We should not ignore balls

		{

			//Orient towards target
			targetHeading = getAngle(x,y,xTarget,yTarget);

			//Rotate towards the target - How do we know we will not bump into something?
			GoToHeading(targetHeading, 30);

			tempX = tempY = 0;
			float lastX = x, lastY = y;
			passedTargetVar = false;
			StartForward(30);
			while ( !obstacleInFront(OBSTACLE_RANGE_CM, ignoreBalls) &&
				!arrived(xTarget,yTarget) &&
			!passedTargetVar)
			{
				prevDist = euclidDistance(xTarget, yTarget, tempX, tempY);
				curDist = euclidDistance(xTarget, yTarget, x, y);
				passedTargetVar = prevDist < curDist;

				if (passedTargetVar == true)
				{
					StopMotors();
					passedTargetVar = true;
					break;
				}

				if (getDistance(BALL_SENSOR, 2) < BALL_RANGE_CM)
					if (getDistance (OBSTACLE_SENSOR, 2) > BALL_RANGE_CM + 30){
					StopMotors();
					return BALL_FOUND;
				}

				//Sweeping code goes here
				if ( (euclidDistance(lastX, lastY, x, y) > 400) ){
					StopMotors();
					lastX = 0;
					lastY = 0;
					int found = 0;
					float initial = theta;

					found = sweepForBalls(RIGHT, deg2rad(70));
					if (found){
						return BALL_FOUND;
					}

					if (found != STEER_AWAY)
						RotateHelper(deg2rad(45), 30);
					else {
						float startX = x, startY = y;
						RotateHelper(deg2rad(90), 30);
						while ( //Move until you passed the obstacle ~ 18 cm +
							!obstacleInFront(OBSTACLE_RANGE_CM, ignoreBalls)
						&& !arrived(xTarget,yTarget)
						&& (euclidDistance(startX, startY, x, y) < 600)
						);


						StopMotors();
					}

					found = sweepForBalls(LEFT, deg2rad(70));
					if (found)
						return BALL_FOUND;

					GoToHeading(initial, 30);
					StartForward(30);
				}
			}

			StopMotors();

			if (arrived(xTarget,yTarget))
				return ARRIVED_AT_TARGET;

			if (passedTargetVar){
				passedTargetVar = false;
				continue;				//Reorient towards the target
			}

			//Obstacle found
			float edgeDistance = -1;
			edgeDistance = 	getDistance(OBSTACLE_SENSOR, 3);

			int rotDirection = findOpening(RIGHT, ignoreBalls);

			if (rotDirection == RIGHT)
				RotateHelper( -safetyMargin(edgeDistance), 30 ); //This should look at the sign/direction!
			else
				RotateHelper( safetyMargin(edgeDistance), 30 ); //This should look at the sign/direction!

			//Opening found. Go through it
			//GoForward(edgeDistance * 5, 40);	//Move forward 50% the distance, without checking

			float startX = x, startY = y;
			StartForward(30);


			if (getDistance(BALL_SENSOR, 2) < BALL_RANGE_CM)
				if (getDistance (OBSTACLE_SENSOR, 2) > BALL_RANGE_CM + 30){
				StopMotors();
				return BALL_FOUND;
			}

			while ( //Move until you passed the obstacle ~ 18 cm +
				!obstacleInFront(OBSTACLE_RANGE_CM, ignoreBalls)
			&& !arrived(xTarget,yTarget)
			&& (euclidDistance(startX, startY, x, y) < 600)
			);


			StopMotors();
		}

	}

	if (arrived(xTarget,yTarget))
		return ARRIVED_AT_TARGET;
	else return FATAL_ERROR;
}


/*
task main() {
StartTask (UpdateValues);
//	StartTask(showStuff);
//StartTask (UpdateFlags);
//GoForward(50, 30);
targetX = 3000;
targetY = 3000;

int stat = navigate(targetX, targetY, false, false, 90000);
//	if (stat == BALL_FOUND)
//		FindBallHeading();

targetX = 0;
//	Rotate(PI/3);
//RotateHelper(PI/2,50);
//GoForward(200, 40);
while (true)
wait10Msec(10);
}*/
