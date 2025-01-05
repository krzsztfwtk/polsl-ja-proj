import os
import csv
import time

from tkinter import messagebox

from gui.config import (
    TEMP_DIR,
    COMPUTATION_TIME_FILE,
    TIMEOUT_SECONDS,
    EVENT_NAME
)
from gui.events import trigger_event
from gui.file_utils import (
    generate_coefficients_file,
    generate_points_file,
    write_config_file
)


def show_experiment_info():
    """
    Wyświetla w okienku informacyjnym opis eksperymentu:
    - 3 scenariusze (n=10/100/1000, różne kroki),
    - implementacja: {cpp, asm},
    - AVX: {False, True},
    - multithreading: zawsze True, ale threads=1..16,
    - 10 uruchomień każdej konfiguracji,
    - brak progress.tmp (nie śledzimy postępu w jednym uruchomieniu),
    - brak zapisu wyników, brak generowania wykresu,
    - odczytujemy finalny czas z computation.time.
    """
    info_text = (
        "EXPERIMENT DESCRIPTION:\n\n"
        "We run 5 scenarios:\n"
        " 1) Scenario 1:\n"
        "    Coefficients: n=10,   min=-1, max=1\n"
        "    Points: min=-100, max=100, step=0.001\n\n"
        " 2) Scenario 2:\n"
        "    Coefficients: n=100,  min=-1, max=1\n"
        "    Points: min=-100, max=100, step=0.001\n\n"
        " 3) Scenario 3:\n"
        "    Coefficients: n=1000, min=-1, max=1\n"
        "    Points: min=-100, max=100, step=0.001\n\n"
        " 4) Scenario 4:\n"
        "    Coefficients: n=10000, min=-1, max=1\n"
        "    Points: min=-100, max=100, step=0.001\n\n"
        " 5) Scenario 5:\n"
        "    Coefficients: n=10000, min=-1, max=1\n"
        "    Points: min=-100, max=100, step=0.001\n\n"
        "For each scenario, we test:\n"
        " - Implementation: {cpp, asm}\n"
        " - AVX: {False, True}\n"
        " - Multithreading: always True, Number of Threads=1..16,\n"
        "We run each config 5 times.\n\n"
        "No polynomial results are saved, no chart is generated.\n"
        "All execution times in seconds are saved in 'experiment_results.csv'.\n"
    )
    messagebox.showinfo("Experiment Info", info_text)


