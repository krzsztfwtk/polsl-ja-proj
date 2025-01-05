import matplotlib.pyplot as plt
from tkinter import messagebox

def plot_results(app, points, results):
    """
    Wyświetla wykres wyników wielomianu obliczonego metodą Hornera
    dla zadanych punktów.

    :param app: Referencja do głównej aplikacji (zawierająca m.in. zmienną generate_chart)
    :param points: Lista punktów (x)
    :param results: Lista wartości wielomianu w tych punktach (p(x))
    """
    # Sprawdź, czy użytkownik w ogóle chce generować wykres
    if not app.generate_chart.get():
        return

    # Ustaw styl wykresu
    plt.style.use("seaborn-darkgrid")

    # Tworzenie okna figury
    plt.figure(figsize=(10, 6))

    # Rysowanie wykresu
    plt.plot(
        points,
        results,
        color="#1C7ED6",
        linewidth=2,
        marker='o',
        markersize=6,
        markerfacecolor='white',
        markeredgewidth=1.2,
        markeredgecolor="#1C7ED6",
        label='Polynomial Value'
    )

    # Opisy osi (z niewielkim zwiększeniem rozmiaru czcionki i pogrubieniem)
    plt.xlabel('X', fontsize=12, fontweight='bold')
    plt.ylabel('P(X)', fontsize=12, fontweight='bold')

    # Tytuł wykresu
    plt.title('Horner Polynomial Evaluation', fontsize=16, fontweight='bold')

    # Dodaj legendę
    plt.legend()

    # Włącz linie siatki
    plt.grid(True, linestyle='--', alpha=0.7)

    # Dopasowanie elementów wykresu, by nie nachodziły na siebie
    plt.tight_layout()

    # Wyświetl wykres
    plt.show()
