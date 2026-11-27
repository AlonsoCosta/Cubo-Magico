from math import pi

from vpython import box, canvas, color, compound, rate, vector

TAMANHO_CUBINHO = 0.95 # Tamanho de cada cubinho do cubo
LARGURA_ADESIVO = 0.85 # Tamanho do adesivo que sinaliza cada cor
ESPESSURA_ADESIVO = 0.02 
DESLOCAMENTO_ADESIVO = 0.48 # Deslocamento do adesivo para posicionar corretamente

CORES = {
    "U": color.white,
    "D": color.yellow,
    "F": color.green,
    "B": color.blue,
    "R": color.red,
    "L": color.orange,
}

def criar_cena() -> canvas:
    """Inicializa a cena 3D com câmera diagonal exibindo as faces U, F e R."""
    scene = canvas(         # configs da tela
        title="Cubo Mágico 3x3x3",
        width=1200,
        height=900,
        background=color.gray(0.15),
    )
    scene.center = vector(0, 0, 0)        #
    scene.up = vector(0, 1, 0)            # Posicionamento da câmera para mostrar os três lados do cubo
    scene.forward = vector(-1, -1, -1)    #
    return scene


_FACES_ADESIVOS = (
    (
        "x",
        1,
        "R",
        vector(DESLOCAMENTO_ADESIVO, 0, 0),
        vector(ESPESSURA_ADESIVO, LARGURA_ADESIVO, LARGURA_ADESIVO),
    ),
    (
        "x",
        -1,
        "L",
        vector(-DESLOCAMENTO_ADESIVO, 0, 0),
        vector(ESPESSURA_ADESIVO, LARGURA_ADESIVO, LARGURA_ADESIVO),
    ),
    (
        "y",
        1,
        "U",
        vector(0, DESLOCAMENTO_ADESIVO, 0),
        vector(LARGURA_ADESIVO, ESPESSURA_ADESIVO, LARGURA_ADESIVO),
    ),
    (
        "y",
        -1,
        "D",
        vector(0, -DESLOCAMENTO_ADESIVO, 0),
        vector(LARGURA_ADESIVO, ESPESSURA_ADESIVO, LARGURA_ADESIVO),
    ),
    (
        "z",
        1,
        "F",
        vector(0, 0, DESLOCAMENTO_ADESIVO),
        vector(LARGURA_ADESIVO, LARGURA_ADESIVO, ESPESSURA_ADESIVO),
    ),
    (
        "z",
        -1,
        "B",
        vector(0, 0, -DESLOCAMENTO_ADESIVO),
        vector(LARGURA_ADESIVO, LARGURA_ADESIVO, ESPESSURA_ADESIVO),
    ),
)


def criar_cubinho(x: int, y: int, z: int) -> compound:
    """Cria um cubinho (bloco preto + adesivos externos) posicionado em (x, y, z)."""
    coords = {"x": x, "y": y, "z": z}
    partes = [
        box(
            pos=vector(0, 0, 0),
            size=vector(TAMANHO_CUBINHO, TAMANHO_CUBINHO, TAMANHO_CUBINHO),
            color=color.black,
        )
    ]
    for eixo, valor, face, pos_local, tamanho in _FACES_ADESIVOS:
        if coords[eixo] == valor:
            partes.append(box(pos=pos_local, size=tamanho, color=CORES[face]))

    cubinho = compound(partes)
    cubinho.pos = vector(x, y, z)
    return cubinho


PASSOS_POR_GIRO = 15

# Cada eixo aponta para fora da face; pela regra da mão direita no VPython,
# o giro horário de 90° (visto de fora) corresponde sempre a -pi / 2.
MOVIMENTOS = {
    "R": ("x", 1, vector(1, 0, 0)),
    "L": ("x", -1, vector(-1, 0, 0)),
    "U": ("y", 1, vector(0, 1, 0)),
    "D": ("y", -1, vector(0, -1, 0)),
    "F": ("z", 1, vector(0, 0, 1)),
    "B": ("z", -1, vector(0, 0, -1)),
}

_MODIFICADORES = {
    "": (1, PASSOS_POR_GIRO),
    "'": (-1, PASSOS_POR_GIRO),
    "2": (2, 2 * PASSOS_POR_GIRO),
}

