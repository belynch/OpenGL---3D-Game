#include "NPC.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#define BOUNDING_RADIUS 2.5f

NPC::NPC(glm::vec3 position = glm::vec3(0.0f,0.0f,0.0f), float scale = 1.0f, float speed = 0.1f) 
	: mAlive(true), mPosition(position), mScale(scale), mSpeed(speed), mHit(false) {

	//ensure larger models have larger hit boxes
	mBoundRadius = scale * 10;
}

/**
 * 
 * Draws the model associated with the NPC after translating it towards the origin and scaling it.
 *
**/
void NPC::draw(Model model1, int model_matrix_location, Shader shader) {
	glm::mat4 model_matrix;
	calcMovement();
	model_matrix = glm::translate(model_matrix, this->mPosition);
	model_matrix = glm::scale(model_matrix, glm::vec3(this->mScale, this->mScale, this->mScale));
	glUniformMatrix4fv(model_matrix_location, 1, GL_FALSE, glm::value_ptr(model_matrix));
	model1.Draw(shader);
}

/**
 *
 * Updates the NPCs position vector by moving towards the origin.
 *
**/
void NPC::calcMovement() {
	// the player platform is a 9x9 square
	float origin_boundry = 9.0f;
	// south west
	if (this->mPosition.x < -origin_boundry && this->mPosition.z < -origin_boundry) {
		this->mPosition.x += this->mSpeed;
		this->mPosition.z += this->mSpeed;
	}	
	// north west
	else if (this->mPosition.x < -origin_boundry && this->mPosition.z > origin_boundry) {
		this->mPosition.x += this->mSpeed;
		this->mPosition.z -= this->mSpeed;
	}
	// south east
	else if (this->mPosition.x > origin_boundry && this->mPosition.z < -origin_boundry) {
		this->mPosition.x -= this->mSpeed;
		this->mPosition.z += this->mSpeed;
	}
	
	// north east
	else if (mPosition.x > origin_boundry && this->mPosition.z > origin_boundry) {
		this->mPosition.x -= this->mSpeed;
		this->mPosition.z -= this->mSpeed;
	}
	/////////////////////////////////////
	//
	else if (this->mPosition.x < -origin_boundry && this->mPosition.z > -origin_boundry) {
		this->mPosition.x += this->mSpeed;
	}
	// east
	else if (this->mPosition.x > origin_boundry && this->mPosition.z < origin_boundry) {
		this->mPosition.x -= this->mSpeed;
	}
	///////////////////////////////////////
	//
	else if (this->mPosition.x < origin_boundry && this->mPosition.z > origin_boundry) {
		this->mPosition.z -= this->mSpeed;
	}
	// south
	else if (this->mPosition.x > -origin_boundry && this->mPosition.z < -origin_boundry) {
		this->mPosition.z += this->mSpeed;
	}
	else {
		mAlive = false;
		gameOver = true;
	}
	
}

