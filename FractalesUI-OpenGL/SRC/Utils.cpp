#include <iostream>
#include <fstream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "ExternalLibs/stb_image.h"

#include "Utils.hpp"

const std::string SHADERS_PATH = "Shaders/";
const std::string ASSETS_PATH = "Assets/";

GLFWwindow* initOpenGL(std::string titre, int width, int height, bool isFullScreen){
	
	// Initialisation GLFW
	int glfw_init_res = glfwInit();
	if (!glfw_init_res){
		std::cerr << "Could not initialize GLFW window. Ending program." << std::endl;
		return nullptr;
	}

	// Min OpenGL versions
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Profil de compatibilité: on force un profil "core" (on ne pourra pas appeler les fonctions legacy)
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// MacOS: compatabilité des versions "en avant"
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* window = NULL;
	const char* f_title = (char*) titre.c_str();
	if (isFullScreen){
		GLFWmonitor* ecran = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(ecran);
		if (mode!=NULL) window = glfwCreateWindow(mode->width, mode->height, f_title, ecran, NULL);
	} else {
		window = glfwCreateWindow(width, height, f_title, NULL, NULL);
	}

	if (window == NULL) {
		std::cerr << "Could not open GLFW window. Ending program." << std::endl;
		glfwTerminate();
		return nullptr;
	}

	// Initialisation OpenGL

	// Sélectionner "window" comme la fenêtre principale
	glfwMakeContextCurrent(window);

	// Initialisation de glew (interroge la CG, intialise les bons pointeurs de fonctions...)
	GLenum res = glewInit();
	if (res != GLEW_OK){
		std::cerr << "Could not initalize Glew. Ending program." << std::endl;
		glfwTerminate();
		return nullptr;
	}
	
	// Nécéssaire si on utilise l'OpenGL moderne
	glewExperimental = GL_TRUE;
	return window;
}

GLFWwindow* initOpenGL3D(std::string titre, int width, int height, bool isFullScreen){
	
	// Initialisation GLFW
	int glfw_init_res = glfwInit();
	if (!glfw_init_res){
		std::cerr << "Could not initialize GLFW window. Ending program." << std::endl;
		return nullptr;
	}

	// Min OpenGL versions
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Profil de compatibilité: on force un profil "core" (on ne pourra pas appeler les fonctions legacy)
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // MacOS: compatabilité des versions "en avant"

	GLFWwindow* window = NULL;
	const char* f_title = (char*) titre.c_str();
	if (isFullScreen){
		GLFWmonitor* ecran = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(ecran);
		if (mode!=NULL) window = glfwCreateWindow(mode->width, mode->height, f_title, ecran, NULL);
	} else {
		window = glfwCreateWindow(width, height, f_title, NULL, NULL);
	}

	if (window == NULL) {
		std::cerr << "Could not open GLFW window. Ending program." << std::endl;
		glfwTerminate();
		return nullptr;
	}

	// Initialisation OpenGL

	// Sélectionner "window" comme la fenêtre principale
	glfwMakeContextCurrent(window);

	// Initialisation de glew (interroge la CG, intialise les bons pointeurs de fonctions...)
	GLenum res = glewInit();
	if (res != GLEW_OK){
		std::cerr << "Could not initalize Glew. Ending program." << std::endl;
		glfwTerminate();
		return nullptr;
	}
	
	// Nécéssaire si on utilise l'OpenGL moderne
	glewExperimental = GL_TRUE;

	// glViewport(0, 0, width, height); // Invalid for macOS: buffer size is different from window size
	glEnable(GL_DEPTH_TEST);
	int w, h;
	glfwGetFramebufferSize(window, &w, &h);
	glViewport(0, 0, w, h);

	return window;
}

