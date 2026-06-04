#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "juego/juegoGranja.h"
#include <filesystem>
namespace fs = std::filesystem;
using namespace std;


float deltaTime = 0.0f;
float lastFrame = 0.0f;

JuegoGranja juego;


void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Recuperamos el puntero a nuestro juego
    JuegoGranja* juego = static_cast<JuegoGranja*>(glfwGetWindowUserPointer(window));
    if (!juego) return;

    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_W) juego->moverArriba();
        if (key == GLFW_KEY_S) juego->moverAbajo();
        if (key == GLFW_KEY_A) juego->moverIzquierda();
        if (key == GLFW_KEY_D) juego->moverDerecha();
        if (key == GLFW_KEY_SPACE) juego->sembrarPapa();

        // Cambio a la tecla M
        if (key == GLFW_KEY_M) juego->sembrarPapa();
        // regar tierra con la tecla R
        if (key == GLFW_KEY_R) juego->regarTierra();
        // --- TECLA PARA COSECHAR ---
        if (key == GLFW_KEY_C) juego->cosechar();

        if (key == GLFW_KEY_1) juego->comprarSemillaPapa();

        if (key == GLFW_KEY_2) juego->comprarAgua();

        if (key == GLFW_KEY_3) juego->comprarDron();

        // --- TECLA PARA EXPANDIR ---
        if (key == GLFW_KEY_5) juego->expandirMundo();

        // --- TECLA PARA PAUSA ---
        if (key == GLFW_KEY_P) juego->alternarPausa();
    }
}



unsigned int crearProgramaShader(const char* rutaVertex, const char* rutaFragment) {
    std::ifstream vShaderFile(rutaVertex), fShaderFile(rutaFragment);
    std::stringstream vShaderStream, fShaderStream;
    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();
    std::string vCode = vShaderStream.str(), fCode = fShaderStream.str();
    const char* vShaderCode = vCode.c_str();
    const char* fShaderCode = fCode.c_str();
    
    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    
    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    
    unsigned int ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    return ID;
}

void drawCube(unsigned int modelLoc, unsigned int colorLoc, unsigned int useTextureLoc, const glm::vec3& position, const glm::vec3& scale, const glm::vec4& color) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, scale);
    glUniform1i(useTextureLoc, 0);
    glUniform4fv(colorLoc, 1, glm::value_ptr(color));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void drawAgricultor(unsigned int modelLoc, unsigned int colorLoc, unsigned int useTextureLoc, const glm::vec3& basePos, float escala) {
    const glm::vec4 bodyColor = glm::vec4(0.1f, 0.3f, 0.8f, 1.0f); // camisa azul
    const glm::vec4 headColor = glm::vec4(0.95f, 0.8f, 0.6f, 1.0f); // piel
    const glm::vec4 pantColor = glm::vec4(0.2f, 0.15f, 0.1f, 1.0f); // pantalón oscuro
    const glm::vec4 hairColor = glm::vec4(0.1f, 0.05f, 0.0f, 1.0f); // pelo oscuro

    float s = escala;
    glm::vec3 bodyScale = glm::vec3(0.4f * s, 0.65f * s, 0.25f * s);
    glm::vec3 headScale = glm::vec3(0.28f * s, 0.28f * s, 0.28f * s);
    glm::vec3 armScale = glm::vec3(0.12f * s, 0.55f * s, 0.12f * s);
    glm::vec3 legScale = glm::vec3(0.14f * s, 0.55f * s, 0.14f * s);
    glm::vec3 hatScale = glm::vec3(0.32f * s, 0.10f * s, 0.32f * s);

    drawCube(modelLoc, colorLoc, useTextureLoc, basePos + glm::vec3(0.0f, 0.4f * s, 0.0f), bodyScale, bodyColor);
    drawCube(modelLoc, colorLoc, useTextureLoc, basePos + glm::vec3(0.0f, 0.95f * s, 0.0f), headScale, headColor);
    drawCube(modelLoc, colorLoc, useTextureLoc, basePos + glm::vec3(-0.32f * s, 0.2f * s, 0.0f), armScale, bodyColor);
    drawCube(modelLoc, colorLoc, useTextureLoc, basePos + glm::vec3(0.32f * s, 0.2f * s, 0.0f), armScale, bodyColor);
    drawCube(modelLoc, colorLoc, useTextureLoc, basePos + glm::vec3(-0.12f * s, -0.5f * s, 0.0f), legScale, pantColor);
    drawCube(modelLoc, colorLoc, useTextureLoc, basePos + glm::vec3(0.12f * s, -0.5f * s, 0.0f), legScale, pantColor);
    drawCube(modelLoc, colorLoc, useTextureLoc, basePos + glm::vec3(0.0f, 1.18f * s, 0.0f), hatScale, hairColor);
}

