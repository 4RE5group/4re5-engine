#ifndef ARES_ENGINE
#define ARES_ENGINE

#include <X11/Xlib.h>
#include <X11/Xutil.h>

// definitions
#define PI	3.14159265359


typedef struct {
	double x, y, z;
}		Vect3;

// vector.c
void	vectAdd(Vect3 *out, Vect3 *vect1, Vect3 *vect2);
void	vectSub(Vect3 *out, Vect3 *vect1, Vect3 *vect2);
void	vectDiv(Vect3 *out, float coeff);
float	vectNorm(Vect3 *vect1);
void	vectCross(Vect3 *out, Vect3 *vect1, Vect3 *vect2);
double	vectDot(Vect3 *vect1, Vect3 *vect2);

// engine.c
extern Vect3	camera_pos;
extern Vect3	look_at;
extern Vect3	up;
int		__ARESengine__Init(char *window_name, int window_width, int window_height);
void	__ARESengine__displayUpdate(void);

// math.c
double	fast_sqrt(float nb);
double	approx_sin(double x);
double	approx_tan(double x);

// graphics.c
extern Display					*display;
extern Window					window;
extern GC						gc;
int		setupWindow(int width, int height, const char *name);
void	stopWindow(void);

#endif
