/*

	gcc ifs.c -lglfw -lGL -lm -o ifs
	./ifs

	To generate a point cloud we start with a spawn point
	Then we will apply our current transformation matrix (CTM) on it
	CTM will then be multiplied with one of several random matricies

	never mind.. we will start with only using the random ones at first

 */

#define POINT_TYPE_THING GL_POINTS

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GLFW/glfw3.h>
#include <string.h>

/*! 16 consecutive values that are used as the elements of a 4 × 4 row-major matrix. */
typedef float matrix[16];

struct vec3 {
	float x;
	float y;
	float z;
};


/*

	Matrix multiplication

	/ a b c \   / α β γ \   / (aα + bλ + cρ) (aβ + bμ + cσ) (aγ + bν + cτ) \
	| p q r | x | λ μ ν | = | (pα + qλ + rρ) (pβ + qμ + rσ) (pγ + qν + rτ) |
	\ u v w /   \ ρ σ τ / = \ (uα + vλ + wρ) (uβ + vμ + wσ) (uγ + vν + wτ) /


	for (int r=0; r<4; r++) {
		for (int c=0; c<4; c++) {
			result[r*4+c] = A[r*4+c] * (B[c*4+r] + B[c*4+r] + B[c*4+r]);
		}
	}


*/


/*!

	Matrix translation

	/ 1 0 0 x \   / α \   / α + x \
	| 0 1 0 y | x | β | = | β + y |
	| 0 0 1 z |   | γ |   | γ + z |
	\ 0 0 0 1 /   \ 1 /   \   1   /


 */

#define randf() ((float)rand() / (float)RAND_MAX)
#define v1_random() (randf()*2.0-1.0)
#define v3_random() v3(v1_random(), v1_random(), v1_random())


#define MAX_matrixes 10

int ITMATCount = 3;

matrix ITMAT[MAX_matrixes];



#define m16_m16_mul(A, B) ((matrix)	{ \
		A[0]*B[0] +  A[1]*B[4] +  A[2]*B[8] +  A[3]*B[12], 		A[0]*B[1] +  A[1]*B[5] +  A[2]*B[9] +  A[3]*B[13],		A[0]*B[2] +  A[1]*B[6] +  A[2]*B[10] +  A[3]*B[14],		A[0]*B[3] + A[1]*B[7] + A[2]*B[11] + A[3]*B[15],		\
		A[4]*B[0] +  A[5]*B[4] +  A[6]*B[8] +  A[7]*B[12], 		A[4]*B[1] +  A[5]*B[5] +  A[6]*B[9] +  A[7]*B[13],		A[4]*B[2] +  A[5]*B[6] +  A[6]*B[10] +  A[7]*B[14],		A[4]*B[3] + A[5]*B[7] + A[6]*B[11] + A[7]*B[15],		\
		A[8]*B[0] +  A[9]*B[4] +  A[10]*B[8] + A[11]*B[12], 	A[8]*B[1] +  A[9]*B[5] +  A[10]*B[9] + A[11]*B[13],		A[8]*B[2] +  A[9]*B[6] +  A[10]*B[10] + A[11]*B[14],	A[8]*B[3] + A[9]*B[7] + A[10]*B[11] + A[11]*B[15],	\
		A[12]*B[0] + A[13]*B[4] + A[14]*B[8] + A[15]*B[12], 	A[12]*B[1] + A[13]*B[5] + A[14]*B[9] + A[15]*B[13],		A[12]*B[2] + A[13]*B[6] + A[14]*B[10] + A[15]*B[14],	A[12]*B[3] + A[13]*B[7] + A[14]*B[11] + A[15]*B[15]	\
	})


#define v3(x, y, z) ((struct vec3) {x, y, z})
#define v3_v3_genlinop(ax, ay, az, bx, by, bz, op) v3(ax op bx, ay op by, az op bz)
#define v3_v3_genlinop_floor(ax, ay, az, bx, by, bz, op) v3((int)(ax op bx), (int)(ay op by), (int)(az op bz))

#define v3_v3_add(a, b) v3_v3_genlinop(a.x, a.y, a.z, b.x, b.y, b.z, +)
#define s_v3_add(a, b) v3_v3_genlinop(a, a, a, b.x, b.y, b.z, +)
#define v3_s_add(a, b) v3_v3_genlinop(a.x, a.y, a.z, b, b, b, +)

#define v3_v3_sub(a, b) v3_v3_genlinop(a.x, a.y, a.z, b.x, b.y, b.z, -)
#define s_v3_sub(a, b) v3_v3_genlinop(a, a, a, b.x, b.y, b.z, -)
#define v3_s_sub(a, b) v3_v3_genlinop(a.x, a.y, a.z, b, b, b, -)

#define v3_s_mul(a, b) v3_v3_genlinop(a.x, a.y, a.z, b, b, b, *)
#define s_v3_mul(a, b) v3_v3_genlinop(a, a, a, b.x, b.y, b.z, *)

#define v3_s_div(a, b) v3_v3_genlinop(a.x, a.y, a.z, b, b, b, /)
#define v3_s_idiv(a, b) v3_v3_genlinop_floor(a.x, a.y, a.z, b, b, b, /)