void showFPS(GLFWwindow* fen, const std::string primary_title){
	static double previousSeconds = 0.0;
	static int frameCount = 0;
	double elapsedSeconds;
	double currentSeconds = glfwGetTime(); // nombre de secondes depuis le début de l'exécution

	elapsedSeconds = currentSeconds - previousSeconds;

	// update FPS text ~1x par seconde
	if (elapsedSeconds>0.97){
		previousSeconds = currentSeconds;
		double fps = (double) frameCount / elapsedSeconds;
		double msPerFrame = 1000.0 / fps;

		std::ostringstream outs;
		outs.precision(3);
		outs << std::fixed << primary_title << " - FPS: " << fps << " (" << msPerFrame << " ms)";
		glfwSetWindowTitle(fen, outs.str().c_str());

		frameCount = 0;
	}

	frameCount++;
}

static std::vector<std::string> split(std::string s, std::string t){
	using std::string, std::vector;

	vector<string> res;
	while(true){
		int pos = s.find(t);
		if (pos == -1) {
			res.push_back(s);
			break;
		}
		res.push_back(s.substr(0, pos));
		s = s.substr(pos+1);
	}

	return res;
}

#pragma region ShaderProgram

ShaderProgram::ShaderProgram(){
	this->mHandle = 0;
}

ShaderProgram::~ShaderProgram(){
	if (mHandle>0) glDeleteProgram(mHandle);
}

bool ShaderProgram::loadShaders(const std::string vsFilename, const std::string fsFilename){
	// std::string vsString = fileToString(vsFilename);
	// std::string fsString = fileToString(fsFilename);
	std::string vsString = fileToString(vsFilename);
	std::string fsString = fileToString(fsFilename);
	const GLchar* vsSourcePtr = vsString.c_str();
	const GLchar* fsSourcePtr = fsString.c_str();

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vs, 1, &vsSourcePtr, NULL);
	glShaderSource(fs, 1, &fsSourcePtr, NULL);

	glCompileShader(vs);
	checkCompileErrors(vs, VERTEX);
	glCompileShader(fs);
	checkCompileErrors(fs, FRAGMENT);

	this->mHandle = glCreateProgram();
	glAttachShader(mHandle, vs);
	glAttachShader(mHandle, fs);

	glLinkProgram(mHandle);
	checkCompileErrors(mHandle, PROGRAM);

	// On libère les shaders, ils ont été copiés dans le "program"
	glDeleteShader(vs);
	glDeleteShader(fs);
	
	return true;
}

void ShaderProgram::use(){
	if (!(mHandle>0)) return;

	glUseProgram(mHandle);
}

void ShaderProgram::setUniform(const GLchar* name, const glm::vec2 &v){
	GLint loc = getUniformLocation((GLchar*) name);
	glUniform2f(loc, v.x, v.y);
}

void ShaderProgram::setUniform(const GLchar* name, const glm::vec3 &v){
	GLint loc = getUniformLocation((GLchar*) name);
	glUniform3f(loc, v.x, v.y, v.z);
}

void ShaderProgram::setUniform(const GLchar* name, const glm::vec4 &v){
	GLint loc = getUniformLocation((GLchar*) name);
	glUniform4f(loc, v.x, v.y, v.z, v.w);
}

void ShaderProgram::setUniform(const GLchar* name, const glm::mat4 &m){
	GLint loc = getUniformLocation((GLchar*) name);
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(m));
}

void ShaderProgram::setUniform(const GLchar* name, const GLfloat f){
	GLint loc = getUniformLocation((GLchar*) name);
	glUniform1f(loc, f);
}

void ShaderProgram::setUniform(const GLchar* name, const GLint b){
	GLint loc = getUniformLocation((GLchar*) name);
	glUniform1i(loc, b);
}

void ShaderProgram::setUniformSampler(const GLchar* name, const GLint &slot){
	GLint loc = getUniformLocation((GLchar*) name);
	glActiveTexture(GL_TEXTURE0 + slot);
	glUniform1i(loc, slot);
}

GLuint ShaderProgram::getProgramObjectId() const {
	return mHandle;
}

