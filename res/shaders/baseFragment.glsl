#version 330 core

out vec4 fragmentColor;

in vec3 Normal;
in vec3 fragPos;

in vec2 texCoords;

uniform vec3 viewPosition;

uniform int matShininess;

uniform vec3 lightPosition;
uniform vec3 lightDirection;
uniform vec3 lightAmbient;
uniform vec3 lightDiffuse;
uniform vec3 lightSpecular;

uniform sampler2D diffuseTex;

float near = 0.1f;
float far = 1000.0f; //near and far planes used for depth linearization

//uniform sampler2D texture_diffuse1;
//uniform sampler2D texture_specular1;

struct DirLight{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirLight dirlight1;

vec4 calcDirectional(DirLight light, vec3 normal, vec3 viewDir){
    //vec3 difftex = vec3(texture(texture_diffuse1, texCoords));
    //vec3 spectex = vec3(texture(texture_specular1, texCoords).r);

    vec4 diffuseTexture = texture(diffuseTex, texCoords);

    vec3 lightDir = normalize(-light.direction);

    float diff = max(dot(normal, lightDir), 0.0f);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 64);

    vec4 ambient = vec4(light.ambient, 1.0f);
    vec4 diffuse = vec4(light.diffuse * diff, 1.0f);
    vec4 specular = vec4(light.specular * spec, 1.0f);
    return (ambient + diffuse + specular) * diffuseTexture;
}

float linearizeDepth(float depth){
    float ndc = depth * 2.0f - 1.0f;
    float linearDepth = (2.0f * near * far) / (far + near - ndc * (far - near));
    return linearDepth;
} //linearize depth value of depth buffer for demonstration

void main(){
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPosition - fragPos);

    vec4 result = calcDirectional(dirlight1, norm, viewDir);

    //float thelindepth = linearizeDepth(gl_FragCoord.z) / far; //gl_FragCoord is a built in OpenGL variable

    fragmentColor = result;
}