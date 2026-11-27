#ifndef CUBO_HPP
#define CUBO_HPP

#include <array>
#include <cstdint>
#include <iostream>

constexpr int NUM_QUINAS = 8;
constexpr int NUM_ARESTAS = 12;

// Numeração das posições conforme CONTRATO.md (padrão Kociemba):
//
// Quinas (0..7) — ordem horária das faces começando por U ou D:
//   0: URF (+x, +y, +z)    4: DFR (+x, -y, +z)
//   1: UFL (-x, +y, +z)    5: DLF (-x, -y, +z)
//   2: ULB (-x, +y, -z)    6: DBL (-x, -y, -z)
//   3: UBR (+x, +y, -z)    7: DRB (+x, -y, -z)
//
// Arestas (0..11) — 1ª face é a referência de orientação (U/D ou F/B):
//   0: UR (+x, +y,  0)     4: DR (+x, -y,  0)     8: FR (+x,  0, +z)
//   1: UF ( 0, +y, +z)     5: DF ( 0, -y, +z)     9: FL (-x,  0, +z)
//   2: UL (-x, +y,  0)     6: DL (-x, -y,  0)    10: BL (-x,  0, -z)
//   3: UB ( 0, +y, -z)     7: DB ( 0, -y, -z)    11: BR (+x,  0, -z)

enum Quina : uint8_t {
    URF = 0,
    UFL = 1,
    ULB = 2,
    UBR = 3,
    DFR = 4,
    DLF = 5,
    DBL = 6,
    DRB = 7
};

enum Aresta : uint8_t {
    UR = 0,
    UF = 1,
    UL = 2,
    UB = 3,
    DR = 4,
    DF = 5,
    DL = 6,
    DB = 7,
    FR = 8,
    FL = 9,
    BL = 10,
    BR = 11
};

struct Cubo {
    std::array<uint8_t, NUM_QUINAS> cp;   // permutação das quinas: qual quina está na posição i
    std::array<uint8_t, NUM_QUINAS> co;   // orientação das quinas: 0, 1 ou 2
    std::array<uint8_t, NUM_ARESTAS> ep;  // permutação das arestas: qual aresta está na posição i
    std::array<uint8_t, NUM_ARESTAS> eo;  // orientação das arestas: 0 ou 1

    bool estaResolvido() const;
    bool operator==(const Cubo& outro) const;
    bool operator!=(const Cubo& outro) const;
};

Cubo cuboResolvido();
bool estadoValido(const Cubo& cubo);
void imprimirEstado(const Cubo& cubo, std::ostream& out = std::cout);

#endif  // CUBO_HPP
