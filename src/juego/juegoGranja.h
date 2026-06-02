#pragma once
#include <filesystem>

namespace fs = std::filesystem;
// --- ESTRUCTURAS DEL DRON ---
enum Direccion
{
    NORTE,
    ESTE,
    SUR,
    OESTE
};

enum ComandoDron {
    CMD_AVANZAR,
    CMD_COSECHAR,
    CMD_REGAR,
    CMD_SEMBRAR
};

struct InstruccionDron {
    ComandoDron comando;
    Direccion parametroDir; // Solo se usará si el comando es CMD_AVANZAR
};

struct DronAutomatizado
{
    bool activo = false;
    int x = 0; 
    int z = 0; 
    Direccion mirando = static_cast<Direccion>(NORTE);

    // --- NUEVAS VARIABLES VISUALES ---
    glm::vec3 posicion3D;
    //float escala = 0.4f;
    //float alturaFlote = 0.85f;

    std::vector<InstruccionDron> rutina;
    int pasoActual = 0;
    float tiempoUltimoPaso = 0.0f;
    bool repiteInfinito = false;

    std::string rutaScript = "script_dron.txt";
    std::filesystem::file_time_type fechaUltimaLectura;
};

enum EstadoBloqueTierra
{
    VACIO,
    SEMBRADO,
    CRECIENDO,
    LISTO,
    MARCHITO
};

enum TipoSemilla
{
    NINGUNO,
    PAPA,
    QUINUA,
    HABAS
};

struct BloqueTierra
{
    glm::vec3 posicion;
    EstadoBloqueTierra estado;
    TipoSemilla cultivo; // Para saber qué tipo de planta está creciendo
    float diasPlantado;  // Controla la edad de la planta
    bool necesitaAgua;   // Controla si pide agua
};

#include "compiladorDron.h"

class JuegoGranja
{
public:
    // --- INVENTARIO ---
    int semillasPapa;
    int semillasQuinua;
    int semillasHabas;
    float aguaLitros;
    int dineroSoles;
    // --- CONTROL DEL TIEMPO
    float diasGlobales;
    int diaActualEntero;
    bool juegoPausado;

    // --- AGRICULTOR ---
    int gridX; // 
    int gridZ; // 
    glm::vec3 posicionAgricultor;
    float escalaAgricultor;

    // --- DRON VISUALES ---
    glm::vec3 posicionDron;
    float escalaDron;

    // --- TERRENO ---
    int tamanioMundo;
    std::vector<BloqueTierra> terreno;
    DronAutomatizado miDron;

    #pragma region JuegoGranja
    JuegoGranja()
    {
        // Inventario inicial
        semillasPapa = 10;
        semillasQuinua = 0;
        semillasHabas = 0;
        aguaLitros = 10.0f;
        dineroSoles = 400;

        // Tiempo global del juego
        diasGlobales = 0.0f;
        diaActualEntero = 0;

        // Inicia en la coordenada lógica (1, 1) del tablero
        gridX = 0;
        gridZ = 0;
        escalaAgricultor = 0.5f;
        escalaDron = 0.3f;
        tamanioMundo = 4;
        
        juegoPausado = false;

        // Inicializamos el terreno con bloques vacíos
        actualizarPosicionAgricultor();
        generarTerreno();
        mostrarRecursos();
    }
    #pragma endregion

    #pragma region actualizarPosiciones
    // actualiza la posicion del agricultor en el mundo 3D basado en su coordenada lógica (gridX, gridZ)
    void actualizarPosicionAgricultor()
    {
        float offset = (tamanioMundo - 1) / 2.0f;
        posicionAgricultor = glm::vec3(gridX - offset, 0.75f, gridZ - offset);
    }
    
    void actualizarPosicionDron()
    {
        // Usamos la misma fórmula dinámica para que se adapte si el mapa crece
        float offset = (tamanioMundo - 1) / 2.0f;
        posicionDron = glm::vec3(miDron.x - offset, 0.85f, miDron.z - offset);
    }
    #pragma endregion

