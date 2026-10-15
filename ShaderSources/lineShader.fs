#version 330

in float Fade;
out vec4 FragColor;

uniform vec3 TrailColor;

void main(){
    FragColor = vec4(TrailColor, Fade);
}
