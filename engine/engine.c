#include "engine.h"

#define NUM_VERTICES	8
#define NUM_FACES		6

double   vertices[NUM_VERTICES][4] = {
	{-1.0f, -1.0f,  -1.0f,  1},  // 0
	{ 1.0f,  -1.0f,  -1.0f, 1}, // 1
	{ 1.0f,   1.0f,  -1.0f, 1}, // 2
	{-1.0f,  1.0f,  -1.0f,  1},  // 3
	{-1.0f, -1.0f,   1.0f,  1},  // 4
	{ 1.0f,  -1.0f,   1.0f, 1}, // 5
	{ 1.0f,   1.0f,   1.0f, 1}, // 6
	{-1.0f,  1.0f,   1.0f,  1}   // 7
};

int   faces[NUM_FACES][4] = {
	{0, 1, 2, 3},  // Bottom
	{4, 5, 6, 7},  // Top
	{0, 1, 5, 4},  // Front
	{2, 3, 7, 6},  // Back
	{0, 3, 7, 4},  // Left
	{1, 2, 6, 5}   // Right
};

Vect3	camera_pos		= {3, 2, 5};
Vect3	look_at			= {0, 0, 0};
Vect3	up				= {0, 1, 0};

float	fov 			= PI / 3;  // 60 degrees
float	aspect_ratio	= 1;
float	near 			= 0.1;
float	far				= 100;
int	screen_width		= 800;
int	screen_height		= 600;


Vect3	z_axis 			= {0, 0, 0}; 
Vect3	x_axis 			= {0, 0, 0}; 
Vect3	y_axis 			= {0, 0, 0};


// View matrix
double	view_matrix[4][4] = {
	{0, 0, 0, 0},
	{0, 0, 0, 0},
	{0, 0, 0, 0},
	{0, 0, 0, 1}
};

// perspective matrix
double	f;

double	projection_matrix[4][4] = {
	{0, 0, 0,    0},
	{0, 0, 0,    0},
	{0, 0, 0,    0},
	{0, 0, -1.0, 0}
};

void transpose(double out[4][4], double in[4][4])
{
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            out[j][i] = in[i][j];
        }
    }
}

void matrix_multiply(double out[4][4], double mat[4][4], double vec[4][4])
{
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            out[i][j] = 0;
            for (int k = 0; k < 4; k++) {
                out[i][j] += mat[i][k] * vec[k][j]; // Correct indexing
            }
        }
    }
}

void transform_to_screen()
{
    double vertices_view[NUM_VERTICES][4];
    double vertices_clip[NUM_VERTICES][4];
    double vertices_ndc[NUM_VERTICES][3];
    XPoint vertices_screen[NUM_VERTICES];

    // apply view transformation to all vertices
    for (int i = 0; i < NUM_VERTICES; i++)
	{
        double vertex[4] = {vertices[i][0], vertices[i][1], vertices[i][2], vertices[i][3]};
        double vertex_view[4] = {0, 0, 0, 0};
        for (int j = 0; j < 4; j++)
		{
            for (int k = 0; k < 4; k++)
			{
                vertex_view[j] += view_matrix[j][k] * vertex[k];
            }
        }
        for (int j = 0; j < 4; j++)
		{
            vertices_view[i][j] = vertex_view[j];
        }
    }

    // Apply projection transformation to all vertices
    for (int i = 0; i < NUM_VERTICES; i++)
	{
        double vertex_clip[4] = {0, 0, 0, 0};
        for (int j = 0; j < 4; j++)
		{
            for (int k = 0; k < 4; k++)
			{
                vertex_clip[j] += projection_matrix[j][k] * vertices_view[i][k];
            }
        }
        for (int j = 0; j < 4; j++)
		{
            vertices_clip[i][j] = vertex_clip[j];
        }
    }

    // Perspective divide and map to NDC
    for (int i = 0; i < NUM_VERTICES; i++)
	{
        double w = vertices_clip[i][3];
        if (w == 0.0) {
			w = 1.0;
		}
        vertices_ndc[i][0] = vertices_clip[i][0] / w;
        vertices_ndc[i][1] = vertices_clip[i][1] / w;
        vertices_ndc[i][2] = vertices_clip[i][2] / w;
    }

    // Map NDC to screen coordinates
    for (int i = 0; i < NUM_VERTICES; i++)
	{
        double x_ndc = vertices_ndc[i][0];
        double y_ndc = vertices_ndc[i][1];
        vertices_screen[i].x = (int)((x_ndc + 1) * 0.5 * screen_width);
        vertices_screen[i].y = (int)((1 - y_ndc) * 0.5 * screen_height);
    }

    // Draw each face
    for (int f = 0; f < NUM_FACES; f++)
	{
        XPoint face_vertices_temp[5];
		for (int i = 0; i < 4; i++)
		{
			int vertex_idx = faces[f][i];
			face_vertices_temp[i] = vertices_screen[vertex_idx];
		}
		face_vertices_temp[4] = face_vertices_temp[0]; // Close the polygon
        // Fill the face
        // XSetForeground(display, gc, 0xFF0000); // red
        // XFillPolygon(display, window, gc, face_vertices_temp, 4, Complex, CoordModeOrigin);
        // Draw the outline
        XSetForeground(display, gc, 0x000000); // Black
		XDrawLines(display, window, gc, face_vertices_temp, 5, CoordModeOrigin);
    }

    XFlush(display);
}


int	engineInit(void)
{
	// init all axis
	
	// z_axis = (camera_pos - look_at) / ||camera_pos - look_at||
	vectSub(&z_axis, &camera_pos, &look_at);
	vectDiv(&z_axis, vectNorm(&z_axis));

	// x_axis = cross(up, z_axis) / ||cross(up, z_axis)||
	vectCross(&x_axis, &up, &z_axis);
	vectDiv(&x_axis, vectNorm(&x_axis));

	// y_axis = cross(z_axis, x_axis)
	vectCross(&y_axis, &z_axis, &x_axis);


	// init view matrix
	view_matrix[0][0] = x_axis.x;
	view_matrix[0][1] = x_axis.y;
	view_matrix[0][2] = x_axis.z;
	view_matrix[0][3] = -vectDot(&x_axis, &camera_pos);

	view_matrix[1][0] = y_axis.x;
	view_matrix[1][1] = y_axis.y;
	view_matrix[1][2] = y_axis.z;
	view_matrix[1][3] = -vectDot(&y_axis, &camera_pos);

	view_matrix[2][0] = z_axis.x;
	view_matrix[2][1] = z_axis.y;
	view_matrix[2][2] = z_axis.z;
	view_matrix[2][3] = -vectDot(&z_axis, &camera_pos);

	// init f and projection matrix
	f = 1 / approx_tan(fov / 2);

	projection_matrix[0][0] = f / aspect_ratio;
	projection_matrix[1][1] = f;
	projection_matrix[2][2] = (far + near) / (near - far);
	projection_matrix[2][3] = (2 * far * near) / (near - far);


	return (0);
}