#define v3_inv(a) v3_v3_genlinop(0, 0, 0, a.x, a.y, a.z, -)

#define v3_v3_cross(a, b) v3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x)
#define v3_v3_dot(a, b) (a.x * b.x + a.y * b.y + a.z * b.z)
#define v3_abs(a) sqrt(v3_v3_dot(a, a))
#define v3_norm(a) v3_s_div(a, v3_abs(a))


#define m16_v3_mul(a, b) v3((a)[0]*b.x + (a)[4]*b.y + (a)[8]*b.z + (a)[12], (a)[1]*b.x + (a)[5]*b.y + (a)[9]*b.z + (a)[13], (a)[2]*b.x + (a)[6]*b.y + (a)[10]*b.z + (a)[14])

#define get_translation_matrix(x, y, z) ((matrix)	\
	{	1, 0, 0, 0,									\
		0, 1, 0, 0,									\
		0, 0, 1, 0,									\
		x, y, z, 1 									\
	})


#define get_scale_matrix(s) ((matrix)	\
	{	s, 0, 0, 0,									\
		0, s, 0, 0,									\
		0, 0, s, 0,									\
		0, 0, 0, s 									\
	})


/*!

	Rotation matrix

	R = / cos(θ) -sin(θ) \
	    \ sin(θ)  cos(θ) /


	Basic rotations


		    	/ 1		0		0 		\
		Rx(θ) = | 0		cos(θ)	-sin(θ) |
				\ 0		sin(θ)	cos(θ)	/

		    	/ cos(θ)	0	sin(θ) \
		Ry(θ) = | 0			1	0 	   |
				\ -sin(θ)	0	cos(θ) /

		    	/ cos(θ)	-sin(θ)	0 \
		Rz(θ) = | sin(θ)	cos(θ)	0 |
				\ 0			0		1 /




 */

#define get_rotation_matrix(x, y, z) ({\
\
	float A = x*M_PI/180.0;\
	float B = y*M_PI/180.0;\
	float C = z*M_PI/180.0;\
\
	float xcos = cos(A);\
	float xsin = sin(A);\
	float xnsin = -xsin;\
\
	float ycos = cos(B);\
	float ysin = sin(B);\
	float ynsin = -ysin;\
\
	float zcos = cos(C);\
	float zsin = sin(C);\
	float znsin = -zsin;\
\
	(matrix)													\
	{															\
		ycos * zcos,	znsin,			ysin,			0,		\
		zsin,			xcos * zcos,	xnsin,			0,		\
		ynsin,			xsin,			xcos * ycos,	0,		\
																\
		0,				0,				0,				1,		\
																\
																\
	};															\
})



#define matrix_translate(mtx, x, y, z) m16_m16_mul((mtx), get_translation_matrix(x, y, z))
#define matrix_rotate(mtx, x, y, z) m16_m16_mul((mtx), get_rotation_matrix(x, y, z))
#define matrix_scale(mtx, s) m16_m16_mul((mtx), get_scale_matrix(s))



#define copy_array(src, dst) memcpy(dst, src, sizeof(src))


struct vec3 projection_plane = {0.8, 1.2, 20000.0};

#define POINTS 2000000


int seq[POINTS];

void load_seq() {
    for (int i=0; i<POINTS; i++) {
    	seq[i] = rand() % ITMATCount;

    }
}


void load_matrices() {

	matrix identity = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};

	float a = 360;
	float b = 50;
	float c = 0.8;

	for (int f1=0; f1<ITMATCount; f1++) {
		copy_array(identity, ITMAT[f1]);
		// copy_array(matrix_translate(ITMAT[f1],v1_random()*b, v1_random()*b, v1_random()*b), ITMAT[f1]);
		// copy_array(matrix_rotate(ITMAT[f1], v1_random()*a, v1_random()*a, v1_random()*a), ITMAT[f1]);
		// copy_array(matrix_translate(ITMAT[f1],v1_random()*b, v1_random()*b, v1_random()*b), ITMAT[f1]);
		// copy_array(matrix_scale(ITMAT[f1], 1.0 + v1_random()*c), ITMAT[f1]);

		struct vec3 T = v3_s_mul(v3_random(), 1000);
		float pw = 4.0;
		copy_array(matrix_translate(ITMAT[f1], T.x, T.y, T.z), ITMAT[f1]);
		copy_array(matrix_rotate(ITMAT[f1], pow(v1_random(), pw)*a, pow(v1_random(), pw)*a, pow(v1_random(), pw)*a), ITMAT[f1]);
		copy_array(matrix_translate(ITMAT[f1], -T.x, -T.y, -T.z), ITMAT[f1]);

		struct vec3 T2 = v3_s_mul(v3_random(), 1000);
		copy_array(matrix_translate(ITMAT[f1], T2.x, T2.y, T2.z), ITMAT[f1]);
		copy_array(matrix_scale(ITMAT[f1], 1.0 - pow(v1_random(), pw)*c), ITMAT[f1]);


		printf("Matrix %i: ", f1); for (int i=0; i<16; i++) printf("%f ", ITMAT[f1][i]); printf("\n");

	}


}


