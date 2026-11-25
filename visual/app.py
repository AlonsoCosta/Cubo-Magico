from vpython import box, canvas, color, rate, vector


def main() -> None:
    scene = canvas(
        title="Cubo Magico 3x3x3 - Visualizador 3D (Provisorio)",
        width=800,
        height=600,
        background=color.gray(0.15),
    )
    scene.center = vector(0, 0, 0)

    box(pos=vector(0, 0, 0), size=vector(2, 2, 2), color=color.cyan)
    print("Cena 3D aberta no navegador. Pressione Ctrl+C para encerrar.")

    try:
        while True:
            rate(30)
    except KeyboardInterrupt:
        print("\nVisualizador encerrado.")


if __name__ == "__main__":
    main()
