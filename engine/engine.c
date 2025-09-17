#include "engine.h"
#include <math.h>

Vect3	camera_pos		= {3, 2, 5};
Vect3	look_at			= {0, 0, 0};
Vect3	up				= {0, 1, 0};
Vect3	forward			= {0, 0, 0};
Vect3	right			= {0, 0, 0};

float	fov 			= PI / 2;
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

void	__ARESengine__updateDirections()
{
	// forward
	vectSub(&forward, &look_at, &camera_pos); // forward = look_at - camera_pos
	vectNormalize(&forward); // norm the vector

	up.x = 0;
	up.y = 1;
	up.z = 0;

	// right
	vectCross(&right, &up, &forward);
	vectNormalize(&right);
}

void	__ARESengine__rotateCamera(double pitch, double yaw)
{
    // Calculate the direction vector from camera to look_at
    Vect3 direction;
    direction.x = look_at.x - camera_pos.x;
    direction.y = look_at.y - camera_pos.y;
    direction.z = look_at.z - camera_pos.z;

    // Calculate the distance from camera to look_at
    // double distance = fast_sqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
	double distance = 5.0;

    // Calculate spherical coordinates (theta and phi)
    double theta = atan2(direction.z, direction.x); // azimuthal angle
    double phi = acos(direction.y / distance);     // polar angle

    // Apply yaw and pitch
    theta += yaw;
    phi += pitch;

    // Ensure phi stays within bounds to avoid flipping
    if (phi < 0.01) phi = 0.01;
    if (phi > 3.13) phi = 3.13; // ~Pi - small value to avoid flipping

    // Convert back to Cartesian coordinates
    look_at.x = camera_pos.x + distance * approx_sin(phi) * approx_cos(theta);
    look_at.y = camera_pos.y + distance * approx_cos(phi);
    look_at.z = camera_pos.z + distance * approx_sin(phi) * approx_sin(theta);
}

