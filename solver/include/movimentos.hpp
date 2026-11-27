#ifndef MOVIMENTOS_HPP
#define MOVIMENTOS_HPP

#include "cubo.hpp"

#include <array>
#include <cstdint>
#include <string>
#include <vector>

constexpr int NUM_FACES = 6;
constexpr int NUM_MOVIMENTOS = 18;

// Numeração canônica dos 18 movimentos conforme CONTRATO.md:
// face = m / 3 (0=U, 1=R, 2=F, 3=D, 4=L, 5=B)
// tipo = m % 3 (0=90° horário, 1=180°, 2=90° anti-horário)
enum Movimento : uint8_t {
    MOV_U = 0,
    MOV_U2 = 1,
    MOV_U_LINHA = 2,
    MOV_R = 3,
    MOV_R2 = 4,
    MOV_R_LINHA = 5,
    MOV_F = 6,
    MOV_F2 = 7,
    MOV_F_LINHA = 8,
    MOV_D = 9,
    MOV_D2 = 10,
    MOV_D_LINHA = 11,
    MOV_L = 12,
    MOV_L2 = 13,
    MOV_L_LINHA = 14,
    MOV_B = 15,
    MOV_B2 = 16,
    MOV_B_LINHA = 17
};

// Semântica de tabela ("pull"):
//   novo.cp[i] = atual.cp[tabela.cp[i]]
//   novo.co[i] = (atual.co[tabela.cp[i]] + tabela.co[i]) % 3
//   novo.ep[i] = atual.ep[tabela.ep[i]]
//   novo.eo[i] = (atual.eo[tabela.ep[i]] + tabela.eo[i]) % 2
struct TabelaMovimento {
    std::array<uint8_t, NUM_QUINAS> cp;
    std::array<uint8_t, NUM_QUINAS> co;
    std::array<uint8_t, NUM_ARESTAS> ep;
    std::array<uint8_t, NUM_ARESTAS> eo;
};

const TabelaMovimento& tabelaMovimento(int movimento);

Cubo aplicarMovimento(const Cubo& cubo, int movimento);
void aplicarMovimentoEm(Cubo& cubo, int movimento);
Cubo aplicarSequencia(const Cubo& cubo, const std::vector<int>& movimentos);

int movimentoInverso(int movimento);
int faceDoMovimento(int movimento);

int textoParaMovimento(const std::string& token);
std::string movimentoParaTexto(int movimento);
bool parsearSequencia(const std::string& texto, std::vector<int>& movimentos, std::string& erro);
std::string sequenciaParaTexto(const std::vector<int>& movimentos);

#endif  // MOVIMENTOS_HPP