def run_experiment(app):
    """
    Uruchamia eksperyment iteracyjnie - 
    1) Generuje pliki współczynników i punktów,
    2) Zapisuje config.ini,
    3) trigger_event(EVENT_NAME),
    4) czeka na pojawienie się computation.time,
    5) odczytuje i zapisuje w pliku CSV.
    
    Brak użycia progress.tmp. Postęp wypisujemy w GUI (status_label).
    """

    # Nazwa pliku CSV z wynikami
    csv_filename = "experiment_results.csv"

    # Definicja scenariuszy
    scenarios = [
        {
            "name": "Scenario1",
            "coeff_n": 10,
            "coeff_min": -1,
            "coeff_max": 1,
            "points_min": -100,
            "points_max": 100,
            "points_step": 0.001
        },
        {
            "name": "Scenario2",
            "coeff_n": 100,
            "coeff_min": -1,
            "coeff_max": 1,
            "points_min": -100,
            "points_max": 100,
            "points_step": 0.001
        },
        {
            "name": "Scenario3",
            "coeff_n": 1000,
            "coeff_min": -1,
            "coeff_max": 1,
            "points_min": -100,
            "points_max": 100,
            "points_step": 0.001
        },
        {
            "name": "Scenario4",
            "coeff_n": 10000,
            "coeff_min": -1,
            "coeff_max": 1,
            "points_min": -100,
            "points_max": 100,
            "points_step": 0.001
        },
        {
            "name": "Scenario5",
            "coeff_n": 100000,
            "coeff_min": -1,
            "coeff_max": 1,
            "points_min": -100,
            "points_max": 100,
            "points_step": 0.001
        }
    ]

    implementations = ["cpp", "asm"]
    avx_options = [False, True]

    runs_per_config = 5
    threads_range = range(1, 17)  # 1..16

    # Obliczamy liczbę wszystkich uruchomień
    total_runs = len(scenarios) * len(implementations) * len(avx_options) * len(threads_range) * runs_per_config
    done = 0  # liczba zakończonych uruchomień

    # Tworzymy/otwieramy plik CSV
    fieldnames = ["scenario", "implementation", "avx", "threads", "run_number", "time_seconds"]
    with open(csv_filename, mode="w", newline="", encoding="utf-8") as csvfile:
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        writer.writeheader()

        # Zablokuj przycisk w GUI:
        app.compute_button.config(state='disabled')
        app.status_label.config(text="Experiment running...")
        app.progress['value'] = 0

        for scenario in scenarios:
            # Ustaw w app: brak zapisu wyników, brak wykresu, multithreading=ON
            app.save_results.set(False)
            app.generate_chart.set(False)
            app.use_multithreading.set(True)

            # Generujemy pliki wejściowe (bez okien)
            coeff_file = generate_coefficients_file(
                app,
                n=scenario["coeff_n"],
                coeff_min=scenario["coeff_min"],
                coeff_max=scenario["coeff_max"]
            )
            points_file = generate_points_file(
                app,
                start=scenario["points_min"],
                end=scenario["points_max"],
                step=scenario["points_step"]
            )
            app.coefficients_file = coeff_file
            app.points_file = points_file

            for impl in implementations:
                for avx in avx_options:
                    for threads in threads_range:
                        # Dla każdej konfiguracji 10 uruchomień
                        for run_idx in range(1, runs_per_config + 1):
                            if os.path.exists(COMPUTATION_TIME_FILE):
                                os.remove(COMPUTATION_TIME_FILE)
                                
                            # --- Tworzymy config.ini ---
                            write_config_file(
                                impl,                       # implementation
                                True,                       # use_multithreading
                                threads,                    # threads_number
                                avx,                        # use_avx
                                False,                      # save_results
                                False,                      # generate_chart
                                "",                         # output_file (pusty)
                                app.coefficients_file,
                                app.points_file
                            )

                            # Tworzymy katalog temp (na wszelki wypadek)
                            if not os.path.exists(TEMP_DIR):
                                os.makedirs(TEMP_DIR)

                            # Wyzwalamy zdarzenie - program C++/ASM startuje
                            trigger_event(EVENT_NAME)

                            # Teraz czekamy, aż pojawi się computation.time
                            # w pętli (z timeoutem), bo nie używamy progress.tmp

                            start_wait = time.time()
                            time_elapsed = -1  # Domyślnie błąd
                            while True:
                                if os.path.exists(COMPUTATION_TIME_FILE):
                                    # Odczytujemy zawartość
                                    with open(COMPUTATION_TIME_FILE, 'r') as f:
                                        content = f.read().strip()
                                        if content:
                                            try:
                                                time_elapsed = float(content)
                                            except ValueError:
                                                time_elapsed = -1
                                            break
                                # Sprawdzamy timeout
                                if (time.time() - start_wait) > TIMEOUT_SECONDS:
                                    # Błąd - nie doczekaliśmy się
                                    break
                                time.sleep(0.1)

                            # Zapisujemy w CSV
                            writer.writerow({
                                "scenario": scenario["name"],
                                "implementation": impl,
                                "avx": avx,
                                "threads": threads,
                                "run_number": run_idx,
                                "time_seconds": time_elapsed
                            })

                            # Raportowanie postępu
                            done += 1
                            progress_percent = (done / total_runs) * 100.0

                            print(f"[{done}/{total_runs}] "
                                  f"scenario={scenario['name']}, impl={impl}, avx={avx}, "
                                  f"threads={threads}, run={run_idx}, time={time_elapsed}")

                            # Aktualizacja GUI (status_label, progress)
                            app.status_label.config(
                                text=f"Exp: {done}/{total_runs} runs done. "
                                     f"Scenario={scenario['name']}, impl={impl}, avx={avx}, threads={threads}"
                            )
                            app.progress['value'] = progress_percent

                            # Pozwalamy na odświeżenie interfejsu
                            app.master.update()

        # Koniec całej pętli
        app.compute_button.config(state='normal')
        app.status_label.config(text="Experiment finished.")
        app.progress['value'] = 100

    # Komunikat
    messagebox.showinfo(
        "Experiment Completed",
        f"Experiment finished!\nResults saved to {csv_filename}"
    )