void	__ARESengine__displayUpdate(Scene *scene)
{
	// clear window
	XClearWindow(display, window);

	// update forward, up and right vectors
	__ARESengine__updateDirections();

	// init all axis
	// z_axis = (camera_pos - look_at) / ||camera_pos - look_at||
	vectSub(&z_axis, &camera_pos, &look_at);
	vectNormalize(&z_axis);

	// x_axis = cross(up, z_axis) / ||cross(up, z_axis)||
	vectCross(&x_axis, &up, &z_axis);
	vectNormalize(&x_axis);

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

	view_matrix[3][0] = 0.0;
	view_matrix[3][1] = 0.0;
	view_matrix[3][2] = 0.0;
	view_matrix[3][3] = 1.0;

	for (int obj_index = 0; obj_index < scene->object_count; obj_index++)
	{
		int NUM_VERTICES	= scene->objects[obj_index].vertices_count;
		int NUM_FACES		= scene->objects[obj_index].faces_count;
		if (NUM_VERTICES <= 0 || NUM_FACES <= 0) return; // prevent crashes

		double vertices_view[NUM_VERTICES][4];
		double vertices_clip[NUM_VERTICES][4];
		double vertices_ndc[NUM_VERTICES][4];
		XPoint vertices_screen[NUM_VERTICES];

		// apply view transformation to all vertices
		for (int i = 0; i < NUM_VERTICES; i++)
		{
			double vertex[4] = 
			{
				// apply pos + scale
				scene->objects[obj_index].pos.x + scene->objects[obj_index].vertices[i][0] * scene->objects[obj_index].scale.x, 
				scene->objects[obj_index].pos.y + scene->objects[obj_index].vertices[i][1] * scene->objects[obj_index].scale.y, 
				scene->objects[obj_index].pos.z + scene->objects[obj_index].vertices[i][2] * scene->objects[obj_index].scale.z, 
				scene->objects[obj_index].vertices[i][3]
			};

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
			// Prevent division by zero
			if (w == 0.0) w = 1.0;
			if (ABS(w) < 0.0001) w = 0.0001;

			vertices_ndc[i][0] = vertices_clip[i][0] / w;
			vertices_ndc[i][1] = vertices_clip[i][1] / w;
			vertices_ndc[i][2] = vertices_clip[i][2] / w;
			vertices_ndc[i][3] = w;

			// Clip against near/far planes
            if (vertices_ndc[i][2] < -1.0 || vertices_ndc[i][2] > 1.0) {
                vertices_ndc[i][2] = vertices_ndc[i][2] < -1.0 ? -1.0 : 1.0;
            }
		}

		// map NDC to screen coordinates
		for (int i = 0; i < NUM_VERTICES; i++)
		{
			double x_ndc = vertices_ndc[i][0];
			double y_ndc = vertices_ndc[i][1];
			vertices_screen[i].x = (int)((x_ndc + 1) * 0.5 * screen_width);
			vertices_screen[i].y = (int)((1 - y_ndc) * 0.5 * screen_height);
		}

		// draw each face
		for (int f = 0; f < NUM_FACES; f++)
		{
			XPoint face_vertices_temp[5];
			int total_vertices_count = 0;
			Vect3 delta;
			double k;
			short display_face = 1;

			for (int i = 0; i < 4; i++)
			{
				int vertex_idx = scene->objects[obj_index].faces[f][i] - 1;

				// check if vertex is on the back of the camera, then skip it if so
				// this mean we have to calculate k and j with k positive, if not, it's behind
				// vertex = camera_pos + k * forward + j * right
				
				delta.x = scene->objects[obj_index].vertices[vertex_idx][0] - camera_pos.x;
				delta.y = scene->objects[obj_index].vertices[vertex_idx][1] - camera_pos.y;
				delta.z = scene->objects[obj_index].vertices[vertex_idx][2] - camera_pos.z;

				// calculate the determinant of the system
				// (forward.x * right.z - forward.z * right.x) for x-y plane
				// (forward.y * right.z - forward.z * right.y) for y-z plane
				// (forward.x * right.y - forward.y * right.x) for x-z plane
				// We'll use the x-y plane for simplicity (ignore z for 2D projection)
				double determinant = (forward.x * right.y - forward.y * right.x);

				if (ABS(determinant) >= 1e-10)
				{
					// vectors are not parallel
					// solve for k and j using Cramer's rule
					k = (delta.x * right.y - delta.y * right.x) / determinant;
					// __builtin_printf("k: %f\n", k);
					if (k < 0.0f)
					{
						// display_face = 0;
						// break;
					}
				}


				if (vertex_idx < 0 || vertex_idx >= NUM_VERTICES) continue; // skip invalid vertices index
				face_vertices_temp[i] = vertices_screen[vertex_idx];
				total_vertices_count++;
			}
			if (total_vertices_count < 3 || !display_face) continue;
			// close the polygon
			face_vertices_temp[total_vertices_count] = face_vertices_temp[0];
			
			// Draw the outline
			XSetForeground(display, gc, 0x000000);
			XDrawLines(display, window, gc, face_vertices_temp, total_vertices_count+1, CoordModeOrigin);
		}
	}
    XFlush(display);
}


int	__ARESengine__Init(char *window_name, int window_width, int window_height)
{
	// init f and projection matrix
	f = 1 / approx_tan(fov / 2);

	projection_matrix[0][0] = f / aspect_ratio;
	projection_matrix[1][1] = f;
	projection_matrix[2][2] = (far + near)/(near - far);  // Should be (far + near)/(near - far)
	projection_matrix[2][3] = (2*far*near)/(near - far);  // Correct
	projection_matrix[3][2] = -1.0;                       // Should be -1.0
	projection_matrix[3][3] = 0.0;                        // Should be 0.0

	// set window properties
	screen_width = window_width;
	screen_height = window_height;

	// set camera default values
	look_at.x = camera_pos.x;
	look_at.y = camera_pos.y;
	look_at.z = camera_pos.z - 2;

	forward.x = 0;
	forward.y = 0;
	forward.z = 1;

	up.x = 0;
	up.y = 1;
	up.z = 0;

	// initialize right vector
	vectCross(&right, &up, &forward);
	vectNormalize(&right);
	
	return (setupWindow(screen_width, screen_height, window_name));
}
