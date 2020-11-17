#include <iostream>
using namespace std;
#include <cmath>

//--- OpenGL ---
#include "GL\glew.h"
#include "GL\wglew.h"
#pragma comment(lib, "glew32.lib")
//--------------

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"
#include "glm\gtc\matrix_inverse.hpp"

#include "GL\freeglut.h"

#include "Images\FreeImage.h"

#include "shaders\Shader.h"
#include "sphere\Sphere.h"

glm::mat4 objectTransformation;

Shader* myShader;  ///shader object 
Shader* myBasicShader;
Shader* sphereShader;

//MODEL LOADING
#include "3DStruct\threeDModel.h"
#include "Obj\OBJLoader.h"

float amount = 0;
float temp = 0.002f;

ThreeDModel plane; //A threeDModel object for the plane
ThreeDModel terrain; //A threeDModel object for the terrain

//Sphere used to test collisions
Sphere mySphere;
float mySphereCentre[3];
Sphere terrainSphere;
float terrainSphereCentre[3];

OBJLoader objLoader;	//this object is used to load the 3d models.
///END MODEL LOADING

glm::mat4 ProjectionMatrix; // matrix for the orthographic projection
glm::mat4 ModelViewMatrix;  // matrix for the modelling and viewing

glm::vec3 pos = glm::vec3(0.0f,0.0f,0.0f); //vector for the position of the object.

//Material properties
float Material_Ambient[4] = {0.1f, 0.1f, 0.1f, 1.0f};
float Material_Diffuse[4] = {0.8f, 0.8f, 0.74f, 1.0f};
float Material_Specular[4] = {0.9f,0.9f,0.84f,1.0f};
float Material_Shininess = 50;

//Light Properties
float Light_Ambient_And_Diffuse[4] = {1.4f, 1.4f, 1.34f, 1.6f};
float Light_Specular[4] = {1.6f,1.6f,1.54f,1.6f};
float LightPos[4] = {0.0f, 0.0f, 1.0f, 0.0f};

//
int	mouse_x=0, mouse_y=0;
bool LeftPressed = false;
int screenWidth = 960, screenHeight = 540;

//booleans to handle when the arrow keys are pressed or released.
bool Left = false;
bool Right = false;
bool Up = false;
bool Down = false;
bool A = false;
bool D = false;
bool SPACE = false;

float spin=180;
float speed=0.1f;

//OPENGL FUNCTION PROTOTYPES
void display();				//called in winmain to draw everything to the screen
void reshape(int width, int height);				//called when the window is resized
void init();				//called in winmain when the program starts.
void processKeys();         //called in winmain to process keyboard input
void idle();		//idle function
void updateTransform(float xinc, float yinc, float zinc);
//bool checkSphereCol();

