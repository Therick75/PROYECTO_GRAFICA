enum EstadoBloqueTierra {
    VACIO,
    SEMBRADO,
    CRECIENDO,
    LISTO,
    MARCHITO
};

enum TipoSemilla {
    NINGUNO,
    PAPA,
    QUINUA,
    HABAS
};

struct BloqueTierra {
    glm::vec3 posicion;
    EstadoBloqueTierra estado;
    TipoSemilla cultivo; // Para saber qué tipo de planta está creciendo
    float diasPlantado;  // Controla la edad de la planta
    bool necesitaAgua;   // Controla si pide agua
    
};

class JuegoGranja {
public:
    // --- INVENTARIO ---
    int semillasPapa;
    int semillasQuinua;
    int semillasHabas;
    float aguaLitros;
    int dineroSoles;
    float diasGlobales;
    int diaActualEntero;

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

        // Tiempo global del juego
        diasGlobales = 0.0f;
        diaActualEntero = 0;

        // Inicia en la coordenada lógica (1, 1) del tablero
        gridX = 1;
        gridZ = 1;
        escalaAgricultor = 0.5f;
        actualizarPosicionAgricultor();

        filasTerreno = 4;
        columnasTerreno = 4;
        generarTerreno();
        mostrarRecursos();
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
            terreno[indice].cultivo = PAPA;
            semillasPapa--;
            std::cout << "Papa sembrada. Semillas restantes: " << semillasPapa << "\n";
            mostrarRecursos(); 
        } else if (terreno[indice].estado != VACIO) {
            std::cout << "La tierra ya esta ocupada.\n";
        } else {
            std::cout << "No tienes semillas de papa.\n";
        }
    }

    void mostrarRecursos() {
        std::cout << "\n--- ESTADO DE RECURSOS ---\n";
        std::cout << "Semillas de Papa:   " << semillasPapa << "\n";
        std::cout << "Semillas de Quinua: " << semillasQuinua << "\n";
        std::cout << "Semillas de Habas:  " << semillasHabas << "\n";
        std::cout << "Agua:               " << aguaLitros << " Litros\n";
        std::cout << "Dinero:             " << dineroSoles << " Soles\n";
        std::cout << "--------------------------\n";
    }

    // --- TIENDA ---
    void comprarSemillaPapa() {
        int costo = 5;
        
        if (dineroSoles >= costo) {
            dineroSoles -= costo;
            semillasPapa++;
            std::cout << "¡Compraste 1 semilla de papa por " << costo << " soles!\n";
            mostrarRecursos(); // Imprime el inventario actualizado
        } else {
            std::cout << "No tienes suficiente dinero. Una semilla de papa cuesta " << costo 
                      << " soles (Tienes " << dineroSoles << ").\n";
        }
    }

    void comprarAgua() {
        int costo = 5;
        float litros = 5.0f;
        
        if (dineroSoles >= costo) {
            dineroSoles -= costo;
            aguaLitros += litros;
            std::cout << "¡Compraste " << litros << " litros de agua por " << costo << " soles!\n";
            mostrarRecursos(); // Imprime el inventario actualizado
        } else {
            std::cout << "No tienes suficiente dinero. " << litros << "L de agua cuestan " << costo 
                      << " soles (Tienes " << dineroSoles << ").\n";
        }
    }

