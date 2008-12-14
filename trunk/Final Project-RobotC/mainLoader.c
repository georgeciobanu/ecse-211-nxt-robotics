





intialHeading();

int arrived = NOT_ARRIVED;
int state = COLLECTING_BALLS;
	
while (true){
	while (ballsCollected < 3 || (TimeSpent == 5 minutes){
		int result = navigate(x,y, ignoreBalls = false);

		if (result == ARRIVED_AT_TARGET){
			SelectNextWaipoint(global array of waypoints);
		}
		else //(result == FOUND_BALL)
		{			
			findBallHeading();
			FindBallDistance();
			int isLoaded = loadBall();
			if (isLoaded == true)
				ballsCollected++;
		}
	}

	//At this point we should have 3 balls or be late

	sendToScooper(GOTO_XYTHETA, x,y,theta close to me);

	while(!received confirmation from Scooper) //Wait until it gets here
		wait(1msec); 
	
	sendToScooper(lowerScoop);

	while(!received confirmation from Scooper) //Wait until it gets here
		wait(1msec);

	unload_Ball(); //1 or three balls??

	sendToScooper(LIFT_SCOOP);

	while(!received confirmation from Scooper) //Wait until it gets here
		wait(1msec);

	sendToScooper(UNLOAD_BALLS);

	while(!received confirmation from Scooper) //Wait until it gets here
		wait(1msec);

	sendToScooper(GO_TO_WAITING_STATE); //wherever the hell that is

	while(!received confirmation from Scooper) //Wait until it gets here
		wait(1msec);

	ballsCollected = 0;
}
	

					

						

		
	


	