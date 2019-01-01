#version 330  //告诉编译器我们的目标GLSL编译器版本是3.3

in vec2 TexCoord0;

out vec4 FragColor;  // 片段着色器的输出颜色变量

uniform sampler2D gSampler;

// 着色器的唯一入口函数
void main()
{
    // 颜色值作为片段着色器fs的输出
    FragColor = texture2D(gSampler, TexCoord0.st);
}