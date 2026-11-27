# Cubo Mágico 3×3×3 (IDA* + VPython) — Contrato C++ ↔ Python e Convenções do Cubo (#2)

Este documento define o contrato único entre o resolvedor em **C++ (`solver/`)** e a interface 3D em **Python/VPython (`visual/`)**.

> **Regra do grupo:** qualquer alteração neste arquivo precisa ser combinada e aprovada por **todos os integrantes** antes de ser mesclada na `main`, pois impacta diretamente as duas metades do sistema.

---

## 1. Orientação de Referência (Cubo Resolvido)

Adotamos o esquema de cores ocidental padrão (**WCA**): **Branco em cima (`U`)** e **Verde na frente (`F`)**.

```text
                +-----------+
                |     U     |
                |  Branco   |
                |   (+y)    |
    +-----------+-----------+-----------+-----------+
    |     L     |     F     |     R     |     B     |
    |  Laranja  |   Verde   | Vermelho  |   Azul    |
    |   (-x)    |   (+z)    |   (+x)    |   (-z)    |
    +-----------+-----------+-----------+-----------+
                |     D     |
                |  Amarelo  |
                |   (-y)    |
                +-----------+
```

### Tabela de Faces, Cores e Eixos no VPython

No VPython, o sistema de coordenadas é **destro (mão direita)**:
- `+x` aponta para a **direita**;
- `+y` aponta para **cima**;
- `+z` aponta para **fora da tela** (em direção ao observador).

Os 26 cubinhos têm seus centros nas coordenadas inteiras `(x, y, z) ∈ {-1, 0, 1}³ \ {(0, 0, 0)}`, e os 6 centros fixos ficam em `(±1, 0, 0)`, `(0, ±1, 0)` e `(0, 0, ±1)`.

| Letra | Face | Cor | Cor sugerida no VPython | Camada (coord.) | Normal externa |
|---|---|---|---|---|---|
| **U** | Cima (*Up*) | Branco | `color.white` | `y = +1` | `(0, +1, 0)` |
| **D** | Baixo (*Down*) | Amarelo | `color.yellow` | `y = -1` | `(0, -1, 0)` |
| **F** | Frente (*Front*) | Verde | `color.green` | `z = +1` | `(0, 0, +1)` |
| **B** | Trás (*Back*) | Azul | `color.blue` | `z = -1` | `(0, 0, -1)` |
| **R** | Direita (*Right*) | Vermelho | `color.red` | `x = +1` | `(+1, 0, 0)` |
| **L** | Esquerda (*Left*) | Laranja | `color.orange` | `x = -1` | `(-1, 0, 0)` |

---

## 2. Notação dos Movimentos e Sentido Horário no VPython

São **6 faces × 3 variações = 18 movimentos** (métrica *Half-Turn Metric* — HTM):
- `X`: giro de **90° no sentido horário**, olhando de frente para a face `X` pelo **lado de fora** do cubo.
- `X'`: giro de **90° no sentido anti-horário**, olhando de frente para a face `X` pelo lado de fora.
- `X2`: giro de **180°**.

### Mapeamento para `obj.rotate(angle=..., axis=..., origin=vector(0, 0, 0))`

No VPython, `rotate` segue a **regra da mão direita**: olhando contra a ponta do vetor `axis`, um ângulo **positivo** gira no sentido **anti-horário**. Logo:
- Se `axis` for a **normal externa** da face, o giro horário (`X`) tem sempre `angle = -pi/2`.
- Usando apenas os eixos principais positivos `vector(1,0,0)`, `vector(0,1,0)` e `vector(0,0,1)`:

