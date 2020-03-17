
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



class BookInstance
{

public:
	BookInstance(glm::vec3 minPoint, glm::vec3 maxPoint, unsigned int objID);


private:

	Objects **books;
	float offset;

	//not going to have weird transforms



};

#endif // LAB471_OBJECTS_H_INCLUDED
