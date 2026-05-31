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
#include <vector> // Necesario para std::vector
#include "juego/juegoGranja.h"


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
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int textura;
    glGenTextures(1, &textura);
    glBindTexture(GL_TEXTURE_2D, textura);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    stbi_set_flip_vertically_on_load(true); 
    int w, h, ch;
    unsigned char *data = stbi_load("logo.png", &w, &h, &ch, 0);
    if (data) { 
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data); 
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
    // Vincular la instancia del juego a la ventana para los controles
    glfwSetWindowUserPointer(window, &juego);
    glfwSetKeyCallback(window, key_callback);

    glUseProgram(shaderProgram);

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
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
        
        // --- DIBUJAR LOS 16 BLOQUES DEL TERRENO ---
        // Obtener la ubicación de la variable 'objectColor' en la tarjeta gráfica
        unsigned int colorLoc = glGetUniformLocation(shaderProgram, "objectColor");

        // -----------------------------------------
        // 1. DIBUJAR LOS BLOQUES DEL TERRENO
        // -----------------------------------------
        for (unsigned int i = 0; i < juego.terreno.size(); i++) {
            glm::mat4 modelTerreno = glm::mat4(1.0f);
            modelTerreno = glm::translate(modelTerreno, juego.terreno[i].posicion);
            
            // Evaluamos el estado de la tierra para cambiar su color
            if (juego.terreno[i].estado == VACIO) {
                // Tierra seca / Normal (Marrón claro)
                glUniform4f(colorLoc, 0.54f, 0.27f, 0.07f, 1.0f); 
            } else if (juego.terreno[i].estado == SEMBRADO) {
                // Tierra húmeda / Sembrada (Marrón oscuro)
                glUniform4f(colorLoc, 0.35f, 0.16f, 0.04f, 1.0f); 
            }

            unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelTerreno));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // -----------------------------------------
        // 2. DIBUJAR EL AGRICULTOR (Negro)
        // -----------------------------------------
        glUniform4f(colorLoc, 0.0f, 0.0f, 0.0f, 1.0f);

        glm::mat4 modelAgricultor = glm::mat4(1.0f);
        modelAgricultor = glm::translate(modelAgricultor, juego.posicionAgricultor);
        modelAgricultor = glm::scale(modelAgricultor, glm::vec3(juego.escalaAgricultor));
        
        unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelAgricultor));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glDeleteVertexArrays(1, &VAO); 
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}