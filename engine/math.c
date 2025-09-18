#include "engine.h"

#define SQRT_PRECISION_DELTA 0.00005

/*
	Computes the square root of a number.
*/
double	fast_sqrt(float	nb)
{
	float guess[2];
	guess[0] = nb / 2;
	guess[1] = nb; // delta to check when result stabilize
	while (ABS(guess[1] - guess[0]) >= SQRT_PRECISION_DELTA)
	{
		guess[1] = guess[0];
		guess[0] = 0.5 * (guess[0] + (nb / guess[0]));
	}
	return (guess[0]);
}

/*
	Simple mod that handle double type modulos.
*/
double	d_mod(double x, double y)
{
	if (y == 0.0) // divide by 0
		return (0.0f);

	double quotient = x / y;
	if (quotient >= 0)
		quotient = (int)quotient;
	else 
		quotient = (int)quotient - ((quotient != (int)quotient)?1:0);
	return (x - quotient * y);
}


/*
	Approximation of sin using Taylor series and Horner's method
*/
double	approx_sin(double x)
{
	// Reduce x to the range [-pi, pi] for better convergence
	x = d_mod(x, 2 * PI);
	if (x < -PI) x += 2 * PI;
	if (x >  PI) x -= 2 * PI;

	// Taylor series coefficients for sin(x) up to x^13
	// Coefficients: x - x^3/3! + x^5/5! - x^7/7! + x^9/9! - x^11/11! + x^13/13!
	const double coefficients[] = {
		 1.0,			 // 1!
		-1.0 / 6,		 // 3!
		 1.0 / 120,	   // 5!
		-1.0 / 5040,	  // 7!
		 1.0 / 362880,	// 9!
		-1.0 / 39916800,  // 11!
		 1.0 / 6227020800 // 13!
	};

	double x_squared = x * x;
	double result = x;
	double term = x;

	for (int i = 1; i < 7; i++) {
		term *= x_squared;
		result += coefficients[i] * term;
	}

	return result;
}

/*
	Approximation of cos using Taylor series and Horner's method
*/
double	approx_cos(double x)
{
	// Reduce x to the range [-pi, pi] for better convergence
	x = d_mod(x, 2 * PI);
	if (x < -PI) x += 2 * PI;
	if (x >  PI) x -= 2 * PI;

	// Taylor series coefficients for cos(x) up to x^12
	// Coefficients: 1 - x^2/2! + x^4/4! - x^6/6! + x^8/8! - x^10/10! + x^12/12!
	const double coefficients[] = {
		-0.5,				// 2!
		 1.0 / 24,			// 4!
		-1.0 / 720,			// 6!
		 1.0 / 40320,		// 8!
		-1.0 / 3628800,		// 10!
		 1.0 / 479001600	// 12!
	};

	double x_squared = x * x;
	double result = 1;
	double term = 1;

	for (int i = 0; i < 6; i++)
	{
		term *= x_squared;
		result += coefficients[i] * term;
	}

	return result;
}

/*
	Approximation of tan using Taylor series
*/
double	approx_tan(double x)
{
	// reduce x modulo pi to bring it into the range [-π/2, π/2]
	x = d_mod(x, PI);
	if (x > PI/2)
		x -= PI;
	else if (x < -PI/2)
		x += PI;

	// Taylor series approximation for tan(x) around 0
	double x2 = x * x;
	double x3 = x2 * x;
	double x5 = x3 * x2;
	double x7 = x5 * x2;

	return x + (x3 / 3.0) + (2.0 * x5 / 15.0) + (17.0 * x7 / 315.0);
}

/*
	Returns the minimum between two double.
*/
double	d_min(double a, double b)
{
	if (a < b)
		return (a);
	return (b);
}

/*
	Returns the maximum between two double.
*/
double	d_max(double a, double b)
{
	if (a > b)
		return (a);
	return (b);
}