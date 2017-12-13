//Some Windows Headers (For Time, IO, etc.)
#include <windows.h>
#include <mmsystem.h>
#include <MMSystem.h>
//GL includes
#include <GL/glew.h>
#include <GL/freeglut.h>
//IO includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
//STD
#include <stdio.h>
#include <math.h>
#include <vector> // STL dynamic memory.
#include <random>
//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.hpp"
#include "Model.hpp" 
#include "Light.hpp"
#include "Skybox.hpp"
#include "NPC.hpp"
#include "Bullet.hpp"
#include "UI.hpp"
#include "Player.hpp"

using namespace std;

// Random Number generation
default_random_engine generator;
uniform_int_distribution<int> enemy(0, 7);
uniform_real_distribution<> speed(0.25, 0.5);
uniform_real_distribution<> scale(0.25, 1.0);

// Properties
int width = 800;
int height = 600;

// Function prototypes
void keypress(unsigned char key, int x, int y);
void keyRelease(unsigned char key, int x, int y);
void mouseMotion(int x, int y);
void mouseScroll(int button, int state, int x, int y);

void spawnEnemies();
void drawEnemies(Model model, int matrix_location, Shader shader);
void drawBullets(Model model, int matrix_location, Shader shader);
void shoot();
void fillAmmo();
void detectCollision();
bool checkBoundingSphere(glm::vec3 point1, glm::vec3 point2, float radius1, float radius2);

// Shaders
Shader shaderProgram("../Shaders/lighting.vertex", "../Shaders/lighting.fragment");
Shader lampShader("../Shaders/lamp.vertex", "../Shaders/lamp.fragment");
Shader skyboxShader("../Shaders/skybox.vertex", "../Shaders/skybox.fragment");
Shader explodeShader("../Shaders/explode.vertex", "../Shaders/explode.fragment", "../Shaders/explode.geometry");
Shader textShader("../Shaders/text.vertex", "../Shaders/text.fragment");

// Camera
Camera camera(glm::vec3(0.0f, 5.5f,0.0f));
GLint lastX = 400, lastY = 300;
bool firstMouse = true;
int windowId;

#pragma region lights
// Direct Light
glm::vec3 directLightColor = glm::vec3(0.1f, 0.05f, 0.1f);
glm::vec3 directLightDirection = glm::vec3(-0.2f, -1.0f, -0.3f);
Light directLight(glm::vec3(0.0f), directLightDirection, directLightColor, DIRECT_LIGHT);
// Point lights
glm::vec3 pointLightPositions[] = {
	glm::vec3(9.0f, 8.0f, 9.0f),
	glm::vec3(9.0f, 8.0f, -9.0f),
	glm::vec3(-9.0f, 8.0f, 9.0f),
	glm::vec3(-9.0f, 8.0f, -9.0f)
};
glm::vec3 pointLightColors = glm::vec3(0.5f, 0.5f, 0.5f);
Light pointLight1(pointLightPositions[0], glm::vec3(0.0f), pointLightColors, POINT_LIGHT);
Light pointLight2(pointLightPositions[1], glm::vec3(0.0f), pointLightColors, POINT_LIGHT);
Light pointLight3(pointLightPositions[2], glm::vec3(0.0f), pointLightColors, POINT_LIGHT);
Light pointLight4(pointLightPositions[3], glm::vec3(0.0f), pointLightColors, POINT_LIGHT);

// Spotlights
glm::vec3 spotLightPositions[] = {
	//glm::vec3(-10.0f, 15.0f, -10.0f)
	glm::vec3(-80.0f, 15.0f, -80.0f),
	glm::vec3(80.0f, 15.0f, -80.0f),
	glm::vec3(80.0f, 15.0f, 80.0f),
	glm::vec3(-80.0f, 15.0f, 80.0f),
};
glm::vec3 spotlightColors = glm::vec3(1.0f, 0.0f, 0.0f);
glm::vec3 spotlightDirection = glm::vec3(0.0f, -1.0f, 0.0f);
Light spotLight1(spotLightPositions[0], spotlightDirection, spotlightColors, SPOTLIGHT);
Light spotLight2(spotLightPositions[1], spotlightDirection, spotlightColors, SPOTLIGHT);
Light spotLight3(spotLightPositions[2], spotlightDirection, spotlightColors, SPOTLIGHT);
Light spotLight4(spotLightPositions[3], spotlightDirection, spotlightColors, SPOTLIGHT);

