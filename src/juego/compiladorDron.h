#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include "juegoGranja.h"

// --- NUESTROS TOKENS PARA EL JUEGO ---
enum TipoToken {
    TOK_IDENTIFICADOR, // Palabras genéricas (variables)
    TOK_NUMERO,        // 1, 2, 3...
    
    // Palabras reservadas tipo Python
    TOK_WHILE,
    TOK_TRUE,
    TOK_FALSE,
    TOK_IF,
    TOK_ELSE,

    // Comandos del Dron
    TOK_AVANZAR,
    TOK_COSECHAR,
    TOK_REGAR,
    TOK_SEMBRAR,

    // Direcciones
    TOK_NORTE, TOK_SUR, TOK_ESTE, TOK_OESTE,

    // Simbolos
    TOK_PAR_IZQ,    // (
    TOK_PAR_DER,    // )
    TOK_DOSPUNTOS,  // :
    
    TOK_FIN_ARCHIVO,
    TOK_ERROR
};

// Estructura de cada pieza leída (Igual a tu clase 'Atributos' pero más ligera)
struct Token {
    TipoToken tipo;
    std::string lexema;
    int linea;
};

class LexerDron {
private:
    std::string codigo;
    int posicionActual;
    int lineaActual;

    char caracterActual() {
        if (posicionActual >= codigo.length()) return '\0';
        return codigo[posicionActual];
    }

    void avanzar() {
        posicionActual++;
    }

    void saltarEspacios() {
        while (caracterActual() == ' ' || caracterActual() == '\t' || caracterActual() == '\n' || caracterActual() == '\r') {
            if (caracterActual() == '\n') lineaActual++;
            avanzar();
        }
    }

    // Compara palabras con nuestras palabras clave (Como tu 'BuscarPClave')
    TipoToken identificarPalabraReservada(std::string lexema) {
        if (lexema == "while") return TOK_WHILE;
        if (lexema == "True") return TOK_TRUE;
        if (lexema == "avanzar") return TOK_AVANZAR;
        if (lexema == "cosechar") return TOK_COSECHAR;
        if (lexema == "regar") return TOK_REGAR;
        if (lexema == "sembrar") return TOK_SEMBRAR;
        if (lexema == "Norte") return TOK_NORTE;
        if (lexema == "Sur") return TOK_SUR;
        if (lexema == "Este") return TOK_ESTE;
        if (lexema == "Oeste") return TOK_OESTE;
        return TOK_IDENTIFICADOR; // Si no es reservada, es una variable del jugador
    }

public:
    LexerDron(std::string _codigo) {
        codigo = _codigo;
        posicionActual = 0;
        lineaActual = 1;
    }

    // Evolución de tu 'getToken()'
    std::vector<Token> escanearTodo() {
        std::vector<Token> listaTokens;

        while (posicionActual < codigo.length()) {
            saltarEspacios();
            if (posicionActual >= codigo.length()) break;

            char c = caracterActual();

            // 1. Es un símbolo
            if (c == '(') { listaTokens.push_back({TOK_PAR_IZQ, "(", lineaActual}); avanzar(); continue; }
            if (c == ')') { listaTokens.push_back({TOK_PAR_DER, ")", lineaActual}); avanzar(); continue; }
            if (c == ':') { listaTokens.push_back({TOK_DOSPUNTOS, ":", lineaActual}); avanzar(); continue; }

            // 2. Es una palabra (Comando, variable o reservada)
            if (isalpha(c)) {
                std::string lexema = "";
                while (isalpha(caracterActual()) || isdigit(caracterActual()) || caracterActual() == '_') {
                    lexema += caracterActual();
                    avanzar();
                }
                TipoToken tipo = identificarPalabraReservada(lexema);
                listaTokens.push_back({tipo, lexema, lineaActual});
                continue;
            }

            // 3. Es un número
            if (isdigit(c)) {
                std::string lexema = "";
                while (isdigit(caracterActual())) {
                    lexema += caracterActual();
                    avanzar();
                }
                listaTokens.push_back({TOK_NUMERO, lexema, lineaActual});
                continue;
            }

            // Si llegamos aquí, escribió un símbolo que no reconocemos (ej. @, $)
            std::cout << "[ERROR LEXICO] Simbolo no reconocido en linea " << lineaActual << ": " << c << "\n";
            listaTokens.push_back({TOK_ERROR, std::string(1, c), lineaActual});
            avanzar();
        }

        listaTokens.push_back({TOK_FIN_ARCHIVO, "EOF", lineaActual});
        return listaTokens;
    }
};


// Estructura que devolverá el Parser al juego
struct ResultadoCompilacion {
    std::vector<InstruccionDron> rutina;
    bool esBucleInfinito = false; // Detectará el "while True:"
    bool exito = true;
};

class ParserDron {
private:
    std::vector<Token> tokens;
    int posicionActual;

