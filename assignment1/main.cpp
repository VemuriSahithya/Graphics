#include<iostream>
#include<string>
#include<fstream>
#include<vector>
#include<sstream>
#include<algorithm>
#include <stdio.h>
#include <stdlib.h>
//GLEW
#include<GL/glew.h>
//GLFW
#include <GLFW/glfw3.h>
#include <GL/glut.h>
//OpenGL Math libs
#include<glm/glm.hpp>
#include<glm/vec3.hpp>
#include<glm/vec4.hpp>
#include<glm/mat4x4.hpp>
#define GLEW_STATIC


GLFWwindow* w;

using namespace std;

// This function reads the file "path", writes the data in vertices and returns false if something went wrong
bool loadTheObject(const char* path,vector<glm:: vec3> &vertices){

//Declaring some temporary variables to store the contents of the .obj
	vector<glm:: vec3> temp_vertices;
	vector< unsigned int> vertexIndex;

	FILE* file = fopen(path, "r");
	if(file==NULL){
		cout<<"Couldn't read the file"<<endl;
		return false;
	}
//Read the file until the end 
	while(1){
		char lineHeader[256];
		int res = fscanf(file, "%s", lineHeader);
		if(res == EOF){
			break;
		}
//If the first word of the line is “v”, then the rest has to be 3 floats, so create a glm::vec3 out of them, and add it to the vector.
		if(strcmp(lineHeader, "v")==0){
			glm :: vec3 tmp;
			fscanf(file,"%f %f %f\n", &tmp.x,&tmp.y,&tmp.z);
			tmp.x /=1.75;
			tmp.y /=1.75;
			tmp.z /=1.75;
			temp_vertices.push_back(tmp);
		}
		else if(strcmp(lineHeader, "f")==0){
			int v1, v2, v3;
			int count = fscanf(file, "%d %d %d\n", &v1, &v2, &v3);
			//If count is not equal to 3 then the file can't be read by our parser.
			if(count!=3){
				cout<<"ERR"<<endl;
				return false;
			}
			vertexIndex.push_back(v1);
			vertexIndex.push_back(v2);
			vertexIndex.push_back(v3);
		}
	}
	unsigned int i=0;
	while(i<vertexIndex.size()){
		unsigned int id = vertexIndex[i];
		glm::vec3 tmp = temp_vertices[id - 1];
		vertices.push_back(tmp);
		i++;
	}
	
	return true;
}


int main(int argc, char** argv){

	if( !glfwInit() )
	{
		fprintf( stderr, "Couldn't initialize GLFW\n" );
		getchar();
		return -1;
	}

	
	w = glfwCreateWindow( 1024, 768, "Assignment 1", NULL, NULL);
	if( w == NULL ){
		fprintf( stderr, "Couldn't open GLFW window\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(w);

	// Initialize GLEW
	glewExperimental = true; //For core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Couldn't initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwSetInputMode(w, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable mouvement
    glfwSetInputMode(w, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(w, 512, 512);

	// Dark purple background
	glClearColor(0.2f, 0.0f, 0.2f, 0.0f);

	GLuint vID;
	glGenVertexArrays(1, &vID);
	glBindVertexArray(vID);

	vector<glm:: vec3> v;

	int sol = loadTheObject("violin.obj", v);
	GLuint vbuffer;
	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &vbuffer);
	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
	glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(glm::vec3), &v[0], GL_STATIC_DRAW);

	do{
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
		glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, v.size() );

		glDisableVertexAttribArray(0);
		// Swap buffers
		glfwSwapBuffers(w);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(w, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(w) == 0 );
	glDeleteBuffers(1, &vbuffer);
	glDeleteVertexArrays(1, &vID);
	glfwTerminate();

	return 0;
}