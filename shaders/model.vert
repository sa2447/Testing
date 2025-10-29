version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject
{
    mat4    model;
    mat4    view;
    mat4    proj;
    vec4    color;
    vec4    camera;
    vec4    lightPos;
    vec4    lightColor;
} ubo;

out gl_PerVertex
{
    vec4 gl_Position;
};

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec4 colorMod;
layout(location = 3) out vec4 worldPosition;
layout(location = 4) out vec4 cameraPos;
layout(location = 5) out vec4 lightPos;
layout(location = 6) out vec4 lightColor;

void main()
{
    mat3 normalMatrix;
    mat4 mvp = ubo.proj * ubo.view * ubo.model;

    //positions
    gl_Position =  mvp * vec4(inPosition, 1.0);
    worldPosition = ubo.model * vec4(inPosition,1.0);

    //normals
    normalMatrix = transpose(inverse(mat3(ubo.model)));
    outNormal = normalize(normalMatrix*inNormal);

    //pass throughs
    colorMod = ubo.color;
    cameraPos = ubo.camera;
    fragTexCoord = inTexCoord;
    lightPos = ubo.lightPos;
    lightColor = ubo.lightColor;
}