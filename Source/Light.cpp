#include "Light.hpp"
#include <iostream>
#include <string>

Light::Light(glm::vec3 position, glm::vec3 direction, glm::vec3 colour, Light_type type)
	: mPosition(position), mDirection(direction), mColour(colour), mType(type) {
	std::cout << "Light of type " << mType << "created." << std::endl;
}

void Light::Use(Shader shader, int lightIndex) {
	if (mType == DIRECT_LIGHT) {
		glUniform3f(glGetUniformLocation(shader.Program, "dirLight.direction"), mDirection.x, mDirection.y, mDirection.z);
		glUniform3f(glGetUniformLocation(shader.Program, "dirLight.ambient"), mColour.x, mColour.y, mColour.z);
		glUniform3f(glGetUniformLocation(shader.Program, "dirLight.diffuse"), mColour.x, mColour.y, mColour.z);
		glUniform3f(glGetUniformLocation(shader.Program, "dirLight.specular"), mColour.x, mColour.y, mColour.z);
		
	}
	else if (mType == POINT_LIGHT) {
		glUniform3f(glGetUniformLocation(shader.Program, ("pointLights[" + std::to_string(lightIndex) + "].position").c_str()), mPosition.x, mPosition.y, mPosition.z);
		glUniform3f(glGetUniformLocation(shader.Program, ("pointLights[" + std::to_string(lightIndex) + "].ambient").c_str()), mColour.x * 0.1f, mColour.y * 0.1f, mColour.z * 0.1f);
		glUniform3f(glGetUniformLocation(shader.Program, ("pointLights[" + std::to_string(lightIndex) + "].diffuse").c_str()), mColour.x, mColour.y, mColour.z);
		glUniform3f(glGetUniformLocation(shader.Program, ("pointLights[" + std::to_string(lightIndex) + "].specular").c_str()), mColour.x, mColour.y, mColour.z);
		glUniform1f(glGetUniformLocation(shader.Program, ("pointLights[" + std::to_string(lightIndex) + "].constant").c_str()), 1.0f);
		glUniform1f(glGetUniformLocation(shader.Program, ("pointLights[" + std::to_string(lightIndex) + "].linear").c_str()), 0.009f);
		glUniform1f(glGetUniformLocation(shader.Program, ("pointLights[" + std::to_string(lightIndex) + "].quadratic").c_str()), 0.0032f);
	}
	else {
		glUniform3f(glGetUniformLocation(shader.Program, ("spotLights[" + std::to_string(lightIndex) + "].position").c_str()), mPosition.x, mPosition.y, mPosition.z);
		glUniform3f(glGetUniformLocation(shader.Program, ("spotLights[" + std::to_string(lightIndex) + "].direction").c_str()), mDirection.x, mDirection.y, mDirection.z);
		glUniform1f(glGetUniformLocation(shader.Program, ("spotLights[" + std::to_string(lightIndex) + "].cutOff").c_str()), glm::cos(glm::radians(12.5f)));
		glUniform1f(glGetUniformLocation(shader.Program, ("spotLights[" + std::to_string(lightIndex) + "].outerCutOff").c_str()), glm::cos(glm::radians(17.5f)));
		glUniform3f(glGetUniformLocation(shader.Program, ("spotLights[" + std::to_string(lightIndex) + "].ambient").c_str()), mColour.x * 0.1f, mColour.y * 0.1f, mColour.z * 0.1f);
		glUniform3f(glGetUniformLocation(shader.Program, ("spotLights[" + std::to_string(lightIndex) + "].diffuse").c_str()), mColour.x, mColour.y, mColour.z);
		glUniform3f(glGetUniformLocation(shader.Program, ("spotLights[" + std::to_string(lightIndex) + "].specular").c_str()), mColour.x, mColour.y, mColour.z);
		glUniform1f(glGetUniformLocation(shader.Program, ("spotLights[" + std::to_string(lightIndex) + "].constant").c_str()), 1.0f);
		glUniform1f(glGetUniformLocation(shader.Program, ("spotLights[" + std::to_string(lightIndex) + "].linear").c_str()), 0.009f);
		glUniform1f(glGetUniformLocation(shader.Program, ("spotLights[" + std::to_string(lightIndex) + "].quadratic").c_str()), 0.0032f);
	}

}