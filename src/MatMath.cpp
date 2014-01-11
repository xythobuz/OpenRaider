/*!
 *
 * \file src/MatMath.cpp
 * \brief Vector and Matrix math
 *
 * \author Mongoose
 */

#include <stdlib.h>
#include <math.h>

#include <MatMath.h>
#include <Vector3d.h>
#include <Matrix.h>

bool tmpHelSphereIntersectLine(Vector3d pos, Vector3d lastPos,
										 Vector3d center, vec_t radius)
{
	Vector3d seg, segToCenter, delta;
	vec_t s, dSquare;


	seg = pos - lastPos;
	segToCenter = center - lastPos;

	s = seg * segToCenter;

	if (s >= 1.0f || s <= 0.0f)
		return false;

	seg.normalize();
	seg = seg * s;
	seg = seg + lastPos;

	delta = seg - center;

	dSquare = delta * delta;

	if (radius >= dSquare)
		return true;
	else
		return false;
}


vec_t helIntersectionOfAbstractSpheres(vec3_t centerA, vec_t radiusA,
													vec3_t centerB, vec_t radiusB)
{
	Vector3d a = Vector3d(centerA);
	Vector3d b = Vector3d(centerB);
	Vector3d d = a - b;
	vec_t dist, minDist;

	dist = Vector3d::dot(d, d);
	minDist = radiusA + radiusB;

	return (dist <= minDist * minDist);
}


inline vec_t square(vec_t a)
{
	return a * a;
}


// Returns number of intersections and intersection position(s)
// Got algorithm from http://astronomy.swin.edu.au/~pbourke/geometry/
int helIntersectionOfAbstractSphereAndLine(vec3_t center, vec_t radius,
														 vec3_t posA, vec3_t posB,
														 vec3_t intersectionA,
														 vec3_t intersectionB)
{
	// float x , y , z;
	vec_t a, b, c, mu, i ;


	a = (square(posB[0] - posA[0]) +
		  square(posB[1] - posA[1]) +
		  square(posB[2] - posA[2]));
	b = (2 * ((posB[0] - posA[0]) * (posA[0] - center[0]) +
				 (posB[1] - posA[1]) * (posA[1] - center[1]) +
				 (posB[2] - posA[2]) * (posA[2] - center[2])));
	c = (square(center[0]) + square(center[1]) +
		  square(center[2]) + square(posA[0]) +
		  square(posA[1]) + square(posA[2]) -
		  2 * (center[0]*posA[0] + center[1]*posA[1] + center[2]*posA[2]) -
		  square(radius));

	i = b * b - 4 * a * c;


	if (i < 0.0)
	{
		// No intersection
		return 0;
	}
	else if (i == 0.0)
	{
		// One intersection
		mu = -b/(2*a) ;
		intersectionA[1] = posA[0] + mu*(posB[0]-posA[0]);
		intersectionA[2] = posA[1] + mu*(posB[1]-posA[1]);
		intersectionA[3] = posA[2] + mu*(posB[2]-posA[2]);

		return 1;
	}
	else
	{
		// Two intersections

		// First intersection
		mu = (-b + sqrt( square(b) - 4*a*c)) / (2*a);
		intersectionA[1] = posA[0] + mu*(posB[0]-posA[0]);
		intersectionA[2] = posA[1] + mu*(posB[1]-posA[1]);
		intersectionA[3] = posA[2] + mu*(posB[2]-posA[2]);

		// Second intersection
		mu = (-b - sqrt(square(b) - 4*a*c)) / (2*a);
		intersectionB[0] = posA[0] + mu*(posB[0]-posA[0]);
		intersectionB[1] = posA[1] + mu*(posB[1]-posA[1]);
		intersectionB[2] = posA[2] + mu*(posB[2]-posA[2]);

		return 2;
	}
}


