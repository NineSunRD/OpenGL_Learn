#pragma once

#include "technique.h"
#include "math_3d.h"

#define MAX_POINT_LIGHTS 3

struct BaseLight
{
    Vector3f Color;
    float AmbientIntensity;
    float DiffuseIntensity;

    BaseLight()
    {
        Color = Vector3f(0.0f, 0.0f, 0.0f);
        AmbientIntensity = 0.0f;
        DiffuseIntensity = 0.0f;
    }
};

struct DirectionLight : public BaseLight
{
    Vector3f Direction;
    
    DirectionLight()
    {
        Direction = Vector3f(0.0f, 0.0f, 0.0f);
    }
};

struct PointLight : public BaseLight
{
    Vector3f Position;

    struct
    {
        float Constant;
        float Linear;
        float Exp;
    } Attenuation;

    PointLight()
    {
        Position = Vector3f(0.0f, 0.0f, 0.0f);
        Attenuation.Constant = 1.0f;
        Attenuation.Linear = 0.0f;
        Attenuation.Exp = 0.0f;
    }
};

class LightingTechnique : public Technique
{
    public:
        LightingTechnique();
        virtual bool Init();

        void SetWVP(const Matrix4f* WVP);
        void SetWorldMatrix(const Matrix4f* WorldInverse);
        void SetTextureUnit(unsigned int TextureUnit);
        void SetDirectionalLight(const DirectionLight& Light);

        void SetEyeWorldPos(const Vector3f& EyeWorldPos);
        void SetMatSpecularIntensity(float Intensity);
        void SetMatSpecularPower(float Power);

        void SetPointLights(unsigned int NumLights, const PointLight* pLights);

        bool ReadShaderFile(const char* fileName, std::string& fileString);

    private:
        GLuint m_WVPLocation;
        GLuint m_WorldMatrixLocation;
        GLuint m_samplerLocation;

        GLuint m_eyeWorldPosLocation;
        GLuint m_matSpecularIntensityLocation;
        GLuint m_matSpecularPowerLocation;
        GLuint m_numPointLightsLocation;

        struct {
            GLuint Color;
            GLuint AmbientIntensity;
            GLuint Direction;
            GLuint DiffuseIntensity;
        } m_dirLightLocation;

        struct {
            GLuint Color;
            GLuint AmbientIntensity;
            GLuint DiffuseIntensity;
            GLuint Position;
            struct{
                GLuint Constant;
                GLuint Linear;
                GLuint Exp;
            } Atten;
        } m_pointLightsLocation[MAX_POINT_LIGHTS];
};