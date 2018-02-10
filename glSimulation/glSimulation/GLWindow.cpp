#include "GLWindow.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <algorithm>

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII
	
// シェーダーの読み込み
GLuint LoadShaders(const std::string& vertexShaderFile, const std::string& fragmentShaderFile);
// DDS ファイルの読み込み
GLuint LoadDDS(const std::string& filePath);

using namespace std;
	
GLWindow::GLWindow(const int width, const int height, const string& windowName)
	: height(height)
	, width(width)
	, window(glfwCreateWindow(width, height, windowName.c_str(), NULL, NULL))
	, grayImg(height, width, CV_8UC1)
{
	if (window == NULL) {
		cerr << "Failed to open GLFW window." << endl;
		getchar();
		glfwTerminate();
		exit(-1);
	}
	glfwMakeContextCurrent(window);
}

void GLWindow::setup(const std::vector<glm::vec3>& rectanglePos) {
	// 画像を投影する場所
	//vector<glm::vec3> positions(6);
	GLfloat positions[6*3];
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			positions[i * 3 + j] = rectanglePos[i][j];
		}
	}
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			positions[(i + 3) * 3 + j] = rectanglePos[(i + 2) % 4][j];
		}
	}
	// テクスチャの uv 座標
	vector<glm::vec2> uvPos = {
		glm::vec2(0.000059f, 0.000004f),
		glm::vec2(0.000103f, 0.336048f),
		glm::vec2(0.335973f, 0.335903f),
		glm::vec2(0.335973f, 0.335903f),
		glm::vec2(0.335973f, 0.000004f),
		glm::vec2(0.000059f, 0.000004f),
	};

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	programID = LoadShaders("shader/point.vert", "shader/point.frag");

	matrixID = glGetUniformLocation(programID, "MVP");

	glGenBuffers(1, &vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, 6*3 * sizeof(GLfloat), positions, GL_STATIC_DRAW);

	GLuint texture = LoadDDS("shader/uvtemplate.DDS");

	textureID = glGetUniformLocation(programID, "myTextureSampler");
	
	glGenBuffers(1, &uvBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
	glBufferData(GL_ARRAY_BUFFER, uvPos.size() * sizeof(glm::vec2), &uvPos[0], GL_STATIC_DRAW);

	// PBO 設定
	glGenBuffers(1, &pboID);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboID);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, width * height * 1, 0, GL_STREAM_READ);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