void regarTierra() {
        int indice = (gridZ * columnasTerreno) + gridX;
        BloqueTierra& bloque = terreno[indice];

        if (bloque.necesitaAgua) {
            if (bloque.estado == CRECIENDO) {
                if (aguaLitros >= 1.0f) {
                    aguaLitros -= 1.0f;
                    bloque.necesitaAgua = false;
                    std::cout << "Planta regada (Consumio 1L).\n";
                    mostrarRecursos();
                } else {
                    std::cout << "Falta agua. Necesitas 1L.\n";
                }
            } else if (bloque.estado == MARCHITO) {
                if (aguaLitros >= 2.0f) {
                    aguaLitros -= 2.0f;
                    bloque.necesitaAgua = false;
                    bloque.estado = CRECIENDO; // Se recupera de estar marchita
                    std::cout << "Planta recuperada (Consumio 2L).\n";
                    mostrarRecursos();
                } else {
                    std::cout << "Falta agua. Necesitas 2L para recuperar esta planta.\n";
                }
            }
        } else {
            std::cout << "Esta tierra no necesita agua en este momento.\n";
        }
    }

    // Función que se llamará en cada ciclo del juego
    void pasarElTiempo(float deltaTime) {
        // Convertimos los segundos reales a días del juego
        float diasPasados = deltaTime / 2.0f; 
        diasGlobales += diasPasados;

        // Imprimir en consola cada vez que cambia el día entero
        if (static_cast<int>(diasGlobales) > diaActualEntero) {
            diaActualEntero = static_cast<int>(diasGlobales);
            std::cout << "\n=== DIA " << diaActualEntero << " ===\n";
        }

        for (int i = 0; i < terreno.size(); i++) {
            if (terreno[i].estado != VACIO) {
                terreno[i].diasPlantado += diasPasados;

                // Día 10: Empieza a pedir agua (estado CRECIENDO)
                if (terreno[i].estado == SEMBRADO && terreno[i].diasPlantado >= 10.0f) {
                    terreno[i].estado = CRECIENDO;
                    terreno[i].necesitaAgua = true;
                }

                // Día 15: Si no le echaste agua, se marchita
                if (terreno[i].estado == CRECIENDO && terreno[i].necesitaAgua && terreno[i].diasPlantado >= 15.0f) {
                    terreno[i].estado = MARCHITO;
                    std::cout << "Alerta: Una planta se ha marchitado.\n";
                }

                // Día 21: Si llegó marchita hasta aquí, muere y pierdes la ganancia
                if (terreno[i].estado == MARCHITO && terreno[i].diasPlantado >= 21.0f) {
                    terreno[i].estado = VACIO;
                    terreno[i].diasPlantado = 0.0f;
                    terreno[i].necesitaAgua = false;
                    terreno[i].cultivo = NINGUNO; // Limpiamos la semilla
                    std::cout << "Una planta murio y la tierra quedo vacia. Cero ganancias.\n";
                }
                
                // Día 20: Si fue regada a tiempo (!necesitaAgua) y llegó a su tiempo, está LISTA
                if (terreno[i].estado == CRECIENDO && !terreno[i].necesitaAgua && terreno[i].diasPlantado >= 20.0f) {
                    terreno[i].estado = LISTO;
                }
            }
        }
    }

    // --- NUEVA FUNCIÓN PARA COSECHAR ---
    void cosechar() {
        int indice = (gridZ * columnasTerreno) + gridX;
        BloqueTierra& bloque = terreno[indice];

        if (bloque.estado == LISTO) {
            int ganancia = 0;
            std::string nombreCultivo = "";

            // Calculamos la ganancia según el tipo de semilla que tenía la tierra
            if (bloque.cultivo == PAPA) {
                ganancia = 10;
                nombreCultivo = "Papa";
            } else if (bloque.cultivo == HABAS) {
                ganancia = 8;
                nombreCultivo = "Habas";
            } else if (bloque.cultivo == QUINUA) {
                ganancia = 9;
                nombreCultivo = "Quinua";
            }

            // Sumamos el dinero y limpiamos la tierra
            dineroSoles += ganancia;
            bloque.estado = VACIO;
            bloque.cultivo = NINGUNO;
            bloque.diasPlantado = 0.0f;
            bloque.necesitaAgua = false;

            std::cout << "¡Cosechaste " << nombreCultivo << " y ganaste " << ganancia << " soles!\n";
            mostrarRecursos();
        } else if (bloque.estado == VACIO) {
            std::cout << "No hay nada sembrado aqui.\n";
        } else {
            std::cout << "La planta aun no esta lista para ser cosechada.\n";
        }
    }

private:
    void generarTerreno() {
        for (int z = 0; z < filasTerreno; z++) {
            for (int x = 0; x < columnasTerreno; x++) {
                BloqueTierra bloque;
                bloque.posicion = glm::vec3(x - 1.5f, 0.0f, z - 1.5f);
                bloque.estado = VACIO; // Todo inicia vacío
                bloque.diasPlantado = 0.0f;
                bloque.necesitaAgua = false;
                bloque.cultivo = NINGUNO;
                terreno.push_back(bloque);
            }
        }
    }
};