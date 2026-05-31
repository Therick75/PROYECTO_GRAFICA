enum EstadoBloqueTierra {
    VACIO,
    SEMBRADO,
    CRECIENDO,
    LISTO
};

struct BloqueTierra {
    glm::vec3 posicion;
    EstadoBloqueTierra estado;
};

class JuegoGranja {
public:
    // --- INVENTARIO ---
    int semillasPapa;
    int semillasQuinua;
    int semillasHabas;
    float aguaLitros;
    int dineroSoles;

    // --- AGRICULTOR ---
    int gridX; // Posición en columnas (0 a 3)
    int gridZ; // Posición en filas (0 a 3)
    glm::vec3 posicionAgricultor;
    float escalaAgricultor; 

    // --- TERRENO ---
    int filasTerreno;
    int columnasTerreno;
    std::vector<BloqueTierra> terreno;

    JuegoGranja() {
        semillasPapa = 1;
        semillasQuinua = 0;
        semillasHabas = 0;
        aguaLitros = 10.0f;
        dineroSoles = 0;

        // Inicia en la coordenada lógica (1, 1) del tablero
        gridX = 1;
        gridZ = 1;
        escalaAgricultor = 0.5f;
        actualizarPosicionAgricultor();

        filasTerreno = 4;
        columnasTerreno = 4;
        generarTerreno();
    }

    // Traduce la posición lógica (0,1,2,3) a coordenadas 3D del mundo
    void actualizarPosicionAgricultor() {
        posicionAgricultor = glm::vec3(gridX - 1.5f, 0.75f, gridZ - 1.5f);
    }

    // Movimiento con límites (no salir del 4x4)
    void moverArriba() { if(gridZ > 0) { gridZ--; actualizarPosicionAgricultor(); } }
    void moverAbajo()  { if(gridZ < filasTerreno - 1) { gridZ++; actualizarPosicionAgricultor(); } }
    void moverIzquierda() { if(gridX > 0) { gridX--; actualizarPosicionAgricultor(); } }
    void moverDerecha()   { if(gridX < columnasTerreno - 1) { gridX++; actualizarPosicionAgricultor(); } }

    // Función para sembrar en la posición actual
    void sembrarPapa() {
        // Calculamos el índice del arreglo 1D a partir de 2D: (fila * total_columnas) + columna
        int indice = (gridZ * columnasTerreno) + gridX;
        
        if (terreno[indice].estado == VACIO && semillasPapa > 0) {
            terreno[indice].estado = SEMBRADO;
            semillasPapa--;
            std::cout << "Papa sembrada. Semillas restantes: " << semillasPapa << "\n";
        } else if (terreno[indice].estado != VACIO) {
            std::cout << "La tierra ya esta ocupada.\n";
        } else {
            std::cout << "No tienes semillas de papa.\n";
        }
    }

private:
    void generarTerreno() {
        for (int z = 0; z < filasTerreno; z++) {
            for (int x = 0; x < columnasTerreno; x++) {
                BloqueTierra bloque;
                bloque.posicion = glm::vec3(x - 1.5f, 0.0f, z - 1.5f);
                bloque.estado = VACIO; // Todo inicia vacío
                terreno.push_back(bloque);
            }
        }
    }
};