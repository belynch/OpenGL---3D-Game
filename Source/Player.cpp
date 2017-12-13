#pragma once
#include "Player.hpp"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Player::Player(glm::vec3 front) : mFront(front), mUp(glm::vec3(0.0f, 1.0f, 0.0f)), mDegrees(0.0f), mRotate(false) {}

void Player::draw(Model base, Model body, int matrix_location, Shader shader, glm::vec3 direction){
	// Player
	glm::mat4 player_base;
	player_base = glm::translate(player_base, glm::vec3(0.0f, 1.5f, 0.0f));
	player_base = glm::scale(player_base, glm::vec3(2.0f, 2.0f, 2.0f));
	glm::mat4 root = player_base;
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, glm::value_ptr(root));
	base.Draw(shader);

	glm::mat4 player_body;
	if (this->mRotate) {
		player_body = rotate(direction);
	}
	else
	{
		player_body = glm::rotate(player_body, this->mDegrees, mUp);
	}
	glm::mat4 child = root * player_body;
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, glm::value_ptr(child));
	body.Draw(shader);
}

glm::mat4 Player::rotate(glm::vec3 direction) {

	direction = glm::normalize(direction);
	this->mFront = glm::normalize(mFront);

	//rotating in the x-z plane
	direction.y = 0.0f;
	this->mFront.y = 0.0f;

	glm::vec3 cross = glm::cross(direction, this->mFront);
	glm::vec3 vRef = glm::vec3(0.0f, 0.0f, 1.0f);
	float dir = glm::dot(cross, vRef);

	bool is_negative = dir < 0.0f;

	float dot = glm::dot(direction, this->mFront);
	float angle = acos(dot);

	if (is_negative)
		angle = -angle;
	
	this->mDegrees = angle;

	glm::mat4 rotationMat;
	return (glm::rotate(rotationMat, this->mDegrees, this->mUp));
}