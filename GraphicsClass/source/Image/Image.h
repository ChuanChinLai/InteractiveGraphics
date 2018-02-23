#pragma once

#include <vector>


namespace Lai
{
	class Image
	{
	public:

		Image(std::string name);

		bool Load(std::string name);

		unsigned w, h;
		std::vector<unsigned char> image;
	};
}