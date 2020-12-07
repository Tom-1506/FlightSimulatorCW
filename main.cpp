#include <iostream>
using namespace std;
#include <cmath>
#include "corecrt_math_defines.h"

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
Shader* skySphereShader;

//MODEL LOADING
#include "3DStruct\threeDModel.h"
#include "Obj\OBJLoader.h"

float amount = 0;
float temp = 0.002f;

ThreeDModel planeBody; //A threeDModel object for the plane body
ThreeDModel planeGlass; //A threeDModel object for the plane glass
ThreeDModel skySphere; //A threeDModel object for the sky sphere

// Terrain
ThreeDModel* models[2];
glm::vec3 modelOffsets[2];
ThreeDModel terrain; //A threeDModel object for the flat ground
ThreeDModel silo;
glm::vec3 terrainOffset = glm::vec3(0, -5, 0);

//Collision spheres
const int numSpheres = 17;
Sphere* sphereList[numSpheres];
glm::vec3 sphereOffsets[numSpheres];

Sphere noseSphere;
glm::vec3 noseSphereOffset;
Sphere frontCockpitSphere;
glm::vec3 frontCockpitSphereOffset;
Sphere midCockpitSphere;
glm::vec3 midCockpitSphereOffset;
Sphere backCockpitSphere;
glm::vec3 backCockpitSphereOffset;
Sphere finBaseSphere;
glm::vec3 finBaseSphereOffset;
Sphere frontWheelSphere;
glm::vec3 frontWheelSphereOffset;
Sphere leftWheelSphere;
glm::vec3 leftWheelSphereOffset;
Sphere rightWheelSphere;
glm::vec3 rightWheelSphereOffset;
Sphere leftWingFrontSphere;
glm::vec3 leftWingFrontSphereOffset;
Sphere leftWingBackSphere;
glm::vec3 leftWingBackSphereOffset;
Sphere rightWingFrontSphere;
glm::vec3 rightWingFrontSphereOffset;
Sphere rightWingBackSphere;
glm::vec3 rightWingBackSphereOffset;
Sphere leftStabSphere;
glm::vec3 leftStabSphereOffset;
Sphere rightStabSphere;
glm::vec3 rightStabSphereOffset;
Sphere finSphere;
glm::vec3 finSphereOffset;
Sphere jetSphere;
glm::vec3 jetSphereOffset;
Sphere baseCockpitSphere;
glm::vec3 baseCockpitSphereOffset;

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
float LightPos[4] = {0.0f, 1.0f, 1.0f, 0.0f};

double minX = -50;
double minY = 0;
double minZ = -50;
double maxX = 50;
double maxY = 10;
double maxZ = 50;

int	mouse_x=0, mouse_y=0;
int screenWidth = 960, screenHeight = 540;

//booleans to handle when the arrow keys are pressed or released.
bool rollLeft = false;
bool rollRight = false;
bool pitchFwd = false;
bool pitchBack = false;
bool yawLeft = false;
bool yawRight = false;
bool accelerate = false;
bool decelerate = false;
bool changeView = false;

bool floorCollideFront = false;
bool floorCollideBack = false;

float spin=180;
double velocity = 0.0f;
double gravity = -0.075f;
int camera = 0;

//OPENGL FUNCTION PROTOTYPES
void display();				//called in winmain to draw everything to the screen
void reshape(int width, int height);				//called when the window is resized
void init();				//called in winmain when the program starts.
void processKeys();         //called in winmain to process keyboard input
void idle();		//idle function
void updateTransform(float xinc, float yinc, float zinc);
void changeCamera();
//bool checkSphereCol();

