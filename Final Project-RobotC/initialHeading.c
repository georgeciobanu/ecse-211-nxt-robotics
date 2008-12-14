//#include "control.c"

float minDist;
float d1;
float d2;
float h;
float iniH =0;
float secondWallAngle;
float firstWallAngle;
int power = 25;
float theta1;
float theta2;
float theta3;
float theta4;
float toX;
float toY;
//use 3.854 for scooper
//use 4.15  for loader
//center of rotation should be on a cross


#ifdef __SCOOPER__
float l=3.85; //heading of light sensor
#endif

#ifdef __LOADER__
float l=4.15; //heading of light sensor
#endif


int threshold=54;//55;



task getMinDist(){

	minDist = SensorValue[OBSTACLE_SENSOR];
	while(true){
		if (SensorValue[OBSTACLE_SENSOR]<minDist){
			minDist = SensorValue[OBSTACLE_SENSOR];
			h = theta;
		}
	}
}


task getLineAngles(){
	while(SensorValue[lightSensor]>threshold); //light sensor
		theta1 = theta;
	wait10Msec(60);

	while(SensorValue[lightSensor]>threshold); //light sensor
		theta2 = theta;
	wait10Msec(60);

	while(SensorValue[lightSensor]>threshold); //light sensor
		theta3 = theta;
	wait10Msec(80);

	while(SensorValue[lightSensor]>threshold); //light sensor
		theta4 = theta;
	wait10Msec(60);
}


void FindLines(){

	StartTask(getLineAngles);
	RotateHelper(-2*PI-0.05,power);
	StopTask(getLineAngles);

	wait10Msec(100);

	GoToHeading((theta1+theta3)/2+l-PI/2, power);

	if (SensorValue[OBSTACLE_SENSOR]<50){ //if you see a wall
		d1 = SensorValue[OBSTACLE_SENSOR];
		Rotate(-PI/2, power);
		if (SensorValue[OBSTACLE_SENSOR]<50){ //if you see another wall
			d2 = SensorValue[OBSTACLE_SENSOR];
			toY = d1;
			toX = d2;
			Rotate(PI, power);
		}
		else {
			Rotate(PI, power);
			d2 = SensorValue[OBSTACLE_SENSOR];
			toX = d1;
			toY = d2;
			Rotate(PI/2, power);
		}
	}
	else {
		Rotate(-PI/2, power);
		if (SensorValue[OBSTACLE_SENSOR]<50){ //if you see another wall
			d1 = SensorValue[OBSTACLE_SENSOR];
			Rotate(-PI/2, power);
			d2 = SensorValue[OBSTACLE_SENSOR];
			toX = d2;
			toY = d1;
			Rotate(PI, power);
		}
		else{
			Rotate(-PI/2, power);
			d1 = SensorValue[OBSTACLE_SENSOR];
			Rotate(-PI/2, power);
			d2 = SensorValue[OBSTACLE_SENSOR];
			toX = d2;
			toY = d1;
			Rotate(PI, power);
		}

	}

	wait10Msec(100);
	StopTask(UpdateValues);
	theta = 0;
	x = toX*10;
	y = toY*10;
	nMotorEncoder[motorA] = 0;
	nMotorEncoder[motorB] = 0;
	StartTask(UpdateValues);

}

void FindLines2(){

	StartTask(getLineAngles);
	RotateHelper(-2*PI-0.05,power);
	StopTask(getLineAngles);


	wait10Msec(100);

	GoToHeading((theta1+theta3)/2+l-PI/2, power);

	if (SensorValue[OBSTACLE_SENSOR]<50){ //if you see a wall
		d1 = SensorValue[OBSTACLE_SENSOR];
		Rotate(-PI/2, power); //Rotate right
		if (SensorValue[OBSTACLE_SENSOR]<50){ //if you see another wall
			d2 = SensorValue[OBSTACLE_SENSOR];
			toX = 400 - d1;
			toY = d2;
			Rotate(PI, power);
		}
		else {
			Rotate(PI, power);
			d2 = SensorValue[OBSTACLE_SENSOR];
			toX = 400-d2;
			toY = d1;
			Rotate(PI/2, power);
		}
	}
	else {
		Rotate(-PI/2, power);
		if (SensorValue[OBSTACLE_SENSOR]<50){ //if you see a wall
			d1 = SensorValue[OBSTACLE_SENSOR];
			Rotate(-PI/2, power);
			d2 = SensorValue[OBSTACLE_SENSOR];
			toX = 400-d1;
			toY = d2;
			Rotate(PI, power);
		}
		else{
			Rotate(-PI/2, power);
			d1 = SensorValue[OBSTACLE_SENSOR];
			Rotate(-PI/2, power);
			d2 = SensorValue[OBSTACLE_SENSOR];
			toX = 400-d1;
			toY = d2;
			Rotate(PI, power);
		}

	}
	Rotate(-PI/2, power);
	wait10Msec(100);
	StopTask(UpdateValues);
	theta = 0;
	x = toX*10;
	y = toY*10;
	nMotorEncoder[motorA] = 0;
	nMotorEncoder[motorB] = 0;
	StartTask(UpdateValues);
	wait10Msec(100);
	Rotate(PI/2, power);

}

void InitialPosition(){

	StartTask(getMinDist);
	RotateHelper(-2*PI, 35);
	StopTask(getMinDist);


	firstWallAngle = h;

	GoToHeading(firstWallAngle, power); // Face closest wall

	while(SensorValue[OBSTACLE_SENSOR]<30)
		StartBackward(power);
	StopMotors();

	Rotate(-PI/2, power); //rotate 90 degrees clockwise


	if (SensorValue[OBSTACLE_SENSOR]<50){ //if you see a wall

	while(SensorValue[OBSTACLE_SENSOR]<30)
		StartBackward(power);
	StopMotors();

	}
	else {					//if not facing a wall

		Rotate(PI, power);  //face wall

		while(SensorValue[OBSTACLE_SENSOR]<30)
			StartBackward(40);
		StopMotors();

	}
FindLines();

}

void InitialPosition2(){

	StartTask(getMinDist);
	RotateHelper(-2*PI, power);
	StopTask(getMinDist);

	firstWallAngle = h;

	GoToHeading(firstWallAngle, power); // Face closest wall

	while(SensorValue[OBSTACLE_SENSOR]<26)
		StartBackward(power);
	StopMotors();

	Rotate(-PI/2, power); //rotate 90 degrees clockwise


	if (SensorValue[OBSTACLE_SENSOR]<50){ //if you see a wall

	while(SensorValue[OBSTACLE_SENSOR]<26)
		StartBackward(power);
	StopMotors();

	}
	else {					//if not facing a wall

		Rotate(PI, power);  //face wall

		while(SensorValue[OBSTACLE_SENSOR]<26)
			StartBackward(40);
		StopMotors();

	}
	FindLines2();

}
/*
task main(){
	StartTask(UpdateValues);
	InitialPosition();

	while(true)
		wait10Msec(1000);
}
*/