void drawDron(unsigned int modelLoc, unsigned int colorLoc, unsigned int useTextureLoc, const glm::vec3& basePos, float escala) {
    const glm::vec4 bodyColor = glm::vec4(0.2f, 0.2f, 0.25f, 1.0f);
    const glm::vec4 armColor = glm::vec4(0.15f, 0.15f, 0.18f, 1.0f);
    const glm::vec4 rotorColor = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f);
    const glm::vec4 glassColor = glm::vec4(0.1f, 0.45f, 0.75f, 1.0f);
    const glm::vec4 legColor = glm::vec4(0.1f, 0.1f, 0.12f, 1.0f);

    float s = escala;
    glm::vec3 bodyScale = glm::vec3(0.45f * s, 0.14f * s, 0.45f * s);
    glm::vec3 glassScale = glm::vec3(0.25f * s, 0.12f * s, 0.18f * s);
    glm::vec3 armScaleH = glm::vec3(0.28f * s, 0.05f * s, 0.05f * s);
    glm::vec3 armScaleV = glm::vec3(0.05f * s, 0.05f * s, 0.28f * s);
    glm::vec3 rotorScale = glm::vec3(0.08f * s, 0.02f * s, 0.08f * s);
    glm::vec3 legScale = glm::vec3(0.05f * s, 0.35f * s, 0.05f * s);

    // Base central
    drawCube(modelLoc, colorLoc, useTextureLoc, basePos + glm::vec3(0.0f, 0.12f * s, 0.0f), bodyScale, bodyColor);
    // Cabina frontal
    drawCube(modelLoc, colorLoc, useTextureLoc, basePos + glm::vec3(0.0f, 0.16f * s, -0.22f * s), glassScale, glassColor);
    // Brazos/X
    drawCube(modelLoc, colorLoc, useTextureLoc, basePos + glm::vec3(0.27f * s, 0.16f * s, 0.0f), armScaleH, armColor);
    drawCube(modelLoc, colorLoc, useTextureLoc, basePos + glm::vec3(-0.27f * s, 0.16f * s, 0.0f), armScaleH, armColor);
    drawCube(modelLoc, colorLoc, useTextureLoc, basePos + glm::vec3(0.0f, 0.16f * s, 0.27f * s), armScaleV, armColor);
    drawCube(modelLoc, colorLoc, useTextureLoc, basePos + glm::vec3(0.0f, 0.16f * s, -0.27f * s), armScaleV, armColor);
    // Motores / rotores
    drawCube(modelLoc, colorLoc, useTextureLoc, basePos + glm::vec3(0.52f * s, 0.16f * s, 0.0f), rotorScale, rotorColor);
    drawCube(modelLoc, colorLoc, useTextureLoc, basePos + glm::vec3(-0.52f * s, 0.16f * s, 0.0f), rotorScale, rotorColor);
    drawCube(modelLoc, colorLoc, useTextureLoc, basePos + glm::vec3(0.0f, 0.16f * s, 0.52f * s), rotorScale, rotorColor);
    drawCube(modelLoc, colorLoc, useTextureLoc, basePos + glm::vec3(0.0f, 0.16f * s, -0.52f * s), rotorScale, rotorColor);
    // Patas de aterrizaje
    drawCube(modelLoc, colorLoc, useTextureLoc, basePos + glm::vec3(0.2f * s, -0.18f * s, 0.2f * s), legScale, legColor);
    drawCube(modelLoc, colorLoc, useTextureLoc, basePos + glm::vec3(-0.2f * s, -0.18f * s, 0.2f * s), legScale, legColor);
    drawCube(modelLoc, colorLoc, useTextureLoc, basePos + glm::vec3(0.2f * s, -0.18f * s, -0.2f * s), legScale, legColor);
    drawCube(modelLoc, colorLoc, useTextureLoc, basePos + glm::vec3(-0.2f * s, -0.18f * s, -0.2f * s), legScale, legColor);
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Terreno de Cultivo 4x4", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glEnable(GL_DEPTH_TEST);

    unsigned int shaderProgram = crearProgramaShader("src/vertex.glsl", "src/fragment.glsl");

    float vertices[] = {
        -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   0.0f, 1.0f
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int texturaVacio, texturaSembrado;
    glGenTextures(1, &texturaVacio);
    glBindTexture(GL_TEXTURE_2D, texturaVacio);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_set_flip_vertically_on_load(true);
    int w, h, ch;
    unsigned char *data = stbi_load("tierra_vacio.jpg", &w, &h, &ch, 0);
    if (data) {
        GLenum format = (ch == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(data);

    glGenTextures(1, &texturaSembrado);
    glBindTexture(GL_TEXTURE_2D, texturaSembrado);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    data = stbi_load("tierra_sembrado.jpg", &w, &h, &ch, 0);
    if (data) {
        GLenum format = (ch == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(data);

    // --- GENERAR POSICIONES DEL TERRENO 4x4 ---
    std::vector<glm::vec3> terrainPositions;
    const int filas = 4;
    const int columnas = 4;

    for (int z = 0; z < filas; z++) {
        for (int x = 0; x < columnas; x++) {
            // Calculamos X y Z para que el cuadrado 4x4 quede centrado en la pantalla
            // Al restar 1.5f, las coordenadas van de -1.5 a +1.5, dejando el centro en 0
            float posX = x - 1.5f;
            float posY = 0.0f; 
            float posZ = z - 1.5f;
            
            terrainPositions.push_back(glm::vec3(posX, posY, posZ));
        }
    }

    glUseProgram(shaderProgram);
    unsigned int textureLoc = glGetUniformLocation(shaderProgram, "texture1");
    unsigned int useTextureLoc = glGetUniformLocation(shaderProgram, "useTexture");
    unsigned int lightDirLoc = glGetUniformLocation(shaderProgram, "lightDir");
    unsigned int lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
    unsigned int ambientStrengthLoc = glGetUniformLocation(shaderProgram, "ambientStrength");
    glUniform1i(textureLoc, 0);
    glUniform3f(lightDirLoc, -0.2f, -1.0f, -0.3f);
    glUniform3f(lightColorLoc, 1.0f, 0.95f, 0.9f);
    glUniform1f(ambientStrengthLoc, 0.35f);

    // Vincular la instancia del juego a la ventana para los controles
    glfwSetWindowUserPointer(window, &juego);
    glfwSetKeyCallback(window, key_callback);

    glUseProgram(shaderProgram);

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        juego.pasarElTiempo(deltaTime);
        juego.vigilarScriptDron(); 
        
        // Ejecución de la rutina que ya tenías
        juego.procesarRutinaDron(deltaTime);
        processInput(window);
        
        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

        // Cámara estática, ligeramente más alta y lejos para ver todo el tablero
        glm::mat4 view = glm::lookAt(
            glm::vec3(0.0f, 6.0f, 8.0f),  
            glm::vec3(0.0f, 0.0f, 0.0f),  
            glm::vec3(0.0f, 1.0f, 0.0f)   
        );
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

        unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
        unsigned int projLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        
        // --- DIBUJAR LOS 16 BLOQUES DEL TERRENO ---
        // Obtener la ubicación de la variable 'objectColor' en la tarjeta gráfica
        unsigned int colorLoc = glGetUniformLocation(shaderProgram, "objectColor");

        // -----------------------------------------
        // 1. DIBUJAR LOS BLOQUES DEL TERRENO
        // -----------------------------------------
        for (unsigned int i = 0; i < juego.terreno.size(); i++) {
            glm::mat4 modelTerreno = glm::mat4(1.0f);
            modelTerreno = glm::translate(modelTerreno, juego.terreno[i].posicion);
            
            // Lógica de colores/texturas basada en el estado del terreno
            if (juego.terreno[i].estado == VACIO) {
                glBindTexture(GL_TEXTURE_2D, texturaVacio);
                glUniform1i(useTextureLoc, 1);
                glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 1.0f);
            } else if (juego.terreno[i].estado == SEMBRADO) {
                glBindTexture(GL_TEXTURE_2D, texturaSembrado);
                glUniform1i(useTextureLoc, 1);
                glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 1.0f);
            } else {
                glUniform1i(useTextureLoc, 0);
                if (juego.terreno[i].estado == CRECIENDO) {
                    if (juego.terreno[i].necesitaAgua) {
                        glUniform4f(colorLoc, 1.0f, 0.0f, 0.0f, 1.0f); // ROJO (Pide agua 1L)
                    } else {
                        glUniform4f(colorLoc, 0.0f, 0.7f, 0.0f, 1.0f); // VERDE (Sana)
                    }
                } else if (juego.terreno[i].estado == MARCHITO) {
                    glUniform4f(colorLoc, 0.5f, 0.0f, 0.0f, 1.0f); // ROJO OSCURO (Pide agua 2L)
                } else if (juego.terreno[i].estado == LISTO) {
                    glUniform4f(colorLoc, 0.8f, 0.8f, 0.0f, 1.0f); // AMARILLO (Lista para cosechar)
                } else {
                    glUniform4f(colorLoc, 0.54f, 0.27f, 0.07f, 1.0f); // Marrón tierra por defecto
                }
            }

            unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelTerreno));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // -----------------------------------------
        // 2. DIBUJAR EL AGRICULTOR Mejorado
        // -----------------------------------------
        unsigned int modelLocAgr = glGetUniformLocation(shaderProgram, "model");
        glUniform1i(useTextureLoc, 0);
        drawAgricultor(modelLocAgr, colorLoc, useTextureLoc, juego.posicionAgricultor, juego.escalaAgricultor);

        // -----------------------------------------
        // 3. DIBUJAR EL DRON AUTOMATIZADO (Azul Claro)
        // -----------------------------------------
        if (juego.miDron.activo) {
            unsigned int modelLocDron = glGetUniformLocation(shaderProgram, "model");
            glUniform1i(useTextureLoc, 0);
            drawDron(modelLocDron, colorLoc, useTextureLoc, juego.posicionDron, juego.escalaDron);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glDeleteVertexArrays(1, &VAO); 
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}