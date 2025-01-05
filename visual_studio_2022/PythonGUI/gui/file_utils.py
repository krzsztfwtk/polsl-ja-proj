import os
import random
import configparser
from tkinter import filedialog, messagebox, Toplevel, Entry, Label, Button

import gui.config as conf


def load_file(title, filetypes):
    return filedialog.askopenfilename(title=title, filetypes=filetypes)


def _generate_coefficients(n: int, coeff_min: float, coeff_max: float) -> str:
    """
    Funkcja 'niższego poziomu' generująca n współczynników
    z zakresu [coeff_min, coeff_max] i zapisująca do pliku w conf.TEMP_DIR.
    
    Zwraca ścieżkę do wygenerowanego pliku.
    """
    if not os.path.exists(conf.TEMP_DIR):
        os.makedirs(conf.TEMP_DIR)
    
    filepath = os.path.abspath(os.path.join(conf.TEMP_DIR, 'generated.coeffs'))
    
    coefficients = [random.uniform(coeff_min, coeff_max) for _ in range(n)]
    
    with open(filepath, 'w') as f:
        for coeff in coefficients:
            f.write(f"{coeff}\n")
    
    return filepath


def _generate_points(start: float, end: float, step: float) -> str:
    """
    Funkcja 'niższego poziomu' generująca parametry start, end, step
    i zapisująca je do pliku w conf.TEMP_DIR.
    
    Zwraca ścieżkę do wygenerowanego pliku.
    """
    if not os.path.exists(conf.TEMP_DIR):
        os.makedirs(conf.TEMP_DIR)
    
    filepath = os.path.abspath(os.path.join(conf.TEMP_DIR, 'generated.points'))
    
    with open(filepath, 'w') as f:
        f.write(f"{start} {end} {step}\n")
    
    return filepath


def generate_coefficients_file(app, n=None, coeff_min=None, coeff_max=None):
    """
    Funkcja do generowania współczynników:
      - Jeśli n, coeff_min, coeff_max != None, to generujemy bez okna dialogowego
        i zwracamy ścieżkę (zapisujemy też w app.coefficients_file).
      - Jeśli parametry są None, to wyświetlamy okno Toplevel, by użytkownik wprowadził dane.
    """

    # Sprawdź, czy parametry zostały przekazane.
    # Jeśli TAK -> generujemy w tle (bez GUI).
    if n is not None and coeff_min is not None and coeff_max is not None:
        # Generuj i zapisz do pliku
        filepath = _generate_coefficients(n, coeff_min, coeff_max)
        app.coefficients_file = filepath
        # Nie wysyłamy messageboxa w trybie eksperymentu
        return filepath

    # Jeżeli parametry są None, to jesteśmy w trybie GUI -> pokaż okno:
    gen_coeff_window = Toplevel(app.master)
    gen_coeff_window.title("Generate Coefficients")

    Label(gen_coeff_window, text="Number of coefficients:").grid(row=0, column=0, padx=5, pady=5)
    num_coeffs_entry = Entry(gen_coeff_window)
    num_coeffs_entry.grid(row=0, column=1, padx=5, pady=5)

    Label(gen_coeff_window, text="Min coefficient value:").grid(row=1, column=0, padx=5, pady=5)
    min_coeff_entry = Entry(gen_coeff_window)
    min_coeff_entry.grid(row=1, column=1, padx=5, pady=5)

    Label(gen_coeff_window, text="Max coefficient value:").grid(row=2, column=0, padx=5, pady=5)
    max_coeff_entry = Entry(gen_coeff_window)
    max_coeff_entry.grid(row=2, column=1, padx=5, pady=5)

    def generate():
        try:
            num_coeffs = int(num_coeffs_entry.get())
            mn = float(min_coeff_entry.get())
            mx = float(max_coeff_entry.get())

            if num_coeffs <= 0:
                raise ValueError("Number of coefficients must be positive.")

            # Generowanie
            filepath = _generate_coefficients(num_coeffs, mn, mx)
            
            app.coefficients_file = filepath
            app.coeff_file_label.config(text=os.path.basename(filepath))

            messagebox.showinfo("Success", "Coefficients generated and saved.")
            gen_coeff_window.destroy()

        except ValueError as e:
            messagebox.showerror("Error", f"Invalid input: {e}")

    Button(gen_coeff_window, text="Generate", command=generate).grid(row=3, column=0, columnspan=2, pady=10)


