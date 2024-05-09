#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/constants.hpp>
#include <glm/glm.hpp>

// Déclaration des constantes (initialisées globalement dans Utils.cpp)
extern const std::string SHADERS_PATH;
extern const std::string ASSETS_PATH;

GLFWwindow* initOpenGL(std::string titre = "OpenGL", int width=720, int height=480, bool isFullScreen=false);
GLFWwindow* initOpenGL3D(std::string titre = "OpenGL", int width=720, int height=480, bool isFullScreen=false);
void showFPS(GLFWwindow* fen, const std::string primary_title);

class ShaderProgram {
	public:
		enum ShaderType { VERTEX, FRAGMENT, PROGRAM };
	private:
		GLuint mHandle;
		std::map<std::string, GLint> mUniformLocations;

	public:
		ShaderProgram();
		~ShaderProgram();
		// bool loadShaders(const char* vsFilename, const char* fsFilename);
		bool loadShaders(const std::string vsFilename, const std::string fsFilename);
		void use();
		void setUniform(const GLchar* name, const glm::vec2 &v);
		void setUniform(const GLchar* name, const glm::vec3 &v);
		void setUniform(const GLchar* name, const glm::vec4 &v);
		void setUniform(const GLchar* name, const glm::mat4 &m);
		void setUniform(const GLchar* name, const GLfloat f);
		void setUniform(const GLchar* name, const GLint b);
		void setUniformSampler(const GLchar* name, const GLint &slot);
		GLuint getProgramObjectId() const;
	private:
		std::string fileToString(const std::string& filename);
		void checkCompileErrors(GLuint shader, ShaderType type);
		GLint getUniformLocation(GLchar* name);
};

class Texture2D {
	private:
		GLuint mTexture;

	public:
		Texture2D();
		virtual ~Texture2D();
		bool loadTexture(const std::string &filename, bool generateMipMaps = true);
		void bind(GLuint texUnit = 0);
		void unbind(GLuint texUnit = 0);
};

// Classe abstraite
class Camera {
	friend class CameraHandler;

	protected:
		static float MOUSE_SENSITIVITY;
		static double ZOOM_SENSITIVITY;
		static float MOVE_SPEED; // unités par seconde
		glm::vec3 mPosition;
		glm::vec3 mTargetPos;
		glm::vec3 mUp;
		glm::vec3 mLook;
		glm::vec3 mRight;
		const glm::vec3 WORLD_UP;
		float mYaw; // Angle d'Euler (radians)
		float mPitch; // Angle d'Euler (radians)
		float mFOV; // en degrés
	
	public:
		glm::mat4 getViewMatrix() const;
		const glm::vec3& getPosition() const;
		const glm::vec3& getUp() const;
		const glm::vec3& getLook() const;
		const glm::vec3& getRight() const;
		float getFOV() const;
		void setFOV(float fov);
		static void setMouseSensitivity(float sens);
		static void setZoomSensitivity(double sens);
		static void setMoveSpeed(float speed);
		virtual void rotate(float yaw, float pitch)=0; // angles en degrés
		virtual void move(const glm::vec3 &offsetPosition)=0;
		virtual void update(GLFWwindow* fen, double elapsedTime)=0;
		virtual void onMouseScroll(GLFWwindow* fen, double deltaX, double deltaY)=0;
		virtual void onMouseMove(GLFWwindow* fen, double posX, double posY)=0;

	protected:
		Camera(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f));
		virtual ~Camera();
		virtual void updateCameraVectors()=0;
};

class OrbitCamera : public Camera {
	private:
		float mRadius;

	public:
		OrbitCamera(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f));
		void setLookAt(const glm::vec3 &target);
		void setRadius(float radius);
		virtual void rotate(float yaw, float pitch); // angles en degrés
		virtual void move(const glm::vec3 &offsetPosition);
		virtual void update(GLFWwindow* fen, double elapsedTime);
		virtual void onMouseScroll(GLFWwindow* fen, double deltaX, double deltaY);
		virtual void onMouseMove(GLFWwindow* fen, double posX, double posY);
	protected:
		virtual void updateCameraVectors();
};

class FirstPersonCamera : public Camera {
	public:
		FirstPersonCamera(glm::vec3 position=glm::vec3(0.0f, 0.0f, 0.0f), float yaw=glm::pi<float>(), float pitch=0.0f);
		virtual void setPosition(const glm::vec3 &position);
		virtual void rotate(float yaw, float pitch); // angles en degrés
		virtual void move(const glm::vec3 &offsetPosition);
		virtual void update(GLFWwindow* fen, double elapsedTime);
		virtual void onMouseScroll(GLFWwindow* fen, double deltaX, double deltaY);
		virtual void onMouseMove(GLFWwindow* fen, double posX, double posY);
	protected:
		virtual void updateCameraVectors();
};

class CameraHandler {
	public:
		enum CameraType { ORBIT_CAMERA, FIRST_PERSON_CAMERA };

	private:
		Camera* cam;
		OrbitCamera orbitCamera;
		FirstPersonCamera fpCamera;
		CameraType curType;

	public:
		CameraHandler(GLFWwindow* fen, CameraType ct = FIRST_PERSON_CAMERA, glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f));
		void setCameraType(GLFWwindow* fen, CameraType ct);
		CameraType getCameraType();

		glm::mat4 getViewMatrix() const;
		const glm::vec3& getPosition() const;
		const glm::vec3& getUp() const;
		const glm::vec3& getLook() const;
		const glm::vec3& getRight() const;
		float getFOV() const;
		void setFOV(float fov);
		void update(GLFWwindow* fen, double elapsedTime);
		void onMouseScroll(GLFWwindow* fen, double deltaX, double deltaY);
		void onMouseMove(GLFWwindow* fen, double posX, double posY);
};

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords; // Texture coordinates
};

class Mesh {
	private:
		bool mLoaded;
		std::vector<Vertex> mVertices;
		GLuint mVBO;
		GLuint mVAO;

	public:
		Mesh();
		~Mesh();
		bool loadOBJ(const std::string &filename);
		void draw();
		void printVerticies(){
			std::cout << this->mVertices.size() << std::endl;
		}
	private:
		void initBuffers();
};

#endif
