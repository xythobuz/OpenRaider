/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*================================================================
 *
 * Project : Freyja
 * Author  : Terry 'Mongoose' Hendrix II
 * Website : http://www.westga.edu/~stu7440/
 * Email   : stu7440@westga.edu
 * Object  : Matrix
 * License : No use w/o permission (C) 2002 Mongoose
 * Comments: 3d Matrix class
 *
 *
 *           This file was generated using Mongoose's C++
 *           template generator script.  <stu7440@westga.edu>
 *
 *-- History -------------------------------------------------
 *
 * 2002.05.11:
 * Mongoose - Created
 =================================================================*/

#include <stdio.h>
#include <math.h>
#include <strings.h>

#include <Matrix.h>


/* <Order> is (r)ow or (c)ol */
void generateMatrixSourceTest(char order)
{
	int i, j, k;


	if (order == 'r')
	{
		printf("/* Row order */\n");
	}
	else
	{
		printf("/* Column order */\n");
	}

	for (i = 0; i < 4; ++i)
	{
		for (j = 0; j < 4; ++j)
		{
			if (order == 'r')
			{
				printf("result[%2i] = ", j+i*4);
			}
			else
			{
				printf("result[%2i] = ", j+i*4);
			}

			for (k = 0; k < 4; ++k)
			{
				if (order == 'r')
				{
					printf("a[%2i] * b[%2i]%s",
							  k+i*4, j+k*4, (k == 3) ? ";\n" : " + ");
				}
				else
				{
					printf("a[%2i] * b[%2i]%s",
							 i+k*4, k+j*4, (k == 3) ? ";\n" : " + ");
				}

				//sum+=(elements[i+k*4]*m.elements[k+j*4]);
			}

			//result.elements[i+j*4]=sum;
		}

		printf("\n");
	}

	printf("\n");

	printf("/* Transpose */\n");
	for(i = 0; i < 4; ++i)
	{
		for (j = 0; j < 4; ++j)
		{
			printf("a[%2i] = b[%2i]%s",
					 j+i*4, i+j*4, (j == 3) ? ";\n" : "; ");
		}
	}
}


int runMatrixUnitTest()
{
	unsigned int i, errs;
	Matrix a, b, c;
	matrix_t m;


	// Test 3 cases of identity use
	for (errs = 0, i = 0; i < 3; ++i)
	{
		// Fill A matrix with garbage
		m[ 0] = m[ 1] = m[ 2] = m[ 3] = 45.0f;
		m[ 4] = m[ 5] = m[ 6] = m[ 7] = 90.0f;
		m[ 8] = m[ 9] = m[10] = m[11] = 180.0f;
		m[12] = m[13] = m[14] = m[15] = 270.0f;
		a.setMatrix(m);

		switch (i)
		{
		case 0:
			printf("Set to Identity");
			a.setIdentity();
			break;
		case 1:
			printf("Identity * Identity");
			c.setIdentity();
			b.setIdentity();
			a = c * b;
			break;
		case 2:
			printf("Identity *= Identity");
			a.setIdentity();
			b.setIdentity();
			a = a * b;
			break;
		}

		if (a.isIdentity())
		{
			printf(" \t[ Passed ]\n");
		}
		else
		{
			++errs;
			printf(" \t[ Failed ]\a\n"); // beep
			a.print();
		}

		printf("\n");
	}

	/* 2003.06.18, Mongoose - These tests are weak and
		only spot check some of the matrix */


	/* Cheap translation test */
	a.setIdentity();
	printf("I -> Translate (10, 20, 30)\n");
	a.translate(10, 20, 30);

#ifdef COLUMN_ORDER
	unsigned char i0  = 0, i1  = 4, i2  =  8, i3  = 12;
	unsigned char i4  = 1, i5  = 5, i6  =  9, i7  = 13;
	unsigned char i8  = 2, i9  = 6, i10 = 10, i11 = 14;
	unsigned char i12 = 3, i13 = 7, i14 = 11, i15 = 15;
#else
	unsigned char i0  =  0, i1  =  1, i2  =  2; // i3  =  3
	unsigned char i4  =  4, i6  =  6; // i5  =  5, i7  =  7
	unsigned char i8  =  8, i9  =  9, i10 = 10; // i11 = 11
	unsigned char i12 = 12, i13 = 13, i14 = 14, i15 = 15;
#endif

	if (a.mMatrix[i12] != 10 ||
		 a.mMatrix[i13] != 20 ||
		 a.mMatrix[i14] != 30)
	{
			++errs;
			printf(" \t[ Failed ]\a\n"); // beep
			a.print();
	}

	/* Cheap X rotation test */
	a.setIdentity();
	printf("I -> Rotate (90 degrees, 0, 0)\n");
	a.rotate(90*0.01745329251994329f, 0, 0);

	if (a.mMatrix[i0] != 1 || a.mMatrix[i15] != 1 ||
		 a.mMatrix[i9] != -1 || a.mMatrix[i6] != 1)
	{
			++errs;
			printf(" \t[ Failed ]\a\n"); // beep
			a.print();
	}

	/* Cheap Y rotation test */
	a.setIdentity();
	printf("I -> Rotate (0, 90 degrees, 0)\n");
	a.rotate(0, 90*0.01745329251994329f, 0);

	if (a.mMatrix[i8] != 1 || a.mMatrix[i2] != -1 ||
		 a.mMatrix[i15] != 1)
	{
			++errs;
			printf(" \t[ Failed ]\a\n"); // beep
			a.print();
	}

	/* Cheap Z rotation test */
	a.setIdentity();
	printf("I -> Rotate (0, 0, 90 degrees)\n");
	a.rotate(0, 0, 90*0.01745329251994329f);

	if (a.mMatrix[i4] != -1 || a.mMatrix[i15] != 1 ||
		 a.mMatrix[i1] != 1 || a.mMatrix[i10] != 1)
	{
			++errs;
			printf(" \t[ Failed ]\a\n"); // beep
			a.print();
	}

	printf("\n%i errors\n", errs);
	printf("\n");

	printf("Prescision test...\n");
	printf("I ->\n");
	a.setIdentity();
	printf(" -> Rotate (0, 0, 90 degrees)\n");
	a.rotate(0, 0, 90*0.01745329251994329f);
	printf(" -> Translate (10, 20, 30)\n");
	a.translate(10, 20, 30);
	printf(" -> scale (10, 10, 10)\n");
	a.scale(10, 10, 10);
	a.print();

	printf("\n");

	printf(" -> scale (0.1, 0.1, 0.1)\n");
	a.scale(0.1, 0.1, 0.1);
	printf(" -> Translate (-10, -20, -30)\n");
	a.translate(-10, -20, -30);
	printf(" -> Rotate (0, 0, -90 degrees)\n");
	a.rotate(0, 0, -90*0.01745329251994329f);
	a.print();

	printf("\n%i errors\n", errs);

	return errs;
}


int main(int argc, char *argv[])
{
	if (argc > 2)
	{
		if (strcmp(argv[1], "-src") == 0)
		{
			generateMatrixSourceTest(argv[2][0]);
			return 0;
		}
	}

	printf("[Matrix class test]\n");
	runMatrixUnitTest();

	return 0;
}

