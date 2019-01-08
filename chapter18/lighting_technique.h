#pragma once

#include "technique.h"
#include "math_3d.h"

struct DirectionLight
{
    Vector3f Color;
    float AmbientIntensity;
    Vector3f Direction;
    float DiffuseIntensity;
};

class LightingTechnique : public Technique
{
    public:
        LightingTechnique();
        virtual bool Init();

        void SetWVP(const Matrix4f* WVP);
        void SetTextureUnit(unsigned int TextureUnit);
        void SetDirectionalLight(const DirectionLight& Light);

        bool ReadShaderFile(const char* fileName, std::string& fileString);

    private:
        GLuint m_WVPLocation;
        GLuint m_WorldMatrixLocation;
        GLuint m_samplerLocation;
        struct {
            GLuint Color;
            GLuint AmbientIntensity;
            GLuint Direction;
            GLuint DiffuseIntensity;
        } m_dirLightLocation;
};