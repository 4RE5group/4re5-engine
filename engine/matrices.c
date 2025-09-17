#include "engine.h"

/*
	Simple matrix multiplication algorithm for 4x4 matrices.
*/
void	matrix_multiply(double out[4][4], double mat[4][4], double vec[4][4])
{
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            out[i][j] = 0;
            for (int k = 0; k < 4; k++) {
                out[i][j] += mat[i][k] * vec[k][j];
            }
        }
    }
}

/*
	Returns a rotation matrix using the given rotation angles.
*/
void	create_combined_rotation_matrix(double mat[4][4], Vect3 rotation)
{
    double rot_x[4][4], rot_y[4][4], rot_z[4][4];
	double temp[4][4];

	// init identity matrices
	for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            rot_x[i][j] = (i == j) ? 1.0 : 0.0;
			rot_y[i][j] = rot_x[i][j];
			rot_z[i][j] = rot_x[i][j];
			temp[i][j] = rot_x[i][j];

        }
    }

    double c = approx_cos(rotation.x);
    double s = approx_sin(rotation.x);

    // Set rotation components for x-axis
    rot_x[1][1] = c;
    rot_x[1][2] = -s;
    rot_x[2][1] = s;
    rot_x[2][2] = c;

	// Set rotation components for y-axis
	c = approx_cos(rotation.y);
    s = approx_sin(rotation.y);
    rot_y[0][0] = c;
    rot_y[0][2] = s;
    rot_y[2][0] = -s;
    rot_y[2][2] = c;

	// Set rotation components for z-axis
	c = approx_cos(rotation.z);
    s = approx_sin(rotation.z);
	rot_z[0][0] = c;
    rot_z[0][1] = -s;
    rot_z[1][0] = s;
    rot_z[1][1] = c;

    // Combine rotations in ZYX order
    matrix_multiply(temp, rot_z, rot_y);
    matrix_multiply(mat, temp, rot_x);
}