| Face | Camada selecionada | `axis` (eixo positivo) | Horário `X` (90°) | Anti-horário `X'` (-90°) | Meia-volta `X2` (180°) |
|---|---|---|---|---|---|
| **U** | `round(pos.y) == +1` | `(0, 1, 0)` | `-pi / 2` | `+pi / 2` | `-pi` |
| **D** | `round(pos.y) == -1` | `(0, 1, 0)` | `+pi / 2` | `-pi / 2` | `+pi` |
| **R** | `round(pos.x) == +1` | `(1, 0, 0)` | `-pi / 2` | `+pi / 2` | `-pi` |
| **L** | `round(pos.x) == -1` | `(1, 0, 0)` | `+pi / 2` | `-pi / 2` | `+pi` |
| **F** | `round(pos.z) == +1` | `(0, 0, 1)` | `-pi / 2` | `+pi / 2` | `-pi` |
| **B** | `round(pos.z) == -1` | `(0, 0, 1)` | `+pi / 2` | `-pi / 2` | `+pi` |

> **Atenção às faces escondidas:** vistas de frente, **D** (olhando por baixo), **L** (olhando pela esquerda) e **B** (olhando por trás) giram no sentido oposto ao das suas faces opostas (`U`, `R`, `F`). Por isso o sinal de `angle` é invertido quando usamos os eixos positivos.

### Ciclos Horários de Referência (C++ ↔ VPython)

Para garantir que C++ e VPython executem exatamente o mesmo giro, em cada movimento horário `X` a peça que está na primeira posição vai para a seguinte (`A → B`):

| Movimento | Ciclo horário das quinas (`pos → nova pos`) | Ciclo horário das arestas (`pos → nova pos`) |
|---|---|---|
| **U** | `URF → UFL → ULB → UBR → URF` (`0 → 1 → 2 → 3 → 0`) | `UR → UF → UL → UB → UR` (`0 → 1 → 2 → 3 → 0`) |
| **D** | `DFR → DRB → DBL → DLF → DFR` (`4 → 7 → 6 → 5 → 4`) | `DF → DR → DB → DL → DF` (`5 → 4 → 7 → 6 → 5`) |
| **R** | `URF → UBR → DRB → DFR → URF` (`0 → 3 → 7 → 4 → 0`) | `UR → BR → DR → FR → UR` (`0 → 11 → 4 → 8 → 0`) |
| **L** | `UFL → DLF → DBL → ULB → UFL` (`1 → 5 → 6 → 2 → 1`) | `UL → FL → DL → BL → UL` (`2 → 9 → 6 → 10 → 2`) |
| **F** | `URF → DFR → DLF → UFL → URF` (`0 → 4 → 5 → 1 → 0`) | `UF → FR → DF → FL → UF` (`1 → 8 → 5 → 9 → 1`) |
| **B** | `UBR → ULB → DBL → DRB → UBR` (`3 → 2 → 6 → 7 → 3`) | `UB → BL → DB → BR → UB` (`3 → 10 → 7 → 11 → 3`) |

### Numeração Canônica dos 18 Movimentos (C++)

Internamente no C++, os movimentos são representados por inteiros de `0` a `17` (face = `m / 3`, tipo = `m % 3`, onde `0 = 90°`, `1 = 180°`, `2 = -90°`):

| Face | `0` (`X`) | `1` (`X2`) | `2` (`X'`) |
|---|---|---|---|
| **U** (`0`) | `0`: `U` | `1`: `U2` | `2`: `U'` |
| **R** (`1`) | `3`: `R` | `4`: `R2` | `5`: `R'` |
| **F** (`2`) | `6`: `F` | `7`: `F2` | `8`: `F'` |
| **D** (`3`) | `9`: `D` | `10`: `D2` | `11`: `D'` |
| **L** (`4`) | `12`: `L` | `13`: `L2` | `14`: `L'` |
| **B** (`5`) | `15`: `B` | `16`: `B2` | `17`: `B'` |

---

## 3. Numeração das Posições das Peças (Padrão Kociemba)

Todo o código C++ (`cubo.hpp`, `movimentos.hpp`, `indice.hpp`) segue estritamente os índices abaixo. No estado resolvido, a peça de número `i` está na posição `i` com orientação `0`.

### Diagrama 3D por Camadas

