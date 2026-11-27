#include "movimentos.hpp"

#include <sstream>
#include <stdexcept>

namespace {

// Apenas os 6 giros horários básicos (U, R, F, D, L, B) são escritos à mão.
// As variações X2 e X' são geradas automaticamente por composição.
constexpr std::array<TabelaMovimento, NUM_FACES> GIROS_BASICOS = {{
    // 0: U — Quinas: URF->UFL->ULB->UBR->URF | Arestas: UR->UF->UL->UB->UR
    {
        {3, 0, 1, 2, 4, 5, 6, 7},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {3, 0, 1, 2, 4, 5, 6, 7, 8, 9, 10, 11},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    },
    // 1: R — Quinas: URF->UBR->DRB->DFR->URF | Arestas: UR->BR->DR->FR->UR
    {
        {4, 1, 2, 0, 7, 5, 6, 3},
        {2, 0, 0, 1, 1, 0, 0, 2},
        {8, 1, 2, 3, 11, 5, 6, 7, 4, 9, 10, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    },
    // 2: F — Quinas: URF->DFR->DLF->UFL->URF | Arestas: UF->FR->DF->FL->UF
    {
        {1, 5, 2, 3, 0, 4, 6, 7},
        {1, 2, 0, 0, 2, 1, 0, 0},
        {0, 9, 2, 3, 4, 8, 6, 7, 1, 5, 10, 11},
        {0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0}
    },
    // 3: D — Quinas: DFR->DRB->DBL->DLF->DFR | Arestas: DF->DR->DB->DL->DF
    {
        {0, 1, 2, 3, 5, 6, 7, 4},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 2, 3, 5, 6, 7, 4, 8, 9, 10, 11},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    },
    // 4: L — Quinas: UFL->DLF->DBL->ULB->UFL | Arestas: UL->FL->DL->BL->UL
    {
        {0, 2, 6, 3, 4, 1, 5, 7},
        {0, 1, 2, 0, 0, 2, 1, 0},
        {0, 1, 10, 3, 4, 5, 9, 7, 8, 2, 6, 11},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    },
    // 5: B — Quinas: UBR->ULB->DBL->DRB->UBR | Arestas: UB->BL->DB->BR->UB
    {
        {0, 1, 3, 7, 4, 5, 2, 6},
        {0, 0, 1, 2, 0, 0, 2, 1},
        {0, 1, 2, 11, 4, 5, 6, 10, 8, 9, 3, 7},
        {0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1}
    }
}};

constexpr TabelaMovimento comporTabelas(const TabelaMovimento& primeiro,
                                        const TabelaMovimento& segundo) {
    TabelaMovimento resultado{};
    for (int i = 0; i < NUM_QUINAS; ++i) {
        const uint8_t origem = segundo.cp[i];
        resultado.cp[i] = primeiro.cp[origem];
        resultado.co[i] = static_cast<uint8_t>((primeiro.co[origem] + segundo.co[i]) % 3);
    }
    for (int i = 0; i < NUM_ARESTAS; ++i) {
        const uint8_t origem = segundo.ep[i];
        resultado.ep[i] = primeiro.ep[origem];
        resultado.eo[i] = static_cast<uint8_t>((primeiro.eo[origem] + segundo.eo[i]) & 1);
    }
    return resultado;
}

constexpr std::array<TabelaMovimento, NUM_MOVIMENTOS> gerarTodasTabelas() {
    std::array<TabelaMovimento, NUM_MOVIMENTOS> tabelas{};
    for (int face = 0; face < NUM_FACES; ++face) {
        const TabelaMovimento& m1 = GIROS_BASICOS[face];
        const TabelaMovimento m2 = comporTabelas(m1, m1);
        const TabelaMovimento m3 = comporTabelas(m2, m1);

        tabelas[face * 3 + 0] = m1;  // X
        tabelas[face * 3 + 1] = m2;  // X2
        tabelas[face * 3 + 2] = m3;  // X'
    }
    return tabelas;
}

constexpr std::array<TabelaMovimento, NUM_MOVIMENTOS> TABELAS_MOVIMENTOS = gerarTodasTabelas();

constexpr std::array<const char*, NUM_MOVIMENTOS> NOMES_MOVIMENTOS = {
    "U", "U2", "U'",
    "R", "R2", "R'",
    "F", "F2", "F'",
    "D", "D2", "D'",
    "L", "L2", "L'",
    "B", "B2", "B'"
};

}  // namespace

