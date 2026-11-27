#include "cubo.hpp"
#include "movimentos.hpp"

#include <iostream>
#include <string>
#include <vector>

int main(int argc, char* argv[]) {
    std::string entrada;

    if (argc == 2) {
        entrada = argv[1];
    } else if (argc == 3 && std::string(argv[1]) == "--debug") {
        entrada = argv[2];
    } else {
        std::cerr << "Uso: " << argv[0] << " \"<embaralhamento>\"\n";
        return 1;
    }

    std::vector<int> movimentos;
    std::string erro;
    if (!parsearSequencia(entrada, movimentos, erro)) {
        std::cerr << "Erro: " << erro << "\n";
        return 1;
    }

    const Cubo cubo = aplicarSequencia(cuboResolvido(), movimentos);
    imprimirEstado(cubo);
    return 0;
}