```text
Camada U (y = +1)             Camada Média (y = 0)          Camada D (y = -1)
    [B, -z]                       [B, -z]                       [B, -z]
  2(ULB)--3(UB)--3(UBR)         10(BL)--- ---11(BR)           6(DBL)--7(DB)--7(DRB)
     |             |               |            |                |             |
[L] 2(UL)   U    0(UR) [R]    [L]  L     ·      R  [R]      [L] 6(DL)   D    4(DR) [R]
     |             |               |            |                |             |
  1(UFL)--1(UF)--0(URF)          9(FL)--- ---8(FR)            5(DLF)--5(DF)--4(DFR)
    [F, +z]                       [F, +z]                       [F, +z]
```

*(As quinas ocupam os cantos das camadas U e D; as arestas ocupam os meios das bordas.)*

### 3.1. Quinas (`0` a `7`)

O nome de cada posição lista suas 3 faces no **sentido horário ao redor da quina**, começando sempre pela face `U` ou `D` (face de referência para a orientação):

| Índice | Posição | Coordenada `(x, y, z)` | Faces (ordem horária) |
|---|---|---|---|
| `0` | **URF** | `(+1, +1, +1)` | U, R, F |
| `1` | **UFL** | `(-1, +1, +1)` | U, F, L |
| `2` | **ULB** | `(-1, +1, -1)` | U, L, B |
| `3` | **UBR** | `(+1, +1, -1)` | U, B, R |
| `4` | **DFR** | `(+1, -1, +1)` | D, F, R |
| `5` | **DLF** | `(-1, -1, +1)` | D, L, F |
| `6` | **DBL** | `(-1, -1, -1)` | D, B, L |
| `7` | **DRB** | `(+1, -1, -1)` | D, R, B |

**Convenção de orientação das quinas (`0`, `1` ou `2`):**
Toma-se como referência o adesivo da quina cuja cor pertence à face **U** (Branco) ou **D** (Amarelo):
- `0`: o adesivo U/D está na face `U` ou `D` (1ª face da sigla);
- `1`: está girado 1 passo no sentido horário em relação à face `U`/`D` (2ª face da sigla);
- `2`: está girado 2 passos no sentido horário (1 passo anti-horário, 3ª face da sigla).

### 3.2. Arestas (`0` a `11`)

A 1ª letra de cada sigla indica o adesivo de referência da aresta (`U`/`D` nas camadas superior e inferior; `F`/`B` na camada média):

| Índice | Posição | Coordenada `(x, y, z)` | Camada |
|---|---|---|---|
| `0` | **UR** | `(+1, +1, 0)` | Superior (`U`) |
| `1` | **UF** | `(0, +1, +1)` | Superior (`U`) |
| `2` | **UL** | `(-1, +1, 0)` | Superior (`U`) |
| `3` | **UB** | `(0, +1, -1)` | Superior (`U`) |
| `4` | **DR** | `(+1, -1, 0)` | Inferior (`D`) |
| `5` | **DF** | `(0, -1, +1)` | Inferior (`D`) |
| `6` | **DL** | `(-1, -1, 0)` | Inferior (`D`) |
| `7` | **DB** | `(0, -1, -1)` | Inferior (`D`) |
| `8` | **FR** | `(+1, 0, +1)` | Média |
| `9` | **FL** | `(-1, 0, +1)` | Média |
| `10` | **BL** | `(-1, 0, -1)` | Média |
| `11` | **BR** | `(+1, 0, -1)` | Média |

**Convenção de orientação das arestas (`0` ou `1`):**
- `0`: o adesivo de referência da peça ocupa a 1ª face da posição onde ela está;
- `1`: a aresta está virada (*flipped*), com o adesivo de referência na 2ª face da posição.

---

## 4. Contrato de Linha de Comando (`./solver`)

O Python (`visual/ponte.py`) executa o binário compilado em `solver/solver` via `subprocess`.

### 4.1. Entrada

```bash
./solver "<embaralhamento>"
```

