
#pragma once

#ifndef LAB471_SHAPE_H_INCLUDED
#define LAB471_SHAPE_H_INCLUDED

#include <string>
#include <vector>
#include <memory>


class Program;


class Objects
{

public:

	void createAABB();
	void updateAABB();
	bool vecIntersect()

private:

	unsigned int objectID;
	glm::vec3 minP;
	glm::vec3 maxP;

};

#endif // LAB471_SHAPE_H_INCLUDED
