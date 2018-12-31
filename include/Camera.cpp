#include "Camera.h"
#include <GL/freeglut.h>
#include "math_3d.h"

const static float StepScale = 0.1f;

Camera::Camera()
{
    m_pos    = Vector3f(0.0f, 0.0f, 0.0f);
    m_target = Vector3f(0.0f, 0.0f, 1.0f);
    m_up     = Vector3f(0.0f, 1.0f, 0.0f);
}

Camera::Camera(const Vector3f& Pos, const Vector3f& Target, const Vector3f& Up)
{
    m_pos    = Pos;
    m_target = Target;
    m_target.Normalize();
    m_up     = Up;
    m_up.Normalize();
}

bool Camera::OnKeyboard(int Key)
{
    bool Ret = false;

    switch (Key) {

    case GLUT_KEY_UP:
    {
        m_pos += (m_target * StepScale);
        Ret = true;
    }
    break;

    case GLUT_KEY_DOWN:
    {
        m_pos -= (m_target * StepScale);
        Ret = true;
    }
    break;

    case GLUT_KEY_LEFT:
    {
        Vector3f Left = m_target.Cross(m_up);
        Left.Normalize();
        Left *= StepScale;
        m_pos += Left;
        Ret = true;
    }
    break;

    case GLUT_KEY_RIGHT:
    {
        Vector3f Right = m_up.Cross(m_target);
        Right.Normalize();
        Right *= StepScale;
        m_pos += Right;
        Ret = true;
    }
    break;
    }

    return Ret;
}