#include "BookInstance.h"
#include <iostream>


#include "GLSL.h"
#include <glm/glm.hpp>
#include "Objects.h"
using namespace std;
using namespace glm;



BookInstance::BookInstance(glm::vec3 minPoint, glm::vec3 maxPoint, unsigned int objID) {
	books[0] = new Objects(minPoint, maxPoint, 0);
	offset = 0;
} 

