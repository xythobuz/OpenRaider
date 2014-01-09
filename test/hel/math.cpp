#include <stdio.h>
#include <math.h>

#include <hel/math.h>

void helMathTest()
{
	printf("180/PI: %f, %f, %f\n",
			 HEL_180_OVER_PI,
			 180.0f / HEL_PI,
			 180.0 / M_PI);
}


int main(int argc, char *argv[])
{
	helMathTest();

	return 0;
}

