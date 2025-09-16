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

Vect3	camera_pos	= {3, 2, 5};
Vect3	look_at		= {0, 0, 0};
Vect3	up		= {0, 1, 0};

float	fov 		= PI / 3;  // 60 degrees
float	aspect_ratio	= 1;
float	near 		= 0.1;
float	far		= 100;
int	screen_width	= 800;
int	screen_height	= 600;


Vect3	z_axis = {0, 0, 0}; 
Vect3	x_axis = {0, 0, 0}; 
Vect3	y_axis = {0, 0, 0};


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
	{0, 0, 0, 0},
	{0, 0, 0, 0},
	{0, 0, 0, 0},
	{0, 0, -1.0, 0}
};

void	matrix_multiply(double out[4][4], double m1[4][4], double m2[4][4])
{
    for (int j = 0; j < 4; j++) {
        for (int i = 0; i < 4; i++) {
            out[j][i] = 0;
            for (int k = 0; k < 4; k++) {  // Start k at 0
                out[j][i] += m1[j][k] * m2[k][i];  // Correct multiplication
            }
        }
    }
}

void	transform_to_screen()
{
    // Apply view and projection transformations
    double vertices_view[4][4], vertices_clip[4][4];
    matrix_multiply(vertices_view, view_matrix, vertices);
    matrix_multiply(vertices_clip, projection_matrix, vertices_view);

    // Perspective divide (NDC coordinates)
    double vertices_ndc[4][3];
    for (int i = 0; i < 4; i++) {
        double w = vertices_clip[i][3];
        vertices_ndc[i][0] = vertices_clip[i][0] / w; // x / w
        vertices_ndc[i][1] = vertices_clip[i][1] / w; // y / w
        vertices_ndc[i][2] = vertices_clip[i][2] / w; // z / w
    }

    // Map NDC to screen coordinates
    XPoint vertices_screen[4];
    for (int i = 0; i < 4; i++) {
        double x_ndc = vertices_ndc[i][0];
        double y_ndc = vertices_ndc[i][1];
        // Map x from [-1, 1] to [0, screen_width]
        vertices_screen[i].x = (int)((x_ndc + 1) * 0.5 * screen_width);
        // Map y from [-1, 1] to [0, screen_height] (flipped)
        vertices_screen[i].y = (int)((1 - y_ndc) * 0.5 * screen_height);
    }

    // Define faces (indices of vertices for each face)
    // Example: 2 triangles forming a quad (adjust as needed)
    int faces[2][3] = {
        {0, 1, 2}, // Triangle 1
        {0, 2, 3}  // Triangle 2
    };
    int num_faces = 2;

    // Draw each face
    for (int f = 0; f < num_faces; f++) {
        XPoint face_vertices[4]; // Max 4 vertices per face (adjust if needed)
        int num_vertices_in_face = 3; // For triangles

        // Extract vertices for this face
        for (int i = 0; i < num_vertices_in_face; i++) {
            int vertex_idx = faces[f][i];
            face_vertices[i] = vertices_screen[vertex_idx];
        }

        // Fill the face (alpha=0.5 simulated with a light color)
        XSetForeground(display, gc, 0x777777); // Light gray
        XFillPolygon(display, window, gc, face_vertices, num_vertices_in_face,
                     Complex, CoordModeOrigin);

        // Draw the outline (black)
        XSetForeground(display, gc, 0x000000); // Black
        // Close the polygon by repeating the first vertex
        face_vertices[num_vertices_in_face] = face_vertices[0];
        XDrawLines(display, window, gc, face_vertices, num_vertices_in_face + 1, CoordModeOrigin);
    }

    // Flush changes to the screen
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
