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

/*
	Rotate a vector on itself by an angle on a certain axis.
*/
void vectRotate(Vect3 *out, Vect3 *v, double angle, char axis)
{
    double cos_theta = approx_cos(angle);
    double sin_theta = approx_sin(angle);

    if (axis == 'x') { // Pitch
        out->x = v->x;
        out->y = v->y * cos_theta - v->z * sin_theta;
        out->z = v->y * sin_theta + v->z * cos_theta;
    } else if (axis == 'y') { // Yaw
        out->x = v->x * cos_theta + v->z * sin_theta;
        out->y = v->y;
        out->z = -v->x * sin_theta + v->z * cos_theta;
    } else if (axis == 'z') { // Roll
        out->x = v->x * cos_theta - v->y * sin_theta;
        out->y = v->x * sin_theta + v->y * cos_theta;
        out->z = v->z;
    }
}

/*
	Normalize a vector.
*/
void	vectNormalize(Vect3 *v) {
    double length = fast_sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
    if (length > 0.0) {
        v->x /= length;
        v->y /= length;
        v->z /= length;
    }
}