#pragma endregion lights

//Models
Model nanosuit("../Resources/nanosuit/nanosuit.obj");
Model room("../Resources/map/jungle_scene.obj");
Model light("../Resources/light/light.obj");
Model playerBody("../Resources/player/player_body.obj");
Model playerBase("../Resources/player/player_base.obj");

Skybox skybox("../Resources/skybox/starfield/");

vector<NPC> enemies;
vector<Bullet> bullets;

Player player(camera.Front);

UI ui(0);

float rotate_player = 0.0f;

int score = 0;
bool game_begin = false;
bool game_over = false;
bool won = false;

void display(){

	//Clear buffers
	glClearColor(0.5f, 0.4f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (unsigned int i = 0; i < enemies.size(); i++) {
		if (enemies[i].gameOver) {
			game_over = true;
		}
	}
	if (score == 8) {
		game_over = true;
		won = true;
	}

	if (!game_begin) {
		glm::mat4 projection = glm::ortho(0.0f, (float)width, 0.0f, (float)height);
		textShader.Use();
		glUniformMatrix4fv(glGetUniformLocation(textShader.Program, "proj"), 1, GL_FALSE, glm::value_ptr(projection));

		ui.RenderText(textShader, "Prevent enemies reaching the platform by shooting them!", (float)width / 3.5f, (float)height / 7, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));
		ui.RenderText(textShader, "Left click to shoot", (float)width / 3.5f, (float)height / 9, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));
		ui.RenderText(textShader, "Press Enter to Play", ((float)width / 2.5f), ((float)height / 2), 0.5f, glm::vec3(0.3, 0.7f, 0.9f));
	}
	else {
		if (!game_over || won)
		{
			/*************************COMPUTATIONS*********************************************/

			detectCollision();

			/*******************UI************************************************/

			glm::mat4 projection = glm::ortho(0.0f, (float)width, 0.0f, (float)height);
			textShader.Use();
			glUniformMatrix4fv(glGetUniformLocation(textShader.Program, "proj"), 1, GL_FALSE, glm::value_ptr(projection));

			ui.RenderText(textShader, "Score: " + to_string(score), (float)width / 8, (float)height / 8, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));

			if (won) {
				ui.RenderText(textShader, "YOU WON!", (float)width / 2, (float)height / 2, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));
			}
			else {
				ui.RenderText(textShader, "+", ((float)width / 2) - 6, ((float)height / 2) - 6, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));
			}

			/******************************SCENE*********************************/
			shaderProgram.Use();

			glm::mat4 view = camera.GetViewMatrix();
			projection = glm::perspective(camera.Zoom, (float)width / (float)height, 0.1f, 10000.0f);

			//Declare your uniform variables that will be used in your shader
			int matrix_location = glGetUniformLocation(shaderProgram.Program, "model");
			int view_mat_location = glGetUniformLocation(shaderProgram.Program, "view");
			int proj_mat_location = glGetUniformLocation(shaderProgram.Program, "proj");

			glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, glm::value_ptr(projection));
			// Set the lighting uniforms
			glUniform3f(glGetUniformLocation(shaderProgram.Program, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);

			//Use Light sources
			directLight.Use(shaderProgram, 0);
			pointLight1.Use(shaderProgram, 0);
			pointLight2.Use(shaderProgram, 1);
			pointLight3.Use(shaderProgram, 2);
			pointLight4.Use(shaderProgram, 3);
			spotLight1.Use(shaderProgram, 0);
			spotLight2.Use(shaderProgram, 1);
			spotLight3.Use(shaderProgram, 2);
			spotLight4.Use(shaderProgram, 3);

			if (!won) {
				drawEnemies(nanosuit, matrix_location, shaderProgram);
			}

			//room
			glm::mat4 model1;
			model1 = glm::scale(model1, glm::vec3(1.6f, 1.6f, 1.6f));
			glUniformMatrix4fv(matrix_location, 1, GL_FALSE, glm::value_ptr(model1));
			room.Draw(shaderProgram);

			player.draw(playerBase, playerBody, matrix_location, shaderProgram, camera.Front);
			player.mRotate = false;
			
			/******************************LAMPS*********************************/
			lampShader.Use();

			//Declare your uniform variables that will be used in your shader
			matrix_location = glGetUniformLocation(lampShader.Program, "model");
			view_mat_location = glGetUniformLocation(lampShader.Program, "view");
			proj_mat_location = glGetUniformLocation(lampShader.Program, "proj");

			glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, glm::value_ptr(projection));

			drawBullets(light, matrix_location, lampShader);

			/******************************SKYBOX*********************************/

			glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content
			skyboxShader.Use();

			view = glm::mat4(glm::mat3(camera.GetViewMatrix()));	// Remove any translation component of the view matrix

			glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "proj"), 1, GL_FALSE, glm::value_ptr(projection));

			skybox.Draw(skyboxShader);

			glDepthFunc(GL_LESS); // Set depth function back to default

			/**********************************************************************/
		}
		else {
			glm::mat4 projection = glm::ortho(0.0f, (float)width, 0.0f, (float)height);
			textShader.Use();
			glUniformMatrix4fv(glGetUniformLocation(textShader.Program, "proj"), 1, GL_FALSE, glm::value_ptr(projection));

			if (!won) {
				ui.RenderText(textShader, "GAME OVER", (float)width / 2, (float)height / 2, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));
			}
			else {
				//ui.RenderText(textShader, "YOU WON!", (float)width / 2, (float)height / 2, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));
			}
			ui.RenderText(textShader, "Score: " + to_string(score), (float)width / 8, (float)height / 8, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));

		}
	}
	glutSwapBuffers();
}

