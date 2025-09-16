#include "engine.h"


/*
	Simple vector adder.
*/
void	vectAdd(Vect3 *out, Vect3 *vect1, Vect3 *vect2)
{
	out->x = vect1->x + vect2->x;
	out->y = vect1->y + vect2->y;
	out->z = vect1->z + vect2->z;
}

/*
	Simple vector substractor.
*/
void	vectSub(Vect3 *out, Vect3 *vect1, Vect3 *vect2)
{
	out->x = vect1->x - vect2->x;
	out->y = vect1->y - vect2->y;
	out->z = vect1->z - vect2->z;
}

/*
	Divide a vector by a coefficient.
*/
void	vectDiv(Vect3 *out, float coeff)
{
	out->x /= coeff;
	out->y /= coeff;
	out->z /= coeff;
}

/*
	Returns the norm of the vector
*/
float	vectNorm(Vect3 *vect1)
{
	return (fast_sqrt(vect1->x * vect1->x + vect1->y * vect1->y + vect1->z * vect1->z));
}

/*
	Compute the cross product of two vectors.
*/
void	vectCross(Vect3 *out, Vect3 *vect1, Vect3 *vect2)
{
	out->x = vect1->y * vect2->z - vect1->z * vect2->y;
	out->y = vect1->z * vect2->x - vect1->x * vect2->z;
	out->z = vect1->x * vect2->y - vect1->y * vect2->x;
}

/*
	Returns the dot product of two vectors
*/
double	vectDot(Vect3 *vect1, Vect3 *vect2)
{
	return (vect1->x * vect2->x + vect1->y * vect2->y + vect1->z * vect2->z);
}
