
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
#include "Objects.h"


class BookInstance
{

public:
	BookInstance();
	void setModelMat(glm::mat4 model);


private:

	Objects *objectArray;
	float offset;

	//not going to have weird transforms



};

#endif // LAB471_OBJECTS_H_INCLUDED