/*************    START OF OPENGL FUNCTIONS   ****************/
void display()									
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glUseProgram(myShader->handle());  // use the shader

	GLuint matLocation = glGetUniformLocation(myShader->handle(), "ProjectionMatrix");  
	glUniformMatrix4fv(matLocation, 1, GL_FALSE, &ProjectionMatrix[0][0]);

	glm::mat4 viewingMatrix = glm::mat4(1.0f);
	
	//environment view
	//viewingMatrix = glm::lookAt(glm::vec3(-10, 10, 10), pos, glm::vec3(0.0f, 1.0f, 0.0));

	//view behind jet, needs work
	viewingMatrix = glm::lookAt(pos + (glm::mat3(objectTransformation) * glm::vec3(0, 0.8, -5)), pos + (glm::mat3(objectTransformation) * glm::vec3(0, 0.8, 0)), glm::vec3(objectTransformation[1]));

	glUniformMatrix4fv(glGetUniformLocation(myShader->handle(), "ViewMatrix"), 1, GL_FALSE, &viewingMatrix[0][0]);

	glUniform4fv(glGetUniformLocation(myShader->handle(), "LightPos"), 1, LightPos);
	glUniform4fv(glGetUniformLocation(myShader->handle(), "light_ambient"), 1, Light_Ambient_And_Diffuse);
	glUniform4fv(glGetUniformLocation(myShader->handle(), "light_diffuse"), 1, Light_Ambient_And_Diffuse);
	glUniform4fv(glGetUniformLocation(myShader->handle(), "light_specular"), 1, Light_Specular);

	glUniform4fv(glGetUniformLocation(myShader->handle(), "material_ambient"), 1, Material_Ambient);
	glUniform4fv(glGetUniformLocation(myShader->handle(), "material_diffuse"), 1, Material_Diffuse);
	glUniform4fv(glGetUniformLocation(myShader->handle(), "material_specular"), 1, Material_Specular);
	glUniform1f(glGetUniformLocation(myShader->handle(), "material_shininess"), Material_Shininess);

	// start rendering
	// PLANE ---------------------------

	glm::mat4 modelmatrix = glm::translate(glm::mat4(1.0f), pos);

	ModelViewMatrix = viewingMatrix * modelmatrix * objectTransformation;
	glUniformMatrix4fv(glGetUniformLocation(myShader->handle(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	
	glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix)); //lighting normals for plane
	glUniformMatrix3fv(glGetUniformLocation(myShader->handle(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);
		
	plane.drawElementsUsingVBO(myShader);	

	glUseProgram(myBasicShader->handle());  // use the shader
	glUniformMatrix4fv(glGetUniformLocation(myBasicShader->handle(), "ProjectionMatrix"), 1, GL_FALSE, &ProjectionMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(myBasicShader->handle(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
		
	//plane.drawOctreeLeaves(myBasicShader);

	// END PLANE -------------------------

	// SPHERE -----------------------------
	/*
	mySphere.setCentre(pos.x, pos.y, pos.z);

	glUseProgram(sphereShader->handle());

	glUniformMatrix4fv(glGetUniformLocation(sphereShader->handle(), "ViewMatrix"), 1, GL_FALSE, &viewingMatrix[0][0]);

	glUniformMatrix4fv(glGetUniformLocation(sphereShader->handle(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);

	normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix)); // lighting normals for terrain
	glUniformMatrix3fv(glGetUniformLocation(sphereShader->handle(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	mySphere.render();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glUniformMatrix4fv(glGetUniformLocation(sphereShader->handle(), "ProjectionMatrix"), 1, GL_FALSE, &ProjectionMatrix[0][0]);

	glm::vec3 sphereCentre = glm::vec3(mySphere.cx, mySphere.cy, mySphere.cz);
	cout << mySphere.cx << " " << mySphere.cy << " " << mySphere.cz << endl;
	*/
	// END SPHERE -------------------------
	
	// TERRAIN ---------------------------

	glUseProgram(myShader->handle());  // use the shader

	ModelViewMatrix = glm::translate(viewingMatrix, glm::vec3(0, -5, 0)); //resets model view for terrain 
	glUniformMatrix4fv(glGetUniformLocation(myShader->handle(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);

	normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix)); // lighting normals for terrain
	glUniformMatrix3fv(glGetUniformLocation(myShader->handle(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);
	
	terrain.drawElementsUsingVBO(myShader);

	glUseProgram(myBasicShader->handle());  // use the shader
	glUniformMatrix4fv(glGetUniformLocation(myBasicShader->handle(), "ProjectionMatrix"), 1, GL_FALSE, &ProjectionMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(myBasicShader->handle(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);

	//terrain.drawOctreeLeaves(myBasicShader);

	// END TERRAIN ------------------------	

	// TERRAIN SPHERE ---------------------
	/*
	terrainSphere.setCentre(10, 10, 5);
	
	glUseProgram(sphereShader->handle());

	glUniformMatrix4fv(glGetUniformLocation(sphereShader->handle(), "ViewMatrix"), 1, GL_FALSE, &viewingMatrix[0][0]);

	ModelViewMatrix = glm::translate(viewingMatrix, glm::vec3(0, 0, 0));
	glUniformMatrix4fv(glGetUniformLocation(sphereShader->handle(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);

	normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix)); // lighting normals for terrain
	glUniformMatrix3fv(glGetUniformLocation(sphereShader->handle(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (checkDist()) {
		terrainSphere.render();
	}	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glUniformMatrix4fv(glGetUniformLocation(sphereShader->handle(), "ProjectionMatrix"), 1, GL_FALSE, &ProjectionMatrix[0][0]);
	*/
	// END TERRAIN SPHERE ------------------

	glFlush();
	glutSwapBuffers();
}

void reshape(int width, int height)		// Resize the OpenGL window
{
	screenWidth=width; screenHeight = height;           // to ensure the mouse coordinates match 
														// we will use these values to set the coordinate system

	glViewport(0,0,width,height);						// Reset The Current Viewport

	//Set the projection matrix
	ProjectionMatrix = glm::perspective(glm::radians(60.0f), (GLfloat)screenWidth/(GLfloat)screenHeight, 0.1f, 200.0f);
}
void init()
{
	glClearColor(1.0,1.0,1.0,0.0);						//sets the clear colour to yellow
														//glClear(GL_COLOR_BUFFER_BIT) in the display function
														//will clear the buffer to this colour
	glEnable(GL_DEPTH_TEST);

	myShader = new Shader();
    if(!myShader->load("BasicView", "glslfiles/basicTransformations.vert", "glslfiles/basicTransformations.frag"))
	{
		cout << "failed to load shader" << endl;
	}		

	myBasicShader = new Shader();
	if(!myBasicShader->load("Basic", "glslfiles/basic.vert", "glslfiles/basic.frag"))
	{
		cout << "failed to load shader" << endl;
	}		

	sphereShader = new Shader();
	if (!sphereShader->load("BasicView", "glslfiles/basicSpecular.vert", "glslfiles/basicSpecular.frag"))
	{
		cout << "failed to load shader" << endl;
	}

	glUseProgram(myShader->handle());  // use the shader

	glEnable(GL_TEXTURE_2D);

	//initialise objectTransformation matrix to identity mat 
	objectTransformation = glm::mat4(1.0f);

	cout << " loading model " << endl;
	if(objLoader.loadModel("Models/TAL16.obj", plane))//returns true if the model is loaded, puts the model in the model parameter
	{
		cout << " model loaded " << endl;		

		//model.calcCentrePoint();
		//model.centreOnZero();
	
		plane.calcVertNormalsUsingOctree();  //the method will construct the octree if it hasn't already been created.				

		//turn on VBO by setting useVBO to true in threeDmodel.cpp default constructor - only permitted on 8 series cards and higher
		plane.initDrawElements();
		plane.initVBO(myShader);
	}
	else
	{
		cout << " model failed to load " << endl;
	}

	if (objLoader.loadModel("Models/terrain.obj", terrain))//returns true if the model is loaded, puts the model in the model parameter
	{
		cout << " model loaded " << endl;

		//model.calcCentrePoint();
		//model.centreOnZero();

		terrain.calcVertNormalsUsingOctree();  //the method will construct the octree if it hasn't already been created.

		//turn on VBO by setting useVBO to true in threeDmodel.cpp default constructor - only permitted on 8 series cards and higher
		terrain.initDrawElements();
		terrain.initVBO(myShader);
	}
	else
	{
		cout << " model failed to load " << endl;
	}

	//Sphere construction for testing collisions
	
	mySphere.setCentre(pos.x, pos.y, pos.z);
	mySphere.setRadius(0.3);
	mySphere.constructGeometry(sphereShader, 10);

	/*
	terrainSphere.setCentre(10, 10, 5);
	terrainSphere.setRadius(3.1);
	terrainSphere.constructGeometry(sphereShader, 10);	
	*/
}

void special(int key, int x, int y){
	switch (key)
	{
	case GLUT_KEY_LEFT:
		Left = true;
		break;
	case GLUT_KEY_RIGHT:
		Right = true;
		break;
	case GLUT_KEY_UP:
		Up = true;
		break;
	case GLUT_KEY_DOWN:
		Down = true;
		break;
	}
}

void specialUp(int key, int x, int y){
	switch (key)
	{
	case GLUT_KEY_LEFT:
		Left = false;
		break;
	case GLUT_KEY_RIGHT:
		Right = false;
		break;
	case GLUT_KEY_UP:
		Up = false;
		break;
	case GLUT_KEY_DOWN:
		Down = false;
		break;	
	}
}

void keyFunc(unsigned char key, int x, int y){
	switch (key) {
		case 97:
			A = true;
			break;
		case 100:
			D = true;
			break;
		case 32:
			SPACE = true;
			break;
		case 27:
			exit(0);
			break;
	}
}

void keyUpFunc(unsigned char key, int x, int y) {
	switch (key) {
		case 97:
			A = false;
			break;
		case 100:
			D = false;
			break;
		case 32:
			SPACE = false;
			break;
	}
}

void processKeys()
{
	float spinXinc = 0.0f, spinYinc = 0.0f, spinZinc = 0.0f;
	if (Left)
	{
		spinZinc = -speed / 3;
	}
	if (Right)
	{
		spinZinc = speed / 3;
	}
	if (Up)
	{
		spinXinc = speed / 4;
	}
	if (Down)
	{
		spinXinc = -speed / 4;
	}
	if (A)
	{
		spinYinc = speed / 8;
	}
	if (D)
	{
		spinYinc = -speed / 8;
	}
	if (SPACE)
	{
		pos.x += objectTransformation[2][0] * speed;
		pos.y += objectTransformation[2][1] * speed;
		pos.z += objectTransformation[2][2] * speed;
	}
	updateTransform(spinXinc, spinYinc, spinZinc);
}

void updateTransform(float xinc, float yinc, float zinc)
{
	objectTransformation = glm::rotate(objectTransformation, xinc, glm::vec3(1,0,0));
	objectTransformation = glm::rotate(objectTransformation, yinc, glm::vec3(0,1,0));
	objectTransformation = glm::rotate(objectTransformation, zinc, glm::vec3(0,0,1));
}

void idle()
{
	processKeys();

	glutPostRedisplay();
}
/**************** END OPENGL FUNCTIONS *************************/

int main(int argc, char **argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(screenWidth, screenHeight);
	glutInitWindowPosition(20, 20);
	glutCreateWindow("Flight Simulator");

	//This initialises glew - it must be called after the window is created.
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		cout << " GLEW ERROR" << endl;
	}

	//initialise the objects for rendering
	init();

	glutReshapeFunc(reshape);
	//specify which function will be called to refresh the screen.
	glutDisplayFunc(display);

	glutSpecialFunc(special);
	glutSpecialUpFunc(specialUp);

	glutKeyboardFunc(keyFunc);
	glutKeyboardUpFunc(keyUpFunc);

	glutIdleFunc(idle);

	//starts the main loop. Program loops and calls callback functions as appropriate.
	glutMainLoop();

	return 0;
}

//Possible sphere collision implementation, not working
/*
bool checkSphereCol() {
	if ((pow((mySphere.cx - terrainSphere.cx), 2) + pow((mySphere.cy - mySphere.cy), 2) + pow((mySphere.cz - terrainSphere.cz), 2)) < pow(6.2, 2)) {
		return false;
	}
	return true;
}
*/