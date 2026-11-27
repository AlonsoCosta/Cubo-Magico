#include "cubo.hpp"

#include <array>

namespace {

constexpr std::array<const char*, NUM_QUINAS> NOMES_QUINAS = {
    "URF", "UFL", "ULB", "UBR", "DFR", "DLF", "DBL", "DRB"
};

constexpr std::array<const char*, NUM_ARESTAS> NOMES_ARESTAS = {
    "UR", "UF", "UL", "UB", "DR", "DF", "DL", "DB", "FR", "FL", "BL", "BR"
};

template <std::size_t N>
int paridadePermutacao(const std::array<uint8_t, N>& perm) {
    int inversoes = 0;
    for (std::size_t i = 0; i < N; ++i) {
        for (std::size_t j = i + 1; j < N; ++j) {
            if (perm[i] > perm[j]) {
                ++inversoes;
            }
        }
    }
    return inversoes % 2;
}

}  // namespace

Cubo cuboResolvido() {
    return Cubo{
        {0, 1, 2, 3, 4, 5, 6, 7},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    };
}

bool Cubo::estaResolvido() const {
    for (int i = 0; i < NUM_QUINAS; ++i) {
        if (cp[i] != static_cast<uint8_t>(i) || co[i] != 0) {
            return false;
        }
    }
    for (int i = 0; i < NUM_ARESTAS; ++i) {
        if (ep[i] != static_cast<uint8_t>(i) || eo[i] != 0) {
            return false;
        }
    }
    return true;
}

bool Cubo::operator==(const Cubo& outro) const {
    return cp == outro.cp && co == outro.co && ep == outro.ep && eo == outro.eo;
}

bool Cubo::operator!=(const Cubo& outro) const {
    return !(*this == outro);
}

bool estadoValido(const Cubo& cubo) {
    std::array<bool, NUM_QUINAS> quinaVista{};
    int somaCo = 0;
    for (int i = 0; i < NUM_QUINAS; ++i) {
        if (cubo.cp[i] >= NUM_QUINAS || quinaVista[cubo.cp[i]]) {
            return false;
        }
        if (cubo.co[i] >= 3) {
            return false;
        }
        quinaVista[cubo.cp[i]] = true;
        somaCo += cubo.co[i];
    }
    if (somaCo % 3 != 0) {
        return false;
    }

    std::array<bool, NUM_ARESTAS> arestaVista{};
    int somaEo = 0;
    for (int i = 0; i < NUM_ARESTAS; ++i) {
        if (cubo.ep[i] >= NUM_ARESTAS || arestaVista[cubo.ep[i]]) {
            return false;
        }
        if (cubo.eo[i] >= 2) {
            return false;
        }
        arestaVista[cubo.ep[i]] = true;
        somaEo += cubo.eo[i];
    }
    if (somaEo % 2 != 0) {
        return false;
    }

    return paridadePermutacao(cubo.cp) == paridadePermutacao(cubo.ep);
}

void imprimirEstado(const Cubo& cubo, std::ostream& out) {
    out << "Resolvido: " << (cubo.estaResolvido() ? "sim" : "nao") << "\n";

    out << "Quinas (pos: peca/ori):";
    for (int i = 0; i < NUM_QUINAS; ++i) {
        out << " " << NOMES_QUINAS[i] << "="
            << NOMES_QUINAS[cubo.cp[i]] << "/" << static_cast<int>(cubo.co[i]);
    }
    out << "\n";

    out << "Arestas (pos: peca/ori):";
    for (int i = 0; i < NUM_ARESTAS; ++i) {
        out << " " << NOMES_ARESTAS[i] << "="
            << NOMES_ARESTAS[cubo.ep[i]] << "/" << static_cast<int>(cubo.eo[i]);
    }
    out << "\n";
}