void updateScene() {	

	// Placeholder code, if you want to work with framerate
	// Wait until at least 16ms passed since start of last frame (Effectively caps framerate at ~60fps)
	static DWORD  last_time = 0;
	DWORD  curr_time = timeGetTime();
	float  delta = (curr_time - last_time) * 0.001f;
	if (delta > 0.03f)
		delta = 0.03f;
	last_time = curr_time;

	
	// Draw the next frame
	glutPostRedisplay();
}

void init()
{
	// Set up the shaders
	shaderProgram.setupShaders();
	lampShader.setupShaders();
	skyboxShader.setupShaders();
	explodeShader.setupShaders();
	textShader.setupShaders();
	//UI
	ui.setup();
	//Skybox
	skybox.SetUp();
	// Load models
	nanosuit.loadModel(nanosuit.mPath);
	room.loadModel(room.mPath);
	light.loadModel(light.mPath);
	playerBody.loadModel(playerBody.mPath);
	playerBase.loadModel(playerBase.mPath);
}

int main(int argc, char** argv){
	//Set up the window
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);

	width = glutGet(GLUT_SCREEN_WIDTH);
	height = glutGet(GLUT_SCREEN_HEIGHT);
    glutInitWindowSize(width, height);
    windowId = glutCreateWindow("Hello Triangle");
	glutFullScreen();
	//Tell glut where the display function is
	glutDisplayFunc(display);
	glutIdleFunc(updateScene);
	glutKeyboardFunc(keypress);
	glutMouseFunc(mouseScroll);
	glutPassiveMotionFunc(mouseMotion);
	glutSetCursor(GLUT_CURSOR_NONE);
	//sets cursor to center of glut window
	glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT) / 2);
	//A call to glewInit() must be done after glut is initialized!
    GLenum res = glewInit();
	//Check for any errors
    if (res != GLEW_OK) {
      fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
      return 1;
    }
	//Set OpenGL options
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	//Set up your objects and shaders
	spawnEnemies();
	fillAmmo();
	init();
	//Begin infinite event loop
	glutMainLoop();

    return 0;
}

