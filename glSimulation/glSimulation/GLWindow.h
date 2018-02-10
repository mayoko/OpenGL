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
	// window の情報
	int width;
	int height;
	GLFWwindow* window;
	// OpenCV のイメージ
	cv::Mat grayImg;
	// OpenGL のバッファー関係
	GLuint vertexArrayID;
	GLuint programID;
	GLuint matrixID;
	GLuint textureID;
	GLuint vertexBufferID;
	GLuint uvBufferID;
	GLuint pboID;

	// OpenGL の window 画像を OpenCV の Mat に画像としてコピー
	void convertImg();
};