#include "pipeline.h"
#include "math_3d.h"

void Pipeline::InitScaleTransform(Matrix4f& m) const
{
    m.m[0][0] = m_scale.x; m.m[0][1] = 0.0f     ; m.m[0][2] = 0.0f     ; m.m[0][3] = 0.0f;
    m.m[1][0] = 0.0f     ; m.m[1][1] = m_scale.y; m.m[1][2] = 0.0f     ; m.m[1][3] = 0.0f;
    m.m[2][0] = 0.0f     ; m.m[2][1] = 0.0f     ; m.m[2][2] = m_scale.z; m.m[2][3] = 0.0f;
    m.m[3][0] = 0.0f     ; m.m[3][1] = 0.0f     ; m.m[3][2] = 0.0f     ; m.m[3][3] = 1.0f;
}

void Pipeline::InitRotateTransform(Matrix4f& m) const
{
    Matrix4f rx, ry, rz;

    const float x = ToRadian(m_rotateInfo.x);
    const float y = ToRadian(m_rotateInfo.y);
    const float z = ToRadian(m_rotateInfo.z);

    rx.m[0][0] = 1.0f; rx.m[0][1] = 0.0f   ; rx.m[0][2] = 0.0f    ; rx.m[0][3] = 0.0f;
    rx.m[1][0] = 0.0f; rx.m[1][1] = cosf(x); rx.m[1][2] = -sinf(x); rx.m[1][3] = 0.0f;
    rx.m[2][0] = 0.0f; rx.m[2][1] = sinf(x); rx.m[2][2] = cosf(x) ; rx.m[2][3] = 0.0f;
    rx.m[3][0] = 0.0f; rx.m[3][1] = 0.0f   ; rx.m[3][2] = 0.0f    ; rx.m[3][3] = 1.0f;

    ry.m[0][0] = cosf(y); ry.m[0][1] = 0.0f; ry.m[0][2] = -sinf(y); ry.m[0][3] = 0.0f;
    ry.m[1][0] = 0.0f   ; ry.m[1][1] = 1.0f; ry.m[1][2] = 0.0f    ; ry.m[1][3] = 0.0f;
    ry.m[2][0] = sinf(y); ry.m[2][1] = 0.0f; ry.m[2][2] = cosf(y) ; ry.m[2][3] = 0.0f;
    ry.m[3][0] = 0.0f   ; ry.m[3][1] = 0.0f; ry.m[3][2] = 0.0f    ; ry.m[3][3] = 1.0f;

    rz.m[0][0] = cosf(z); rz.m[0][1] = -sinf(z); rz.m[0][2] = 0.0f; rz.m[0][3] = 0.0f;
    rz.m[1][0] = sinf(z); rz.m[1][1] = cosf(z) ; rz.m[1][2] = 0.0f; rz.m[1][3] = 0.0f;
    rz.m[2][0] = 0.0f   ; rz.m[2][1] = 0.0f    ; rz.m[2][2] = 1.0f; rz.m[2][3] = 0.0f;
    rz.m[3][0] = 0.0f   ; rz.m[3][1] = 0.0f    ; rz.m[3][2] = 0.0f; rz.m[3][3] = 1.0f;

    m = rz * ry * rx;
}

void Pipeline::InitTranslationTransform(Matrix4f& m, float x, float y, float z) const
{
    m.m[0][0] = 1.0f; m.m[0][1] = 0.0f; m.m[0][2] = 0.0f; m.m[0][3] = x;
    m.m[1][0] = 0.0f; m.m[1][1] = 1.0f; m.m[1][2] = 0.0f; m.m[1][3] = y;
    m.m[2][0] = 0.0f; m.m[2][1] = 0.0f; m.m[2][2] = 1.0f; m.m[2][3] = z;
    m.m[3][0] = 0.0f; m.m[3][1] = 0.0f; m.m[3][2] = 0.0f; m.m[3][3] = 1.0f;
}

void Pipeline::InitPerspectiveProj(Matrix4f& m) const
{
    const float ar = m_persProj.Width / m_persProj.Height;
    const float zNear = m_persProj.zNear;
    const float zFar = m_persProj.zFar;
    const float zRange = zNear - zFar;
    const float tanHalfFOV = tanf(ToRadian(m_persProj.FOV / 2.0));

    m.m[0][0] = 1.0f / (tanHalfFOV * ar); 
    m.m[0][1] = 0.0f;
    m.m[0][2] = 0.0f;
    m.m[0][3] = 0.0f;

    m.m[1][0] = 0.0f;
    m.m[1][1] = 1.0f / tanHalfFOV; 
    m.m[1][2] = 0.0f; 
    m.m[1][3] = 0.0f;

    m.m[2][0] = 0.0f; 
    m.m[2][1] = 0.0f; 
    m.m[2][2] = (-zNear - zFar) / zRange; 
    m.m[2][3] = 2.0f * zFar * zNear / zRange;

    m.m[3][0] = 0.0f;
    m.m[3][1] = 0.0f; 
    m.m[3][2] = 1.0f; 
    m.m[3][3] = 0.0f;
}

void Pipeline::InitCameraTransform(Matrix4f& m) const
{
    Vector3f N = m_camera.Target;
    N.Normalize();
    Vector3f U = m_camera.Up;
    U.Normalize();
    U = U.Cross(N);
    Vector3f V = N.Cross(U);

    m.m[0][0] = U.x; m.m[0][1] = U.y; m.m[0][2] = U.z; m.m[0][3] = 0.0f;
    m.m[1][0] = V.x; m.m[1][1] = V.y; m.m[1][2] = V.z; m.m[1][3] = 0.0f;
    m.m[2][0] = N.x; m.m[2][1] = N.y; m.m[2][2] = N.z; m.m[2][3] = 0.0f;
    m.m[3][0] = 0.0f; m.m[3][1] = 0.0f; m.m[3][2] = 0.0f; m.m[3][3] = 1.0f;
}


const Matrix4f* Pipeline::GetTrans()
{
    Matrix4f ScaleTrans, RotateTrans, TranslationTrans, PersProjTrans, CameraTranslationTrans, CameraRotateTrans;

    InitScaleTransform(ScaleTrans);
    InitRotateTransform(RotateTrans);
    InitTranslationTransform(TranslationTrans, m_worldPos.x, m_worldPos.y, m_worldPos.z);
    InitTranslationTransform(CameraTranslationTrans, -m_camera.Pos.x, -m_camera.Pos.y, -m_camera.Pos.z);
    InitCameraTransform(CameraRotateTrans);
    InitPerspectiveProj(PersProjTrans);

    m_transformation = PersProjTrans  * CameraRotateTrans * CameraTranslationTrans * TranslationTrans * RotateTrans * ScaleTrans;
    return &m_transformation;
}