
initialHeading();

while(true){
	while ( noMessageReceived) ); //Wait here indefinetly
	
	switch (message){
		case GOTO_X,Y,THETA:
		{
			navigate(x,y, ignoreBalls=true);
			gotoHeading(theta);
			send_message_to_slave(CommandPerformed);
		}
		case LOWER_SCOOP:
		{
			lower_scoop();
			send_message_to_slave(CommandPerformed);
		}
		case LIFT_SCOOP: //this is the one where the ball stays in the scoop ok
		{
			lift_scoop();
			send_message_to_slave(CommandPerformed);
		}
		case UNLOAD_BALL:
		{
			go_close_to_bowl();
			find_center_of_bowl();
			find_distance_to_bowl_precisely(); //NOTE THIS CAN BE USED TO RE-ADJUST ODOMETRY
			goToHeading(bowlHeading);
			unload_ball(); //Raise scoop at maximum
			move_away_from_bowl_SAFELY();
			send_message_to_slave(CommandPerformed);
		}
		case GO_TO_WAITING_STATE:
		{
			GoToXY(myOrigin);
			send_message_to_slave(CommandPerformed);
			initialheading(); //Re-calculate heading, for v good precision
		}
	}
}


