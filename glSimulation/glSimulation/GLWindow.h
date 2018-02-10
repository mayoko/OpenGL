#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

class GLWindow
{
public:
	GLWindow(const int width, const int height, const std::string& windowName);
	~GLWindow();
	void setup(const std::vector<glm::vec3>& rectanglePos);
	void update(const glm::mat4& MVP);
	GLFWwindow* getWindow() const;
	const cv::Mat& getGrayImg() const;
	bool GLWindow::isClosed();
private:
	// window �̏��
	int width;
	int height;
	GLFWwindow* window;
	// OpenCV �̃C���[�W
	cv::Mat grayImg;
	// OpenGL �̃o�b�t�@�[�֌W
	GLuint vertexArrayID;
	GLuint programID;
	GLuint matrixID;
	GLuint textureID;
	GLuint vertexBufferID;
	GLuint uvBufferID;
	GLuint pboID;

	// OpenGL �� window �摜�� OpenCV �� Mat �ɉ摜�Ƃ��ăR�s�[
	void convertImg();
};