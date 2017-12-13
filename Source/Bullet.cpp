#include "Bullet.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#define TRAVEL_DISTANCE 100
#define BOUNDING_RADIUS 0.5f

Bullet::Bullet(glm::vec3 position, glm::vec3 direction, float speed, float scale) 
	: mPosition(position), mDirection(direction), mSpeed(speed), mCurrentDistance(0.0f), 
		mMaxDistance(TRAVEL_DISTANCE), mTraveling(true), mScale(scale), mBoundRadius(BOUNDING_RADIUS){}

void Bullet::draw(Model model1, int model_matrix_location, Shader shader) {
	glm::mat4 model_matrix;
	move();
	model_matrix = glm::translate(model_matrix, mPosition);
	model_matrix = glm::scale(model_matrix, glm::vec3(mScale, mScale, mScale));
	glUniformMatrix4fv(model_matrix_location, 1, GL_FALSE, glm::value_ptr(model_matrix));
	model1.Draw(shader);
}

void Bullet::move() {
	if (mCurrentDistance < mMaxDistance) {
		mCurrentDistance += mSpeed;
		mPosition += mDirection * mSpeed;
	}	
}
