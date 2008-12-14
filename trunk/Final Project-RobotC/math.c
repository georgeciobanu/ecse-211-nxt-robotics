#ifndef _MATH_
#define _MATH_




/*
Function that takes a number representing degrees and converts it to the 0-6.283 range

Input: current heading in tenths of a degree (deci degree)
Input can be negative or positive, any value

Returns: current heading in tenths of a degree (deci degree)
It is guaranteed to be positive, in the 0-6.283 range
*/


float NormalizeHeading(float Heading){
	while(Heading<0)
		Heading+=(2*PI);
	while(Heading>2*PI)
		Heading -=(2*PI);

    return Heading;
}



/*
Get shortest distance on a circle

Input: the desired heading, and the current heading, both in the 0-6.283 range
Returns: The number of degrees, signed (positive for right, negative for left), to turn

*/

float CircleDistance(float NewHeading, float CurrentHeading){
	float diff;

	NewHeading = NormalizeHeading(NewHeading);
	CurrentHeading = NormalizeHeading(CurrentHeading);

	diff = NewHeading - CurrentHeading;
	if ( abs(diff) > PI)
		if (diff > 0)
			diff -= 2*PI;
		else diff += 2*PI;

	return diff;
}


#endif