float dis = 300;
int locked_sequence=0;
float psize = 1.0;
float blend=5;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		load_matrices();
		load_seq();
	}

	if (key == GLFW_KEY_Q & action == GLFW_PRESS) {
		locked_sequence = !locked_sequence;
	}

	if (key == GLFW_KEY_D & action == GLFW_PRESS) {
		if (blend>1) {
			blend -= 1;
		}
	}

	if (key == GLFW_KEY_C & action == GLFW_PRESS) {
		if (blend<100) {
			blend += 1;
		}
	}


	if (key == GLFW_KEY_A & action == GLFW_PRESS) {
		dis -= 10;
	}
	if (key == GLFW_KEY_Z & action == GLFW_PRESS) {
		dis += 10;
	}

	if (key == GLFW_KEY_S & action == GLFW_PRESS) {
		if (psize < 50) {
			psize += .1;
			glPointSize(psize);
		}
	}
	if (key == GLFW_KEY_X & action == GLFW_PRESS) {
		if (psize > 0) {
			psize -= .1;
			glPointSize(psize);
		}
	}


	if (key == GLFW_KEY_PAGE_UP && action == GLFW_PRESS) {
		if (ITMATCount < MAX_matrixes) {
			ITMATCount++;
			load_matrices();
			load_seq();
		}
	}
	if (key == GLFW_KEY_PAGE_DOWN && action == GLFW_PRESS) {
		if (ITMATCount > 2) {
			ITMATCount--;
			load_matrices();
			load_seq();
		}
	}

}


int main() {

	GLFWwindow* window;
	glfwInit();
	window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		return 0;
	}

	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);


	glEnable(GL_LIGHT0);
	GLfloat light_position[] = { -1.0, 1.0, 1.0, 0.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_ambient[] = { 0.4, 0.6, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 50.0 };


	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_DIFFUSE);

    glEnable(GL_LIGHTING);

	double ot, nt = glfwGetTime();

	load_matrices();
	load_seq();


	glPointSize(psize);
	glEnable(GL_POINT_SMOOTH);

	while (!glfwWindowShouldClose(window)) {

		ot=nt;
		nt =glfwGetTime();
		float dt = nt - ot;


		float ratio;
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float) height;
		glViewport(0, 0, width, height);


		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();



		glFrustum(-ratio*projection_plane.x, ratio*projection_plane.x, -projection_plane.x, projection_plane.x, projection_plane.y, projection_plane.z);

		glTranslatef(0, 0, -dis);
		glRotatef(fmod(15*nt, 360.0), 0, 1, 0);
		glRotatef(fmod(2*nt, 360.0), 1, 0, 0);

		glScalef(0.1, 0.1, 0.1);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();



		float ps = psize;
	    struct vec3 point={0,0,0};
		glPointSize(ps);

	    glBegin(POINT_TYPE_THING);



	    for (int i=0; i<POINTS; i++) {

			if (i % 10000== 0) {
		    	glColor4f(0.5+cos(nt+i*.000001)*0.5, 0.5+cos(nt*.3+i*.000003)*0.5, 0.5+cos(nt*-.61+i*.00002)*0.5, 1.0/blend);
		    	if (point.x < -1000 || point.x > 1000 || point.y < -1000 || point.y > 1000 || point.z < -1000 || point.z > 1000) {
		    		point = v3(0,0,0);
		    	}
		    	glEnd();
		    	glPointSize(ps);
		    	glBegin(POINT_TYPE_THING);
		    	ps*=.97;


		    }

		    int f;

		    if (locked_sequence) {
	    		f = seq[i];
	    	} else {
	    		f = rand() % ITMATCount;
	    	}

	    	// printf("Matrix: %f, %f, %f, %f,     %f, %f, %f, %f,      %f, %f, %f, %f,     %f, %f, %f, %f\n", ITMAT[f]);


			// copy_array(m16_m16_mul(ITMAT[f], ITMAT[rand() % ITMATCount]), ITMAT[f]);	//Multiply with random for animation

	    	point = m16_v3_mul(ITMAT[f], point);
	    	glVertex3f(point.x, point.y, point.z);
	    }
	    //printf("ps: %f\n", ps);

	    //Animation, change count for more/less

	    // for (int i=0; i<1; i++) {
		int f1 = rand() % ITMATCount;
		float a = .3;
		float b = .1;
		float c = 0.001;
		copy_array(matrix_rotate(ITMAT[f1], v1_random()*a, v1_random()*a, v1_random()*a), ITMAT[f1]);
		f1 = rand() % ITMATCount;
		copy_array(matrix_translate(ITMAT[f1],v1_random()*b, v1_random()*b, v1_random()*b), ITMAT[f1]);
		f1 = rand() % ITMATCount;
		copy_array(matrix_scale(ITMAT[f1], 1.0 + v1_random()*c), ITMAT[f1]);



	    glEnd();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	glfwDestroyWindow(window);

	glfwTerminate();
	return(EXIT_SUCCESS);
}


