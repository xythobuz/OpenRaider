#include "Vector.h"


int main()
{
   Vector<int> numbers;
   int num[8];
   int fill = 32;
   unsigned int i;


   num[0] = 14;
   num[1] = 311;   

   for (i = 2; i < 8; ++i)
   {
     num[i] = fill;
   }

   numbers.pushBack(num[0]);
   numbers.pushBack(num[1]);
   numbers.resize(8, fill);

   for (numbers.mIndex = numbers.begin(); numbers.mIndex < numbers.end(); 
	++numbers.mIndex)
   {
     printf("numbers[%i] = %3i   |   numbers[%i] = %3i\n",  
	    numbers.mIndex, num[numbers.mIndex],
	    numbers.mIndex, numbers[numbers.mIndex]);
   }

   printf("\n");

   for (numbers.start(); numbers.forward(); numbers.next())
   {
     printf("numbers[%i] = %3i   |   numbers[%i] = %3i\n",  
	    numbers.mIndex, num[numbers.mIndex],
	    numbers.mIndex, numbers[numbers.mIndex]);
   }

   printf("\n");

   for (numbers.finish(); numbers.backward(); numbers.prev())
   {
     printf("numbers[%i] = %3i   |   numbers[%i] = %3i\n",  
	    numbers.mIndex, num[numbers.mIndex],
	    numbers.mIndex, numbers[numbers.mIndex]);
   }

   return 0;
}
