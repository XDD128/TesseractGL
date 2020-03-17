
#pragma once

#ifndef LAB471_OBJECTS_H_INCLUDED
#define LAB471_OBJECTS_H_INCLUDED

#include <string>
#include <vector>
#include <memory>
#include "glm/glm.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"



class Objects
{

public:
	Objects(glm::vec3 minPoint, glm::vec3 maxPoint, unsigned int objID);
	//void createAABB(glm::vec3 minPoint, glm::vec3 maxPoint, unsigned int objID);
	void updateAABB();
	bool pointIntersect(glm::vec3 point);
	void transformAABB(glm::mat4 transformMat);
private:

	unsigned int objectID;
	//currenet min and max points
	glm::vec3 minP;
	glm::vec3 maxP;
	//not going to have weird transforms
	vec3 initMinP;
	vec3 initMaxP;


};

#endif // LAB471_OBJECTS_H_INCLUDED
