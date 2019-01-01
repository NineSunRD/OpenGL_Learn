#include <stdio.h>
#include <GL/glew.h>        // GLEW扩展库
#include <GL/freeglut.h>
#include <iostream>
#include <string>  
#include <fstream>  
#include <sstream> 
#include <cassert>

#include "math_3d.h"
#include "pipeline.h"
#include "Camera.h"
#include "texture.h"

// 屏幕宽高宏定义
#define WINDOW_WIDTH 480
#define WINDOW_HEIGHT 320

GLuint VBO;
// 索引缓冲对象的句柄
GLuint IBO;
// 平移变换一致变量的句柄引用
GLuint gWVPLocation;
// 相机对象
Camera* pGameCamera;

GLuint gSampler;
Texture* pTexture = NULL;

float fFOV = 30.0f;
float fHeight = WINDOW_HEIGHT;
float fWidth = WINDOW_WIDTH;
float fZNear = 1.0f;
float fZFar = 100.0f;

// 定义要读取的顶点着色器脚本和片断着色器脚本的文件名，作为文件读取路径（这样的话shader.vs和shader.fs文件要放到工程的根目录下，保证下面定义的是这两个文件的文件路径）
const char* pVSFileName = "shader.vs";
const char* pFSFileName = "shader.fs";

/**
* 渲染回调函数
*/
static void RenderSceneCB() 
{

    pGameCamera->OnRender();

	// 清空颜色缓存
	glClear(GL_COLOR_BUFFER_BIT);

    // 维护一个不断慢慢增大的静态浮点数
    static float Scale = 0.0f;
    Scale += 0.01f;

    // 实例化一个pipeline管线类对象，初始化配置好之后传递给shader
    Pipeline p;
    p.WorldPos(0.0f, 0.0f, 10.0f);
    p.Rotate(0.0f, sinf(Scale) * 90.0f, 0.0f);
    // 设置投影变换的参数
    p.SetPerspectiveProj(fFOV, fHeight, fWidth, fZNear, fZFar);
    // 设置相机参数
    p.SetCamera(pGameCamera->GetPos(), pGameCamera->GetTarget(), pGameCamera->GetUp());

    // 将矩阵数据加载到shader中
    glUniformMatrix4fv(gWVPLocation, 1, GL_TRUE, (const GLfloat*)p.GetTrans());

	// 开启顶点属性
	glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
	// 绑定GL_ARRAY_BUFFER缓冲器
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// 告诉管线怎样解析bufer中的数据
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)12);

    // 每次在绘制之前绑定索引缓冲
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    pTexture->Bind(GL_TEXTURE0);
    // 索引绘制图形
    glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

	//  禁用顶点数据
	glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

	// 交换前后缓存
	glutSwapBuffers();
}

// 传递键盘事件
static void SpecialKeyboardCB(int key, int x, int y)
{
    pGameCamera->OnKeyboard(key);
}

static void KeyboardCB(unsigned char Key, int x, int y)
{
    switch (Key) {
        case 'q':
            exit(0);
    }
}

static void PassiveMouseCB(int x, int y)
{
    pGameCamera->OnMouse(x, y);
}

/**
* 设置渲染回调函数
*/
static void InitializeGlutCallbacks()
{
    glutDisplayFunc(RenderSceneCB);
    // 将渲染回调注册为全局闲置回调
    glutIdleFunc(RenderSceneCB);
    // 注册键盘事件
    glutSpecialFunc(SpecialKeyboardCB);
    glutPassiveMotionFunc(PassiveMouseCB);
    glutKeyboardFunc(KeyboardCB);
}

/**
* 创建顶点缓冲器
*/
static void CreateVertexBuffer()
{
    // 三角体4个顶点
    Vertex Vertices[4] = { Vertex(Vector3f(-1.0f, -1.0f, 0.5773f), Vector2f(0.0f, 0.0f)),
                           Vertex(Vector3f(0.0f, -1.0f, -1.15475), Vector2f(0.5f, 0.0f)),
                           Vertex(Vector3f(1.0f, -1.0f, 0.5773f),  Vector2f(1.0f, 0.0f)),
                           Vertex(Vector3f(0.0f, 1.0f, 0.0f),      Vector2f(0.5f, 1.0f)) };

	// 创建缓冲器
	glGenBuffers(1, &VBO);
	// 绑定GL_ARRAY_BUFFER缓冲器
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// 绑定顶点数据
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
}

// 创建索引缓冲器
static void CreateIndexBuffer()
{
    // 四个三角形面的顶点索引集
    unsigned int Indices[] = { 0, 3, 1,
                               1, 3, 2,
                               2, 3, 0,
                               0, 1, 2 };
    // 创建缓冲区
    glGenBuffers(1, &IBO);
    // 绑定缓冲区
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    // 添加缓冲区数据
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
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
    gWVPLocation = glGetUniformLocation(ShaderProgram, "gWVP");
    assert(gWVPLocation != 0xFFFFFFFF);

    gSampler = glGetUniformLocation(ShaderProgram, "gSampler");
    assert(gSampler != 0xFFFFFFFF);
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
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);      // 窗口尺寸
	glutInitWindowPosition(100, 100);  // 窗口位置
	glutCreateWindow("Tutorial 06");   // 窗口标题

    //glFrontFace(GL_CW);
    //glCullFace(GL_BACK);
    //glEnable(GL_CULL_FACE);

	InitializeGlutCallbacks();

    pGameCamera = new Camera(WINDOW_WIDTH, WINDOW_HEIGHT);

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
    CreateIndexBuffer();

    // 编译着色器
    CompileShaders();

    glUniform1i(gSampler, 0);

    pTexture = new Texture(GL_TEXTURE_2D, "../content/test.png");

    if (!pTexture->Load()) {
        return 1;
    }

	// 通知开始GLUT的内部循环
	glutMainLoop();

	return 0;
}