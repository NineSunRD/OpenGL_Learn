#include <stdio.h>
#include <string.h>

#include "technique.h"

Technique::Technique()
{
    m_shaderProg = 0;
}

Technique::~Technique()
{
    for(ShaderObjList::iterator it = m_shaderObjList.begin(); it != m_shaderObjList.end(); it++)
    {
        glDeleteShader(*it);
    }

    if(m_shaderProg != 0)
    {
        glDeleteProgram(m_shaderProg);
        m_shaderProg = 0;
    }
}

bool Technique::Init()
{
    m_shaderProg = glCreateProgram();

    if (m_shaderProg == 0){
        fprintf(stderr, "Error creating shader program\n");
        return false;
    }

    return true;
}

bool Technique::AddShader(GLenum ShaderType, const char* pShaderText)
{
    GLuint ShaderObj = glCreateShader(ShaderType);

    if (ShaderObj == 0){
        fprintf(stderr, "Error creating shader type %d\n", ShaderType);
        return false;
    }

    m_shaderObjList.push_back(ShaderObj);

    const GLchar* p[1];
    p[0] = pShaderText;
    GLint Lengths[1];
    Lengths[0] = strlen(pShaderText);
    glShaderSource(ShaderObj, 1, p, Lengths);

    //检查和shader编译相关的错误
    GLint success;
    glCompileShader(ShaderObj);
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        GLchar ErrorLog[1024];
        glGetShaderInfoLog(ShaderObj, 1024, NULL, ErrorLog);
        fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, ErrorLog);
        return false;
    }

    glAttachShader(m_shaderProg, ShaderObj);

    return true;
}

bool Technique::Finalize()
{
    GLint Success = 0;

    //链接shader着色器程序，并检查程序相关错误
    glLinkProgram(m_shaderProg);
    glGetProgramiv(m_shaderProg, GL_LINK_STATUS, &Success);
    if (Success == 0){
        GLchar ErrorLog[1024] = {0};
        glGetProgramInfoLog(m_shaderProg, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        return false;
    }

    //检查验证在当前的管线状态程序是否可以被执行
    glValidateProgram(m_shaderProg);
    glGetProgramiv(m_shaderProg, GL_VALIDATE_STATUS, &Success);
    if (Success == 0){
        GLchar ErrorLog[1024] = {0};
        glGetProgramInfoLog(m_shaderProg, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        return false;
    }

    //删除已添加到程序中的中间着色器对象
    for (ShaderObjList::iterator it = m_shaderObjList.begin(); it != m_shaderObjList.end(); it++){
        glDeleteShader(*it);
    }

    m_shaderObjList.clear();

    return true;
}

void Technique::Enable(){
    glUseProgram(m_shaderProg);
}

GLint Technique::GetUniformLocation(const char* pUniformName){
    GLint Location = glGetUniformLocation(m_shaderProg, pUniformName);

    if (Location == 0xFFFFFFFF){
        fprintf(stderr, "Warning! Unable to get the location of uniform '%s'\n", pUniformName);
    }

    return Location;
}