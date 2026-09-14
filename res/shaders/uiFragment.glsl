#version 330 core

out vec4 fragmentColor;

uniform vec3 currentColor;

void main(){
    fragmentColor = vec4(currentColor, 1.0f);
}