std::string ShaderProgram::fileToString(const std::string& filename){
	std::stringstream ss;
	std::ifstream file;

	try {
		file.open(filename, std::ios::in);
		if(file.fail()){ std::cerr << "Error reading file. Cannot read file " << filename << std::endl; return ""; }

		ss << file.rdbuf();
		file.close();

	} catch(std::exception ex){
		std::cerr << "Error reading file. Mayble filename is wrong, or path is incorrect.\n" << ex.what() << std::endl;
		return "";
	}

	return ss.str();
}

void ShaderProgram::checkCompileErrors(GLuint shader, ShaderType type){
	int status = 0;

	if (type == PROGRAM){
		glGetProgramiv(mHandle, GL_LINK_STATUS, &status);
		
		// Handle link error
		if (status == GL_FALSE){
			GLint length = 0;
			glGetProgramiv(mHandle, GL_INFO_LOG_LENGTH, &length);
			std::string errorLog(length, ' ');
			glGetProgramInfoLog(mHandle, length, &length, &errorLog[0]);
			std::cerr << "Error, program failed to link. Log:" << "\n" << errorLog << std::endl;
		}
	}

	if (type == VERTEX || type == FRAGMENT){
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

		// Handle compile error
		if (status == GL_FALSE){
			GLint length = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
			std::string errorLog(length, ' ');
			glGetShaderInfoLog(shader, length, &length, &errorLog[0]);
			std::cerr << "Error, shader failed to compile. Log:" << "\n" << errorLog << std::endl;
		}
	}
}

GLint ShaderProgram::getUniformLocation(GLchar* name){
	std::map<std::string, GLint>::iterator it = mUniformLocations.find(name);

	// Si il n'est pas déjà dans la map, on le recherche et on l'y stocke
	if (it == mUniformLocations.end()){
		mUniformLocations[name] = glGetUniformLocation(mHandle, name);
	}
	return mUniformLocations[name];
}

#pragma endregion

#pragma region Texture2D

Texture2D::Texture2D(){
	this->mTexture = 0;
}

Texture2D::~Texture2D(){

}

bool Texture2D::loadTexture(const std::string &filename, bool generateMipMaps){
	int width, height, components;

	unsigned char* imageData = stbi_load(filename.c_str(), &width, &height, &components, STBI_rgb_alpha);
	if (imageData==NULL) return false;

	// Renverser l'image (pour qu'elle soit à l'endroit)
	int widthInBytes = width*4;
	int halfHeight = height / 2;
	unsigned char* top = NULL;
	unsigned char* bottom = NULL;
	unsigned char temp = 0;
	for(int row = 0 ; row<halfHeight ; row++){
		top = imageData + row * widthInBytes;
		bottom = imageData + (height - row - 1) * widthInBytes;
		for(int col=0 ; col<widthInBytes ; col++){
			temp = *top;
			*top = *bottom;
			*bottom = temp;
			top++;
			bottom++;
		}
	}

	glGenTextures(1, &mTexture);
	glBindTexture(GL_TEXTURE_2D, mTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Axe X
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // Axe Y
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Filtering lorsque la zone d'affichage est plus petite que l'image (dézoom)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_LINEAR); // Filtering... zoom

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
	if (generateMipMaps) glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(imageData);
	glBindTexture(GL_TEXTURE_2D, 0); // unbind
	return true;
}

void Texture2D::bind(GLuint texUnit){
	assert(texUnit>=0);
	glActiveTexture(GL_TEXTURE0 + texUnit); // Max 272 sur la 2080 Ti, ? sur le M1 Pro
	glBindTexture(GL_TEXTURE_2D, mTexture);	
}

void Texture2D::unbind(GLuint texUnit){
	assert(texUnit>=0);
	glActiveTexture(GL_TEXTURE0 + texUnit); // Max 272 sur la 2080 Ti, ? sur le M1 Pro
	glBindTexture(GL_TEXTURE_2D, 0);
}

#pragma endregion

#pragma region Camera

// Clase abstraite