    #pragma region Movimientos
    // movimientos del agricultor
    void moverArriba()
    {
        if (juegoPausado) return;
        gridZ = (gridZ - 1 + tamanioMundo) % tamanioMundo;
        actualizarPosicionAgricultor();
    }
    void moverAbajo()
    {
        if (juegoPausado) return;
        gridZ = (gridZ + 1) % tamanioMundo;
        actualizarPosicionAgricultor();
    }
    void moverIzquierda()
    {
        if (juegoPausado) return;
        gridX = (gridX - 1 + tamanioMundo) % tamanioMundo;
        actualizarPosicionAgricultor();
    }
    void moverDerecha()
    {
        if (juegoPausado) return;
        gridX = (gridX + 1) % tamanioMundo;
        actualizarPosicionAgricultor();
    }
#pragma endregion

    #pragma region sembrar
    // Función para sembrar en la posición actual
    void sembrarPapa()
    {
        if (juegoPausado) return;
        // Calculamos el índice del arreglo 1D a partir de 2D: (fila * total_columnas) + columna
        int indice = (gridZ * tamanioMundo) + gridX;

        if (terreno[indice].estado == VACIO && semillasPapa > 0)
        {
            terreno[indice].estado = SEMBRADO;
            terreno[indice].cultivo = PAPA;
            semillasPapa--;
            std::cout << "Papa sembrada. Semillas restantes: " << semillasPapa << "\n";
            mostrarRecursos();
        }
        else if (terreno[indice].estado != VACIO)
        {
            std::cout << "La tierra ya esta ocupada.\n";
        }
        else
        {
            std::cout << "No tienes semillas de papa.\n";
        }
    }

    // --- ACCIONES FÍSICAS EXCLUSIVAS DEL DRON ---
    void accionDronSembrar() {
        int indice = (miDron.z * tamanioMundo) + miDron.x;
        if (terreno[indice].estado == VACIO && semillasPapa > 0) {
            terreno[indice].estado = SEMBRADO;
            terreno[indice].cultivo = PAPA;
            semillasPapa--;
            std::cout << "[DRON] Papa sembrada en (" << miDron.x << "," << miDron.z << ").\n";
        }
    }

    #pragma endregion

    #pragma region regar
    void regarTierra()
    {
        if (juegoPausado) return;
        int indice = (gridZ * tamanioMundo) + gridX;
        BloqueTierra &bloque = terreno[indice];

        if (bloque.necesitaAgua)
        {
            if (bloque.estado == CRECIENDO)
            {
                if (aguaLitros >= 1.0f)
                {
                    aguaLitros -= 1.0f;
                    bloque.necesitaAgua = false;
                    std::cout << "Planta regada (Consumio 1L).\n";
                    mostrarRecursos();
                }
                else
                {
                    std::cout << "Falta agua. Necesitas 1L.\n";
                }
            }
            else if (bloque.estado == MARCHITO)
            {
                if (aguaLitros >= 2.0f)
                {
                    aguaLitros -= 2.0f;
                    bloque.necesitaAgua = false;
                    bloque.estado = CRECIENDO; // Se recupera de estar marchita
                    std::cout << "Planta recuperada (Consumio 2L).\n";
                    mostrarRecursos();
                }
                else
                {
                    std::cout << "Falta agua. Necesitas 2L para recuperar esta planta.\n";
                }
            }
        }
        else
        {
            std::cout << "Esta tierra no necesita agua en este momento.\n";
        }
    }

    void accionDronRegar() {
        int indice = (miDron.z * tamanioMundo) + miDron.x;
        BloqueTierra &bloque = terreno[indice];
        if (bloque.necesitaAgua && aguaLitros >= 1.0f) {
            aguaLitros -= 1.0f;
            bloque.necesitaAgua = false;
            if (bloque.estado == MARCHITO) { bloque.estado = CRECIENDO; aguaLitros -= 1.0f; } // Cobra doble si está marchito
            std::cout << "[DRON] Tierra regada en (" << miDron.x << "," << miDron.z << ").\n";
        }
    }

