#ifndef ARES_ENGINE
#define ARES_ENGINE

#include <X11/Xlib.h>
#include <X11/Xutil.h>

// definitions
#define PI	3.14159265359
#define ABS(nb) ((nb < 0)?-nb:nb)


typedef struct {
	double x, y, z;
}		Vect3;

typedef struct {
	Vect3	scale;
	Vect3	pos;
	int		vertices_count;
	int		faces_count;
	double	vertices[4096][4];
	int		faces[4096][4];
}		Object;

typedef struct {
	Object	objects[32];
	int		object_count;
}		Scene;

// vector.c
void	vectAdd(Vect3 *out, Vect3 *vect1, Vect3 *vect2);
void	vectSub(Vect3 *out, Vect3 *vect1, Vect3 *vect2);
void	vectDiv(Vect3 *out, float coeff);
float	vectNorm(Vect3 *vect1);
void	vectCross(Vect3 *out, Vect3 *vect1, Vect3 *vect2);
double	vectDot(Vect3 *vect1, Vect3 *vect2);
void	vectRotate(Vect3 *out, Vect3 *v, double angle, char axis);

// engine.c
extern Vect3	camera_pos;
extern Vect3	look_at;
extern Vect3	up;
extern Vect3	forward;
extern Vect3	right;

int		__ARESengine__Init(char *window_name, int window_width, int window_height);
void	__ARESengine__displayUpdate(Scene *scene);
void	__ARESengine__rotateCamera(double pitch, double yaw);

// physics.c
void	__ARESengine__translate(Scene *scene, int obj_index, Vect3 new_pos, float delta_t);


// math.c
double	fast_sqrt(float nb);
double	approx_sin(double x);
double	approx_cos(double x);
double	approx_tan(double x);

// graphics.c
extern Display					*display;
extern Window					window;
extern GC						gc;
int		setupWindow(int width, int height, const char *name);
void	stopWindow(void);


// objects.c
int		__ARESengine__loadObjectFromFile(Scene *scene, char *path, Vect3 pos, Vect3 scale);

// conversions.c
double	parseDouble(const char *str, int *to_increment);
int		parseInt(const char *str, int *to_increment);

#endif