const TabelaMovimento& tabelaMovimento(int movimento) {
    if (movimento < 0 || movimento >= NUM_MOVIMENTOS) {
        throw std::out_of_range("Indice de movimento fora do intervalo [0, 17].");
    }
    return TABELAS_MOVIMENTOS[movimento];
}

Cubo aplicarMovimento(const Cubo& cubo, int movimento) {
    const TabelaMovimento& tab = tabelaMovimento(movimento);
    Cubo novo{};
    for (int i = 0; i < NUM_QUINAS; ++i) {
        const uint8_t origem = tab.cp[i];
        novo.cp[i] = cubo.cp[origem];
        uint8_t ori = static_cast<uint8_t>(cubo.co[origem] + tab.co[i]);
        if (ori >= 3) {
            ori = static_cast<uint8_t>(ori - 3);
        }
        novo.co[i] = ori;
    }
    for (int i = 0; i < NUM_ARESTAS; ++i) {
        const uint8_t origem = tab.ep[i];
        novo.ep[i] = cubo.ep[origem];
        novo.eo[i] = static_cast<uint8_t>(cubo.eo[origem] ^ tab.eo[i]);
    }
    return novo;
}

void aplicarMovimentoEm(Cubo& cubo, int movimento) {
    cubo = aplicarMovimento(cubo, movimento);
}

Cubo aplicarSequencia(const Cubo& cubo, const std::vector<int>& movimentos) {
    Cubo atual = cubo;
    for (int movimento : movimentos) {
        atual = aplicarMovimento(atual, movimento);
    }
    return atual;
}

int movimentoInverso(int movimento) {
    if (movimento < 0 || movimento >= NUM_MOVIMENTOS) {
        return -1;
    }
    const int face = movimento / 3;
    const int tipo = movimento % 3;
    return face * 3 + (2 - tipo);
}

int faceDoMovimento(int movimento) {
    if (movimento < 0 || movimento >= NUM_MOVIMENTOS) {
        return -1;
    }
    return movimento / 3;
}

int textoParaMovimento(const std::string& token) {
    for (int i = 0; i < NUM_MOVIMENTOS; ++i) {
        if (token == NOMES_MOVIMENTOS[i]) {
            return i;
        }
    }
    return -1;
}

std::string movimentoParaTexto(int movimento) {
    if (movimento < 0 || movimento >= NUM_MOVIMENTOS) {
        throw std::out_of_range("Indice de movimento fora do intervalo [0, 17].");
    }
    return NOMES_MOVIMENTOS[movimento];
}

bool parsearSequencia(const std::string& texto, std::vector<int>& movimentos, std::string& erro) {
    movimentos.clear();
    erro.clear();

    std::istringstream iss(texto);
    std::string token;
    while (iss >> token) {
        const int mov = textoParaMovimento(token);
        if (mov < 0) {
            erro = "movimento invalido \"" + token + "\".";
            movimentos.clear();
            return false;
        }
        movimentos.push_back(mov);
    }
    return true;
}

std::string sequenciaParaTexto(const std::vector<int>& movimentos) {
    std::string resultado;
    for (std::size_t i = 0; i < movimentos.size(); ++i) {
        if (i > 0) {
            resultado += ' ';
        }
        resultado += movimentoParaTexto(movimentos[i]);
    }
    return resultado;
}
