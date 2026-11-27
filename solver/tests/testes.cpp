#include "cubo.hpp"
#include "movimentos.hpp"

#include <iostream>
#include <random>
#include <string>
#include <vector>

namespace {

int totalTestes = 0;
int totalFalhas = 0;

void verificar(bool condicao, const std::string& nome) {
    ++totalTestes;
    if (condicao) {
        std::cout << "[OK]    " << nome << "\n";
    } else {
        ++totalFalhas;
        std::cerr << "[FALHA] " << nome << "\n";
    }
}

void testarEstadoBasico() {
    const Cubo resolvido = cuboResolvido();
    verificar(resolvido.estaResolvido(), "Cubo resolvido -> estaResolvido() e verdadeiro");
    verificar(estadoValido(resolvido), "Cubo resolvido -> estadoValido() e verdadeiro");

    Cubo quinasTrocadas = resolvido;
    std::swap(quinasTrocadas.cp[0], quinasTrocadas.cp[1]);
    verificar(!quinasTrocadas.estaResolvido(), "Trocar duas quinas -> estaResolvido() e falso");
    verificar(!estadoValido(quinasTrocadas), "Trocar apenas duas quinas -> paridade invalida");

    Cubo quinaTorcida = resolvido;
    quinaTorcida.co[0] = 1;
    verificar(!quinaTorcida.estaResolvido(), "Torcer uma quina -> estaResolvido() e falso");
    verificar(!estadoValido(quinaTorcida), "Torcer apenas uma quina -> soma de orientacoes invalida");

    Cubo arestaVirada = resolvido;
    arestaVirada.eo[0] = 1;
    verificar(!arestaVirada.estaResolvido(), "Virar uma aresta -> estaResolvido() e falso");
    verificar(!estadoValido(arestaVirada), "Virar apenas uma aresta -> soma de orientacoes invalida");
}

void testarOrdemFaces() {
    const Cubo inicial = cuboResolvido();
    for (int face = 0; face < NUM_FACES; ++face) {
        const int movCw = face * 3 + 0;
        const int mov2 = face * 3 + 1;
        const int movCcw = face * 3 + 2;
        const std::string nome = movimentoParaTexto(movCw);

        Cubo c = inicial;
        bool resolveuAntes = false;
        for (int k = 1; k <= 4; ++k) {
            aplicarMovimentoEm(c, movCw);
            if (k < 4 && c.estaResolvido()) {
                resolveuAntes = true;
            }
        }
        verificar(c.estaResolvido() && !resolveuAntes,
                  "Face " + nome + " girada 4 vezes volta ao resolvido (e nao antes)");

        Cubo c2 = aplicarMovimento(aplicarMovimento(inicial, mov2), mov2);
        verificar(c2.estaResolvido() && !aplicarMovimento(inicial, mov2).estaResolvido(),
                  "Face " + nome + "2 aplicada 2 vezes volta ao resolvido");

        Cubo ccw = inicial;
        for (int k = 0; k < 4; ++k) {
            aplicarMovimentoEm(ccw, movCcw);
        }
        verificar(ccw.estaResolvido(),
                  "Face " + nome + "' girada 4 vezes volta ao resolvido");
    }
}

void testarInversos() {
    const Cubo inicial = cuboResolvido();
    for (int face = 0; face < NUM_FACES; ++face) {
        const int movCw = face * 3 + 0;
        const int mov2 = face * 3 + 1;
        const int movCcw = face * 3 + 2;
        const std::string nome = movimentoParaTexto(movCw);

        verificar(movimentoInverso(movCw) == movCcw &&
                      movimentoInverso(movCcw) == movCw &&
                      movimentoInverso(mov2) == mov2,
                  "movimentoInverso consistente para face " + nome);

        const Cubo idaVolta = aplicarMovimento(aplicarMovimento(inicial, movCw), movCcw);
        const Cubo voltaIda = aplicarMovimento(aplicarMovimento(inicial, movCcw), movCw);
        verificar(idaVolta.estaResolvido() && voltaIda.estaResolvido(),
                  nome + " seguido de " + nome + "' (e vice-versa) volta ao resolvido");
    }
}

void testarSequenciasConhecidas() {
    std::vector<int> seq;
    std::string erro;

    // Sexy move: (R U R' U')^6 = identidade
    verificar(parsearSequencia("R U R' U'", seq, erro), "Parsear sequencia \"R U R' U'\"");
    Cubo c = cuboResolvido();
    bool resolveuAntes = false;
    for (int k = 1; k <= 6; ++k) {
        c = aplicarSequencia(c, seq);
        if (k < 6 && c.estaResolvido()) {
            resolveuAntes = true;
        }
    }
    verificar(c.estaResolvido() && !resolveuAntes,
              "(R U R' U') repetido 6 vezes volta ao resolvido");

    // Ordem 105 para pares de faces adjacentes (valida todas as 6 faces e orientacoes)
    const std::vector<std::string> paresAdjacentes = {
        "R U", "F R", "L D", "B L", "U B", "D F"
    };
    for (const std::string& par : paresAdjacentes) {
        verificar(parsearSequencia(par, seq, erro), "Parsear sequencia \"" + par + "\"");
        Cubo estado = cuboResolvido();
        bool antesDe105 = false;
        for (int k = 1; k <= 105; ++k) {
            estado = aplicarSequencia(estado, seq);
            if (k < 105 && estado.estaResolvido()) {
                antesDe105 = true;
            }
        }
        verificar(estado.estaResolvido() && !antesDe105,
                  "(" + par + ") repetido 105 vezes volta ao resolvido");
    }
}

void testarMilMovimentosAleatorios() {
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist(0, NUM_MOVIMENTOS - 1);

    Cubo c = cuboResolvido();
    std::vector<int> historico;
    historico.reserve(1000);

    bool todosValidos = true;
    for (int i = 0; i < 1000; ++i) {
        const int mov = dist(rng);
        aplicarMovimentoEm(c, mov);
        historico.push_back(mov);
        if (!estadoValido(c)) {
            todosValidos = false;
            break;
        }
    }
    verificar(todosValidos,
              "Apos 1000 movimentos aleatorios, somas de orientacao, permutacoes e paridade permanecem validas");

    for (auto it = historico.rbegin(); it != historico.rend(); ++it) {
        aplicarMovimentoEm(c, movimentoInverso(*it));
    }
    verificar(c.estaResolvido(),
              "Desfazer os 1000 movimentos aleatorios na ordem inversa volta ao resolvido");
}

void testarNotacao() {
    bool roundtripOk = true;
    for (int m = 0; m < NUM_MOVIMENTOS; ++m) {
        const std::string texto = movimentoParaTexto(m);
        if (textoParaMovimento(texto) != m) {
            roundtripOk = false;
        }
    }
    verificar(roundtripOk, "Conversao numero <-> texto consistente para todos os 18 movimentos");

    std::vector<int> seq;
    std::string erro;

    verificar(parsearSequencia("", seq, erro) && seq.empty(),
              "Sequencia vazia e valida (0 movimentos)");

    verificar(parsearSequencia("R R'", seq, erro) &&
                  aplicarSequencia(cuboResolvido(), seq).estaResolvido(),
              "\"R R'\" aplicado ao resolvido permanece resolvido");

    verificar(parsearSequencia("R U F' L2 D' B2", seq, erro) &&
                  sequenciaParaTexto(seq) == "R U F' L2 D' B2",
              "Parsear e formatar \"R U F' L2 D' B2\" preserva a sequencia");

    const std::vector<std::string> invalidas = {"Q3", "r", "R3", "U''", "R Q3 U"};
    for (const std::string& inv : invalidas) {
        verificar(!parsearSequencia(inv, seq, erro) && !erro.empty(),
                  "Rejeita entrada invalida \"" + inv + "\"");
    }
}

}  // namespace

int main() {
    testarEstadoBasico();
    testarOrdemFaces();
    testarInversos();
    testarSequenciasConhecidas();
    testarMilMovimentosAleatorios();
    testarNotacao();

    std::cout << "\nResumo: " << (totalTestes - totalFalhas) << "/" << totalTestes
              << " testes passaram.\n";

    return totalFalhas == 0 ? 0 : 1;
}
