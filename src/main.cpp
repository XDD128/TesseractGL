/*
 * Program 3 base code - includes modifications to shape and initGeom in preparation to load
 * multi shape objects
 * CPE 471 Cal Poly Z. Wood + S. Sueda + I. Dunn
 */

#include <iostream>
#include <glad/glad.h>
#include <unordered_map>

#include "GLSL.h"
#include "Program.h"
#include "Shape.h"
#include "MatrixStack.h"
#include "WindowManager.h"
#include "Texture.h"
#include "stb_image.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader/tiny_obj_loader.h>

 // value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;

	// Our shader program
	std::shared_ptr<Program> prog;
	std::shared_ptr<Program> matProg;
	std::shared_ptr<Program> cubeProg;
	shared_ptr<Texture> texture0;
	shared_ptr<Texture> texture1;
	shared_ptr<Texture> texture2;
	// Shape to be used (from  file) - modify to support multiple
	shared_ptr<Shape> mesh;

	unordered_map<int, shared_ptr<Shape>> bmesh;
	shared_ptr<Shape> cube;

	shared_ptr<Shape> hamster;
	// Contains vertex information for OpenGL
	GLuint VertexArrayID;

	// Data necessary to give our triangle to OpenGL
	GLuint VertexBufferID;

	//example data that might be useful when trying to compute bounds on multi-shape
	bool firstMouse = true;
	vec3 gMin;
	vec3 gMax;
	
	unsigned int textureID;
	int randI = 0;


	//animation data
	float sTheta = 0;



	float vTrans = 0;//mc
	float zTrans = 0;

	float lTransX = 2.0;
	float lTransY = 2.0;
	float lTransZ = 2.9;

	double posX, posY;

	int cx = 400;
		int cy = 300;
	float maxPitch = 80;
	float minPitch = -80;
	float pitch = 0;
	float yaw = -90;
	vec3 eye = vec3(0, 0, 0);
	vec3 la = vec3(0, 0, -1);
	vec3 up = vec3(0, 1, 0);
	float walkSpeed = 0.1;
	float lookSpeed = 0.1;
	int switcher = 3;

	bool w, a, s, d = false;

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}




		if (key == GLFW_KEY_R && action == GLFW_PRESS) {
			eye.y += 0.2;
		}
		if (key == GLFW_KEY_F && action == GLFW_PRESS) {
			eye.y -= 0.2;
		}
		if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
			lTransX -= 0.2;
		}
		if (key == GLFW_KEY_E && action == GLFW_PRESS) {
			lTransX += 0.2;
		}
		if (key == GLFW_KEY_I && action == GLFW_PRESS) {
			lTransZ -= 0.2;
		}
		if (key == GLFW_KEY_K && action == GLFW_PRESS) {
			lTransZ += 0.2;
		}

		if (key == GLFW_KEY_U && action == GLFW_PRESS) {
			lTransY -= 0.2;
		}
		if (key == GLFW_KEY_J && action == GLFW_PRESS) {
			lTransY += 0.2;
		}
		if (key == GLFW_KEY_M && action == GLFW_PRESS) {
			switcher = (switcher + 1) % 4;
		}
		//mc
		if (key == GLFW_KEY_W && action == GLFW_PRESS) {
			w = true;
		}
		if (key == GLFW_KEY_S && action == GLFW_PRESS) {
			s = true;
		}

		if (key == GLFW_KEY_A && action == GLFW_PRESS) {
			a = true;
		}
		if (key == GLFW_KEY_D && action == GLFW_PRESS) {
			d = true;
		}

		if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
			w = false;
		}
		if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
			s = false;
		}

		if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
			a = false;
		}
		if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
			d = false;
		}

		
		if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		if (key == GLFW_KEY_Z && action == GLFW_RELEASE) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}


	void cursorCallback(GLFWwindow * window, double in_deltaX, double in_deltaY)
	{
		

		glfwGetCursorPos(window, &in_deltaY, &in_deltaY);
		if (firstMouse)
		{
			cx = in_deltaY;
			cy = in_deltaY;
			firstMouse = false;
		}
		float dx = in_deltaX - cx;
		float dy = cy - in_deltaY;
		cx = in_deltaX;
		cy = in_deltaY;

		yaw += dx * lookSpeed;
		pitch += dy * lookSpeed;

		if (pitch > maxPitch)
			pitch = maxPitch;
		if (pitch < minPitch)
			pitch = minPitch;
	}

	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{	

		if (action == GLFW_PRESS)
		{
			glfwGetCursorPos(window, &posX, &posY);
			float dx = posX - cx;
			float dy = cy - posY;

			yaw += dx * lookSpeed;
			pitch += dy * lookSpeed;
			
			if (pitch > maxPitch)
				pitch = maxPitch;
			if (pitch < minPitch)
				pitch = minPitch;

			cout << "Pos X " << posX << " Pos Y " << posY << endl;
		}
	}

	void resizeCallback(GLFWwindow *window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	unsigned int createSky(string dir, vector<string> faces) {
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(false);
		for (GLuint i = 0; i < faces.size(); i++) {
			unsigned char *data =
				stbi_load((dir + faces[i]).c_str(), &width, &height, &nrChannels, 0);
			if (data) {
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			}
			else {
				cout << "failed to load: " << (dir + faces[i]).c_str() << endl;
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		cout << " creating cube map any errors : " << glGetError() << endl;
		return textureID;
	}



	void init(const std::string& resourceDirectory)
	{	
	
		GLSL::checkVersion();

		// Set background color.
		glClearColor(.3f, .3f, .9f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);

		matProg = make_shared<Program>();
		matProg->setVerbose(false);
		matProg->setShaderNames(resourceDirectory + "/mat_vert.glsl", resourceDirectory + "/mat_frag.glsl");
		matProg->init();
		matProg->addUniform("P");
		matProg->addUniform("V");
		matProg->addUniform("M");

		matProg->addUniform("MatAmb");
		matProg->addUniform("MatDif");
		matProg->addUniform("MatSpec");
		matProg->addUniform("shine");

		matProg->addUniform("lightPos");
		matProg->addUniform("viewPos");
		matProg->addAttribute("vertPos");
		matProg->addAttribute("vertNor");
		// Initialize the GLSL program.
		//used for textures
		prog = make_shared<Program>();
		prog->setVerbose(true);
		prog->setShaderNames(resourceDirectory + "/tex_vert.glsl", resourceDirectory + "/tex_frag0.glsl");
		prog->init();
		prog->addUniform("P");
		prog->addUniform("V");
		prog->addUniform("M");

		prog->addUniform("Texture0");
		prog->addUniform("MatDif");

		prog->addAttribute("vertPos");
		prog->addAttribute("vertNor");
		prog->addAttribute("vertTex");


		//used for materials


		vector<std::string> faces {
			 "right.jpg",
			 "left.jpg",
			 "top.jpg",
			 "bottom.jpg",
			 "front.jpg",
			 "back.jpg"
		}; 
		textureID = createSky(resourceDirectory + "/space/", faces);
		cubeProg = make_shared<Program>();
		cubeProg->setVerbose(false);
		cubeProg->setShaderNames(resourceDirectory + "/cube_vert.glsl", resourceDirectory + "/cube_frag.glsl");
		cubeProg->init();
		cubeProg->addUniform("P");
		cubeProg->addUniform("V");
		cubeProg->addUniform("M");
		cubeProg->addUniform("skybox");

		cubeProg->addAttribute("vertPos");

	}

	void initGeom(const std::string& resourceDirectory)
	{

		//EXAMPLE set up to read one shape from one obj file - convert to read several
		// Initialize mesh
		// Load geometry
		// Some obj files contain material information.We'll ignore them for this assignment.
		vector<tinyobj::shape_t> TOshapes;
		vector<tinyobj::shape_t> TOshapes2;
		vector<tinyobj::shape_t> TOshapes3;
		vector<tinyobj::shape_t> TOshapes4;

		vector<tinyobj::material_t> objMaterials;
		string errStr;
		int i;
		//load in the mesh and make the shape(s)
		bool rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr, (resourceDirectory + "/sphere.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		}
		else {
			mesh = make_shared<Shape>();
			mesh->createShape(TOshapes[0]);
			mesh->measure();
			mesh->init();
		}
		//read out information stored in the shape about its size - something like this...
		//then do something with that information.....


		cout << "gmin.x: " << gMin.x << "\n gmin.y: " << gMin.y;
		rc = tinyobj::LoadObj(TOshapes4, objMaterials, errStr, (resourceDirectory + "/book.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		}
		else {

			for (i = 0; i < TOshapes4.size(); i++) {
				bmesh[i] = make_shared<Shape>();
				bmesh[i]->createShape(TOshapes4[i]);
				bmesh[i]->measure();
				bmesh[i]->init();

				if ((bmesh[i]->min.x) < gMin.x) gMin.x = (bmesh[i]->min.x);
				if ((bmesh[i]->min.y) < gMin.y) gMin.y = (bmesh[i]->min.y);
				if ((bmesh[i]->min.z) < gMin.z) gMin.z = (bmesh[i]->min.z);

				if ((bmesh[i]->max.x) > gMax.x) gMax.x = (bmesh[i]->max.x);
				if ((bmesh[i]->max.y) > gMax.y) gMax.y = (bmesh[i]->max.y);
				if ((bmesh[i]->max.z) > gMax.z) gMax.z = (bmesh[i]->max.z);
			}

			cout << "gMin: " << gMin.x << ", " << gMin.y << ", " << gMin.z << "\n";
			cout << "gMin: " << gMax.x << ", " << gMax.y << ", " << gMax.z << "\n";
			//cout << "The amount of meshes is " << i;
		}

		gMin.x = mesh->min.x;
		gMin.y = mesh->min.y;

		rc = tinyobj::LoadObj(TOshapes2, objMaterials, errStr, (resourceDirectory + "/cube.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		}
		else {
			cube = make_shared<Shape>();
			cube->createShape(TOshapes2[0]);
			cube->measure();
			cube->init();
		}
		

		rc = tinyobj::LoadObj(TOshapes3, objMaterials, errStr, (resourceDirectory + "/space_humster.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		}
		else {
			hamster = make_shared<Shape>();
			hamster->createShape(TOshapes3[0]);
			hamster->measure();
			hamster->init();
		}

	}

	void setModel(std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack>M) {
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
	}

	void initTex(const std::string& resourceDirectory) {
		texture0 = make_shared<Texture>();
		texture0->setFilename(resourceDirectory + "/humster_BaseColor.png");
		texture0->init();
		texture0->setUnit(0);
		texture0->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
		texture1 = make_shared<Texture>();
		texture1->setFilename(resourceDirectory + "/world.jpg");
		texture1->init();
		texture1->setUnit(1);
		texture1->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
		texture2 = make_shared<Texture>();
		texture2->setFilename(resourceDirectory + "/grass.jpg");
		texture2->init();
		texture2->setUnit(2);
		texture2->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	}


	void SetMaterial(int i) {
		switch (i) {
		case 0: //shiny blue plastic
			glUniform3f(matProg->getUniform("MatAmb"), 0.02, 0.04, 0.2);
			glUniform3f(matProg->getUniform("MatDif"), 0.0, 0.16, 0.9);
			glUniform3f(matProg->getUniform("MatSpec"), 0.14, 0.2, 0.8);
			glUniform1f(matProg->getUniform("shine"), 120.0);
			break;
		case 1: // flat grey
			glUniform3f(matProg->getUniform("MatAmb"), 0.13, 0.13, 0.14);
			glUniform3f(matProg->getUniform("MatDif"), 0.3, 0.3, 0.4);
			glUniform3f(matProg->getUniform("MatSpec"), 0.3, 0.3, 0.4);
			glUniform1f(matProg->getUniform("shine"), 4.0);
			break;
		case 2: //brass
			glUniform3f(matProg->getUniform("MatAmb"), 0.3294, 0.2235, 0.02745);
			glUniform3f(matProg->getUniform("MatDif"), 0.7804, 0.5686, 0.11373);
			glUniform3f(matProg->getUniform("MatSpec"), 0.9922, 0.941176, 0.80784);
			glUniform1f(matProg->getUniform("shine"), 27.9);
			break;

		case 3: //leather
			glUniform3f(matProg->getUniform("MatAmb"), 0.2, 0.1, 0);
			glUniform3f(matProg->getUniform("MatDif"), 0.4, 0.2, 0);
			glUniform3f(matProg->getUniform("MatSpec"), 0.4, 0.2, 0);
			glUniform1f(matProg->getUniform("shine"), 4);
			break;
		}
	}

	//void drawMultMesh()
	void drawBook(shared_ptr<MatrixStack> Model, float tx, float ty, float tz) {
		Model->translate(vec3(tx, ty, tz));
		Model->rotate(-1.5708, vec3(0, 1, 0));
		Model->scale(vec3(0.5, 0.5, 0.5));
		int i = 0;
		setModel(matProg, Model);
		Model->translate(vec3(tx, ty, tz));
		SetMaterial(switcher);
		for (i; i < bmesh.size() - 1; i++) {
			bmesh[i]->draw(matProg);
		}

		SetMaterial((switcher + 3) % 4);
		bmesh[2]->draw(matProg);
	}

	//void drawWideBook(shared_ptr<MatrixStack> Model, float tx, float ty, float tz)

	void drawBookRow(shared_ptr<MatrixStack> Model, int numOfBooks, bool left, bool right) {
		float offset = 0.3 + 0.5*sin(glfwGetTime());
		Model->pushMatrix();
		Model->translate(vec3(0, 0, -5));
		Model->scale(vec3(1, 1, 10));
		drawBook(Model, 0, .7, offset / 10);
		Model->popMatrix();
		int i = 1;
		for (i; i < numOfBooks; i++) {

			Model->pushMatrix();
			offset = 0.3 + 0.5*sin(i*0.4 + glfwGetTime());
			Model->translate(vec3(0, 0, -5));
			Model->scale(vec3(1, 1, 10));
			drawBook(Model, i*0.3, .7, offset / 10);
			Model->popMatrix();
			Model->pushMatrix();
			Model->translate(vec3(0, 0, -5));
			Model->scale(vec3(1, 1, 10));
			drawBook(Model, i*(-0.3), .7, offset / 10);
			Model->popMatrix();

		}
		i -= 2;
		if (left) {
			Model->pushMatrix();
			Model->translate(vec3(i*(-0.3), 0.7, 0));
			Model->scale(vec3(10, 6.5, 1));
			Model->rotate(1.570796, vec3(0, 0, 1));
			drawBook(Model, 0, .7, offset);
			Model->popMatrix();
		}
		if (right) {
			Model->pushMatrix();
			Model->translate(vec3(3.1*i, 0.7, 0));
			Model->scale(vec3(10, 6, 1));
			Model->rotate(1.570796, vec3(0, 0, 1));
			drawBook(Model, 0, .7, offset);
			Model->popMatrix();
		}


	}

	void drawBS(shared_ptr<MatrixStack> Projection, shared_ptr<MatrixStack> View, shared_ptr<MatrixStack> Model, int levels, bool left, bool right) {

		for (int i = 0; i < levels; i++) {

			Model->pushMatrix();
			Model->translate(vec3(0, 1.5, 0));
			Model->pushMatrix();
			drawBookRow(Model, 7, left, right);
			Model->popMatrix();


			SetMaterial(3);
			Model->pushMatrix(); //top of bookshelf
			Model->translate(vec3(0, 1.5, 0));
			Model->scale(vec3(4, 0.1, 2));
			setModel(matProg, Model);
			cube->draw(matProg);

			Model->popMatrix();

			Model->pushMatrix(); //left of bookshelf
			Model->translate(vec3(-2, 0.75, 0));
			Model->scale(vec3(0.1, 1.5, 2));
			setModel(matProg, Model);
			cube->draw(matProg);

			Model->popMatrix();

			Model->pushMatrix(); //right of bookshelf
			Model->translate(vec3(2, 0.75, 0));
			Model->scale(vec3(0.1, 1.5, 2));
			setModel(matProg, Model);
			cube->draw(matProg);
			Model->popMatrix();

			Model->pushMatrix();//back of bookshelf
			Model->translate(vec3(0, 0.75, -.9));
			Model->scale(vec3(4, 1.5, 0.1));
			setModel(matProg, Model);
			cube->draw(matProg);

			Model->popMatrix();


		}

		//draw top stretched row
		Model->pushMatrix();
		Model->translate(vec3(0, 1, 0));
		Model->scale(vec3(1, 10, 1));
		drawBookRow(Model, 7, false, false);
		Model->popMatrix();

		//now we are "levels" deep into the stack, pop out to get back to where we were in the beginning
		for (int i = 0; i < levels; i++) {
			Model->popMatrix();
		}

		Model->pushMatrix();
		SetMaterial(3);
		Model->translate(vec3(0, 1.5, 0));
		Model->scale(vec3(4, 0.1, 2));

		setModel(matProg, Model);
		cube->draw(matProg);
		Model->popMatrix();
		Model->pushMatrix();

		Model->scale(vec3(1, 10, 1));
		Model->translate(vec3(0, -1.2, 0));
		drawBookRow(Model, 7, false, false);
		Model->popMatrix();


	}

	void drawCross(shared_ptr<MatrixStack> Projection, shared_ptr<MatrixStack> View, shared_ptr<MatrixStack> Model) {
		matProg->bind();
		glUniform3f(matProg->getUniform("lightPos"), lTransX, lTransY, lTransZ);
		glUniform3f(matProg->getUniform("viewPos"), eye.x, eye.y, eye.z);
		glUniformMatrix4fv(matProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		glUniformMatrix4fv(matProg->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));
		Model->pushMatrix();
		int i = 0;
		Model->translate(vec3(0,0,-2));
		Model->scale(vec3(0.5, 0.5, 0.5));
		drawBS(Projection, View, Model, 7, true, true);
		Model->popMatrix();
		/*
		for (i = 1; i < 4; i++) {
			Model->pushMatrix();
			Model->translate(vec3(-4 + 2 * i, -4, -2));
			Model->scale(vec3(0.5, 0.5, 0.5));
			drawBS(Projection, View, Model, 7, false, false);
			Model->popMatrix();
		}
		//draw the torso with these transforms


		Model->pushMatrix();
		Model->translate(vec3(-4 + 2 * i, -4, -2));
		Model->scale(vec3(0.5, 0.5, 0.5));
		drawBS(Projection, View, Model, 7, false, true);
		Model->popMatrix();
		*/
		matProg->unbind();

	}

	void render() {
		// Get current frame buffer size.
		int i = 0;
		int width, height;

		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		glViewport(0, 0, width, height);

		//cx = width / 2;
		//cy = height / 2;



		
		la.x = cos(radians(pitch)) * cos(radians(yaw));
		la.y = sin(radians(pitch));
		la.z = cos(radians(pitch)) * sin(radians(yaw));

		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Use the matrix stack for Lab 6
		float aspect = width / (float)height;

		// Create the matrix stacks - please leave these alone for now
		auto Projection = make_shared<MatrixStack>();
		auto View = make_shared<MatrixStack>();
		auto Model = make_shared<MatrixStack>();
		// Apply perspective projection.
		Projection->pushMatrix();
		Projection->perspective(45.0f, aspect, 0.01f, 100.0f);


		//camera movement code
		if (w) eye += walkSpeed * la;
		if (s) eye -= walkSpeed * la;
		if (a) eye -= normalize(cross(la, up))*walkSpeed;
		if (d) eye += normalize(cross(la, up))*walkSpeed;
		// View is global translation along negative z for now
		View->pushMatrix();
		View->loadIdentity();

		View->lookAt(eye, eye + la, up);
		/*


		View->rotate(rTrans, vec3(0, 1, 0));
		View->translate(vec3(0, 0, -5));
		*/
		//to draw the sky box bind the right shader
		cubeProg->bind();
		//set the projection matrix - can use the same one
		glUniformMatrix4fv(cubeProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		//set the depth function to always draw the box!
		glDepthFunc(GL_LEQUAL);
		//set up view matrix to include your view transforms 
		//(your code likely will be different depending
		glUniformMatrix4fv(cubeProg->getUniform("V"), 1,
			GL_FALSE, value_ptr(View->topMatrix()));
		//set and send model transforms - likely want a bigger cube

		Model->pushMatrix();
		Model->scale(vec3(100, 100, 100));
		setModel(cubeProg, Model);
		//glUniformMatrix4fv(cubeProg->getUniform("M"), 1, GL_FALSE, value_ptr(ident->topMatrix()));
		//bind the cube map texture
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
		//draw the actual cube
		cube->draw(cubeProg);
		Model->popMatrix();
		//set the depth test back to normal!
		glDepthFunc(GL_LESS);
		//unbind the shader for the skybox
		cubeProg->unbind();

		// Draw a stack of cubes with indiviudal transforms
		prog->bind();
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		texture0->bind(prog->getUniform("Texture0")); 





		Model->pushMatrix();
			Model->loadIdentity();
			//draw hamster
			Model->pushMatrix();
			Model->translate(vec3(-2, -1, 2));
			Model->rotate(sin(sTheta), vec3(0, 1, 0));
			Model->rotate(.707, vec3(1, 0, 0));
			Model->scale(vec3(0.25, 0.25, 0.25));
			setModel(prog, Model);
			hamster->draw(prog);
			Model->popMatrix();
			prog->unbind();


			//bookshelves have lighting and material
			//draw front bookshelf
			Model->pushMatrix();
			Model->translate(vec3(0, 0, -5.5));
			drawCross(Projection, View, Model);
			Model->popMatrix();
			//draw back bookshelf
			Model->pushMatrix();
			Model->translate(vec3(0, 0, 5.5));
			Model->rotate(radians(180.0f), vec3(0, 1, 0));
			drawCross(Projection, View, Model);
			Model->popMatrix();
			//draw left bookshelf
			Model->pushMatrix();
			Model->translate(vec3(-5.5,0, 0));
			Model->rotate(radians(90.0f), vec3(0, 1, 0));
			drawCross(Projection, View, Model);
			Model->popMatrix();
			//draw right bookshelf
			Model->pushMatrix();
			Model->translate(vec3(5.5, 0, 0));
			Model->rotate(radians(-90.0f), vec3(0, 1, 0));
			drawCross(Projection, View, Model);
			Model->popMatrix();

	
		Model->popMatrix();


		//animation update example
		sTheta = sin(glfwGetTime());

		// Pop matrix stacks.
		Projection->popMatrix();
		View->popMatrix();

	}
};

int main(int argc, char *argv[])
{
	// Where the resources are loaded from
	std::string resourceDir = "../resources";

	if (argc >= 2)
	{
		resourceDir = argv[1];
	}

	Application *application = new Application();

	// Your main will always include a similar set up to establish your window
	// and GL context, etc.

	WindowManager *windowManager = new WindowManager();
	windowManager->init(640, 480);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	// This is the code that will likely change program to program as you
	// may need to initialize or set up different data and state

	application->init(resourceDir);
	application->initGeom(resourceDir);
	application->initTex(resourceDir);

	// Loop until the user closes the window.
	while (!glfwWindowShouldClose(windowManager->getHandle()))
	{
		// Render scene.
		application->render();

		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}