_cubinhos_ativos: list[compound] = []


def criar_cubo() -> list[compound]:
    """Cria os 26 cubinhos do cubo 3x3x3 e retorna a lista de objetos."""
    global _cubinhos_ativos
    cubinhos = [
        criar_cubinho(x, y, z)
        for x in (-1, 0, 1)
        for y in (-1, 0, 1)
        for z in (-1, 0, 1)
        if not (x == 0 and y == 0 and z == 0)
    ]
    _cubinhos_ativos = cubinhos
    return cubinhos


def parse_movimento(movimento: str) -> tuple[str, int, vector, float, int]:
    """Converte notação como 'R', \"U'\" ou 'F2' em parâmetros de rotação."""
    mov = movimento.strip()
    if not mov or mov[0] not in MOVIMENTOS or mov[1:] not in _MODIFICADORES:
        raise ValueError(f"Movimento inválido: {movimento!r}")

    eixo_nome, coordenada, eixo_vec = MOVIMENTOS[mov[0]]
    multiplicador, num_passos = _MODIFICADORES[mov[1:]]
    angulo_total = multiplicador * (-pi / 2)
    return eixo_nome, coordenada, eixo_vec, angulo_total, num_passos


def selecionar_camada(
    cubinhos: list[compound], eixo: str, coordenada: int
) -> list[compound]:
    """Seleciona os cubinhos cuja posição atual arredondada pertence à camada."""
    return [c for c in cubinhos if round(getattr(c.pos, eixo)) == coordenada]


def _arredondar_vetor(v: vector) -> vector:
    return vector(round(v.x), round(v.y), round(v.z))


def arredondar_cubinhos(cubinhos: list[compound]) -> None:
    """Corrige pequenos erros de ponto flutuante após cada giro."""
    for c in cubinhos:
        c.pos = _arredondar_vetor(c.pos)
        c.axis = _arredondar_vetor(c.axis.norm()) * c.axis.mag
        c.up = _arredondar_vetor(c.up.norm()) * c.up.mag


def animar(
    movimento: str,
    cubinhos: list[compound] | None = None,
    taxa: int | None = 60,
) -> None:
    """Anima um movimento ('R', \"U'\", 'F2', ...) na cena 3D."""
    alvo = _cubinhos_ativos if cubinhos is None else cubinhos
    if not alvo:
        raise RuntimeError("Nenhum cubinho inicializado. Chame criar_cubo() antes.")

    eixo_nome, coordenada, eixo_vec, angulo_total, num_passos = parse_movimento(
        movimento
    )
    camada = selecionar_camada(alvo, eixo_nome, coordenada)
    delta = angulo_total / num_passos

    for _ in range(num_passos):
        if taxa is not None:
            rate(taxa)
        for c in camada:
            c.rotate(angle=delta, axis=eixo_vec, origin=vector(0, 0, 0))

    arredondar_cubinhos(camada)


def _inverter_sequencia(movimentos: list[str]) -> list[str]:
    """Retorna a sequência inversa de uma lista de movimentos."""
    inverso = []
    for mov in reversed(movimentos):
        face, sufixo = mov[0], mov[1:]
        if sufixo == "":
            inverso.append(f"{face}'")
        elif sufixo == "'":
            inverso.append(face)
        else:
            inverso.append(mov)
    return inverso


if __name__ == "__main__":
    import random

    criar_cena()
    cubo = criar_cubo()

    # 1) R aplicado 4 vezes volta ao inicial
    for m in ["R"] * 4:
        animar(m, cubo)

    # 2) R U R' U' repetido 6 vezes volta ao resolvido
    for m in ["R", "U", "R'", "U'"] * 6:
        animar(m, cubo, taxa=120)

    # 3) 200 movimentos aleatórios + sequência inversa
    faces = list(MOVIMENTOS.keys())
    sufixos = ["", "'", "2"]
    aleatorios = [f"{random.choice(faces)}{random.choice(sufixos)}" for _ in range(200)]
    for m in aleatorios + _inverter_sequencia(aleatorios):
        animar(m, cubo, taxa=600)

    print("Testes visuais concluídos. Pressione Ctrl+C para encerrar.")
    try:
        while True:
            rate(30)
    except KeyboardInterrupt:
        print("\nVisualizador encerrado.")