/*************    START OF OPENGL FUNCTIONS   ****************/
void display()									
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glUseProgram(myShader->handle());  // use the shader

	GLuint matLocation = glGetUniformLocation(myShader->handle(), "ProjectionMatrix");  
	glUniformMatrix4fv(matLocation, 1, GL_FALSE, &ProjectionMatrix[0][0]);

	glm::mat4 viewingMatrix = glm::mat4(1.0f);
	
	switch (camera) {
	case 0:
		//environment view
		viewingMatrix = glm::lookAt(glm::vec3(4, 4, 4), pos, glm::vec3(0.0f, 1.0f, 0.0));
		break;
	case 1:
		//view behind jet
		viewingMatrix = glm::lookAt(pos + (glm::mat3(objectTransformation) * glm::vec3(0, 0.8, -5)), pos + (glm::mat3(objectTransformation) * glm::vec3(0, 0.8, 0)), glm::vec3(objectTransformation[1]));
		//viewingMatrix = glm::lookAt(pos + (glm::mat3(objectTransformation) * glm::vec3(7, 3, -7)), pos + (glm::mat3(objectTransformation) * glm::vec3(0, 0, 0)), glm::vec3(objectTransformation[1]));
		break;
	case 2:
		//cockpit view, needs interior work
		viewingMatrix = glm::lookAt(pos + (glm::mat3(objectTransformation) * glm::vec3(0, 0.18, 1.23)), pos + (glm::mat3(objectTransformation) * glm::vec3(0, 0.178, 1.3)), glm::vec3(objectTransformation[1]));
		break;
	}	

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

	// SPHERES --------------------------
	
	glm::mat4 modelmatrix = glm::translate(glm::mat4(1.0f), pos);

	ModelViewMatrix = viewingMatrix * modelmatrix * objectTransformation;

	glUseProgram(sphereShader->handle());
	glUniformMatrix4fv(glGetUniformLocation(sphereShader->handle(), "ViewMatrix"), 1, GL_FALSE, &viewingMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(sphereShader->handle(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	for (int i = 0; i < numSpheres; i++) {
		sphereList[i]->setCentre(glm::vec3(pos + (glm::mat3(objectTransformation) * sphereOffsets[i])));		
		
		for (ThreeDModel* model : models) {
			if (model->checkPointInOctree(sphereList[i]->getCentre(), terrainOffset)) {
				sphereList[i]->render();
				if (i >= 11) {
					floorCollideBack = true;
				}
				else {
					floorCollideFront = true;
				}
			}	
		}			
	}

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glUniformMatrix4fv(glGetUniformLocation(sphereShader->handle(), "ProjectionMatrix"), 1, GL_FALSE, &ProjectionMatrix[0][0]);
	
	// END SPHERES ----------------------

	// SKY SPHERE ----------------------
	
	glUseProgram(skySphereShader->handle());  // use the shader

	glUniformMatrix4fv(glGetUniformLocation(skySphereShader->handle(), "ProjectionMatrix"), 1, GL_FALSE, &ProjectionMatrix[0][0]);

	ModelViewMatrix = glm::translate(viewingMatrix, glm::vec3(0, 0, 0)); //resets model view for skySphere
	glUniformMatrix4fv(glGetUniformLocation(skySphereShader->handle(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);

	glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix)); //lighting normals for skySphere
	glUniformMatrix3fv(glGetUniformLocation(skySphereShader->handle(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);

	skySphere.drawElementsUsingVBO(skySphereShader);

	glUseProgram(myBasicShader->handle());  // use the shader
	glUniformMatrix4fv(glGetUniformLocation(myBasicShader->handle(), "ProjectionMatrix"), 1, GL_FALSE, &ProjectionMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(myBasicShader->handle(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	
	// END SKY SPHERE ------------------
	
	// TERRAIN -------------------------

	glUseProgram(myShader->handle());  // use the shader

	ModelViewMatrix = glm::translate(viewingMatrix, terrainOffset); //resets model view for terrain 
	glUniformMatrix4fv(glGetUniformLocation(myShader->handle(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);

	normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix)); // lighting normals for terrain
	glUniformMatrix3fv(glGetUniformLocation(myShader->handle(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);
		
	for (ThreeDModel* model : models) {
		model->drawElementsUsingVBO(myShader);
	}

	glUseProgram(myBasicShader->handle());  // use the shader
	glUniformMatrix4fv(glGetUniformLocation(myBasicShader->handle(), "ProjectionMatrix"), 1, GL_FALSE, &ProjectionMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(myBasicShader->handle(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
		
	for (ThreeDModel* model : models) {
		//model->drawOctreeLeaves(myBasicShader);
		//model->drawBoundingBox(myBasicShader);
	}

	// END TERRAIN ---------------------

	// PLANE BODY ---------------------------

	glUseProgram(myShader->handle());  // use the shader

	ModelViewMatrix = glm::translate(viewingMatrix, glm::vec3(0, 5, 0)); //resets model view for plane 
	ModelViewMatrix = viewingMatrix * modelmatrix * objectTransformation;
	glUniformMatrix4fv(glGetUniformLocation(myShader->handle(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);

	normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix)); // lighting normals for plane
	glUniformMatrix3fv(glGetUniformLocation(myShader->handle(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);

	planeBody.drawElementsUsingVBO(myShader);

	glUseProgram(myBasicShader->handle());  // use the shader
	glUniformMatrix4fv(glGetUniformLocation(myBasicShader->handle(), "ProjectionMatrix"), 1, GL_FALSE, &ProjectionMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(myBasicShader->handle(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);

	//plane.drawOctreeLeaves(myBasicShader);

	// END PLANE BODY -----------------------

	// PLANE BODY ---------------------------

	glUseProgram(myShader->handle());  // use the shader

	ModelViewMatrix = glm::translate(viewingMatrix, glm::vec3(0, 5, 0)); //resets model view for plane 
	ModelViewMatrix = viewingMatrix * modelmatrix * objectTransformation;
	glUniformMatrix4fv(glGetUniformLocation(myShader->handle(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);

	normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix)); // lighting normals for plane
	glUniformMatrix3fv(glGetUniformLocation(myShader->handle(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	planeGlass.drawElementsUsingVBO(myShader);

	glUseProgram(myBasicShader->handle());  // use the shader
	glUniformMatrix4fv(glGetUniformLocation(myBasicShader->handle(), "ProjectionMatrix"), 1, GL_FALSE, &ProjectionMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(myBasicShader->handle(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);

	//plane.drawOctreeLeaves(myBasicShader);

	// END PLANE BODY -----------------------

	glFlush();
	glutSwapBuffers();
}

void reshape(int width, int height)		// Resize the OpenGL window
{
	screenWidth=width; screenHeight = height;           // to ensure the mouse coordinates match 
														// we will use these values to set the coordinate system

	glViewport(0,0,width,height);						// Reset The Current Viewport

	//Set the projection matrix
	ProjectionMatrix = glm::perspective(glm::radians(60.0f), (GLfloat)screenWidth/(GLfloat)screenHeight, 0.01f, 800.0f);
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

	skySphereShader = new Shader();
	if (!skySphereShader->load("BasicView", "glslfiles/sky.vert", "glslfiles/sky.frag"))
	{
		cout << "failed to load shader" << endl;
	}

	sphereShader = new Shader();
	if (!sphereShader->load("BasicView", "glslfiles/basicTransformations.vert", "glslfiles/basicTransformations.frag"))
	{
		cout << "failed to load shader" << endl;
	}

	glUseProgram(myShader->handle());  // use the shader

	glEnable(GL_TEXTURE_2D);

	//initialise objectTransformation matrix to identity mat 
	objectTransformation = glm::mat4(1.0f);

	cout << " loading models " << endl;
	if(objLoader.loadModel("Models/TAL16body.obj", planeBody))//returns true if the model is loaded, puts the model in the model parameter
	{
		cout << " model loaded " << endl;		

		//model.calcCentrePoint();
		//model.centreOnZero();
	
		planeBody.calcVertNormalsUsingOctree();  //the method will construct the octree if it hasn't already been created.				

		//turn on VBO by setting useVBO to true in threeDmodel.cpp default constructor - only permitted on 8 series cards and higher
		planeBody.initDrawElements();
		planeBody.initVBO(myShader);
	}
	else
	{
		cout << " model failed to load " << endl;
	}

	if (objLoader.loadModel("Models/TAL16glass.obj", planeGlass))//returns true if the model is loaded, puts the model in the model parameter
	{
		cout << " model loaded " << endl;

		//model.calcCentrePoint();
		//model.centreOnZero();

		planeGlass.calcVertNormalsUsingOctree();  //the method will construct the octree if it hasn't already been created.				

		//turn on VBO by setting useVBO to true in threeDmodel.cpp default constructor - only permitted on 8 series cards and higher
		planeGlass.initDrawElements();
		planeGlass.initVBO(myShader);
	}
	else
	{
		cout << " model failed to load " << endl;
	}
	
	if (objLoader.loadModel("Models/flatGroundBox.obj", terrain))//returns true if the model is loaded, puts the model in the model parameter
	{
		cout << " model loaded " << endl;

		//model.calcCentrePoint();
		//model.centreOnZero();

		terrain.calcVertNormalsUsingOctree();  //the method will construct the octree if it hasn't already been created.
		terrain.calcBoundingBox(minX, minY, minZ, maxX, maxY, maxZ);

		//turn on VBO by setting useVBO to true in threeDmodel.cpp default constructor - only permitted on 8 series cards and higher
		terrain.initDrawElements();
		terrain.initVBO(myShader);
		models[0] = &terrain;
	}
	else
	{
		cout << " model failed to load " << endl;
	}

	if (objLoader.loadModel("Models/silo.obj", silo))//returns true if the model is loaded, puts the model in the model parameter
	{
		cout << " model loaded " << endl;

		silo.calcCentrePoint();
		//model.centreOnZero();

		silo.calcVertNormalsUsingOctree();  //the method will construct the octree if it hasn't already been created.
		silo.calcBoundingBox(minX, minY, minZ, maxX, maxY, maxZ);

		//turn on VBO by setting useVBO to true in threeDmodel.cpp default constructor - only permitted on 8 series cards and higher
		silo.initDrawElements();
		silo.initVBO(myShader);
		models[1] = &silo;
	}
	else
	{
		cout << " model failed to load " << endl;
	}

	if (objLoader.loadModel("Models/skySphere.obj", skySphere))//returns true if the model is loaded, puts the model in the model parameter
	{
		cout << " model loaded " << endl;

		skySphere.calcCentrePoint();
		skySphere.centreOnZero();

		//turn on VBO by setting useVBO to true in threeDmodel.cpp default constructor - only permitted on 8 series cards and higher
		skySphere.initDrawElements();
		skySphere.initVBO(myShader);
	}
	else
	{
		cout << " model failed to load " << endl;
	}

	// Define Collision spheres
	
	noseSphereOffset = glm::vec3(0.0, -0.25, 2.95);
	noseSphere.setCentre(pos + noseSphereOffset);
	noseSphere.setRadius(0.4);
	noseSphere.constructGeometry(sphereShader, 10);
	sphereList[0] = &noseSphere;
	sphereOffsets[0] = noseSphereOffset;

	frontCockpitSphereOffset = glm::vec3(0.0, 0.0, 2.0);
	frontCockpitSphere.setCentre(pos + frontCockpitSphereOffset);
	frontCockpitSphere.setRadius(0.4);
	frontCockpitSphere.constructGeometry(sphereShader, 10);
	sphereList[1] = &frontCockpitSphere;
	sphereOffsets[1] = frontCockpitSphereOffset;
	
	midCockpitSphereOffset = glm::vec3(0.0, 0.3, 1.0);
	midCockpitSphere.setCentre(pos + midCockpitSphereOffset);
	midCockpitSphere.setRadius(0.4);
	midCockpitSphere.constructGeometry(sphereShader, 10);
	sphereList[2] = &midCockpitSphere;
	sphereOffsets[2] = midCockpitSphereOffset;

	backCockpitSphereOffset = glm::vec3(0.0, 0.28, 0.0);
	backCockpitSphere.setCentre(pos + backCockpitSphereOffset);
	backCockpitSphere.setRadius(0.4);
	backCockpitSphere.constructGeometry(sphereShader, 10);
	sphereList[3] = &backCockpitSphere;
	sphereOffsets[3] = backCockpitSphereOffset;
	
	frontWheelSphereOffset = glm::vec3(0.0, -0.79, 1.95);
	frontWheelSphere.setCentre(pos + frontWheelSphereOffset);
	frontWheelSphere.setRadius(0.4);
	frontWheelSphere.constructGeometry(sphereShader, 10);
	sphereList[4] = &frontWheelSphere;
	sphereOffsets[4] = frontWheelSphereOffset;

	baseCockpitSphereOffset = glm::vec3(0.0, -0.35, 0.6);
	baseCockpitSphere.setCentre(pos + baseCockpitSphereOffset);
	baseCockpitSphere.setRadius(0.4);
	baseCockpitSphere.constructGeometry(sphereShader, 10);
	sphereList[5] = &baseCockpitSphere;
	sphereOffsets[5] = baseCockpitSphereOffset;

	finBaseSphereOffset = glm::vec3(0.0, 0.25, -1.6);
	finBaseSphere.setCentre(pos + finBaseSphereOffset);
	finBaseSphere.setRadius(0.4);
	finBaseSphere.constructGeometry(sphereShader, 10);
	sphereList[6] = &finBaseSphere;
	sphereOffsets[6] = finBaseSphereOffset; //

	rightWingFrontSphereOffset = glm::vec3(-0.8, -0.25, -0.25);
	rightWingFrontSphere.setCentre(pos + rightWingFrontSphereOffset);
	rightWingFrontSphere.setRadius(0.4);
	rightWingFrontSphere.constructGeometry(sphereShader, 10);
	sphereList[7] = &rightWingFrontSphere;
	sphereOffsets[7] = rightWingFrontSphereOffset;

	leftWingFrontSphereOffset = glm::vec3(0.8, -0.25, -0.25);
	leftWingFrontSphere.setCentre(pos + leftWingFrontSphereOffset);
	leftWingFrontSphere.setRadius(0.4);
	leftWingFrontSphere.constructGeometry(sphereShader, 10);
	sphereList[8] = &leftWingFrontSphere;
	sphereOffsets[8] = leftWingFrontSphereOffset;

	leftWingBackSphereOffset = glm::vec3(1.65, -0.25, -0.7);
	leftWingBackSphere.setCentre(pos + leftWingBackSphereOffset);
	leftWingBackSphere.setRadius(0.4);
	leftWingBackSphere.constructGeometry(sphereShader, 10);
	sphereList[9] = &leftWingBackSphere;
	sphereOffsets[9] = leftWingBackSphereOffset; 

	rightWingBackSphereOffset = glm::vec3(-1.65, -0.25, -0.7);
	rightWingBackSphere.setCentre(pos + rightWingBackSphereOffset);
	rightWingBackSphere.setRadius(0.4);
	rightWingBackSphere.constructGeometry(sphereShader, 10);
	sphereList[10] = &rightWingBackSphere;
	sphereOffsets[10] = rightWingBackSphereOffset; 

	leftWheelSphereOffset = glm::vec3(0.7, -0.79, -0.6);
	leftWheelSphere.setCentre(pos + leftWheelSphereOffset);
	leftWheelSphere.setRadius(0.4);
	leftWheelSphere.constructGeometry(sphereShader, 10);
	sphereList[11] = &leftWheelSphere;
	sphereOffsets[11] = leftWheelSphereOffset; //
	
	rightWheelSphereOffset = glm::vec3(-0.7, -0.79, -0.6);
	rightWheelSphere.setCentre(pos + rightWheelSphereOffset);
	rightWheelSphere.setRadius(0.4);
	rightWheelSphere.constructGeometry(sphereShader, 10);
	sphereList[12] = &rightWheelSphere;
	sphereOffsets[12] = rightWheelSphereOffset; //

	leftStabSphereOffset = glm::vec3(0.85, -0.25, -2.3);
	leftStabSphere.setCentre(pos + leftStabSphereOffset);
	leftStabSphere.setRadius(0.4);
	leftStabSphere.constructGeometry(sphereShader, 10);
	sphereList[13] = &leftStabSphere;
	sphereOffsets[13] = leftStabSphereOffset; //

	rightStabSphereOffset = glm::vec3(-0.85, -0.25, -2.3);
	rightStabSphere.setCentre(pos + rightStabSphereOffset);
	rightStabSphere.setRadius(0.4);
	rightStabSphere.constructGeometry(sphereShader, 10);
	sphereList[14] = &rightStabSphere;
	sphereOffsets[14] = rightStabSphereOffset; //

	finSphereOffset = glm::vec3(0.0, 0.9, -2.1);
	finSphere.setCentre(pos + finSphereOffset);
	finSphere.setRadius(0.4);
	finSphere.constructGeometry(sphereShader, 10);
	sphereList[15] = &finSphere;
	sphereOffsets[15] = finSphereOffset; //

	jetSphereOffset = glm::vec3(0.0, 0.0, -3.1);
	jetSphere.setCentre(pos + jetSphereOffset);
	jetSphere.setRadius(0.4);
	jetSphere.constructGeometry(sphereShader, 10);
	sphereList[16] = &jetSphere;
	sphereOffsets[16] = jetSphereOffset; //
}

void special(int key, int x, int y){
	switch (key)
	{
	case GLUT_KEY_LEFT:
		rollLeft = true;
		break;
	case GLUT_KEY_RIGHT:
		rollRight = true;
		break;
	case GLUT_KEY_UP:
		pitchFwd = true;
		break;
	case GLUT_KEY_DOWN:
		pitchBack = true;
		break;
	}
}

void specialUp(int key, int x, int y){
	switch (key)
	{
	case GLUT_KEY_LEFT:
		rollLeft = false;
		break;
	case GLUT_KEY_RIGHT:
		rollRight = false;
		break;
	case GLUT_KEY_UP:
		pitchFwd = false;
		break;
	case GLUT_KEY_DOWN:
		pitchBack = false;
		break;	
	}
}

void keyFunc(unsigned char key, int x, int y){
	switch (key) {
		case 97: // a
			yawLeft = true;
			break;
		case 65: // A
			yawLeft = true;
			break;
		case 100: // d
			yawRight = true;
			break;
		case 68: // D
			yawRight = true;
			break;
		case 99: // c
			changeView = true;
			break;
		case 67: // C
			changeView = true;
			break;
		case 32: // space
			accelerate = true;
			break;
		case 115: // s
			decelerate = true;
			break;
		case 83: // S
			decelerate = true;
			break;
		case 27: // esc
			exit(0);
			break;
	}
}

void keyUpFunc(unsigned char key, int x, int y) {
	switch (key) {
		case 97: // a
			yawLeft = false;
			break;
		case 65: // A
			yawLeft = false;
			break;
		case 100: // d
			yawRight = false;
			break;
		case 68: // D
			yawRight = false;
			break;
		case 32: // space
			accelerate = false;
			break;
		case 115: // s
			decelerate = false;
			break;
		case 83: // S
			decelerate = false;
			break;
	}
}

void processKeys()
{
	float spinXinc = 0.0f, spinYinc = 0.0f, spinZinc = 0.0f;
	if (rollRight)
	{
		spinZinc = 0.0333;
	}
	if (rollLeft)
	{
		spinZinc = -0.0333;
	}
	if (pitchFwd)
	{
		spinXinc = 0.02;
	}
	if (pitchBack)
	{
		spinXinc = -0.02;
	}
	if (yawLeft)
	{
		spinYinc = 0.0125;
	}
	if (yawRight)
	{
		spinYinc = -0.0125;
	}
	if (changeView)
	{
		changeCamera();
		changeView = false;
	}
	if (accelerate && velocity < 1)
	{
		velocity += 0.001;
	}
	if (decelerate)
	{
		if (velocity > 0) {
			velocity -= 0.003;
		}
	}
	updateTransform(spinXinc, spinYinc, spinZinc);
}

void updateTransform(float xinc, float yinc, float zinc)
{
    if (velocity > 0) { //natural deceleration
		velocity = 0.999 * velocity;
	}
	if (velocity < 0) { //prevents reversing
		velocity = 0;
	} 
	if (floorCollideFront) {
		//headingVec - objectTransformationZ
		//new - updatedVecZ
		//dot product and get the angle
		//glm::rotate angle and axis objectTransformationX

		glm::vec3 headingVec = glm::normalize(objectTransformation[2]);
		glm::vec3 updatedVecZ = glm::normalize(glm::reflect(glm::vec3(objectTransformation[2]), glm::vec3(0, 1, 0)));

		float numerator = glm::dot(headingVec, updatedVecZ);
		float denominator = (glm::length(headingVec) * (glm::length(updatedVecZ)));

		std::cout << "rotate around x axis by: " << -(acosf(numerator / denominator) * (180.0f/M_PI)) << std::endl; 

		objectTransformation = glm::rotate(objectTransformation, -acosf(numerator / denominator), glm::vec3(1,0,0));		

		pos.y += objectTransformation[2][1] * (velocity * 5);
	}
	if (floorCollideBack) {
		//headingVec - objectTransformationZ
		//new - flat z axis

		glm::vec3 headingVec = glm::normalize(objectTransformation[2]);
		glm::vec3 updatedVecZ = glm::normalize(glm::vec3(0, 0, 1));

		float numerator = glm::dot(headingVec, updatedVecZ);
		float denominator = (glm::length(headingVec) * (glm::length(updatedVecZ)));

		std::cout << "rotate around x axis by: " << -(acosf(numerator / denominator) * (180.0f / M_PI)) << std::endl;

		objectTransformation = glm::rotate(objectTransformation, -acosf(numerator / denominator), glm::vec3(1, 0, 0));
	}
	//std::cout << velocity << std::endl;
	
	pos.x += objectTransformation[2][0] * velocity;
	pos.y += objectTransformation[2][1] * velocity;
	pos.y += gravity;
	pos.z += objectTransformation[2][2] * velocity;
	objectTransformation = glm::rotate(objectTransformation, xinc, glm::vec3(1,0,0));
	objectTransformation = glm::rotate(objectTransformation, yinc, glm::vec3(0,1,0));
	objectTransformation = glm::rotate(objectTransformation, zinc, glm::vec3(0,0,1));
	floorCollideFront = false;
	floorCollideBack = false;
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

void changeCamera() {
	camera++;
	if (camera > 2)
		camera = 0;
}

//sphere collision implementation
/*
bool checkSphereCol() {
	if ((sphereCentre.x < maxX && sphereCentre.x > minX) && (sphereCentre.y < maxY && sphereCentre.y > minY) && (sphereCentre.z < maxZ && sphereCentre.z > minZ)) {
		return true;
	}
	else {
		return false;
	}
}*/