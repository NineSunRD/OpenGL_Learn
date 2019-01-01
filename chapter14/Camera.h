#pragma once
#include "math_3d.h"

class Camera
{
public:
    Camera();
    Camera(const Vector3f& Pos, const Vector3f& Target, const Vector3f& Up);
    bool OnKeyboard(int Key);
    const Vector3f& GetPos() const { return m_pos;};
    const Vector3f& GetTarget() const { return m_target;};
    const Vector3f& GetUp() const { return m_up;};

private:
    Vector3f m_pos;
    Vector3f m_target;
    Vector3f m_up;
};