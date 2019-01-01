#version 330  //告诉编译器我们的目标GLSL编译器版本是3.3

layout (location = 0) in vec3 Position; // 绑定定点属性名和属性，方式二缓冲属性和shader属性对应映射
layout (location = 1) in vec2 TexCoord;

// 变换矩阵一致变量
uniform mat4 gWVP;

out vec2 TexCoord0;

void main()
{
    // 用变换矩阵乘以图形顶点位置对应的4X4矩阵相乘，完成变换
    gl_Position = gWVP * vec4(Position, 1.0);
    // 颜色插值
    TexCoord0 = TexCoord;
    //Color = vec4(Position, 1.0);
}