def generate_points_file(app, start=None, end=None, step=None):
    """
    Funkcja do generowania pliku z parametrami punktów:
      - Jeśli start, end, step != None, to generujemy bez okna dialogowego
        i zwracamy ścieżkę (zapisujemy też w app.points_file).
      - Jeśli parametry są None, to wyświetlamy okno Toplevel, by użytkownik wprowadził dane.
    """

    # Sprawdź, czy parametry zostały przekazane (tryb eksperymentu).
    if start is not None and end is not None and step is not None:
        filepath = _generate_points(start, end, step)
        app.points_file = filepath
        return filepath

    # Jeżeli parametry są None, to jesteśmy w trybie GUI -> pokaż okno:
    gen_points_window = Toplevel(app.master)
    gen_points_window.title("Generate Points")

    from tkinter import Label, Entry, Button

    Label(gen_points_window, text="Start value:").grid(row=0, column=0, padx=5, pady=5)
    start_entry = Entry(gen_points_window)
    start_entry.grid(row=0, column=1, padx=5, pady=5)

    Label(gen_points_window, text="End value:").grid(row=1, column=0, padx=5, pady=5)
    end_entry = Entry(gen_points_window)
    end_entry.grid(row=1, column=1, padx=5, pady=5)

    Label(gen_points_window, text="Step size:").grid(row=2, column=0, padx=5, pady=5)
    step_entry = Entry(gen_points_window)
    step_entry.grid(row=2, column=1, padx=5, pady=5)

    def generate():
        try:
            st = float(start_entry.get())
            ed = float(end_entry.get())
            sp = float(step_entry.get())

            if sp <= 0:
                raise ValueError("Step size must be positive.")

            filepath = _generate_points(st, ed, sp)
            
            app.points_file = filepath
            app.points_file_label.config(text=os.path.basename(filepath))

            messagebox.showinfo("Success", "Points parameters saved.")
            gen_points_window.destroy()

        except ValueError as e:
            messagebox.showerror("Error", f"Invalid input: {e}")

    Button(gen_points_window, text="Generate", command=generate).grid(row=3, column=0, columnspan=2, pady=10)



def write_config_file(implementation, multithreading, threads_number, avx, save_results,
                      generate_chart, output_file, coefficients_file, points_file):
    progress_file = conf.PROGRESS_FILE
    computation_time_file = conf.COMPUTATION_TIME_FILE
    results_file = output_file if save_results else os.path.abspath(os.path.join(conf.TEMP_DIR, 'results.out'))

    config = configparser.ConfigParser()
    config['Settings'] = {
        'implementation': implementation,
        'multithreading': '1' if multithreading else '0',
        'threads_number': threads_number,
        'avx': '1' if avx else '0',
        'save_results': '1' if save_results else '0',
        'generate_chart': '1' if generate_chart else '0',
        'output_file': results_file,
        'input_coeffs_file': os.path.abspath(coefficients_file),
        'input_points_file': os.path.abspath(points_file),
        'progress_file': progress_file,
        'computation_time_file': computation_time_file
    }

    with open(conf.CONFIG_FILE, 'w') as configfile:
        config.write(configfile)

    return conf.CONFIG_FILE


def read_results_and_display(app):
    try:
        # Odczytaj czas obliczeń
        computation_time_file = conf.COMPUTATION_TIME_FILE
        with open(computation_time_file, 'r') as time_file:
            time_content = time_file.read().strip()
            if not time_content:
                raise ValueError("Computation time file is empty.")
            computation_time = float(time_content)

        results = []

        if app.save_results.get():
            # Odczytaj wyniki
            results_file = app.output_file if app.save_results.get() else os.path.abspath(os.path.join(conf.TEMP_DIR, 'results.out'))
            with open(results_file, 'r') as results_file_obj:
                lines = results_file_obj.readlines()
                if not lines:
                    raise ValueError("Results file is empty.")
                results = [float(line.strip()) for line in lines]
        else:
            results = None

        # Odczytaj punkty do wykresu
        with open(app.points_file, 'r') as points_file_obj:
            content = points_file_obj.read().strip()
            if not content:
                raise ValueError("Points file is empty.")
            parts = content.split()
            if len(parts) != 3:
                raise ValueError("Points file must contain exactly three values: start, end, and step.")
            start, end, step = map(float, parts)
            if step <= 0:
                raise ValueError("Step size must be positive.")
            points = []
            current = start
            while current <= end:
                points.append(current)
                current += step
            if not points:
                raise ValueError("No points generated.")

        app.master.after(0, app.display_results, computation_time, points, results)

    except Exception as e:
        messagebox.showerror("Error", f"An error occurred while reading results: {e}")
    finally:
        app.compute_button.config(state='normal')
        app.progress.stop()
