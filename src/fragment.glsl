#version 330 core
out vec4 FragColor;

// Esta variable recibirá el color exacto desde nuestro código en C++
uniform vec4 objectColor; 

void main() {
    FragColor = objectColor;
}