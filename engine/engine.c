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

typedef struct {
    double u, v, w;
} Barycentric;

UVCoord interpolateUV(Barycentric coord, UVCoord uv0, UVCoord uv1, UVCoord uv2) {
    UVCoord uv;
    uv.u = coord.u * uv0.u + coord.v * uv1.u + coord.w * uv2.u;
    uv.v = coord.u * uv0.v + coord.v * uv1.v + coord.w * uv2.v;
    return uv;
}

Barycentric calculateBarycentric(XPoint p, XPoint a, XPoint b, XPoint c) {
    Barycentric coord;
    double areaABC = (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
    double areaPBC = (b.x - p.x) * (c.y - p.y) - (b.y - p.y) * (c.x - p.x);
    double areaPCA = (c.x - p.x) * (a.y - p.y) - (c.y - p.y) * (a.x - p.x);
    double areaPAB = (a.x - p.x) * (b.y - p.y) - (a.y - p.y) * (b.x - p.x);

    coord.u = areaPBC / areaABC;
    coord.v = areaPCA / areaABC;
    coord.w = areaPAB / areaABC;

    return coord;
}

void	drawTexturedFace(XPoint vertices[3], UVCoord uvs[3], Texture *texture)
{
    // Find the bounding box of the face
    int minX = d_min(d_min(vertices[0].x, vertices[1].x), vertices[2].x);
    int maxX = d_max(d_max(vertices[0].x, vertices[1].x), vertices[2].x);
    int minY = d_min(d_min(vertices[0].y, vertices[1].y), vertices[2].y);
    int maxY = d_max(d_max(vertices[0].y, vertices[1].y), vertices[2].y);

    // Iterate over each pixel in the bounding box
    for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {
            XPoint p = {x, y};
            Barycentric coord = calculateBarycentric(p, vertices[0], vertices[1], vertices[2]);

            // Check if the point is inside the face
            if (coord.u >= 0 && coord.v >= 0 && coord.w >= 0)
			{
                UVCoord uv = interpolateUV(coord, uvs[0], uvs[1], uvs[2]);

                // Sample the texture
                int texX = (int)(uv.u * (texture->width - 1));
                int texY = (int)(uv.v * (texture->height - 1));
                unsigned int pixel = texture->pixels[texY * texture->width + texX];

                // Set the pixel color
                XSetForeground(display, gc, pixel);
                XDrawPoint(display, window, gc, x, y);
            }
        }
    }
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

		double rotation_matrix[4][4];
		create_combined_rotation_matrix(rotation_matrix, scene->objects[obj_index].rotation);

		// Apply view transformation to all vertices
		for (int i = 0; i < NUM_VERTICES; i++)
		{
			double rotated_vertex[4] = {0, 0, 0, 0};
			for (int j = 0; j < 4; j++) {
				for (int k = 0; k < 4; k++) {
					rotated_vertex[j] += rotation_matrix[j][k] * ((double*)&scene->objects[obj_index].vertices[i])[k];
				}
			}

			double vertex[4] = {
				scene->objects[obj_index].pos.x + rotated_vertex[0] * scene->objects[obj_index].scale.x,
				scene->objects[obj_index].pos.y + rotated_vertex[1] * scene->objects[obj_index].scale.y,
				scene->objects[obj_index].pos.z + rotated_vertex[2] * scene->objects[obj_index].scale.z,
				1.0
			};

			// Transform vertex to view space
			double vertex_view[4] = {0, 0, 0, 0};
			for (int j = 0; j < 4; j++)
			{
				for (int k = 0; k < 4; k++)
				{
					vertex_view[j] += view_matrix[j][k] * vertex[k];
				}
			}

			// Skip vertices behind the camera (z > 0 in view space)
			if (vertex_view[2] > 0.0) {
				vertices_view[i][0] = 0.0;
				vertices_view[i][1] = 0.0;
				vertices_view[i][2] = 0.0;
				vertices_view[i][3] = 1.0;
				continue;
			}

			// Store the transformed vertex
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
		// tmp, load texture 
		Texture texture1;
		__ARESengine__loadTexture(&texture1, "./textures/texture1.bmp");
		for (int f = 0; f < NUM_FACES; f++)
		{
			XPoint face_vertices_temp[5];
			int total_vertices_count = 0;
			UVCoord face_uvs[4];

			// Collect vertices for this face
			Vect3 face_vertices_view[4];
			for (int i = 0; i < 4; i++)
			{
				int vertex_idx = scene->objects[obj_index].faces[f][i] - 1;
				if (vertex_idx < 0 || vertex_idx >= NUM_VERTICES) continue;

				face_uvs[i] = scene->objects[obj_index].vertices[vertex_idx].uv;

				face_vertices_temp[i] = vertices_screen[vertex_idx];
				face_vertices_view[i].x = vertices_view[vertex_idx][0];
				face_vertices_view[i].y = vertices_view[vertex_idx][1];
				face_vertices_view[i].z = vertices_view[vertex_idx][2];
				total_vertices_count++;
			}

			if (total_vertices_count < 3) continue;

			// Calculate face normal in view space
			Vect3 edge1, edge2;
			edge1.x = face_vertices_view[1].x - face_vertices_view[0].x;
			edge1.y = face_vertices_view[1].y - face_vertices_view[0].y;
			edge1.z = face_vertices_view[1].z - face_vertices_view[0].z;

			edge2.x = face_vertices_view[2].x - face_vertices_view[0].x;
			edge2.y = face_vertices_view[2].y - face_vertices_view[0].y;
			edge2.z = face_vertices_view[2].z - face_vertices_view[0].z;

			Vect3 face_normal;
			vectCross(&face_normal, &edge1, &edge2);
			vectNormalize(&face_normal);

			// Skip face if it's pointing away from the camera (backface)
			// if (vectDot(&face_normal, &z_axis) > 0) continue;

			// Close the polygon
			face_vertices_temp[total_vertices_count] = face_vertices_temp[0];

			drawTexturedFace(face_vertices_temp, face_uvs, &texture1);
			// Draw the outline
			XSetForeground(display, gc, 0x000000);
			XDrawLines(display, window, gc, face_vertices_temp, total_vertices_count + 1, CoordModeOrigin);
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