    #pragma endregion

    #pragma region cosechar
    // --- NUEVA FUNCIÓN PARA COSECHAR ---
    void cosechar()
    {
        int indice = (gridZ * tamanioMundo) + gridX;
        BloqueTierra &bloque = terreno[indice];

        if (bloque.estado == LISTO)
        {
            int ganancia = 0;
            std::string nombreCultivo = "";

            // Calculamos la ganancia según el tipo de semilla que tenía la tierra
            if (bloque.cultivo == PAPA)
            {
                ganancia = 10;
                nombreCultivo = "Papa";
            }
            else if (bloque.cultivo == HABAS)
            {
                ganancia = 8;
                nombreCultivo = "Habas";
            }
            else if (bloque.cultivo == QUINUA)
            {
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
        }
        else if (bloque.estado == VACIO)
        {
            std::cout << "No hay nada sembrado aqui.\n";
        }
        else
        {
            std::cout << "La planta aun no esta lista para ser cosechada.\n";
        }
    }

    void accionDronCosechar() {
        int indice = (miDron.z * tamanioMundo) + miDron.x;
        BloqueTierra &bloque = terreno[indice];
        if (bloque.estado == LISTO) {
            dineroSoles += 10; // Suponemos que cosechó papa
            bloque.estado = VACIO;
            bloque.cultivo = NINGUNO;
            bloque.diasPlantado = 0.0f;
            bloque.necesitaAgua = false;
            std::cout << "[DRON] Cosecha recolectada en (" << miDron.x << "," << miDron.z << ").\n";
        }
    }

    #pragma endregion
    
    void mostrarRecursos()
    {
        std::cout << "\n--- ESTADO DE RECURSOS ---\n";
        std::cout << "Semillas de Papa:   " << semillasPapa << "\n";
        std::cout << "Semillas de Quinua: " << semillasQuinua << "\n";
        std::cout << "Semillas de Habas:  " << semillasHabas << "\n";
        std::cout << "Agua:               " << aguaLitros << " Litros\n";
        std::cout << "Dinero:             " << dineroSoles << " Soles\n";
        std::cout << "--------------------------\n";
    }

    #pragma region comprarRecursos
    // --- TIENDA ---
    void comprarSemillaPapa()
    {
        int costo = 5;

        if (dineroSoles >= costo)
        {
            dineroSoles -= costo;
            semillasPapa++;
            std::cout << "¡Compraste 1 semilla de papa por " << costo << " soles!\n";
            mostrarRecursos(); // Imprime el inventario actualizado
        }
        else
        {
            std::cout << "No tienes suficiente dinero. Una semilla de papa cuesta " << costo
                      << " soles (Tienes " << dineroSoles << ").\n";
        }
    }

    void comprarAgua()
    {
        int costo = 5;
        float litros = 5.0f;

        if (dineroSoles >= costo)
        {
            dineroSoles -= costo;
            aguaLitros += litros;
            std::cout << "¡Compraste " << litros << " litros de agua por " << costo << " soles!\n";
            mostrarRecursos(); // Imprime el inventario actualizado
        }
        else
        {
            std::cout << "No tienes suficiente dinero. " << litros << "L de agua cuestan " << costo
                      << " soles (Tienes " << dineroSoles << ").\n";
        }
    }

    void comprarDron()
    {
        //int costo = 1000;
        int costo = 10; // Precio reducido para pruebas
        if (dineroSoles >= costo && !miDron.activo)
        {
            dineroSoles -= costo;
            miDron.activo = true;
            miDron.x = 0;
            miDron.z = 0;
            actualizarPosicionDron();

            // Creamos el archivo vacío al comprar el dron para que el jugador lo edite
            if (!std::filesystem::exists(miDron.rutaScript))
            {
                std::ofstream nuevoArchivo(miDron.rutaScript);
                //nuevoArchivo << "# Escribe tu codigo de automatizacion aqui:\n";
                nuevoArchivo.close();
            }
            miDron.fechaUltimaLectura = std::filesystem::last_write_time(miDron.rutaScript);

            juegoPausado = true;

            std::cout << "\n[SISTEMA] Dron adquirido. Archivo 'script_dron.txt' listo.\n";
            mostrarRecursos();
            std::system("code script_dron.txt");
        }
        else if (miDron.activo)
        {
            std::cout << "Ya tienes un dron.\n";
        }
        else
        {
            std::cout << "El dron cuesta " << costo << " soles (Tienes " << dineroSoles << ").\n";
        }
    }

    // --- COMPRAR EXPANSIÓN DEL MUNDO ---
    void expandirMundo()
    {
        // Costo dinámico: Depende del tamaño actual. 
        // Ej: Pasar de 4x4 a 5x5 cuesta 4 * 50 = 200 soles.
        int costoExpansion = tamanioMundo * 50;

        if (dineroSoles >= costoExpansion)
        {
            dineroSoles -= costoExpansion;

            int nuevaDimension = tamanioMundo + 1;
            std::vector<BloqueTierra> nuevoTerreno;
            float nuevoOffset = (nuevaDimension - 1) / 2.0f;

            // Reconstruimos el mundo con el nuevo tamaño
            for (int z = 0; z < nuevaDimension; z++)
            {
                for (int x = 0; x < nuevaDimension; x++)
                {
                    BloqueTierra bloque;
                    bloque.posicion = glm::vec3(x - nuevoOffset, 0.0f, z - nuevoOffset);

                    // Si la coordenada existía en el mapa viejo, copiamos su estado exacto
                    if (x < tamanioMundo && z < tamanioMundo)
                    {
                        int indiceViejo = (z * tamanioMundo) + x;
                        bloque.estado = terreno[indiceViejo].estado;
                        bloque.cultivo = terreno[indiceViejo].cultivo;
                        bloque.diasPlantado = terreno[indiceViejo].diasPlantado;
                        bloque.necesitaAgua = terreno[indiceViejo].necesitaAgua;
                    }
                    else
                    {
                        // Son los nuevos bloques de expansión (Bordes nuevos)
                        bloque.estado = VACIO;
                        bloque.cultivo = NINGUNO;
                        bloque.diasPlantado = 0.0f;
                        bloque.necesitaAgua = false;
                    }
                    nuevoTerreno.push_back(bloque);
                }
            }

            // Aplicamos los cambios al juego
            terreno = nuevoTerreno;
            tamanioMundo = nuevaDimension;

            // Recalculamos el centro visual de los personajes
            actualizarPosicionAgricultor();
            if (miDron.activo) actualizarPosicionDron();

            std::cout << "\n[SISTEMA] ¡EXPANSION EXITOSA! El mundo ahora es " << tamanioMundo << "x" << tamanioMundo << "\n";
            mostrarRecursos();
        }
        else
        {
            std::cout << "Necesitas " << costoExpansion << " soles para expandir la granja (Tienes " << dineroSoles << ").\n";
        }
    }
    #pragma endregion

    void procesarRutinaDron(float deltaTime) {
        if (!miDron.activo) return;
        // FUTURO: Aquí es donde el dron ejecutará paso a paso la lista de comandos
        // Si el juego está pausado, el dron no tiene memoria, o ya terminó... no hace nada
        if (juegoPausado || !miDron.activo || miDron.rutina.empty()) return;
        if (miDron.pasoActual >= miDron.rutina.size() && !miDron.repiteInfinito) return;

        // Sumamos el tiempo que ha pasado en la vida real
        miDron.tiempoUltimoPaso += deltaTime;

        // VELOCIDAD DEL DRON: Ejecuta 1 instrucción cada 1.0 segundos
        if (miDron.tiempoUltimoPaso >= 1.0f) {
            
            // Si llegó al final pero tiene "while True:", lo regresamos a la línea 0
            if (miDron.pasoActual >= miDron.rutina.size() && miDron.repiteInfinito) {
                miDron.pasoActual = 0;
            }

            // Sacamos la instrucción que toca leer hoy
            InstruccionDron instruccionActual = miDron.rutina[miDron.pasoActual];

            std::cout << "[DRON] Ejecutando paso " << miDron.pasoActual << "...\n";

            // --- EL CEREBRO FÍSICO DEL DRON ---
            switch (instruccionActual.comando) {
                case CMD_AVANZAR:
                    if (instruccionActual.parametroDir == NORTE) miDron.z--;
                    else if (instruccionActual.parametroDir == SUR) miDron.z++;
                    else if (instruccionActual.parametroDir == ESTE) miDron.x++;
                    else if (instruccionActual.parametroDir == OESTE) miDron.x--;
                    
                    // Aplicamos el efecto Pac-Man (Toroide) para que no se salga del mapa
                    miDron.z = (miDron.z + tamanioMundo) % tamanioMundo;
                    miDron.x = (miDron.x + tamanioMundo) % tamanioMundo;
                    
                    actualizarPosicionDron(); // Le avisamos a la tarjeta gráfica que se movió
                    break;

                case CMD_SEMBRAR:
                    accionDronSembrar(); // Llamamos a su función física (la crearemos abajo)
                    break;

                case CMD_REGAR:
                    accionDronRegar(); 
                    break;

                case CMD_COSECHAR:
                    accionDronCosechar(); 
                    break;
            }

            // Reiniciamos el cronómetro y avanzamos al siguiente paso en la memoria
            miDron.tiempoUltimoPaso = 0.0f;
            miDron.pasoActual++;
        }
    }

    #pragma region tiempo
    // Función que se llamará en cada ciclo del juego
    void pasarElTiempo(float deltaTime)
    {
        if (juegoPausado) return;
        // Convertimos los segundos reales a días del juego
        float diasPasados = deltaTime / 2.0f;
        diasGlobales += diasPasados;

        // Imprimir en consola cada vez que cambia el día entero
        if (static_cast<int>(diasGlobales) > diaActualEntero)
        {
            diaActualEntero = static_cast<int>(diasGlobales);
            std::cout << "\n=== DIA " << diaActualEntero << " ===\n";
        }

        for (int i = 0; i < terreno.size(); i++)
        {
            if (terreno[i].estado != VACIO)
            {
                terreno[i].diasPlantado += diasPasados;

                // Día 10: Empieza a pedir agua (estado CRECIENDO)
                if (terreno[i].estado == SEMBRADO && terreno[i].diasPlantado >= 10.0f)
                {
                    terreno[i].estado = CRECIENDO;
                    terreno[i].necesitaAgua = true;
                }

                // Día 15: Si no le echaste agua, se marchita
                if (terreno[i].estado == CRECIENDO && terreno[i].necesitaAgua && terreno[i].diasPlantado >= 15.0f)
                {
                    terreno[i].estado = MARCHITO;
                    std::cout << "Alerta: Una planta se ha marchitado.\n";
                }

                // Día 21: Si llegó marchita hasta aquí, muere y pierdes la ganancia
                if (terreno[i].estado == MARCHITO && terreno[i].diasPlantado >= 21.0f)
                {
                    terreno[i].estado = VACIO;
                    terreno[i].diasPlantado = 0.0f;
                    terreno[i].necesitaAgua = false;
                    terreno[i].cultivo = NINGUNO; // Limpiamos la semilla
                    std::cout << "Una planta murio y la tierra quedo vacia. Cero ganancias.\n";
                }

                // Día 20: Si fue regada a tiempo (!necesitaAgua) y llegó a su tiempo, está LISTA
                if (terreno[i].estado == CRECIENDO && !terreno[i].necesitaAgua && terreno[i].diasPlantado >= 20.0f)
                {
                    terreno[i].estado = LISTO;
                }
            }
        }
    }

    void alternarPausa()
    {
        juegoPausado = !juegoPausado;
        if (juegoPausado) {
            std::cout << "\n[================ JUEGO PAUSADO ================]\n";
        } else {
            std::cout << "\n[================ JUEGO REANUDADO ==============]\n";
        }
    }
    #pragma endregion

    void vigilarScriptDron()
    {
        if (!miDron.activo) return;

        // 1. Si el archivo no existe, lo creamos para que el jugador lo vea
        if (!fs::exists(miDron.rutaScript))
        {
            std::ofstream nuevoArchivo(miDron.rutaScript);
            nuevoArchivo << "while True:\n    avanzar()\n";
            nuevoArchivo.close();

            // Guardamos la fecha inicial
            miDron.fechaUltimaLectura = fs::last_write_time(miDron.rutaScript);
            std::cout << "[SISTEMA] Archivo 'script_dron.txt' creado en la carpeta del juego.\n";
            return;
        }

        // 2. Leemos la fecha de modificación actual del archivo en Windows
        auto fechaActual = fs::last_write_time(miDron.rutaScript);

        // 3. Si la fecha actual es más nueva que la que recordamos, ¡el jugador guardó cambios!
        if (fechaActual > miDron.fechaUltimaLectura)
        {
            miDron.fechaUltimaLectura = fechaActual; // Actualizamos la memoria

            std::cout << "\n============================================\n";
            std::cout << "[SISTEMA] ¡Cambio detectado (Ctrl+S)! Recargando código del dron...\n";

            juegoPausado = false;

            leerYCompilarScript();
        }
    }

    void leerYCompilarScript()
    {
        std::ifstream archivo(miDron.rutaScript);
        std::string linea;
        std::string codigoCompleto = "";

        // 1. Leer el archivo completo
        while (std::getline(archivo, linea)) {
            codigoCompleto += linea + "\n";
        }
        
        // Si el archivo está vacío, no hacemos nada
        if (codigoCompleto.empty()) return;

        std::cout << "--- COMPILANDO SCRIPT ---\n";
        
        // 2. FASE 1: Lexer (Texto a Tokens)
        LexerDron lexer(codigoCompleto);
        std::vector<Token> tokens = lexer.escanearTodo();
        
        // 3. FASE 2: Parser (Tokens a Rutina)
        ParserDron parser(tokens);
        ResultadoCompilacion resultado = parser.parsear();

        // 4. Guardar en el Dron si no hubo errores
        if (resultado.exito) {
            miDron.rutina = resultado.rutina;
            miDron.pasoActual = 0; // Reiniciamos el puntero de instrucción
            
            // Un pequeño truco MVP para el while True: 
            // Guardamos si el bucle es infinito en una nueva variable booleana del Dron.
            // (Añade 'bool repiteInfinito = false;' a tu struct DronAutomatizado).
            miDron.repiteInfinito = resultado.esBucleInfinito;

            std::cout << "[EXITO] Codigo compilado correctamente. " << miDron.rutina.size() << " instrucciones cargadas.\n";
        } else {
            std::cout << "[FALLO] El dron no se movera hasta que corrijas el codigo.\n";
        }
        std::cout << "-------------------------\n";
    }

    
    

private:

    #pragma region generarTerreno
    void generarTerreno()
    {
        terreno.clear(); // Limpiamos por seguridad
        float offset = (tamanioMundo - 1) / 2.0f;

        for (int z = 0; z < tamanioMundo; z++)
        {
            for (int x = 0; x < tamanioMundo; x++)
            {
                BloqueTierra bloque;
                bloque.posicion = glm::vec3(x - offset, 0.0f, z - offset);
                bloque.estado = VACIO; // Todo inicia vacío
                bloque.diasPlantado = 0.0f;
                bloque.necesitaAgua = false;
                bloque.cultivo = NINGUNO;
                terreno.push_back(bloque);
            }
        }
    }
    #pragma endregion

};
