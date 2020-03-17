#include "Objects.h"
#include <iostream>


#include "GLSL.h"
#include <glm/glm.hpp>

using namespace std;
using namespace glm;



Objects::Objects(glm::vec3 minPoint, glm::vec3 maxPoint, unsigned int objID) {
	minP = minPoint;
	maxP = maxPoint;
	objectID = objectID;
} 

void Objects::transformAABB(glm::mat4 transformMat) {
	glm::vec3 minPoint = vec3(transformMat*glm::vec4(minP, 1.0));
	glm::vec3 maxPoint = vec3(transformMat*glm::vec4(maxP, 1.0));
	minP = minPoint;
	maxP = maxPoint;
}
void Objects::updateAABB() {
	glm::vec3 minPoint = vec3(min(minP.x, maxP.x), min(minP.y, maxP.y), min(minP.z, maxP.z));
	glm::vec3 maxPoint = vec3(max(minP.x, maxP.x), max(minP.y, maxP.y), max(minP.z, maxP.z));
	minP = minPoint;
	maxP = maxPoint;
}
bool Objects::pointIntersect(glm::vec3 point) {
	return (minP.x <= point.x && point.x <= maxP.x
		&& minP.y <= point.y && point.y <= maxP.y
		&& minP.z <= point.z && point.z <= maxP.z);
}