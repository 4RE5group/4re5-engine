#ifndef ARES_ENGINE
#define ARES_ENGINE

#include "config.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdint.h>

// definitions
#define PI	3.14159265359
#define ABS(nb) ((nb < 0)?-nb:nb)


typedef struct {
	double x, y, z;
}		Vect3;

typedef struct {
    double u, v;
}		UVCoord;

typedef struct {
    double x, y, z, w;
    UVCoord uv;
}		Vertex;

typedef struct {
	Vect3	rotation;
	Vect3	scale;
	Vect3	pos;
	int		vertices_count;
	int		faces_count;
	UVCoord	uv[MAX_UVCOORDS_PER_OBJECT];
	Vertex	vertices[MAX_VERTICES_PER_OBJECT];
	int		faces[MAX_FACES_PER_OBJECT][4];
}		Object;

typedef struct {
	Object	objects[32];
	int		object_count;
}		Scene;

typedef struct {
	int width;
	int height;
	int	bytes_per_pixel;
	unsigned int *pixels;
}		Texture;

// vector.c
void	vectAdd(Vect3 *out, Vect3 *vect1, Vect3 *vect2);
void	vectSub(Vect3 *out, Vect3 *vect1, Vect3 *vect2);
void	vectDiv(Vect3 *out, float coeff);
float	vectNorm(Vect3 *vect1);
void	vectCross(Vect3 *out, Vect3 *vect1, Vect3 *vect2);
double	vectDot(Vect3 *vect1, Vect3 *vect2);
void	vectRotate(Vect3 *out, Vect3 *v, double angle, char axis);
void	vectNormalize(Vect3 *v);

// engine.c
extern Vect3	camera_pos;
extern Vect3	look_at;
extern Vect3	up;
extern Vect3	forward;
extern Vect3	right;
int		__ARESengine__Init(char *window_name, int window_width, int window_height);
void	__ARESengine__displayUpdate(Scene *scene);
void	__ARESengine__rotateCamera(double pitch, double yaw);
void	__ARESengine__updateDirections();


// physics.c
void	__ARESengine__translate(Scene *scene, int obj_index, Vect3 new_pos, float delta_t);
void	__ARESengine__rotate(Scene *scene, int obj_index, Vect3 rotate, float delta_t);

// texture.c
int		__ARESengine__loadTexture(Texture *out, const char *path);


// math.c
double	fast_sqrt(float nb);
double	approx_sin(double x);
double	approx_cos(double x);
double	approx_tan(double x);
double	d_mod(double x, double y);
double	d_min(double a, double b);
double	d_max(double a, double b);

// graphics.c
extern Display					*display;
extern Window					window;
extern GC						gc;
int		setupWindow(int width, int height, const char *name);
void	stopWindow(void);


// objects.c
int		__ARESengine__loadObjectFromFile(Scene *scene, char *path, Vect3 pos, Vect3 scale, Vect3 rotation);

// conversions.c
double	parseDouble(const char *str, int *to_increment);
int		parseInt(const char *str, int *to_increment);

// matrices.c
void	create_combined_rotation_matrix(double mat[4][4], Vect3 rotation);
void	matrix_multiply(double out[4][4], double mat[4][4], double vec[4][4]);

#endif