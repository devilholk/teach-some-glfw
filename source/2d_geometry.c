#include "2d_geometry.h"
#include "local_math.h"
#include <GL/gl.h>

void ngon(int corners, double radius, double angle) {
	//Draws a polygon using GL_LINESTRIP
	//See https://www.khronos.org/opengl/wiki/Primitive#Line_primitives
	glBegin(GL_LINE_STRIP);
	for (int c=0; c <= corners; c++) {		//Note that we do (coners + 1) points, repeating the first one
		double a = fmod(angle + (double) c * M_TAU / (double) corners, M_TAU);
		glVertex3f(cos(a) * radius, sin(a) * radius, 0);
	}
	glEnd();
}