    // Mira el token actual sin avanzar
    Token tokenActual() {
        if (posicionActual >= tokens.size()) return tokens.back(); // Retorna TOK_FIN_ARCHIVO
        return tokens[posicionActual];
    }

    // Avanza al siguiente token
    void avanzar() {
        posicionActual++;
    }

    // Función "Mágica": Verifica si el token actual es el que esperamos. 
    // Si lo es, lo consume (avanza). Si no, devuelve falso.
    bool coincidir(TipoToken tipoEsperado) {
        if (tokenActual().tipo == tipoEsperado) {
            avanzar();
            return true;
        }
        return false;
    }

public:
    ParserDron(std::vector<Token> _tokens) {
        tokens = _tokens;
        posicionActual = 0;
    }

    ResultadoCompilacion parsear() {
        ResultadoCompilacion resultado;

        // Leemos hasta que se acaben los tokens
        while (tokenActual().tipo != TOK_FIN_ARCHIVO) {
            
            // 1. REGLA: Bucle Infinito -> "while True:"
            if (tokenActual().tipo == TOK_WHILE) {
                avanzar(); // Consumimos 'while'
                
                // Exigimos que siga un 'True' y luego ':'
                if (coincidir(TOK_TRUE) && coincidir(TOK_DOSPUNTOS)) {
                    resultado.esBucleInfinito = true;
                } else {
                    std::cout << "[ERROR SINTAXIS Linea " << tokenActual().linea << "] Se esperaba 'True:' despues de 'while'\n";
                    resultado.exito = false; return resultado;
                }
            }
            // 2. REGLA: Accion -> "avanzar()"
            else if (tokenActual().tipo == TOK_AVANZAR) {
                avanzar(); // Consumimos 'avanzar'
                
                if (coincidir(TOK_PAR_IZQ)) {
                    Direccion dirElegida;
                    
                    // Verificamos qué dirección escribió el usuario
                    if (tokenActual().tipo == TOK_NORTE) { dirElegida = NORTE; avanzar(); }
                    else if (tokenActual().tipo == TOK_SUR) { dirElegida = SUR; avanzar(); }
                    else if (tokenActual().tipo == TOK_ESTE) { dirElegida = ESTE; avanzar(); }
                    else if (tokenActual().tipo == TOK_OESTE) { dirElegida = OESTE; avanzar(); }
                    else {
                        std::cout << "[ERROR SINTAXIS Linea " << tokenActual().linea << "] Se esperaba Norte, Sur, Este u Oeste dentro de avanzar()\n";
                        resultado.exito = false; return resultado;
                    }

                    // Verificamos que cierre el paréntesis
                    if (coincidir(TOK_PAR_DER)) {
                        // ¡Guardamos la instrucción completa!
                        resultado.rutina.push_back({CMD_AVANZAR, dirElegida});
                    } else {
                        std::cout << "[ERROR SINTAXIS Linea " << tokenActual().linea << "] Falta ')' despues de la direccion\n";
                        resultado.exito = false; return resultado;
                    }
                } else {
                    std::cout << "[ERROR SINTAXIS Linea " << tokenActual().linea << "] Falta '(' despues de avanzar\n";
                    resultado.exito = false; return resultado;
                }
            }
            // 3. REGLA: Accion -> "cosechar()"
            else if (tokenActual().tipo == TOK_COSECHAR) {
                avanzar();
                if (coincidir(TOK_PAR_IZQ) && coincidir(TOK_PAR_DER)) {
                    resultado.rutina.push_back({CMD_COSECHAR});
                } else {
                    std::cout << "[ERROR SINTAXIS Linea " << tokenActual().linea << "] Falta '()'\n";
                    resultado.exito = false; return resultado;
                }
            }
            // 4. REGLA: Accion -> "regar()"
            else if (tokenActual().tipo == TOK_REGAR) {
                avanzar();
                if (coincidir(TOK_PAR_IZQ) && coincidir(TOK_PAR_DER)) {
                    resultado.rutina.push_back({CMD_REGAR});
                } else {
                    std::cout << "[ERROR SINTAXIS Linea " << tokenActual().linea << "] Falta '()'\n";
                    resultado.exito = false; return resultado;
                }
            }
            // 5. REGLA: Accion -> "sembrar()"
            else if (tokenActual().tipo == TOK_SEMBRAR) {
                avanzar();
                if (coincidir(TOK_PAR_IZQ) && coincidir(TOK_PAR_DER)) {
                    resultado.rutina.push_back({CMD_SEMBRAR});
                } else {
                    std::cout << "[ERROR SINTAXIS Linea " << tokenActual().linea << "] Falta '()'\n";
                    resultado.exito = false; return resultado;
                }
            }

            // Si el jugador escribió basura o algo que no hemos programado aún
            else {
                std::cout << "[ERROR SINTAXIS Linea " << tokenActual().linea << "] Instruccion no reconocida: '" << tokenActual().lexema << "'\n";
                resultado.exito = false; return resultado;
            }
        }

        return resultado;
    }
};