void keypress(unsigned char key, int x, int y) {
	GLfloat deltaTime = 0;
	switch (key) {
	case 8: // space
		camera.ProcessKeyboard(DOWN, deltaTime);
		break;
	case 13:
		game_begin = true;
		break;
	case 27: // esc
		glutDestroyWindow(windowId);
		exit(0);
		break;
	case 32: // space
		camera.ProcessKeyboard(UP, deltaTime);
		break;
	case 'w':
		camera.ProcessKeyboard(FORWARD, deltaTime);
		break;
	case 'a':
		camera.ProcessKeyboard(LEFT, deltaTime);
		break;
	case 's':
		camera.ProcessKeyboard(BACKWARD, deltaTime);
		break;
	case 'd':
		camera.ProcessKeyboard(RIGHT, deltaTime);
		break;
	}
}

void mouseMotion(int xpos, int ypos)
{
	if (xpos < 50 || xpos > (glutGet(GLUT_WINDOW_WIDTH) - 50) 
			|| ypos < 50 || ypos > glutGet(GLUT_WINDOW_HEIGHT)) 
	{
		lastX = width / 2;
		lastY = height / 2;

		glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) /2, glutGet(GLUT_WINDOW_HEIGHT) / 2);
		return;
	}

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	if (lastX > xpos) {
		rotate_player = rotate_player + 0.035f;
	}
	else if (lastX < xpos) {
		rotate_player = rotate_player - 0.035f;
	}

	GLint xoffset = xpos - lastX;
	GLint yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement((GLfloat)xoffset, (GLfloat)yoffset);
	player.mRotate = true;
}

void mouseScroll(int button, int state, int xoffset, int yoffset)
{
	// Wheel reports as button 3(scroll up) and button 4(scroll down)
	switch (button) {
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_UP) return; // Disregard redundant GLUT_UP events
		shoot();
		break;
	case 3:
		if (state == GLUT_UP) return; // Disregard redundant GLUT_UP events
		camera.ProcessMouseScroll(0, 0.25f);
		break;
	case 4:
		if (state == GLUT_UP) return;
		camera.ProcessMouseScroll(1, 0.25f);
		break;
	}
}

/**
 * Spawns foure enemies in the first four speicifed spawn locations
**/
void spawnEnemies() {

	float speed1 = (float)speed(generator);
	float speed2 = (float)speed(generator);
	float speed3 = (float)speed(generator);
	float speed4 = (float)speed(generator);

	float size1 = (float)scale(generator);
	float size2 = (float)scale(generator);
	float size3 = (float)scale(generator);
	float size4 = (float)scale(generator);

	int enemy1 = enemy(generator);
	int enemy2 = enemy(generator);
	int enemy3 = enemy(generator);
	int enemy4 = enemy(generator);

	vector<NPC> possibleSpawns;
	possibleSpawns.push_back(NPC(glm::vec3(-80.0f, 0.0f, -80.0f), size1, speed1));
	possibleSpawns.push_back(NPC(glm::vec3(80.0f, 0.0f, -80.0f), size2, speed2));
	possibleSpawns.push_back(NPC(glm::vec3(80.0f, 0.0f, 80.0f), size3, speed3));
	possibleSpawns.push_back(NPC(glm::vec3(-80.0f, 0.0f, 80.0f), size4, speed4));
	possibleSpawns.push_back(NPC(glm::vec3(-80.0f, 0.0f, 0.0f), size1, speed1));
	possibleSpawns.push_back(NPC(glm::vec3(80.0f, 0.0f, 0.0f), size2, speed2));
	possibleSpawns.push_back(NPC(glm::vec3(0.0f, 0.0f, -80.0f), size3, speed3));
	possibleSpawns.push_back(NPC(glm::vec3(0.0f, 0.0f, 80.0f), size4, speed4));

	enemies.push_back(possibleSpawns[enemy1]);
	enemies.push_back(possibleSpawns[enemy2]);
	enemies.push_back(possibleSpawns[enemy3]);
	enemies.push_back(possibleSpawns[enemy4]);
}

