# Cubo Mágico 3×3×3 — Resolvedor Ótimo (IDA* + VPython)

Programa que recebe um cubo mágico 3×3×3 embaralhado, encontra a **menor sequência de movimentos possível** para resolvê-lo e exibe a solução animada em 3D.

---

## Visão Geral da Arquitetura

O projeto é dividido em duas metades desacopladas que se comunicam apenas por texto:

- **C++ (`solver/`) — o "cérebro":** recebe o embaralhamento, executa a busca IDA* examinando milhões de estados por segundo e devolve a sequência ótima de movimentos.
- **Python (`visual/`) — a "tela":** renderiza o cubo em 3D, oferece os botões da interface e anima os movimentos um a um no navegador.

### Caminho de um clique em "Resolver"
1. O Python (`visual/`) executa `./solver` passando o embaralhamento como texto.
2. O C++ (`solver/`) calcula a solução usando a Pattern Database pré-calculada em `data/quinas.bin`.
3. A solução retorna como texto padrão (notação de movimentos).
4. O Python anima cada movimento na cena 3D.

O C++ é organizado em **três programas separados**, cada um com seu `main`:
- `gerar_pdb`: gera a tabela pré-computada uma única vez;
- `solver`: resolve um cubo embaralhado;
- `testes`: executa os testes automatizados em C++.

---

## Estrutura de Pastas

```text
solver/            Código C++
  Makefile
  include/         Cabeçalhos (.hpp)
  src/             Implementações (.cpp)
  tests/           Testes automáticos C++
data/              Tabela pré-computada (~88 MB, fora do Git)
visual/            Interface 3D em Python (app.py, cena.py, ponte.py, notacao.py)
  tests/           Testes Python (pytest)
experimentos/      Scripts e gráficos para o relatório
```

> **Por que `data/*.bin` fica fora do Git?** A tabela pré-calculada possui ~88 MB e cada integrante gera a sua localmente com um comando (`make pdb`).

---

## Pré-requisitos

- **g++** com suporte a **C++17** e **GNU Make**
- **Python 3.10+**

---

## Configuração do Ambiente Python

Na raiz do repositório, crie e ative o ambiente virtual e instale as dependências:

```bash
python -m venv .venv
```

- **Linux / macOS:**
  ```bash
  source .venv/bin/activate
  ```
- **Windows (PowerShell):**
  ```powershell
  .venv\Scripts\Activate.ps1
  ```

Com o ambiente ativo, instale os pacotes:

```bash
pip install -r requirements.txt
```

---

## Como Compilar e Executar

### 1. Compilar e rodar o Solver (C++)
```bash
cd solver
make && ./solver
```

### 2. Gerar a tabela Pattern Database (C++)
```bash
cd solver
make pdb
```

### 3. Rodar os testes
- **Testes C++:**
  ```bash
  cd solver
  make test
  ```
- **Testes Python:**
  ```bash
  pytest
  ```

### 4. Limpar arquivos compilados
```bash
cd solver
make clean
```

### 5. Abrir a interface 3D
Com o ambiente virtual ativo, a partir da raiz do projeto:
```bash
python visual/app.py
```

---

## Fluxo de Trabalho da Equipe

- **Uma branch por tarefa:** crie uma branch específica a partir da `main` antes de começar (`git checkout -b feat/nome-da-tarefa`).
- **Commits pequenos:** faça commits frequentes, objetivos e fáceis de revisar.
- **Revisão obrigatória:** todo Pull Request (PR) deve ser revisado por outro integrante do grupo antes do merge na `main`.
