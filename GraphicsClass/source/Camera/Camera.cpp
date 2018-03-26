#include "Camera.h"

Lai::Camera::Camera() : direction(0, 0, -1)
{

}

glm::vec3 Lai::Camera::GetPosition()
{
	return position;
}

glm::vec2 Lai::Camera::GetRotation()
{
	return rotation;
}

glm::mat4 Lai::Camera::GetCameraMat()
{
	return cameraMat;
}

void Lai::Camera::SetPosition(glm::vec3 i_vec)
{
	position = i_vec;
	UpdateMat();
}

void Lai::Camera::SetRotation(glm::vec2 i_vec)
{
	rotation = i_vec;
	UpdateMat();
}


void Lai::Camera::UpdateMat()
{
	glm::mat4 ViewRotateX = glm::rotate(glm::mat4(1.0f), rotation.y, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 View = glm::rotate(ViewRotateX, rotation.x, glm::vec3(0.0f, 1.0f, 0.0f));

	direction = glm::vec3( View * glm::vec4(direction, 1.0));


	cameraMat = glm::lookAt(position, position + direction, glm::vec3(0, 1, 0));;
//	cameraMat = trans * rot;
}
