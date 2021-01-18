#include <bits/stdc++.h>
//GLEW
#include<GL/glew.h>
//GLFW
#include <GLFW/glfw3.h>
#include <GL/glut.h>
//OpenGL Math libs
#include <glm/glm.hpp>
#include<glm/vec3.hpp>
#include<glm/vec4.hpp>
#include<glm/mat4x4.hpp>

#define GLEW_STATIC
#define pi 3.14
#define deg 180
using namespace std;
GLFWwindow* w;

//This callback function is for zooming in and out the object, whenever the mouse wheel or touchpad gesture is scrolled down and up respectively.
void scroll_callback(GLFWwindow* w, double x, double y){
    double s = 1 + 0.1*y;
    //This function is for performing scaling 
    glScalef(s,s,s);
}

// This function reads the file "path", writes the data in vertices and returns false if the file can't be opened.
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
        if(strcmp(lineHeader, "f")==0){
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
    //Check if GLFW can be initialized or not
    if( !glfwInit() )
    {
        fprintf( stderr, "Couldn't initialize GLFW\n" );
        getchar();
        return -1;
    }
    //Create a window of size 1024*1024
    w = glfwCreateWindow( 1024, 1024, "Assignment 2", NULL, NULL);
    //Check if the window has been created and if it can be opened or not
    if( w == NULL ){
        fprintf( stderr, "Couldn't open GLFW window\n" );
        getchar();
        glfwTerminate();
        return -1;
    }
    //This function makes the OpenGL context of the specified window current on the calling thread. 
    glfwMakeContextCurrent(w);

    // Initialize GLEW
    glewExperimental = true; //For core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Couldn't initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }
    //This function sets an input mode option for the specified window.If sticky keys are enabled, a key press will ensure that glfwGetKey returns GLFW_PRESS the next time it is called even if the key had been released before the call.
    glfwSetInputMode(w, GLFW_STICKY_KEYS, GL_TRUE);
    
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
    double prevX = 0.0; 
    double prevY = 0.0;
    //This function is called to get notified when the user scrolls, whether with a mouse wheel or touchpad gesture, set a scroll callback.
    //This is called for the sake of zooming feature
    glfwSetScrollCallback(w, scroll_callback);

    do{
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //This function enables the generic vertex attribute array specified by index .
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
          
        double curX, curY,angle;
        //If we press the mouse left button then the object will rotate around x-aixs
        if (glfwGetMouseButton(w, GLFW_MOUSE_BUTTON_LEFT) ==  GLFW_PRESS) 
        {
            
            //Getting the cursor position
            glfwGetCursorPos(w, &curX, &curY);
            angle = atan2(curX - prevX, 1);
            prevX = curX; prevY = curY;
            glRotatef(angle*deg/100*pi, 1,0,0);
            
        }
        //If we press the mouse right button then the object will rotate around y-aixs
        else if (glfwGetMouseButton(w, GLFW_MOUSE_BUTTON_RIGHT) ==  GLFW_PRESS) 
        {
            
            //Getting the cursor position
            glfwGetCursorPos(w, &curX, &curY);
            angle = atan2(curX - prevX, 1);
            prevX = curX; prevY = curY;
            glRotatef(angle*deg/100*pi, 0,1,0);
        }
        //If we do not press any mouse button then on moving the cursor the object can rotate around z-axis
        else{
            //getting the cursor position
            glfwGetCursorPos(w, &curX, &curY);
            angle = atan2(curX - prevX, 1);
            prevX = curX; prevY = curY;
            glRotatef(angle*deg/100*pi, 0,0,1);
        }

        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, v.size() );
        //This function disables the generic vertex attribute array specified by index .
        glDisableVertexAttribArray(0);
        // Swap buffers
        glfwSwapBuffers(w);
        //This function processes only those events that are already in the event queue and then returns immediately. 
        glfwPollEvents();

    } // Keep rotating and zooming if required until the Esc key is pressed or the window is closed manually
    while( glfwWindowShouldClose(w) == 0 && glfwGetKey(w, GLFW_KEY_ESCAPE ) != GLFW_PRESS);
    // Delete all the buffers and vertex arrays
    glDeleteBuffers(1, &vbuffer);
    glDeleteVertexArrays(1, &vID);
    //This function destroys all remaining windows and cursors, restores any modified gamma ramps and frees any other allocated resources.
    glfwTerminate();

    return 0;
}