- O embaralhamento é passado em **um único argumento** de texto, com movimentos separados por espaço em branco (ex.: `./solver "R U F' L2"`).
- **Cubo já resolvido:** argumento vazio `./solver ""` (ou apenas espaços) é válido e representa 0 movimentos de embaralhamento.
- **Tokens válidos:** exatamente um dos 18 símbolos em maiúsculas:
  `U`, `U'`, `U2`, `D`, `D'`, `D2`, `F`, `F'`, `F2`, `B`, `B'`, `B2`, `R`, `R'`, `R2`, `L`, `L'`, `L2`.
- Qualquer outro token (ex.: `Q3`, `r`, `R3`, `U''`) ou número inválido de argumentos deve ser rejeitado com erro.
- **Opção de depuração (reservada para #3 / #5):** `./solver --debug "<embaralhamento>"` pode imprimir informações extras de estado para depuração humana; a interface Python **não** usa `--debug`.

### 4.2. Saída em Caso de Sucesso (`stdout` e código `0`)

O programa escreve **exatamente duas linhas** em `stdout` e termina com **código de saída `0`**:

```text
<solucao>
nos=<inteiro> tempo=<decimal> profundidade=<inteiro>
```

- **Linha 1 (`<solucao>`):** sequência ótima de movimentos que resolve o cubo, no mesmo formato da entrada (separados por um único espaço, sem espaços extras no início ou no fim). Se o cubo já estiver resolvido, a linha 1 é **vazia** (`""`, apenas a quebra de linha `\n`).
- **Linha 2 (estatísticas):** pares `chave=valor` separados por um espaço, sempre nessa ordem:
  - `nos`: número inteiro (`>= 0`) de nós expandidos na busca;
  - `tempo`: tempo decorrido da busca em **segundos (`s`)**, formatado com ponto decimal (`.`, independente de locale), por exemplo `0.004` ou `1.250`;
  - `profundidade`: inteiro (`>= 0`) igual à quantidade de movimentos na linha 1.

#### Exemplos de Execução

**Caso normal:**
```bash
$ ./solver "R U"
U' R'
nos=42 tempo=0.0003 profundidade=2
```

**Cubo já resolvido (linha 1 vazia):**
```bash
$ ./solver ""

nos=1 tempo=0.0000 profundidade=0
```

**Teste face a face (obrigatório na integração #8):**
Para toda face `X ∈ {U, D, F, B, R, L}`:
```bash
$ ./solver "X"
X'
nos=... tempo=... profundidade=1
```

### 4.3. Erros (`stderr` e código ≠ 0)

Em qualquer falha, o `./solver`:
1. **Não** imprime nada em `stdout` (ou nada deve ser interpretado como solução pelo chamador);
2. Escreve uma mensagem clara de erro em **`stderr`**;
3. Encerra com **código de saída diferente de zero**:

| Código | Situação | Exemplo de mensagem em `stderr` |
|---|---|---|
| `0` | Sucesso | — |
| `1` | Uso incorreto ou movimento inválido na entrada | `Erro: movimento invalido "Q3".` |
| `2` | Tabela `data/quinas.bin` ausente, ilegível ou com tamanho incorreto | `Erro: arquivo data/quinas.bin nao encontrado. Rode "make pdb" na pasta solver/.` |
| `> 2` | Outros erros internos inesperados | `Erro interno: ...` |

---

## 5. Checklist de Validação do Contrato

Antes de considerar a integração pronta (#8), C++ e Python devem concordar em todos os testes abaixo:

- [ ] Para cada face `X ∈ {U, D, F, B, R, L}`, `./solver "X"` retorna `X'` na linha 1 e `profundidade=1` na linha 2.
- [ ] No VPython, animar `X` seguido de `X'` retorna visualmente ao cubo resolvido para todas as 6 faces.
- [ ] `R U F` aplicado no VPython produz exatamente a mesma disposição de cores que num cubo físico segurado com Branco em `U` e Verde em `F`.
- [ ] `./solver ""` retorna linha 1 vazia, `profundidade=0` e código de saída `0`.
- [ ] `./solver "Q3"` imprime erro em `stderr` e retorna código de saída `1`.
