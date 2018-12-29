#include <stdio.h>
#include <GL/glew.h>        // GLEW��չ��
#include <GL/freeglut.h>
#include <iostream>
#include <string>  
#include <fstream>  
#include <sstream> 
#include <cassert>
#include "math_3d.h"
#include "pipeline.h"

// ��Ļ��ߺ궨��
#define WINDOW_WIDTH 480
#define WINDOW_HEIGHT 320

GLuint VBO;
// �����������ľ��
GLuint IBO;
// ƽ�Ʊ任һ�±����ľ������
GLuint gWorldLocation;

float fFOV = 30.0f;
float fHeight = WINDOW_HEIGHT;
float fWidth = WINDOW_WIDTH;
float fZNear = 1.0f;
float fZFar = 100.0f;

// ����Ҫ��ȡ�Ķ�����ɫ���ű���Ƭ����ɫ���ű����ļ�������Ϊ�ļ���ȡ·���������Ļ�shader.vs��shader.fs�ļ�Ҫ�ŵ����̵ĸ�Ŀ¼�£���֤���涨������������ļ����ļ�·����
const char* pVSFileName = "shader.vs";
const char* pFSFileName = "shader.fs";

/**
* ��Ⱦ�ص�����
*/
static void RenderSceneCB() {

	// �����ɫ����
	glClear(GL_COLOR_BUFFER_BIT);

    // ά��һ��������������ľ�̬������
    static float Scale = 0.0f;
    Scale += 0.01f;

    // ʵ����һ��pipeline��������󣬳�ʼ�����ú�֮�󴫵ݸ�shader
    Pipeline p;
    p.WorldPos(0.0f, 0.0f, 5.0f);
    p.Rotate(0.0f, sinf(Scale) * 90.0f, 0.0f);
    // ����ͶӰ�任�Ĳ���
    p.SetPerspectiveProj(fFOV, fHeight, fWidth, fZNear, fZFar);

    // ���������ݼ��ص�shader��
    glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, (const GLfloat*)p.GetTrans());

	// ������������
	glEnableVertexAttribArray(0);
	// ��GL_ARRAY_BUFFER������
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// ���߹�����������bufer�е�����
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // ÿ���ڻ���֮ǰ����������
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    // ��������ͼ��
    glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

	//  ���ö�������
	glDisableVertexAttribArray(0);

	// ����ǰ�󻺴�
	glutSwapBuffers();
}

/**
* ������Ⱦ�ص�����
*/
static void InitializeGlutCallbacks()
{
    glutDisplayFunc(RenderSceneCB);
    // ����Ⱦ�ص�ע��Ϊȫ�����ûص�
    glutIdleFunc(RenderSceneCB);
}

/**
* �������㻺����
*/
static void CreateVertexBuffer()
{
    // ������8������
    Vector3f Vertices[4];
    Vertices[0] = Vector3f(-1.0f, -1.0f, 0.5773f);
    Vertices[1] = Vector3f(0.0f, -1.0f, -1.15475f);
    Vertices[2] = Vector3f(1.0f, -1.0f, 0.5773f);
    Vertices[3] = Vector3f(0.0f, 1.0f, 0.0f);

	// ����������
	glGenBuffers(1, &VBO);
	// ��GL_ARRAY_BUFFER������
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// �󶨶�������
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
}

// ��������������
static void CreateIndexBuffer()
{
    // �ĸ���������Ķ���������
    unsigned int Indices[] = { 0, 3, 1,
                               1, 3, 2,
                               2, 3, 0,
                               0, 1, 2 };
    // ����������
    glGenBuffers(1, &IBO);
    // �󶨻�����
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    // ��ӻ���������
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
}

// ʹ��shader�ı�����shader���󣬲���shader���뵽��ɫ��������
static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
    // ����shader���Ͳ�����������shader����
    GLuint ShaderObj = glCreateShader(ShaderType);
    // ����Ƿ���ɹ�
    if (ShaderObj == 0) {
        fprintf(stderr, "Error creating shader type %d\n", ShaderType);
        exit(0);
    }

    // ����shader�Ĵ���Դ
    const GLchar* p[1];
    p[0] = pShaderText;
    GLint Lengths[1];
    Lengths[0]= strlen(pShaderText);
    glShaderSource(ShaderObj, 1, p, Lengths);
    glCompileShader(ShaderObj);// ����shader����

    // ����shader��صĴ���
    GLint success;
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
        getchar();
    }

    // ������õ�shader����󶨵�program object���������
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

// ������ɫ������
static void CompileShaders()
{
    // ������ɫ������
    GLuint ShaderProgram = glCreateProgram();
    // ����Ƿ񴴽��ɹ�
    if (ShaderProgram == 0) {
        fprintf(stderr, "Error creating shader program\n");
        getchar();
    }

    // �洢��ɫ���ı����ַ�������
    std::string vs, fs;
    // �ֱ��ȡ��ɫ���ļ��е��ı����ַ���������
    if (!ReadFile(pVSFileName, vs)) {
        getchar();
    };
    if (!ReadFile(pFSFileName, fs)) {
        getchar();
    };

    // ��Ӷ�����ɫ����Ƭ����ɫ��
    AddShader(ShaderProgram, vs.c_str(), GL_VERTEX_SHADER);
    AddShader(ShaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);

    // ����shader��ɫ�����򣬲���������ش���
    GLint Success = 0;
    GLchar ErrorLog[1024] = { 0 };
    glLinkProgram(ShaderProgram);
    glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);
    if (Success == 0) {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        getchar();
    }

    // �����֤�ڵ�ǰ�Ĺ���״̬�����Ƿ���Ա�ִ��
    glValidateProgram(ShaderProgram);
    glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
    if (!Success) {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        getchar();
    }

    // ���õ�������������ʹ������ɹ�������shader����
    glUseProgram(ShaderProgram);

    // ��ѯ��ȡһ�±�����λ��
    gWorldLocation = glGetUniformLocation(ShaderProgram, "gWorld");
    // ������

    std::cout << "gWorldLocation " << gWorldLocation << '\n';

    assert(gWorldLocation != 0xFFFFFFFF);
}

/**
* ������
*/
int main(int argc, char ** argv) {

	// ��ʼ��GLUT
	glutInit(&argc, argv);

	// ��ʾģʽ��˫���塢RGBA
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	// ��������
	glutInitWindowSize(480, 320);      // ���ڳߴ�
	glutInitWindowPosition(100, 100);  // ����λ��
	glutCreateWindow("Tutorial 06");   // ���ڱ���

									   // ��ʼ��Ⱦ
	InitializeGlutCallbacks();

	// ���GLEW�Ƿ����������Ҫ��GLUT��ʼ��֮��
	GLenum res = glewInit();
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}

	printf("GL version: %s\n", glGetString(GL_VERSION));

	// ������պ����ɫֵ
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// �������㻺����
	CreateVertexBuffer();
    CreateIndexBuffer();

    // ������ɫ��
    CompileShaders();

	// ֪ͨ��ʼGLUT���ڲ�ѭ��
	glutMainLoop();

	return 0;
}