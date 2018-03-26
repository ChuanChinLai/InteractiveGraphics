#pragma once

#include <glm\glm\glm.hpp>
#include <glm\glm\gtx\euler_angles.hpp>
#include <glm\glm\gtc\matrix_transform.hpp>

namespace Lai
{
	class Camera
	{
	public:

		Camera();

		glm::vec3 GetPosition();
		glm::vec2 GetRotation();
		glm::mat4 GetCameraMat();

		void SetPosition(glm::vec3 i_vec);
		void SetRotation(glm::vec2 i_vec);

	private:

		void UpdateMat();

		glm::vec3 position;
		glm::vec3 direction;
		glm::vec2 rotation;

		glm::mat4 cameraMat;
	};
}