int helIntersectionLineAndPolygon(vec3_t intersect,
											 vec3_t p1, vec3_t p2,
											 unsigned int vertexCount, vec3_t *polygon)
{
	//	vec3_t normal, a, b;
	Vector3d a, b, normal, pA, pB;
	vec_t d, denominator, mu;
	double theta;


	pA = Vector3d(p1);
	pB = Vector3d(p2);

	// Find normal
	//mtkVectorSubtract(polygon[1], polygon[0], a);
	a = Vector3d(polygon[1]) - Vector3d(polygon[0]);
	//mtkVectorSubtract(polygon[2], polygon[0], b);
	b = Vector3d(polygon[2]) - Vector3d(polygon[0]);
	normal = Vector3d::cross(a, b);
	//mtkVectorCrossProduct(a, b, normal);
	normal.normalize();
	//mtkVectorNormalize(normal, normal);

	// find D
	//d = (normal[0] * polygon[0][0] -
	//	  normal[1] * polygon[0][1] -
	//	  normal[2] * polygon[0][2]);
	d = (normal.mVec[0] * polygon[0][0] -
		  normal.mVec[1] * polygon[0][1] -
		  normal.mVec[2] * polygon[0][2]);

	// line segment parallel to plane?
	//mtkVectorSubtract(p2, p1, a); // cache p2 - p1 => a
	a = pB - pA;

	//denominator = (normal[0] * a[0] +
	//					normal[1] * a[1] +
	//					normal[2] * a[2]);
	denominator = Vector3d::dot(normal, a);

	if (denominator > 0.0)
		return 0;

	// Line segment contains intercept point?
	//mu = - ((d + normal[0] * p1[0] + normal[1] * p1[1] + normal[2] * p1[2]) /
	//		  denominator);
	mu = -((d + Vector3d::dot(normal, pA)) / denominator);

	if (mu < 0.0 || mu > 1.0)
		return 0;

	//intersect[0] = p1[0] + mu * a[0];
	//intersect[1] = p1[1] + mu * a[1];
	//intersect[2] = p1[2] + mu * a[2];
	b = pA + (a * mu);
	intersect[0] = b.mVec[0];
	intersect[1] = b.mVec[1];
	intersect[2] = b.mVec[2];


	// See if the intercept is bound by polygon by winding number
#ifdef WINDING_NUMBERS_TRIANGLE
	mtkVectorSubtract(polygon[0], intersect, a);
	mtkVectorNormalize(a, a);
	mtkVectorSubtract(polygon[1], intersect, b);
	mtkVectorNormalize(b, b);
	mtkVectorSubtract(polygon[2], intersect, c);
	mtkVectorNormalize(c, c);

	t0 = mtkVectorDotProduct(a, b);
	t1 = mtkVectorDotProduct(b, c);
	t2 = mtkVectorDotProduct(c, a);

	total = HEL_RAD_TO_DEG(acos(t0) + acos(t1) + acos(t2));

	if (total - 360 < 0.0)
		return 0;
#else // assume convex polygons here for sure
	//mtkVectorSubtract(intersect, polygon[0], a);
	//theta = mtkVectorDotProduct(a, normal);
	theta = Vector3d::dot(b - Vector3d(polygon[0]), normal); // b = intersect

	if (theta >= 90.0) // Yeah I know
		return 0;
#endif

	return 1;
}


vec_t helDistToSphereFromPlane3v(vec3_t center,	vec_t radius, vec4_t plane)
{
	vec_t d;


	d = (plane[0] * center[0] +
		  plane[1] * center[1] +
		  plane[2] * center[2] +
		  plane[3]);

	if (d <= -radius)
		return 0;

	return d + radius;
}


vec_t helDistToBboxFromPlane3v(vec3_t min, vec3_t max, vec4_t plane)
{
	vec3_t center;
	vec_t d, radius;


	helMidpoint3v(min, max, center);

	d = (plane[0] * center[0] +
		  plane[1] * center[1] +
		  plane[2] * center[2] +
		  plane[3]);

	radius = helDist3v(max, center);

	if (d <= -radius)
		return 0;

	return d + radius;
}


vec_t helDist3v(vec3_t a, vec3_t b)
{
	return (sqrt( ((b[0] - a[0]) * (b[0] - a[0])) +
					  ((b[1] - a[1]) * (b[1] - a[1])) +
					  ((b[2] - a[2]) * (b[2] - a[2]))));
}


void helMidpoint3v(vec3_t a, vec3_t b, vec3_t mid)
{
	mid[0] = (a[0] + b[0]) / 2;
	mid[1] = (a[1] + b[1]) / 2;
	mid[2] = (a[2] + b[2]) / 2;
}


vec_t helNorm4v(vec4_t v)
{
	return (sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2] + v[3]*v[3]));
}


vec_t helNorm3v(vec3_t v)
{
	return (sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]));
}


vec_t helNorm2v(vec2_t v)
{
	return (sqrt(v[0]*v[0] + v[1]*v[1]));
}


vec_t helRandomNum(vec_t from, vec_t to)
{
	return from + ((to - from) * rand() / (RAND_MAX + 1.0));
}

