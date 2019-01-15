#pragma once
#include "math_3d.h"

class Camera
{
public:
    Camera();
    Camera(int WindowWidth, int WindowHeight);
    Camera(int WindowWidth, int WindowHeight, const Vector3f& Pos, const Vector3f& Target, const Vector3f& Up);

    void Init();
    bool OnKeyboard(int Key);
    void OnMouse(int x, int y);
    const Vector3f& GetPos() const { return m_pos;};
    const Vector3f& GetTarget() const { return m_target;};
    const Vector3f& GetUp() const { return m_up;};

    void Update();
    void OnRender();

private:
    Vector3f m_pos;
    Vector3f m_target;
    Vector3f m_up;

    int m_windowWidth;
    int m_windowHeight;

    bool m_OnUpperEdge;
    bool m_OnLowerEdge;
    bool m_OnLeftEdge;
    bool m_OnRightEdge;

    Vector2i m_mousePos;

    float m_AngleH;
    float m_AngleV;
};