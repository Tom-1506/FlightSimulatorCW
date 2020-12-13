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
glm::mat4 ufoTransformation;

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
ThreeDModel ufoBody; //A threeDModel object for the ufo body
ThreeDModel ufoGlass; //A threeDModel object for the ufo glass
ThreeDModel skySphere; //A threeDModel object for the sky sphere

// Terrain
ThreeDModel terrain; //A threeDModel object for the flat ground
glm::vec3 terrainOffset = glm::vec3(0, -5, 0);

// Environment models
ThreeDModel* models[4];
ThreeDModel rock1;
glm::vec3 rock1Centre = glm::vec3(-80, 0, 60);
ThreeDModel rock2;
glm::vec3 rock2Centre = glm::vec3(35, 0, -22);
ThreeDModel rock3;
glm::vec3 rock3Centre = glm::vec3(-30, 0, -60);
ThreeDModel rock4;
glm::vec3 rock4Centre = glm::vec3(-57, 0, -35);
glm::vec3 rockCentres[4] = { rock1Centre, rock2Centre, rock3Centre, rock4Centre };
bool colliders[4] = {false, false, false, false};

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

glm::vec3 pos = glm::vec3(0.0f,-3.2999f,0.0f); //vector for the position of the object.
glm::vec3 ufoPos = glm::vec3(-60.0f, 20.0f, 80.0f); //vector for the position of the ufo.
bool ufoCollide = false;
bool ufoCrash = false;

//Material properties
float Material_Ambient[4] = {0.1f, 0.1f, 0.1f, 1.0f};
float Material_Diffuse[4] = {0.8f, 0.8f, 0.74f, 1.0f};
float Material_Specular[4] = {0.9f,0.9f,0.84f,1.0f};
float Material_Shininess = 50;

//Light Properties
float Light_Ambient_And_Diffuse[4] = {1.4f, 1.4f, 1.34f, 1.6f};
float Light_Specular[4] = {1.6f,1.6f,1.54f,1.6f};
float LightPos[4] = {0.0f, 15.0f, 30.0f, 0.0f};

//Wing light properties
float Jet_LightPos[4] = { pos.x, pos.y, pos.z, 1.0f };
float Jet_Light_Diffuse[4] = { 0.0f, 0.0f, 0.0f, 1.6f };

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

bool objectCollide = false;
bool floorCollide = true;
bool floorCollideFront = true;
bool floorCollideBack = true;
bool midCockpitDown = false;
bool anyWheelDown = true;
bool frontWheelDown = true;
bool leftWheelDown = true;
bool rightWheelDown = true;
bool backWheelsDown = true;
bool allWheelsDown = true;
bool leftWingDown = false;
bool rightWingDown = false;
bool finDown = false;
bool landed = true;
bool crashed = false;
bool crashedUpsideDown = false;

float spin = 180;
double velocity = 0.0f;
double heavyGravity = -0.09f;
double gravity = -0.045f;
int camera = 0;
bool crashCam = false;
bool light = false;
bool jetUp = true;

