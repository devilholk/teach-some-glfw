#include <stdlib.h>
#include <stdio.h>
#include <GLFW/glfw3.h>
#include "2d_geometry.h"
#include "local_math.h"

void render_frame(GLFWwindow* window) {

	double t = glfwGetTime();


	//Get dimensions of window and adjust viewport
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);	
	glViewport(0, 0, width, height);

	//Set up projection matrix for orthogonal projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();							//We start with identity matrix
	glOrtho(0, width-1, height-1, 0, 0, 1);		//Then we multiply with orthogonal matrix

	//Clear colors and depths (not using masks)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	//Draw an ngon
	glLineWidth(2); // https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glLineWidth.xhtml

	//Move 100, 100 pixels
	glTranslatef(100, 100, 0);
	ngon(5, 30 + sin(fmod(5*t, M_TAU))*5, fmod(t, M_TAU));


	//Swap buffers - present to screen
	glfwSwapBuffers(window);

}

int main() {

	GLFWwindow* window;

	//Init glfw and create window
	glfwInit();
	window = glfwCreateWindow(640, 480, "Vectors and Matrices such", NULL, NULL);

	if (!window) {
		fprintf(stderr, "Failed to create window\n");
		glfwTerminate();
		return 1;
	}


	//Initialize opengl
	glfwMakeContextCurrent(window);	
	//Setup parameters for line drawing
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	//Enable blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);


	//Render loop
	while (!glfwWindowShouldClose(window)) {

		render_frame(window);
		glfwPollEvents();
	}



	return 0;

}