float Camera::MOUSE_SENSITIVITY = 0.25f;
double Camera::ZOOM_SENSITIVITY = 0.1;
float Camera::MOVE_SPEED = 5.0f;

Camera::Camera(glm::vec3 pos) : WORLD_UP(0.0f, 1.0f, 0.0f){
	this->mPosition = pos;
	this->mTargetPos = glm::vec3(0.0f, 0.0f, 0.0f);
	this->mUp = glm::vec3(0.0f, 1.0f, 0.0f);
	this->mYaw = glm::pi<float>();
	this->mPitch = 0.0f;
	this->mLook = glm::vec3(0.0f, 0.0f, 0.0f);
	this->mRight = glm::vec3(0.0f, 0.0f, 0.0f);
	this->mFOV = 45.0f; // en degrés
}

Camera::~Camera(){}

glm::mat4 Camera::getViewMatrix() const {
	return glm::lookAt(mPosition, mTargetPos, mUp);
}

const glm::vec3& Camera::getPosition() const {
	return this->mPosition;
}

const glm::vec3& Camera::getUp() const {
	return this->mUp;
}

const glm::vec3& Camera::getLook() const {
	return this->mLook;
}

const glm::vec3& Camera::getRight() const {
	return this->mRight;
}

float Camera::getFOV() const {
	return this->mFOV;
}

void Camera::setFOV(float fov){
	this->mFOV = fov;
}

void Camera::setMouseSensitivity(float sens){
	Camera::MOUSE_SENSITIVITY = sens;
}

void Camera::setZoomSensitivity(double sens){
	Camera::ZOOM_SENSITIVITY = sens;
}

void Camera::setMoveSpeed(float speed){
	Camera::MOVE_SPEED = speed;
}

// Orbit

OrbitCamera::OrbitCamera(glm::vec3 pos) : Camera(pos) {
	this->mRadius = 10.0f;
	setLookAt(glm::vec3(0.0f, 0.0f, 0.0f));
}

void OrbitCamera::setLookAt(const glm::vec3 &target){
	mTargetPos = target;
}

void OrbitCamera::setRadius(float radius){
	this->mRadius = glm::clamp(radius, 2.0f, 80.0f);
}

void OrbitCamera::rotate(float yaw, float pitch){
	mYaw = glm::radians(yaw);
	mPitch = glm::radians(pitch);
	mPitch = glm::clamp(mPitch, -glm::pi<float>() / 2.0f + 0.1f, glm::pi<float>() / 2.0f - 0.1f);

	updateCameraVectors();
}

void OrbitCamera::move(const glm::vec3 &offsetPosition){
	return; // Nothing to do for this type of camera
}

void OrbitCamera::update(GLFWwindow* fen, double elapsedTime){
	// Nothing to do here
}

void OrbitCamera::onMouseScroll(GLFWwindow* fen, double deltaX, double deltaY){
	// Nothing to do here
}

void OrbitCamera::onMouseMove(GLFWwindow* fen, double posX, double posY){
	static glm::vec2 lastMousePos = glm::vec2(0, 0);
	
	float gYaw = glm::degrees(mYaw);
	float gPitch = glm::degrees(mPitch);
	float gRadius = mRadius;

	// Update angles based on left mouse button input to orbit around the cube
	if (glfwGetMouseButton(fen, GLFW_MOUSE_BUTTON_LEFT) == 1){
		// Each pixel represents a quarter of a degree rotation (this is our mouse sensitivity)
		gYaw -= ((float)posX - lastMousePos.x) * MOUSE_SENSITIVITY;
		gPitch += ((float)posY - lastMousePos.y) * MOUSE_SENSITIVITY;
 	}

	// Change orbit camera radius with the right click
	if (glfwGetMouseButton(fen, GLFW_MOUSE_BUTTON_RIGHT) == 1){
		float dx = 0.01f * ((float)posX - lastMousePos.x);
		float dy = 0.01f * ((float)posY - lastMousePos.y);
		gRadius += dx - dy;
	}

	// Apply changes
	rotate(gYaw, gPitch);
	setRadius(gRadius);

	lastMousePos.x = float(posX);
	lastMousePos.y = float(posY);
}