void GLWindow::update(const glm::mat4& MVP) {
	glUseProgram(programID);

	glUniformMatrix4fv(matrixID, 1, GL_FALSE, &MVP[0][0]);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
	glVertexAttribPointer(
		1,
		2,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glfwSwapBuffers(window);
	glfwPollEvents();
	//convertImg();
}
	
const cv::Mat& GLWindow::getGrayImg() const {
	return grayImg;
}

bool GLWindow::isClosed() {
	return glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwWindowShouldClose(window);
}

void GLWindow::convertImg() {
	//static void* dataBuffer = (GLubyte*)malloc(width * height);

	//// 読み取るOpneGLのバッファを指定 GL_FRONT:フロントバッファ　GL_BACK:バックバッファ
	//glReadBuffer(GL_BACK);

	//// OpenGLで画面に描画されている内容をバッファに格納
	//glReadPixels(
	//	0,                 //読み取る領域の左下隅のx座標
	//	0,                 //読み取る領域の左下隅のy座標 //0 or getCurrentWidth() - 1
	//	width,             //読み取る領域の幅
	//	height,            //読み取る領域の高さ
	//	GL_RED, //it means GL_BGR,           //取得したい色情報の形式
	//	GL_UNSIGNED_BYTE,  //読み取ったデータを保存する配列の型
	//	dataBuffer      //ビットマップのピクセルデータ（実際にはバイト配列）へのポインタ
	//);
	//GLubyte* p = static_cast<GLubyte*>(dataBuffer);

	//memcpy(grayImg.data, p, sizeof(GLubyte) * height * width);

	glBindBuffer(GL_PIXEL_PACK_BUFFER, pboID);
	glReadPixels(0, 0, width, height, GL_RED, GL_UNSIGNED_BYTE, 0);
	GLubyte *ptr = (GLubyte*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
	if (ptr) {
		memcpy(grayImg.data, ptr, width * height);
		glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
	}
	else {
		cerr << "TOTEMO TSURAI" << endl;
	}
	glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, 0);

	cv::flip(grayImg, grayImg, 0);
}

GLWindow::~GLWindow()
{
	glfwDestroyWindow(window);
}
	
GLuint LoadShaders(const string& vertexShaderFile, const string& fragmentShaderFile) {

    // シェーダを作ります。
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // ファイルから頂点シェーダのコードを読み込みます。
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertexShaderFile, std::ios::in);
    if(VertexShaderStream.is_open())
    {
        std::string Line = "";
        while(getline(VertexShaderStream, Line))
            VertexShaderCode += "\n" + Line;
        VertexShaderStream.close();
    }

    // ファイルからフラグメントシェーダを読み込みます。
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragmentShaderFile, std::ios::in);
    if(FragmentShaderStream.is_open()){
        std::string Line = "";
        while(getline(FragmentShaderStream, Line))
            FragmentShaderCode += "\n" + Line;
        FragmentShaderStream.close();
    }

    GLint Result = GL_FALSE;
    int InfoLogLength;

    // 頂点シェーダをコンパイルします。
	cout << "Compiling shader : " << vertexShaderFile << endl;
    char const * VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
    glCompileShader(VertexShaderID);

    // 頂点シェーダをチェックします。
	auto printErrorMessage = [&](const std::vector<GLchar> m) {
		//fprintf(stdout, "%sn", &VertexShaderErrorMessage[0]);
		for (GLchar c : m) {
			cout << c;
		}
		cout << endl;
	};

    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<GLchar> VertexShaderErrorMessage(InfoLogLength);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printErrorMessage(VertexShaderErrorMessage);
	}

    // フラグメントシェーダをコンパイルします。
	cout << "Compiling shader : " << fragmentShaderFile << endl;
    char const * FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
    glCompileShader(FragmentShaderID);

    // フラグメントシェーダをチェックします。
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<GLchar> FragmentShaderErrorMessage(InfoLogLength);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printErrorMessage(FragmentShaderErrorMessage);
	}

    // プログラムをリンクします。
	cout << "Linking program" << endl;
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    // プログラムをチェックします。
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<GLchar> ProgramErrorMessage(std::max(InfoLogLength, int(1)));
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printErrorMessage(ProgramErrorMessage);
	}

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    return ProgramID;

}
	
GLuint LoadDDS(const std::string& filePath) {
	unsigned char header[124];
	FILE *fp;

	if (fopen_s(&fp, filePath.c_str(), "rb") != 0) {
		cout << filePath << " could not be opened. Are you in the right directory?" << endl;
		getchar();
		return 0;
	}

	char filecode[4];
	fread(filecode, 1, 4, fp);
	if (strncmp(filecode, "DDS ", 4) != 0) {
		cout << "Not a correct BMP file" << endl;
		fclose(fp);
		return 0;
	}
	fread(&header, 124, 1, fp);

	unsigned int height = *(unsigned int*)&(header[8]);
	unsigned int width = *(unsigned int*)&(header[12]);
	unsigned int linearSize = *(unsigned int*)&(header[16]);
	unsigned int mipMapCount = *(unsigned int*)&(header[24]);
	unsigned int fourCC = *(unsigned int*)&(header[80]);

	unsigned char *buffer;
	unsigned int bufSize;
	bufSize = mipMapCount > 1 ? linearSize * 2 : linearSize;
	buffer = (unsigned char*)malloc(bufSize * sizeof(unsigned char));
	fread(buffer, 1, bufSize, fp);
	fclose(fp);

	unsigned int components = (fourCC == FOURCC_DXT1) ? 3 : 4;
	unsigned int format;
	switch (fourCC) {
	case FOURCC_DXT1:
		format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		break;
	case FOURCC_DXT3:
		format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		break;
	case FOURCC_DXT5:
		format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		break;
	default:
		free(buffer);
		return 0;
	}
	// Create OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	glBindTexture(GL_TEXTURE_2D, textureID);
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
	unsigned int offset = 0;

	// load mip map
	for (unsigned int level = 0; level < mipMapCount && (width || height); ++level) {
		unsigned int size = ((width + 3) / 4) * ((height + 3) / 4) * blockSize;
		glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height, 0, size, buffer + offset);
		offset += size;
		width /= 2;
		height /= 2;
	}
	free(buffer);
	return textureID;
}

GLFWwindow* GLWindow::getWindow() const {
	return window;
}

