#pragma once

namespace Engine
{
	class GraphicsEngine
	{
		bool Init(const char i_TITLE[], int i_SCREEN_WIDTH, int i_SCREEN_HEIGHT);
		void Update();
		void Render();
		void Loop();
	};
}