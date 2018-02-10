// OpenGL
#include <GL/glew.h>
#include <GLFW/glfw3.h>
// glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
// imgui
#include "../../imgui-master/imgui-master/examples/opengl3_example/imgui_impl_glfw_gl3.h"
#include "../../imgui-master/imgui-master/imgui.h"
// std
#include <iostream>
#include <vector>
// my file
#include "GLWindow.h"
#include "Geometory.h"

using namespace std;

struct Parameters {
	float alpha;
	float beta;
	float L;
	float tx;
	float ty;
	float tz;
	float camRotX;
	float camRotY;
	float camRotZ;
	float fx;
	float fy;
	float cx;
	float cy;
	Parameters()
		: alpha(0.0f)
		, beta(0.0f)
		, L(20.0f)
		, tx(-20.0f)
		, ty(10.0f)
		, tz(0.0f)
		, camRotX(-90)
		, camRotY(0)
		, camRotZ(90)
		, fx(2200)
		, fy(2200)
		, cx(512)
		, cy(384)
	{}
};

void ImGuiDisplay(Parameters& p) {
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::SliderFloat("alpha", &(p.alpha), -30.0f, 30.0f);
	ImGui::SliderFloat("beta", &(p.beta),   -30.0f, 30.0f);
	ImGui::SliderFloat("L", &(p.L),   0.0f, 40.0f);
	ImGui::SliderFloat("tx", &(p.tx),   -30.0f, 30.0f);
	ImGui::SliderFloat("ty", &(p.ty),   -30.0f, 30.0f);
	ImGui::SliderFloat("tz", &(p.tz),   -30.0f, 30.0f);
	ImGui::SliderFloat("camRotX", &(p.camRotX),   -180.0f, 180.0f);
	ImGui::SliderFloat("camRotY", &(p.camRotY),   -180.0f, 180.0f);
	ImGui::SliderFloat("camRotZ", &(p.camRotZ),   -180.0f, 180.0f);
	ImGui::SliderFloat("fx", &(p.fx),   1000.0f, 3000.0f);
	ImGui::SliderFloat("fy", &(p.fy),   1000.0f, 3000.0f);
	ImGui::SliderFloat("cx", &(p.cx),   300.0f, 700.0f);
	ImGui::SliderFloat("cy", &(p.cy),   200.0f, 500.0f);
}

int main() {
	// Initialize GLFW
	if (!glfwInit()) {
		cerr << "Failed to initialize GLFW" << endl;
		getchar();
		return -1;
	}
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//// Open a window and create its OpenGL context
	GLWindow window(1024, 768, "window");

	// Initialize GLEW
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		cerr << "Failed to initialize GLEW" << endl;
		getchar();
		glfwTerminate();
		return -1;
	}
	glClearColor(0.0f, 0.0f, 0.2f, 0.0f);

	//vector<glm::vec3> rectanglePos = {
	//	glm::vec3(-0.5f, 0.5f, 0.5f),
	//	glm::vec3(-0.5f, -0.5f, 0.5f),
	//	glm::vec3(0.5f, -0.5f, 0.5f),
	//	glm::vec3(0.5f, 0.5f, 0.5f)
	//};
	const vector<glm::vec3> rectanglePos = {
		glm::vec3(-50.f,  50.f, 1700.0f),
		glm::vec3(-50.f, -50.f, 1700.0f),
		glm::vec3( 50.f, -50.f, 1700.0f),
		glm::vec3( 50.f,  50.f, 1700.0f)
	};
	window.setup(rectanglePos);

	// enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	if (!ImGui_ImplGlfwGL3_Init(window.getWindow(), true)) {
		std::cerr << "ImGui Init error" << std::endl;
		getchar();
		glfwTerminate();
		return -1;
	}

	Parameters p;
	while (!window.isClosed()) {
		const cv::Mat1d Tp = (cv::Mat_<double>(3, 1) << p.tx, p.ty, p.tz);
		const cv::Mat1d K = (cv::Mat_<double>(3, 3) <<
				p.fx, 0, p.cx,
				0, p.fy, p.cy,
				0, 0, 1
			);
		Geometory geometory(Tp, SMMath::R(p.camRotX, p.camRotY, p.camRotZ), K, cv::Size(1024, 768), p.L);

		geometory.update(p.alpha, p.beta);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//window.update(glm::mat4());
		ImGui_ImplGlfwGL3_NewFrame();
		ImGuiDisplay(p);
		ImGui::Render();
		//std::cout << to_string(geometory.getMVP()) << std::endl;
		window.update(geometory.getMVP());
		//cv::imshow("cv", window.getGrayImg());
		//cv::waitKey(1);
	}

	ImGui_ImplGlfwGL3_Shutdown();
	glfwTerminate();
	return 0;
}