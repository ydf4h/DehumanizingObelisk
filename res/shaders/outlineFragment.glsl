#version 330 core

out vec4 fragmentColor;

uniform vec3 outlineColor;

void main(){
    fragmentColor = vec4(outlineColor, 1.0f);
}