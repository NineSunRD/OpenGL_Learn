#include <stdio.h>
#include <GL/glew.h>        // GLEW扩展库
#include <GL/freeglut.h>
#include <iostream>
#include <string>  
#include <fstream>  
#include <sstream> 
#include <cassert>
#include "math_3d.h"

GLuint VBO;
GLuint gScaleLocation; // 位置中间变量

// 定义要读取的顶点着色器脚本和片断着色器脚本的文件名，作为文件读取路径（这样的话shader.vs和shader.fs文件要放到工程的根目录下，保证下面定义的是这两个文件的文件路径）
const char* pVSFileName = "shader.vs";
const char* pFSFileName = "shader.fs";

/**
* 渲染回调函数
*/
static void RenderSceneCB() {
	// 清空颜色缓存
	glClear(GL_COLOR_BUFFER_BIT);

    // 维护一个不断慢慢增大的静态浮点数
    static float Scale = 0.0f;
    Scale += 0.01f;
    // 将值传递给shader
    glUniform1f(gScaleLocation, sinf(Scale));

	// 开启顶点属性
	glEnableVertexAttribArray(0);
	// 绑定GL_ARRAY_BUFFER缓冲器
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// 告诉管线怎样解析bufer中的数据
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// 开始绘制几何图形(绘制一个点)
	glDrawArrays(GL_TRIANGLES, 0, 3);

	//  禁用顶点数据
	glDisableVertexAttribArray(0);

	// 交换前后缓存
	glutSwapBuffers();
}

/**
* 设置渲染回调函数
*/
static void InitializeGlutCallbacks()
{
    glutDisplayFunc(RenderSceneCB);
    // 将渲染回调注册为全局闲置回调
    glutIdleFunc(RenderSceneCB);
}

/**
* 创建顶点缓冲器
*/
static void CreateVertexBuffer()
{
    // 创建含有3个顶点的顶点数组
    Vector3f Vertices[3];
    // 三角形的三个顶点位置
    Vertices[0] = Vector3f(-1.0f, -1.0f, 0.0f);
    Vertices[1] = Vector3f(1.0f, -1.0f, 0.0f);
    Vertices[2] = Vector3f(0.0f, 1.0f, 0.0f);

	// 创建缓冲器
	glGenBuffers(1, &VBO);
	// 绑定GL_ARRAY_BUFFER缓冲器
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// 绑定顶点数据
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
}

// 使用shader文本编译shader对象，并绑定shader都想到着色器程序中
static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
    // 根据shader类型参数定义两个shader对象
    GLuint ShaderObj = glCreateShader(ShaderType);
    // 检查是否定义成功
    if (ShaderObj == 0) {
        fprintf(stderr, "Error creating shader type %d\n", ShaderType);
        exit(0);
    }

    // 定义shader的代码源
    const GLchar* p[1];
    p[0] = pShaderText;
    GLint Lengths[1];
    Lengths[0]= strlen(pShaderText);
    glShaderSource(ShaderObj, 1, p, Lengths);
    glCompileShader(ShaderObj);// 编译shader对象

    // 检查和shader相关的错误
    GLint success;
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
        getchar();
    }

    // 将编译好的shader对象绑定到program object程序对象上
    glAttachShader(ShaderProgram, ShaderObj);
}

bool ReadFile(const char* fileName, std::string& fileString)
{
	std::ifstream fileStrm(fileName);
	if (!fileStrm.is_open()) {
		std::cout << "failed to open " << fileName << '\n';
		return false;
	}

    std::stringstream fileBuf;
    fileBuf << fileStrm.rdbuf(); 
    fileString = fileBuf.str();
	
	return true;
}

// 编译着色器函数
static void CompileShaders()
{
    // 创建着色器程序
    GLuint ShaderProgram = glCreateProgram();
    // 检查是否创建成功
    if (ShaderProgram == 0) {
        fprintf(stderr, "Error creating shader program\n");
        getchar();
    }

    // 存储着色器文本的字符串缓冲
    std::string vs, fs;
    // 分别读取着色器文件中的文本到字符串缓冲区
    if (!ReadFile(pVSFileName, vs)) {
        getchar();
    };
    if (!ReadFile(pFSFileName, fs)) {
        getchar();
    };

    // 添加顶点着色器和片段着色器
    AddShader(ShaderProgram, vs.c_str(), GL_VERTEX_SHADER);
    AddShader(ShaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);

    // 链接shader着色器程序，并检查程序相关错误
    GLint Success = 0;
    GLchar ErrorLog[1024] = { 0 };
    glLinkProgram(ShaderProgram);
    glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);
    if (Success == 0) {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        getchar();
    }

    // 检查验证在当前的管线状态程序是否可以被执行
    glValidateProgram(ShaderProgram);
    glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
    if (!Success) {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        getchar();
    }

    // 设置到管线声明中来使用上面成功建立的shader程序
    glUseProgram(ShaderProgram);

    // 查询获取一致变量的位置
    gScaleLocation = glGetUniformLocation(ShaderProgram, "gScale");
    // 检查错误

    std::cout << "gScaleLocation " << gScaleLocation << '\n';

    assert(gScaleLocation != 0xFFFFFFFF);
}

/**
* 主函数
*/
int main(int argc, char ** argv) {

	// 初始化GLUT
	glutInit(&argc, argv);

	// 显示模式：双缓冲、RGBA
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	// 窗口设置
	glutInitWindowSize(480, 320);      // 窗口尺寸
	glutInitWindowPosition(100, 100);  // 窗口位置
	glutCreateWindow("Tutorial 05");   // 窗口标题

									   // 开始渲染
	InitializeGlutCallbacks();

	// 检查GLEW是否就绪，必须要在GLUT初始化之后！
	GLenum res = glewInit();
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}

	printf("GL version: %s\n", glGetString(GL_VERSION));

	// 缓存清空后的颜色值
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// 创建顶点缓冲器
	CreateVertexBuffer();

    // 编译着色器
    CompileShaders();

	// 通知开始GLUT的内部循环
	glutMainLoop();

	return 0;
}