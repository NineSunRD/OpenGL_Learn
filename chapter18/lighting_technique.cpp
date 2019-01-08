#include "lighting_technique.h"
#include <string>
#include <fstream>
#include <sstream>

// 定义要读取的顶点着色器脚本和片断着色器脚本的文件名，作为文件读取路径（这样的话shader.vs和shader.fs文件要放到工程的根目录下，保证下面定义的是这两个文件的文件路径）
const char* pVSFileName = "lighting.vs";
const char* pFSFileName = "lighting.fs";

LightingTechnique::LightingTechnique()
{

}

bool LightingTechnique::ReadShaderFile(const char* fileName, std::string& fileString)
{
	std::ifstream fileStrm(fileName);
	if (!fileStrm.is_open()) {
		return false;
	}

    std::stringstream fileBuf;
    fileBuf << fileStrm.rdbuf();
    fileString = fileBuf.str();

	return true;
}

bool LightingTechnique::Init()
{
    if (!Technique::Init())
    {
        return false;
    }

    // 存储着色器文本的字符串缓冲
    std::string vs, fs;
    // 分别读取着色器文件中的文本到字符串缓冲区
    if (!ReadShaderFile(pVSFileName, vs)) {
        return false;
    };
    if (!ReadShaderFile(pFSFileName, fs)) {
         return false;
    };

    if (!AddShader(GL_VERTEX_SHADER, vs.c_str()))
    {
        return false;
    }

    if (!AddShader(GL_FRAGMENT_SHADER, fs.c_str()))
    {
        return false;
    }

    if (!Finalize())
    {
        return false;
    }

    m_WVPLocation = GetUniformLocation("gWVP");
    m_WorldMatrixLocation = GetUniformLocation("gWorld");
    m_samplerLocation = GetUniformLocation("gSampler");
    m_dirLightLocation.Color = GetUniformLocation("gDirectionalLight.Color");
    m_dirLightLocation.AmbientIntensity = GetUniformLocation("gDirectionalLight.AmbientIntensity");
    m_dirLightLocation.Direction = GetUniformLocation("gDirectionalLight.Direction");
    m_dirLightLocation.DiffuseIntensity = GetUniformLocation("gDirectionalLight.DiffuseIntensity");

    if (m_dirLightLocation.AmbientIntensity == 0xFFFFFFFF ||
        m_WVPLocation == 0xFFFFFFFF ||
        m_WorldMatrixLocation == 0xFFFFFFFF ||
        m_samplerLocation == 0xFFFFFFFF ||
        m_dirLightLocation.Color == 0xFFFFFFFF ||
        m_dirLightLocation.DiffuseIntensity == 0xFFFFFFFF ||
        m_dirLightLocation.Direction == 0xFFFFFFFF) {
        return false;
    }

    return true;
}

void LightingTechnique::SetWVP(const Matrix4f* WVP)
{
    glUniformMatrix4fv(m_WVPLocation, 1, GL_TRUE, (const GLfloat*)WVP->m);
}


void LightingTechnique::SetTextureUnit(unsigned int TextureUnit)
{
    glUniform1i(m_samplerLocation, TextureUnit);
}


void LightingTechnique::SetDirectionalLight(const DirectionLight& Light)
{
    glUniform3f(m_dirLightLocation.Color, Light.Color.x, Light.Color.y, Light.Color.z);
    glUniform1f(m_dirLightLocation.AmbientIntensity, Light.AmbientIntensity);
    Vector3f Direction = Light.Direction;
    Direction.Normalize();
    glUniform3f(m_dirLightLocation.Direction, Direction.x, Direction.y, Direction.z);
    glUniform1f(m_dirLightLocation.DiffuseIntensity, Light.DiffuseIntensity);
}