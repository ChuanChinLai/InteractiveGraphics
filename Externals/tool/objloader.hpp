#pragma once
#include <cyCode\cyPoint.h>


bool loadOBJ
(
	const char * path, 
	std::vector<cy::Point3f> & out_vertices, 
	std::vector<cy::Point2f> & out_uvs,
	std::vector<cy::Point3f> & out_normals
);



bool loadAssImp(
	const char * path, 
	std::vector<unsigned short> & indices,
	std::vector<cy::Point3f> & vertices,
	std::vector<cy::Point2f> & uvs,
	std::vector<cy::Point3f> & normals
);
