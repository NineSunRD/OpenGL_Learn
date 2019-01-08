#version 330

in vec2 TexCoord0;
in vec3 Normal0;

out vec4 FragColor;

struct DirectionalLight
{
    vec3 Color;
    float AmbientIntensity;
    float DiffuseIntensity;
    vec3 Direction;
};

uniform DirectionalLight gDirectionalLight;
uniform sampler2D gSampler;

void main()
{
    vec4 AmbientColor = vec4(gDirectionalLight.Color * gDirectionalLight.AmbientIntensity, 1.0f);
    // 环境光颜色参数的计算没有变化，我们计算并存储它然后用在下面最终的公式中。

    float DiffuseFactor = dot(normalize(Normal0), -gDirectionalLight.Direction);
    /*
    这是漫射光计算的核心。我们通过对光源向量和法线向量做点积计算他们之间夹角的余弦值。这里有三个注意点：
    1. 从顶点着色器传来的法向量在使用之前是经过单位化了的，因为经过插值之后法线向量的长度可能会变化不再是单位向量了；
    2.光源的方向被反过来了，因为本来光线垂直照射到表上时的方向和法线向量实际是相反的成180度角，计算的时候将光源方向取反那么垂直入射时和法线夹角为0，这时才和我们的计算相符合。
    3.光源向量不是单位化的。如果对所有像素的同一个向量都进行反复单位化会很浪费GPU资源。因此我们只要保证应用程序传递的向量在draw call之前被单位化即可。
    */

    vec4 DiffuseColor;
    if (DiffuseFactor > 0) {
        DiffuseColor = vec4(gDirectionalLight.Color * gDirectionalLight.DiffuseIntensity * DiffuseFactor, 1.0f);
    }
    else {
        DiffuseColor = vec4(0, 0, 0, 0);
    }

    // 这里我们根据光的颜色、漫射光强度和光的方向来计算漫射光的部分。如果漫射参数是负的或者为0意味着光线是以一个钝角射到物体表面的（从水平一侧或者表面的背面）,这时候光照是没有效果的同时漫射光的颜色参数会被初始化设置为零。如果夹角大于0我们就可以进行计算漫射光的颜色值了，将基本的颜色值和漫射光强度常量相乘，最后使用漫射参数DiffuseFactor对最后结果进行缩放。如果光是垂直入射那么漫射参数会是1，光的亮度最大。

    FragColor = texture2D(gSampler, TexCoord0.xy) * (AmbientColor + DiffuseColor);
}