void OrbitCamera::updateCameraVectors(){
	// Transformation de coordonnées sphériques vers cartésiennes
	mPosition.x = mTargetPos.x + mRadius*cosf(mPitch)*sinf(mYaw); 	// (x =) rho * sin(theta) * cos(phi)
	mPosition.y = mTargetPos.y + mRadius*sinf(mPitch); 				// (z =) rho * sin(phi)
	mPosition.z = mTargetPos.z + mRadius*cosf(mPitch)*cosf(mYaw); 	// (y =) roh * cos(theta) * cos(phi)
}

// First Person

FirstPersonCamera::FirstPersonCamera(glm::vec3 position, float yaw, float pitch){
	this->mPosition = position;
	this->mYaw = yaw;
	this->mPitch = pitch;
}

void FirstPersonCamera::setPosition(const glm::vec3 &position){
	this->mPosition = position;
}

void FirstPersonCamera::rotate(float yaw, float pitch){
	mYaw += glm::radians(yaw);
	mPitch += glm::radians(pitch);

	mPitch = glm::clamp(mPitch, -glm::pi<float>()/2.0f + 0.1f, glm::pi<float>()/2.0f - 0.1f);
	updateCameraVectors();
}

void FirstPersonCamera::move(const glm::vec3 &offsetPosition){
	this->mPosition += offsetPosition;
	updateCameraVectors();
}