/**
 * Draws all active enemy models
**/
void drawEnemies(Model model, int matrix_location, Shader shader) {
	if (!enemies.empty())
	{
		for (unsigned int i = 0; i < enemies.size(); i++) {
			if (enemies[i].mAlive) {
				enemies[i].draw(model, matrix_location, shader);
			}
			else {
				enemies.erase(enemies.begin() + i);
			}
		}
	}
	else {
		spawnEnemies();
	}
}

/**
 * Draws all active bullet models
**/
void drawBullets(Model model, int matrix_location, Shader shader) {
	for (unsigned int i = 0; i < bullets.size(); i++) {
		if ((bullets[i].mTraveling) && (bullets[i].mCurrentDistance < bullets[i].mMaxDistance)) {
			bullets[i].draw(model, matrix_location, shader);
		}
		else {
			bullets[i].mTraveling = false;
		}
	}
}

/**
 * Creates 50 bullet objects. Used to limit amount of possible active bullets.
**/
void fillAmmo() {
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 direction = glm::vec3(0.0f, 0.0f, 0.0f);
	float speed = 3.5f;
	float scale = 0.1f;

	for (unsigned int i = 0; i < 50; i++) {
		Bullet bullet(position, direction, speed, scale);
		bullet.mTraveling = false;
		bullets.push_back(bullet);
	}
}

/**
 * Makes a currently inactive bullet active, and sets the bullet position to that of 
 * the camera/player.
**/
void shoot() {
	if (!game_over) {
		glm::vec3 position = camera.Position;
		glm::vec3 direction = camera.Front;
		PlaySound(TEXT("../Resources/Audio/shoot.wav"), NULL, SND_ASYNC);

		for (unsigned int i = 0; i < bullets.size(); i++) {
			if (!bullets[i].mTraveling) {
				bullets[i].mPosition = position;
				bullets[i].mDirection = direction;
				bullets[i].mTraveling = true;
				bullets[i].mCurrentDistance = 0.0f;
				return;
			}
		}
	}
}

/**
 * Iterates through the vector of active enemies, for each enemy identify if a collision occured
 * with any active bullet.
**/
void detectCollision() {
	for (unsigned int i = 0; i < enemies.size(); i++)
	{
		for (unsigned int j = 0; j < bullets.size(); j++)
		{
			if (bullets[j].mTraveling && checkBoundingSphere(enemies[i].mPosition, bullets[j].mPosition, enemies[i].mBoundRadius, bullets[j].mBoundRadius))
			{
				enemies[i].mAlive = false;
				enemies[i].mHit = false;
				bullets[i].mTraveling = false;
				score++;
			}
		}
	}
}

/**
 * Calculates whether a collision occured between two points, given the radius of their boudning spheres.
**/
bool checkBoundingSphere(glm::vec3 point1, glm::vec3 point2, float radius1, float radius2) {
	bool collision = false;
	float distance = glm::sqrt(((point1.x - point2.x) * (point1.x - point2.x)) 
								+ ((point1.y - point2.y) * (point1.y - point2.y)) 
								+ ((point1.z - point2.z) * (point1.z - point2.z)));

	if (distance <= radius1 + radius2)
	{
		collision = true;
		PlaySound(TEXT("../Resources/Audio/hit.wav"), NULL, SND_ASYNC);

	}
	return collision;
}