//OPENGL FUNCTIONS
void display();				//called in winmain to draw everything to the screen
void reshape(int width, int height);				//called when the window is resized
void init();				//called in winmain when the program starts.
void processKeys();         //called in winmain to process keyboard input
void idle();		//idle function
void updateTransform(float xinc, float yinc, float zinc); //updates all transformation info and related collisions
void changeCamera(); // changes the camera variable
void checkCols(); // checks collisions for the jet
void floorCollisionRotations(); // applies rotations to the jet when colliding with the floor
bool checkJetUp(); // returns whether the jet is oriented up
void toggleLight(); // toggles the light on and off

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
		viewingMatrix = glm::lookAt(glm::vec3(10, -3, 10), pos, glm::vec3(0.0f, 1.0f, 0.0));
		//viewingMatrix = glm::lookAt(glm::vec3(5, 15, 5), ufoPos, glm::vec3(0.0f, 1.0f, 0.0));
		break;
	case 1:
		//view behind jet
		if ((crashed && pos.y < -1 && !jetUp) || crashCam) {
			crashCam = true;
			viewingMatrix = glm::lookAt(pos + (glm::mat3(objectTransformation) * glm::vec3(3, -4, 5)), pos + (glm::mat3(objectTransformation) * glm::vec3(0, 0, 0)), glm::vec3(0, 1, 0));
		}
		else {
			viewingMatrix = glm::lookAt(pos + (glm::mat3(objectTransformation) * glm::vec3(0, 0.8, -5)), pos + (glm::mat3(objectTransformation) * glm::vec3(0, 0.8, 0)), glm::vec3(objectTransformation[1]));
		}
		break;
	case 2:
		//cockpit view
		viewingMatrix = glm::lookAt(pos + (glm::mat3(objectTransformation) * glm::vec3(0, 0.18, 1.23)), pos + (glm::mat3(objectTransformation) * glm::vec3(0, 0.178, 1.3)), glm::vec3(objectTransformation[1]));
		break;
	}

	glUniformMatrix4fv(glGetUniformLocation(myShader->handle(), "ViewMatrix"), 1, GL_FALSE, &viewingMatrix[0][0]);

	glUniform4fv(glGetUniformLocation(myShader->handle(), "LightPos"), 1, LightPos);
	glUniform4fv(glGetUniformLocation(myShader->handle(), "light_ambient"), 1, Light_Ambient_And_Diffuse);
	glUniform4fv(glGetUniformLocation(myShader->handle(), "light_diffuse"), 1, Light_Ambient_And_Diffuse);
	glUniform4fv(glGetUniformLocation(myShader->handle(), "light_specular"), 1, Light_Specular);

	Jet_LightPos[0] = pos.x;	//updates jet light position to be the position of the jet
	Jet_LightPos[1] = pos.y;
	Jet_LightPos[2] = pos.z;

	// changes the colour of the jet light based on certain parameters
	if (crashed) {	// light is red if crashed
		Jet_Light_Diffuse[0] = 4.0f;
		Jet_Light_Diffuse[1] = 0.0f;
		Jet_Light_Diffuse[2] = 0.0f;
	}
	else if (light && landed) { // light is blue if landed
		Jet_Light_Diffuse[0] = 0.0f;
		Jet_Light_Diffuse[1] = 0.0f;
		Jet_Light_Diffuse[2] = 4.0f;
	}
	else if (light && !landed) { // light is green when in the air
		Jet_Light_Diffuse[0] = 0.0f;
		Jet_Light_Diffuse[1] = 4.0f;
		Jet_Light_Diffuse[2] = 0.0f;
	}
	else {
		Jet_Light_Diffuse[0] = 0.0f;
		Jet_Light_Diffuse[1] = 0.0f;
		Jet_Light_Diffuse[2] = 0.0f;
	}

	glUniform4fv(glGetUniformLocation(myShader->handle(), "JetLightPos"), 1, Jet_LightPos);
	glUniform4fv(glGetUniformLocation(myShader->handle(), "jet_light_diffuse"), 1, Jet_Light_Diffuse);

	glUniform1f(glGetUniformLocation(myShader->handle(), "constantAttenuation"), 0.5f);
	glUniform1f(glGetUniformLocation(myShader->handle(), "linearAttenuation"), 0.5f);
	glUniform1f(glGetUniformLocation(myShader->handle(), "quadraticAttenuation"), 0.05f);

	glUniform4fv(glGetUniformLocation(myShader->handle(), "material_ambient"), 1, Material_Ambient);
	glUniform4fv(glGetUniformLocation(myShader->handle(), "material_diffuse"), 1, Material_Diffuse);
	glUniform4fv(glGetUniformLocation(myShader->handle(), "material_specular"), 1, Material_Specular);
	glUniform1f(glGetUniformLocation(myShader->handle(), "material_shininess"), Material_Shininess);

	// start rendering	

	// SPHERES --------------------------

	glm::mat4 modelmatrix = glm::translate(glm::mat4(1.0f), pos);
	glm::mat4 ufoModelmatrix = glm::translate(glm::mat4(1.0f), ufoPos);

	ModelViewMatrix = viewingMatrix * modelmatrix * objectTransformation;

	glUseProgram(sphereShader->handle());
	glUniformMatrix4fv(glGetUniformLocation(sphereShader->handle(), "ViewMatrix"), 1, GL_FALSE, &viewingMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(sphereShader->handle(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	for (int i = 0; i < numSpheres; i++) {
		sphereList[i]->setCentre(glm::vec3(pos + (glm::mat3(objectTransformation) * sphereOffsets[i])));

		if (terrain.checkPointInOctree(sphereList[i]->getCentre(), terrainOffset)) {
			if (i >= 11) {
				floorCollideBack = true;
				floorCollide = true;
			}
			else {
				floorCollideFront = true;
				floorCollide = true;
			}
		}		

		glm::vec3 ufoLocalPoint = glm::inverse(glm::translate(glm::mat4(1.0), ufoPos) * ufoTransformation) * glm::vec4(sphereList[i]->getCentre(), 1.0f);

		if (ufoBody.checkPointInOctree(ufoLocalPoint, glm::vec3(0.0)) || ufoGlass.checkPointInOctree(ufoLocalPoint, glm::vec3(0.0))){
			objectCollide = true;
			ufoCollide = true;
			ufoCrash = true;
		}

		for (int i = 0; i < 4; i++) {
			if (models[i]->checkPointInOctree(sphereList[i]->getCentre(), terrainOffset)) {
				objectCollide = true;
				colliders[i] = true;
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
		
	terrain.drawElementsUsingVBO(myShader);

	for (ThreeDModel* model : models) {
		model->drawElementsUsingVBO(myShader);
	}

	glUseProgram(myBasicShader->handle());  // use the shader
	glUniformMatrix4fv(glGetUniformLocation(myBasicShader->handle(), "ProjectionMatrix"), 1, GL_FALSE, &ProjectionMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(myBasicShader->handle(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);

	// END TERRAIN ---------------------

	// UFO ----------------------------------
	
	glUseProgram(myShader->handle());  // use the shader

	ModelViewMatrix = glm::translate(viewingMatrix, glm::vec3(0, 0, 0)); //resets model view for ufo 
	ModelViewMatrix = viewingMatrix * ufoModelmatrix * ufoTransformation;
	glUniformMatrix4fv(glGetUniformLocation(myShader->handle(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);

	normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix)); // lighting normals for ufo
	glUniformMatrix3fv(glGetUniformLocation(myShader->handle(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);

	ufoBody.drawElementsUsingVBO(myShader);
	ufoGlass.drawElementsUsingVBO(myShader);

	glUseProgram(myBasicShader->handle());  // use the shader
	glUniformMatrix4fv(glGetUniformLocation(myBasicShader->handle(), "ProjectionMatrix"), 1, GL_FALSE, &ProjectionMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(myBasicShader->handle(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	
	// END UFO ----------------------------

	// PLANE BODY ---------------------------

	glUseProgram(myShader->handle());  // use the shader

	ModelViewMatrix = glm::translate(viewingMatrix, glm::vec3(0, 0, 0)); //resets model view for plane 
	ModelViewMatrix = viewingMatrix * modelmatrix * objectTransformation;
	glUniformMatrix4fv(glGetUniformLocation(myShader->handle(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);

	normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix)); // lighting normals for plane
	glUniformMatrix3fv(glGetUniformLocation(myShader->handle(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);

	planeBody.drawElementsUsingVBO(myShader);

	glUseProgram(myBasicShader->handle());  // use the shader
	glUniformMatrix4fv(glGetUniformLocation(myBasicShader->handle(), "ProjectionMatrix"), 1, GL_FALSE, &ProjectionMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(myBasicShader->handle(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);

	// END PLANE BODY -----------------------

	// PLANE BODY ---------------------------

	glUseProgram(myShader->handle());  // use the shader

	ModelViewMatrix = glm::translate(viewingMatrix, glm::vec3(0, 0, 0)); //resets model view for plane 
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
	glClearColor(1.0,1.0,1.0,0.0);						

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
	ufoTransformation = glm::mat4(1.0f);

	cout << " loading models " << endl;
	if(objLoader.loadModel("Models/TAL16body.obj", planeBody))//returns true if the model is loaded, puts the model in the given threeDmodel object
	{
		cout << " model loaded " << endl;		
	
		planeBody.calcVertNormalsUsingOctree();				

		planeBody.initDrawElements();
		planeBody.initVBO(myShader);
	}
	else
	{
		cout << " model failed to load " << endl;
	}

	if (objLoader.loadModel("Models/TAL16glass.obj", planeGlass))
	{
		cout << " model loaded " << endl;

		planeGlass.calcVertNormalsUsingOctree();				

		planeGlass.initDrawElements();
		planeGlass.initVBO(myShader);
	}
	else
	{
		cout << " model failed to load " << endl;
	}

	if (objLoader.loadModel("Models/ufoBody.obj", ufoBody))
	{
		cout << " model loaded " << endl;

		ufoBody.calcVertNormalsUsingOctree();			

		ufoBody.initDrawElements();
		ufoBody.initVBO(myShader);
	}
	else
	{
		cout << " model failed to load " << endl;
	}

	if (objLoader.loadModel("Models/ufoGlass.obj", ufoGlass))
	{
		cout << " model loaded " << endl;

		ufoGlass.calcVertNormalsUsingOctree();			

		ufoGlass.initDrawElements();
		ufoGlass.initVBO(myShader);
	}
	else
	{
		cout << " model failed to load " << endl;
	}
	
	if (objLoader.loadModel("Models/flatGroundBox.obj", terrain))
	{
		cout << " model loaded " << endl;

		terrain.calcVertNormalsUsingOctree(); 

		terrain.initDrawElements();
		terrain.initVBO(myShader);
	}
	else
	{
		cout << " model failed to load " << endl;
	}
	
	if (objLoader.loadModel("Models/rock1.obj", rock1))
	{
		cout << " model loaded " << endl;

		rock1.calcVertNormalsUsingOctree();

		rock1.initDrawElements();
		rock1.initVBO(myShader);
		models[0] = &rock1;
	}
	else
	{
		cout << " model failed to load " << endl;
	}	
	
	if (objLoader.loadModel("Models/rock2.obj", rock2))
	{
		cout << " model loaded " << endl;

		rock2.calcVertNormalsUsingOctree(); 

		rock2.initDrawElements();
		rock2.initVBO(myShader);
		models[1] = &rock2;
	}
	else
	{
		cout << " model failed to load " << endl;
	}	
	
	if (objLoader.loadModel("Models/rock3.obj", rock3))
	{
		cout << " model loaded " << endl;

		rock3.calcVertNormalsUsingOctree(); 

		rock3.initDrawElements();
		rock3.initVBO(myShader);
		models[2] = &rock3;
	}
	else
	{
		cout << " model failed to load " << endl;
	}	
	
	if (objLoader.loadModel("Models/rock4.obj", rock4))
	{
		cout << " model loaded " << endl;

		rock4.calcVertNormalsUsingOctree(); 

		rock4.initDrawElements();
		rock4.initVBO(myShader);
		models[3] = &rock4;
	}
	else
	{
		cout << " model failed to load " << endl;
	}	
	
	if (objLoader.loadModel("Models/skySphere.obj", skySphere))
	{
		cout << " model loaded " << endl;

		skySphere.calcCentrePoint();
		skySphere.centreOnZero();

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

	leftStabSphereOffset = glm::vec3(0.85, -0.25, -2.6);
	leftStabSphere.setCentre(pos + leftStabSphereOffset);
	leftStabSphere.setRadius(0.4);
	leftStabSphere.constructGeometry(sphereShader, 10);
	sphereList[13] = &leftStabSphere;
	sphereOffsets[13] = leftStabSphereOffset; //

	rightStabSphereOffset = glm::vec3(-0.85, -0.25, -2.6);
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

	jetSphereOffset = glm::vec3(0.0, -0.23, -3.1);
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
		case 108: // l
			toggleLight();
			break;
		case 76: // L
			toggleLight();
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
	if (rollRight && !landed && velocity > 0.2 && !rightWheelDown && !(crashed && floorCollide))
	{
		spinZinc = 0.02;
	}
	if (rollLeft && !landed && velocity > 0.2 && !leftWheelDown && !(crashed && floorCollide))
	{
		spinZinc = -0.02;
	}
	if (pitchFwd && !landed && velocity > 0.2 && !frontWheelDown && !(crashed && floorCollide))
	{
		spinXinc = 0.016;
	}
	if (pitchBack && !landed && velocity > 0.2 && !(crashed && floorCollide))
	{
		spinXinc = -0.016;
	}
	if (yawLeft && velocity > 0.01 && !(crashed && floorCollide))
	{
		spinYinc = 0.0064;
	}
	if (yawRight && velocity > 0.01 && !(crashed && floorCollide))
	{
		spinYinc = -0.0064;
	}
	if (changeView)
	{
		changeCamera();
		changeView = false;
	}
	if (accelerate && velocity < 1 && !crashed)
	{
		velocity += 0.002;
	}
	if (decelerate || crashed)
	{
		if (velocity > 0) {
			velocity -= 0.001;
		}
	}
	updateTransform(spinXinc, spinYinc, spinZinc);
}

void updateTransform(float xinc, float yinc, float zinc)
{
	jetUp = checkJetUp();

    if (velocity > 0) { //natural deceleration
		velocity = 0.999 * velocity;
	}
	if (velocity < 0) { //prevents reversing
		velocity = 0;
	} 
	if (velocity > 0.4) {
		landed = false;
	}

	checkCols();

	if (floorCollide && velocity < 0.1 && !landed && allWheelsDown) {
		landed = true;
	}

	if (((floorCollide && velocity > 0.25 && !anyWheelDown) || (floorCollide && !anyWheelDown)) && !crashed) {
		crashed = true;
	}

	if (finDown && midCockpitDown && velocity < 0.01) {
		crashedUpsideDown = true;
	}
		
	if (floorCollide && !allWheelsDown) {
		floorCollisionRotations();
	}

	if (landed) {
		pos.y = -3.2999999;
	}
	else if (crashedUpsideDown) { // crashed upside down
		pos.y = -3.63;
	}
	else if (floorCollide) {
		pos.y += 0.01;
	}
	else if (crashed && pos.y > -3.1) {
		pos.y += heavyGravity;
	}
	else {
		pos.y += gravity;
	}

	if (floorCollideFront && !landed && velocity > 0.1) { //rotate backwards to bounce if front half of jet collides
		//headingVec - objectTransformationZ
		//new - updatedVecZ
		//dot product and get the angle
		//glm::rotate angle and axis objectTransformationX
		glm::vec3 headingVec = glm::normalize(objectTransformation[2]);
		glm::vec3 updatedVecZ = glm::normalize(glm::reflect(glm::vec3(objectTransformation[2]), glm::vec3(0, 1, 0)));

		float headingNumerator = glm::dot(headingVec, updatedVecZ);
		float headingDenominator = (glm::length(headingVec) * (glm::length(updatedVecZ)));

		float angle = acosf(headingNumerator / headingDenominator);

		if (!isnan(angle)) {
			if (jetUp) {
				objectTransformation = glm::rotate(objectTransformation, -angle, glm::vec3(1, 0, 0));
			}
			else if (!jetUp) {
				crashed = true;
				objectTransformation = glm::rotate(objectTransformation, angle, glm::vec3(1,0,0));
			}
		}

		pos.y += objectTransformation[2][1] * (velocity * 3);
	}

	if (objectCollide) {
		//make jet bounce away from an object it collides with

		crashed = true;

		glm::vec3 headingVec = glm::normalize(objectTransformation[2]);
		glm::vec3 updatedVecZ = glm::vec3(1.0f);

		if (ufoCollide) {
			updatedVecZ = glm::normalize(glm::reflect(glm::vec3(objectTransformation[2]), (pos - ufoPos)));
		}

		for (int i = 0; i < 4; i++) {
			if (colliders[i]) {
				updatedVecZ = glm::normalize(glm::reflect(glm::vec3(objectTransformation[2]), (pos - rockCentres[i])));
			}
		}

		float headingNumerator = glm::dot(headingVec, updatedVecZ);
		float headingDenominator = (glm::length(headingVec) * (glm::length(updatedVecZ)));

		objectTransformation = glm::rotate(objectTransformation, -acosf(headingNumerator / headingDenominator), glm::vec3(0, 1, 0));

		pos.z += objectTransformation[2][2] * (velocity * 3);
	}

	for (int i = 0; i < numSpheres; i++) { //check if any point on the jet collide and if they don't, set collide bools to false
		sphereList[i]->setCentre(glm::vec3(pos + (glm::mat3(objectTransformation) * sphereOffsets[i])));

		for (ThreeDModel* model : models) {
			if (model->checkPointInOctree(sphereList[i]->getCentre(), terrainOffset)) {
				break;
			}
			floorCollide = false;
			floorCollideFront = false;
			floorCollideBack = false;
		}
	}

	if (crashed && !floorCollide && pos.y > -1.5) {
		if (jetUp) {
			objectTransformation = glm::rotate(objectTransformation, 0.005f, glm::vec3(1, 0, 0));
		}
		else {
			objectTransformation = glm::rotate(objectTransformation, -0.005f, glm::vec3(1, 0, 0));
		}
	}	

	pos.x += objectTransformation[2][0] * velocity;
	pos.y += objectTransformation[2][1] * velocity;
	pos.z += objectTransformation[2][2] * velocity;
	objectTransformation = glm::rotate(objectTransformation, xinc, glm::vec3(1,0,0));
	objectTransformation = glm::rotate(objectTransformation, yinc, glm::vec3(0,1,0));
	objectTransformation = glm::rotate(objectTransformation, zinc, glm::vec3(0,0,1));

	if (ufoPos.y > -3) {
		if (ufoCrash) {
			ufoPos.y -= 0.5;
		}
		ufoPos.x += ufoTransformation[2][0] * 0.2;
		ufoPos.z += ufoTransformation[2][2] * 0.2;
		ufoTransformation = glm::rotate(ufoTransformation, 0.008f, glm::vec3(0, 1, 0));
	}	

	objectCollide = false;
	ufoCollide = false;
	for (bool b : colliders) {
		b = false;
	}
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

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		cout << " GLEW ERROR" << endl;
	}

	init();

	glutReshapeFunc(reshape);
	glutDisplayFunc(display);

	glutSpecialFunc(special);
	glutSpecialUpFunc(specialUp);

	glutKeyboardFunc(keyFunc);
	glutKeyboardUpFunc(keyUpFunc);

	glutIdleFunc(idle);

	glutMainLoop();

	return 0;
}

void changeCamera() {
	camera++;
	if (camera > 2)
		camera = 0;
}

void checkCols() {
	if (terrain.checkPointInOctree(sphereList[2]->getCentre(), terrainOffset)) {
		midCockpitDown = true;
	}
	else {
		midCockpitDown = false;
	}

	if (terrain.checkPointInOctree(sphereList[4]->getCentre(), terrainOffset)) {
		frontWheelDown = true;
	}
	else {
		frontWheelDown = false;
	}

	if (terrain.checkPointInOctree(sphereList[11]->getCentre(), terrainOffset)) {
		leftWheelDown = true;
	}
	else {
		leftWheelDown = false;
	}

	if (terrain.checkPointInOctree(sphereList[12]->getCentre(), terrainOffset)) {
		rightWheelDown = true;
	}
	else {
		rightWheelDown = false;
	}

	if (leftWheelDown && rightWheelDown) {
		backWheelsDown = true;
	}
	else {
		backWheelsDown = false;
	}

	if (backWheelsDown && frontWheelDown) {
		allWheelsDown = true;
	}
	else {
		allWheelsDown = false;
	}

	if (frontWheelDown || leftWheelDown || rightWheelDown) {
		anyWheelDown = true;
	}
	else {
		anyWheelDown = false;
	}

	if (terrain.checkPointInOctree(sphereList[15]->getCentre(), terrainOffset)) {
		finDown = true;
	}
	else {
		finDown = false;
	}

	if (terrain.checkPointInOctree(sphereList[9]->getCentre(), terrainOffset) || terrain.checkPointInOctree(sphereList[13]->getCentre(), terrainOffset)) {
		leftWingDown = true;
	}
	else {
		leftWingDown = false;
	}
	
	if (terrain.checkPointInOctree(sphereList[9]->getCentre(), terrainOffset) || terrain.checkPointInOctree(sphereList[14]->getCentre(), terrainOffset)) {
		rightWingDown = true;
	}
	else {
		rightWingDown = false;
	}
}

void floorCollisionRotations() {
	if (leftWingDown) {
		if (jetUp) {
			objectTransformation = glm::rotate(objectTransformation, 0.008f, glm::vec3(0, 0, 1));
		}
		else if (!finDown || !midCockpitDown) {
			objectTransformation = glm::rotate(objectTransformation, -0.008f, glm::vec3(0, 0, 1));
		}
	}
	else if (rightWingDown && jetUp) {
		if (jetUp) {
			objectTransformation = glm::rotate(objectTransformation, -0.008f, glm::vec3(0, 0, 1));
		}
		else if (!finDown || !midCockpitDown) {
			objectTransformation = glm::rotate(objectTransformation, 0.008f, glm::vec3(0, 0, 1));
		}
	}

	if (leftWheelDown) {
		//rotate right about z
		objectTransformation = glm::rotate(objectTransformation, 0.002f, glm::vec3(0, 0, 1));
		objectTransformation = glm::rotate(objectTransformation, 0.002f, glm::vec3(1, 0, 0));
	}
	else if (rightWheelDown) {
		//rotate left about z
		objectTransformation = glm::rotate(objectTransformation, -0.002f, glm::vec3(0, 0, 1));
		objectTransformation = glm::rotate(objectTransformation, 0.002f, glm::vec3(1, 0, 0));
	}
	else if (backWheelsDown) {
		//rotate forward about x
		objectTransformation = glm::rotate(objectTransformation, 0.002f, glm::vec3(1, 0, 0));
	}
	else if (frontWheelDown) {
		//rotate backward about x
		objectTransformation = glm::rotate(objectTransformation, -0.002f, glm::vec3(1, 0, 0));
	}

	if (finDown && !midCockpitDown) {
		objectTransformation = glm::rotate(objectTransformation, -0.008f, glm::vec3(1, 0, 0));
	}
	else if (midCockpitDown && !finDown) {
		objectTransformation = glm::rotate(objectTransformation, 0.008f, glm::vec3(1, 0, 0));
	}

	if (floorCollideBack && !anyWheelDown && !finDown && jetUp) {
		objectTransformation = glm::rotate(objectTransformation, 0.006f, glm::vec3(1, 0, 0));
	}
}

bool checkJetUp() {
	//find orientation of the jet to decide which direction to bounce in (up or down)
	glm::vec3 jetUpVec = glm::normalize(glm::vec3(objectTransformation[1]));
	glm::vec3 up = glm::normalize(glm::vec3(0, 1, 0));

	float upNumerator = glm::dot(jetUpVec, up);
	float upDenominator = (glm::length(jetUpVec) * (glm::length(up)));

	if ((acosf(upNumerator / upDenominator) * (180.0f / M_PI)) < 90) {
		return true;
	}
	else if ((acosf(upNumerator / upDenominator) * (180.0f / M_PI)) > 90) {
		return false;
	}
}

void toggleLight() {
	if (light && !crashed) {
		light = false;
	}
	else {
		light = true;
	}
}