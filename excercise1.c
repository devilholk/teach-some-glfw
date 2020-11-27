/*

	Provide minimal command for compilation of this unit.

	v, n and o are all in ℝ³.
	v will be inside a cube of side 2.
	n will be on the unit sphere.
	o will just be weird, potentially having NaN in it.

	Explain why o is so different from n.

	References:

		https://gcc.gnu.org/onlinedocs/cpp/Macros.html
		https://en.wikipedia.org/wiki/Unit_sphere
		https://en.wikipedia.org/wiki/3D_rotation_group
		https://en.wikipedia.org/wiki/NaN
		
*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

struct vec3 {
	float x;
	float y;
	float z;
};


#define v3(x, y, z) ((struct vec3) {x, y, z})
#define v3_v3_genlinop(ax, ay, az, bx, by, bz, op) v3(ax op bx, ay op by, az op bz)
#define v3_v3_dot(a, b) (a.x * b.x + a.y * b.y + a.z * b.z)
#define v3_abs(a) sqrt(v3_v3_dot(a, a))
#define v3_norm(a) v3_s_div(a, v3_abs(a))
#define v3_s_div(a, b) v3_v3_genlinop(a.x, a.y, a.z, b, b, b, /)

#define randf() ((float)rand() / (float)RAND_MAX)
#define v1_random() (randf()*2.0-1.0)
#define v3_random() v3(v1_random(), v1_random(), v1_random())

int main(int argc, char* argv[]) {
	struct vec3 v = v3_random();
	struct vec3 n = v3_norm(v);
	struct vec3 o = v3_norm(v3_random());
	printf("v: %f, %f, %f\n", v.x, v.y, v.z);
	printf("n: %f, %f, %f\n", n.x, n.y, n.z);
	printf("o: %f, %f, %f\n", o.x, o.y, o.z);

}