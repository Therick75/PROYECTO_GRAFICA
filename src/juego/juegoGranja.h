#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector> // Necesario para std::vector

class JuegoGranja {
public:
    // --- INVENTARIO ---
    int semillasPapa;
    int semillasQuinua;
    int semillasHabas;
    float aguaLitros;
    float dineroSoles;

    // --- AGRICULTOR ---
    glm::vec3 posicionAgricultor;
    float escalaAgricultor; 

    // --- TERRENO ---
    int filasTerreno;
    int columnasTerreno;
    std::vector<glm::vec3> posicionesTerreno;

    // --- CONSTRUCTOR ---
    JuegoGranja() {
        // Inicializar inventario según tus reglas
        semillasPapa = 1;
        semillasQuinua = 0;
        semillasHabas = 0;
        aguaLitros = 10.0f;
        dineroSoles = 0;

        // Inicializar Agricultor (lo ponemos en el centro, un poco elevado para que esté sobre la tierra)
        posicionAgricultor = glm::vec3(0.0f, 0.75f, 0.0f); 
        escalaAgricultor = 0.5f; // Será la mitad del tamaño de un bloque de tierra

        // Inicializar Terreno (4x4)
        filasTerreno = 4;
        columnasTerreno = 4;
        generarTerreno();
    }

private:
    // Función interna para calcular la cuadrícula
    void generarTerreno() {
        for (int z = 0; z < filasTerreno; z++) {
            for (int x = 0; x < columnasTerreno; x++) {
                float posX = x - 1.5f;
                float posY = 0.0f; 
                float posZ = z - 1.5f;
                posicionesTerreno.push_back(glm::vec3(posX, posY, posZ));
            }
        }
    }
};