void FirstPersonCamera::update(GLFWwindow* fen, double elapsedTime){
	double mouseX, mouseY;
	glfwGetCursorPos(fen, &mouseX, &mouseY);

	int width, height;
	glfwGetWindowSize(fen, &width, &height);

	rotate((float)(width/2.0 - mouseX)*MOUSE_SENSITIVITY, (float)(height/2.0 - mouseY)*MOUSE_SENSITIVITY);
	glfwSetCursorPos(fen, width/2.0, height/2.0);

	// Camera FPS movement
	if (glfwGetKey(fen, GLFW_KEY_O) == GLFW_PRESS) move(MOVE_SPEED * (float)elapsedTime * mLook); 			// O: foward
	if (glfwGetKey(fen, GLFW_KEY_L) == GLFW_PRESS) move(MOVE_SPEED * (float)elapsedTime * -mLook); 			// L: backward
	if (glfwGetKey(fen, GLFW_KEY_K) == GLFW_PRESS) move(MOVE_SPEED * (float)elapsedTime * -mRight); 		// K: left
	if (glfwGetKey(fen, GLFW_KEY_SEMICOLON) == GLFW_PRESS) move(MOVE_SPEED * (float)elapsedTime * mRight); 	// M: right
	if (glfwGetKey(fen, GLFW_KEY_SPACE) == GLFW_PRESS) move(MOVE_SPEED * (float)elapsedTime * mUp); 		// Space: up
	if (glfwGetKey(fen, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) move(MOVE_SPEED * (float)elapsedTime * -mUp);	// Right-Shift: down
}

void FirstPersonCamera::onMouseScroll(GLFWwindow* fen, double deltaX, double deltaY){
	double fov = mFOV + deltaY*ZOOM_SENSITIVITY;
	fov = glm::clamp(fov, 1.0, 120.0);
	setFOV((float) fov);
}

void FirstPersonCamera::onMouseMove(GLFWwindow* fen, double posX, double posY){
	// Nothing to do here
}

void FirstPersonCamera::updateCameraVectors(){
	// Transformation de coordonnées sphériques vers cartésiennes
	glm::vec3 look;
	look.x = cosf(mPitch)*sinf(mYaw); 	// (x =) rho * sin(theta) * cos(phi)
	look.y = sinf(mPitch); 				// (z =) rho * sin(phi)
	look.z = cosf(mPitch)*cosf(mYaw); 	// (y =) roh * cos(theta) * cos(phi)
	mLook = glm::normalize(look);

	// On recalcule les vecteurs Right et Up
	// Pour plus de simplicité, le vecteur Right "will be asumed horizontalt w.r.t. the world's Up vecor"
	mRight = glm::normalize(glm::cross(mLook, WORLD_UP));
	mUp = glm::normalize(glm::cross(mRight, mLook));

	mTargetPos = mPosition + mLook;
}

// Camera Handler

CameraHandler::CameraHandler(GLFWwindow* fen, CameraType ct, glm::vec3 pos) : orbitCamera(pos), fpCamera(pos) {
	this->cam = (ct == ORBIT_CAMERA) ? (Camera*) &orbitCamera : (Camera*) &fpCamera;
	this->curType = ct;

	orbitCamera.rotate(0.0, 0.0);
	orbitCamera.setRadius(10.0f);
	orbitCamera.setLookAt(glm::vec3(0.0f, 0.0f, 0.0f));

	if (ct == FIRST_PERSON_CAMERA) {
		// Hides and grab cursor, unlimited movement
		int width, height;
		glfwGetWindowSize(fen, &width, &height);
		glfwSetInputMode(fen, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetCursorPos(fen, width/2.0, height/2.0);
	}
}

void CameraHandler::setCameraType(GLFWwindow* fen, CameraType ct){
	this->cam = (ct == ORBIT_CAMERA) ? (Camera*) &orbitCamera : (Camera*) &fpCamera;
	this->curType = ct;

	int mode = (ct == FIRST_PERSON_CAMERA) ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL;
	glfwSetInputMode(fen, GLFW_CURSOR, mode);

	int width, height;
	glfwGetWindowSize(fen, &width, &height);
	glfwSetCursorPos(fen, width/2.0, height/2.0);

	cam->update(fen, 0.0);
}

CameraHandler::CameraType CameraHandler::getCameraType(){
	return this->curType;
}

glm::mat4 CameraHandler::getViewMatrix() const {
	return (*cam).getViewMatrix();
}

const glm::vec3& CameraHandler::getPosition() const {
	return cam->getPosition();
}

const glm::vec3& CameraHandler::getUp() const {
	return cam->getUp();
}

const glm::vec3& CameraHandler::getLook() const {
	return cam->getLook();
}

const glm::vec3& CameraHandler::getRight() const {
	return cam->getRight();
}

float CameraHandler::getFOV() const {
	return (*cam).getFOV();
}

void CameraHandler::setFOV(float fov) {
	return (*cam).setFOV(fov);
}

void CameraHandler::update(GLFWwindow* fen, double elapsedTime){
	cam->update(fen, elapsedTime);
}

void CameraHandler::onMouseScroll(GLFWwindow* fen, double deltaX, double deltaY){
	cam->onMouseScroll(fen, deltaX, deltaY);
}

void CameraHandler::onMouseMove(GLFWwindow* fen, double posX, double posY){
	cam->onMouseMove(fen, posX, posY);
}

#pragma endregion

#pragma region Mesh

Mesh::Mesh(){
	this->mLoaded = false;
}

Mesh::~Mesh(){
	if (mLoaded){
		glDeleteVertexArrays(1, &mVAO);
		glDeleteBuffers(1, &mVAO);
	}
}

bool Mesh::loadOBJ(const std::string &filename){
	using std::vector, std::string;

	vector<unsigned int> vertexIndices, normalIndices, uvIndices;
	vector<glm::vec3> tempVertices;
	vector<glm::vec3> tempNormals;
	vector<glm::vec2> tempUVs;

	if (filename.find(".obj") == string::npos){
		std::cerr << "Error, only .obj files are supported." << std::endl;
		return false;
	}

	std::ifstream fin(filename, std::ios::in);
	if (!fin){
		std::cerr << "Cannot open file " << filename << std::endl;
		return false;
	}

	std::cout << "Loading OBJ file " << filename << "... " << std::flush;

	int nb_line=0;
	string lineBuffer;
	while(std::getline(fin, lineBuffer)){
		nb_line++;

		std::stringstream ss(lineBuffer);
		string cmd;
		ss >> cmd; // copie du contenu du stringstream dans le string

		// Vertex
		if (cmd == "v"){
			glm::vec3 vertex;
			int dim = 0;
			while(dim < 3 && ss >> vertex[dim]) dim++;
			tempVertices.push_back(vertex);
		}

		// Vertex normal
		if (cmd == "vn"){
			glm::vec3 normal;
			int dim=0;
			while(dim < 3 && ss >> normal[dim]) dim++;
			normal = glm::normalize(normal);
			tempNormals.push_back(normal);
		}

		// Vertex texture
		if (cmd == "vt"){
			glm::vec3 uv;
			int dim=0;
			while(dim < 2 && ss >> uv[dim]) dim++;
			tempUVs.push_back(uv);
		}

		// Face
		if (cmd == "f"){
			string faceData;
			int vertexIndex, normalIndex, uvIndex; // v/vt/vn
			
			while(ss >> faceData){
				vector<string> data = split(faceData, "/");

				// Vertex index
				if (data[0].size() > 0){
					sscanf(data[0].c_str(), "%d", &vertexIndex);
					vertexIndices.push_back(vertexIndex);
				}

				// If the face vertex has a texture coordinate index
				if (data.size() >= 1){
					if (data[1].size() > 0){
						sscanf(data[1].c_str(), "%d", &uvIndex);
						uvIndices.push_back(uvIndex);
					}
				}

				// If the face vertex ahve a normal index
				if (data.size() >= 2){
					sscanf(data[2].c_str(), "%d", &normalIndex);
					normalIndices.push_back(normalIndex);
				}
			}
		}
	}
	
	// Close file
	fin.close();

	// For each vertex of each triangle
	for(unsigned int i=0 ; i<vertexIndices.size() ; i++){
		Vertex meshVertex;
		
		if (tempVertices.size() > 0){
			glm::vec3 vertex = tempVertices[vertexIndices[i] - 1];
			meshVertex.position = vertex;
		}

		if (tempNormals.size() > 0){
			glm::vec3 normal = tempNormals[normalIndices[i] - 1];
			meshVertex.normal = normal;
		}

		if (tempUVs.size() > 0){
			glm::vec2 uv = tempUVs[uvIndices[i] - 1];
			meshVertex.texCoords = uv;
		}

		mVertices.push_back(meshVertex);
	}

	// Create and initialize the buffers
	initBuffers();
	this->mLoaded = true;
	std::cout << "Done !" << std::endl;
	return true;
}

void Mesh::draw(){
	if (!mLoaded) return;

	glBindVertexArray(mVAO);
	glDrawArrays(GL_TRIANGLES, 0, mVertices.size());

	glBindVertexArray(0); // unbind
}

void Mesh::initBuffers(){
	// Buffer de positions
	glGenBuffers(1, &mVBO); // Alloue la mémoire dans la cg
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), &mVertices[0], GL_STATIC_DRAW); // Copie des données

	// Vertex Array de notre mesh
	glGenVertexArrays(1, &mVAO);
	glBindVertexArray(mVAO);

	// Positions (dire au Vertex Shader comment les vertices_pos sont organisés (layed out))
	GLint posId = 0;
	glVertexAttribPointer(posId, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL); // description des attributs
	glEnableVertexAttribArray(posId);

	// Normales
	GLint normId = 1;
	glVertexAttribPointer(normId, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLfloat*) (3 * sizeof(GLfloat)) );
	glEnableVertexAttribArray(normId);

	// Coordonnées de texture
	GLint texId = 2;
	glVertexAttribPointer(texId, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLfloat*) (6 * sizeof(GLfloat)) );
	glEnableVertexAttribArray(texId);

	// glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind ?
	glBindVertexArray(0); // unbind
}

#pragma endregion
