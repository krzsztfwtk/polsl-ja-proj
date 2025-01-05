from asyncio.windows_events import NULL
import tkinter as tk
from tkinter import filedialog, messagebox
from tkinter.ttk import Progressbar
import threading
import os
from PIL import Image, ImageTk
import zlib, base64
import time

import gui.config as conf
from gui.events import trigger_event, monitor_progress_and_completion
from gui.file_utils import (
    generate_coefficients_file,
    generate_points_file,
    load_file,
    write_config_file
)
from gui.plotting import plot_results
from gui.experiment import run_experiment, show_experiment_info

class MainWindow(object):
    """Main window of the GUI application"""
    def __init__(self, master):
        # Ustawienia głównego okna
        self.master = master
        self.master.title("Horner Polynomial Calculator")

        # Ikona programu
        master.iconbitmap("icon.ico")
        image = ImageTk.PhotoImage(file="icon.ico")
        master.tk.call('wm', 'iconphoto', master._w, image)

        # Wczytanie logo dla C++ i ASM
        self.cpp_logo_image = Image.open("gui/images/cpp.png").resize((50, 50), Image.ANTIALIAS)
        self.cpp_logo = ImageTk.PhotoImage(self.cpp_logo_image)

        self.asm_logo_image = Image.open("gui/images/asm.png").resize((50, 50), Image.ANTIALIAS)
        self.asm_logo = ImageTk.PhotoImage(self.asm_logo_image)

        # Zmienne konfiguracyjne
        self.coefficients_file = ''
        self.points_file = ''
        self.output_file = ''
        self.impl_choice = tk.StringVar(value='cpp')  # Domyślnie implementacja C++
        self.use_multithreading = tk.BooleanVar(value=False)
        self.threads_number = 1
        self.use_avx = tk.BooleanVar(value=False)
        self.generate_chart = tk.BooleanVar(value=False)
        self.save_results = tk.BooleanVar(value=False)
        self.is_running = True

        # Przygotowanie UI
        self.init_ui()

    def init_ui(self):
        # ========== Etykieta tytułu (wyróżniony tytuł) ==========
        title_label = tk.Label(
            self.master,
            text="Horner Polynomial Calculator", 
            font=("Arial", 16, "bold")
        )
        title_label.pack(pady=10)

         # ========== Etykieta podtytułu ==========
        subtitle_label = tk.Label(
            self.master,
            text="by krzsztfwtk\ngithub.com/krzsztfwtk", 
            font=("Arial", 12, "italic")
        )
        subtitle_label.pack(pady=5)

        # ========== Główna siatka 2x2 ========== 
        grid_frame = tk.Frame(self.master)
        grid_frame.pack(fill=tk.BOTH, expand=True)

        # Konfiguracja wierszy i kolumn (4 równe części)
        grid_frame.rowconfigure(0, weight=1, uniform="row")
        grid_frame.rowconfigure(1, weight=1, uniform="row")
        grid_frame.columnconfigure(0, weight=1, uniform="col")
        grid_frame.columnconfigure(1, weight=1, uniform="col")

        # ========== 1) Górna-lewa część: Implementacja (C++/ASM) ==========
        top_left_frame = tk.LabelFrame(
            grid_frame,
            text="Select Implementation", 
            font=("Arial", 10, "bold"),
            padx=10,
            pady=10
        )
        top_left_frame.grid(row=0, column=0, sticky="nsew", padx=5, pady=5)

        # Radiobutton z logo C++
        self.cpp_radio = tk.Radiobutton(
            top_left_frame,
            image=self.cpp_logo,     # obrazek
            variable=self.impl_choice,
            value='cpp',
            anchor='w',
            cursor="hand2",
        )
        self.cpp_radio.pack(anchor='w', pady=2)

        # Radiobutton z logo ASM
        self.asm_radio = tk.Radiobutton(
            top_left_frame,
            image=self.asm_logo,     # obrazek
            variable=self.impl_choice,
            value='asm',
            anchor='w',
            cursor="hand2",
        )
        self.asm_radio.pack(anchor='w', pady=2)

        # ========== 2) Górna-prawa część: Opcje (multithreading, AVX, itp.) ==========
        top_right_frame = tk.LabelFrame(
            grid_frame,
            text="Options", 
            font=("Arial", 10, "bold"),
            padx=10,
            pady=10
        )
        top_right_frame.grid(row=0, column=1, sticky="nsew", padx=5, pady=5)

        self.mt_checkbox = tk.Checkbutton(
            top_right_frame,
            text="Enable Multithreading",
            variable=self.use_multithreading,
            command=self.toggle_threads_spinbox,
            cursor="hand2"
        )
        self.mt_checkbox.pack(anchor='w', pady=2)

        # Spinbox w ramce (dla liczby wątków)
        threads_frame = tk.Frame(top_right_frame)
        threads_frame.pack(anchor='w', pady=2)
        tk.Label(threads_frame, text="Number of Threads:").pack(side=tk.LEFT, padx=5)
        self.threads_spinbox = tk.Spinbox(
            threads_frame,
            from_=1,
            to=64,
            width=5,
            command=self.update_threads_number,
            state='disabled'  # domyślnie wyłączony, włącza się po zaznaczeniu checkboxa
        )
        self.threads_spinbox.pack(side=tk.LEFT)
        self.threads_spinbox.delete(0, tk.END)
        self.threads_spinbox.insert(0, self.threads_number)

        self.avx_checkbox = tk.Checkbutton(
            top_right_frame,
            text="Enable AVX",
            variable=self.use_avx,
            cursor="hand2"
        )
        self.avx_checkbox.pack(anchor='w', pady=2)

        self.save_results_checkbox = tk.Checkbutton(
            top_right_frame,
            text="Save Output File",
            variable=self.save_results,
            cursor="hand2"
        )
        self.save_results_checkbox.pack(anchor='w', pady=2)

        self.chart_checkbox = tk.Checkbutton(
            top_right_frame,
            text="Generate Chart",
            variable=self.generate_chart,
            cursor="hand2"
        )
        self.chart_checkbox.pack(anchor='w', pady=2)

        # ========== 3) Dolna-lewa część: Coefficients & Points ==========
        bottom_left_frame = tk.Frame(grid_frame, padx=10, pady=10)
        bottom_left_frame.grid(row=1, column=0, sticky="nsew", padx=5, pady=5)

        # --- Ramka Coefficients ---
        coeff_frame = tk.LabelFrame(
            bottom_left_frame,
            text="Coefficients", 
            font=("Arial", 9, "bold"),
            padx=10,
            pady=10
        )
        coeff_frame.pack(fill=tk.BOTH, expand=True, pady=5)

        self.load_coeff_button = tk.Button(
            coeff_frame,
            text="Load Coefficients File",
            command=self.load_coefficients,
            width=22,
            cursor="hand2",
            activeforeground="black"
        )
        self.load_coeff_button.pack(anchor='w', pady=2)

        self.gen_coeff_button = tk.Button(
            coeff_frame,
            text="Generate Coefficients",
            command=self.generate_coefficients,
            width=22,
            cursor="hand2",
            activeforeground="black"
        )
        self.gen_coeff_button.pack(anchor='w', pady=2)

        self.coeff_file_label = tk.Label(
            coeff_frame,
            text="No file selected",
            fg="blue"
        )
        self.coeff_file_label.pack(anchor='w', pady=2)

        # --- Ramka Points ---
        points_frame = tk.LabelFrame(
            bottom_left_frame,
            text="Points", 
            font=("Arial", 9, "bold"),
            padx=10,
            pady=10
        )
        points_frame.pack(fill=tk.BOTH, expand=True, pady=5)

        self.load_points_button = tk.Button(
            points_frame,
            text="Load Points File",
            command=self.load_points,
            width=22,
            cursor="hand2",
            activeforeground="black"
        )
        self.load_points_button.pack(anchor='w', pady=2)

        self.gen_points_button = tk.Button(
            points_frame,
            text="Generate Points",
            command=self.generate_points,
            width=22,
            cursor="hand2",
            activeforeground="black"
        )
        self.gen_points_button.pack(anchor='w', pady=2)

        self.points_file_label = tk.Label(
            points_frame,
            text="No file selected",
            fg="blue"
        )
        self.points_file_label.pack(anchor='w', pady=2)

        # ========== 4) Dolna-prawa część: Output + Start Computation ==========
        bottom_right_frame = tk.Frame(grid_frame, padx=10, pady=10)
        bottom_right_frame.grid(row=1, column=1, sticky="nsew", padx=5, pady=5)

        output_frame = tk.LabelFrame(
            bottom_right_frame,
            text="Output",
            font=("Arial", 9, "bold"),
            padx=10,
            pady=10
        )
        output_frame.pack(fill=tk.BOTH, expand=True, pady=5)

        self.choose_output_button = tk.Button(
            output_frame,
            text="Choose Output File",
            command=self.choose_output_file,
            width=22,
            cursor="hand2",
            activeforeground="black"
        )
        self.choose_output_button.pack(anchor='w', pady=2)

        self.output_file_label = tk.Label(
            output_frame,
            text="No file selected",
            fg="blue"
        )
        self.output_file_label.pack(anchor='w', pady=2)

        # Duży przycisk Start Computation
        self.compute_button = tk.Button(
            bottom_right_frame,
            text="Sᴛᴀʀᴛ Cᴏᴍᴘᴜᴛᴀᴛɪᴏɴ",
            font=("Arial", 14, "bold"),
            fg="#eeeeee",
            bg="#1C7ED6",
            activebackground="#339AF0",
            activeforeground="#ffffff",  # Tekst pozostaje biały po przytrzymaniu
            cursor="hand2",
            height=2,
            width=20,
            command=self.start_computation
        )
        self.compute_button.pack(fill=tk.X, expand=False, pady=(30, 5))

        # ========== Eksperymenty ==========

        # 1) Przycisk "Run Experiment"
        self.run_experiment_button = tk.Button(
            bottom_right_frame,
            text="Run Experiment",
            fg="#ffffff",
            bg="#e67700",           # Pomarańczowy
            activebackground="#ff922b",
            activeforeground="#ffffff",
            cursor="hand2",
            command=lambda: run_experiment(self)  # Wywołanie funkcji z pliku experiment.py
        )
        self.run_experiment_button.pack(fill=tk.X, expand=False, pady=(10, 5))

        # 2) Przycisk "Experiment Info"
        self.experiment_info_button = tk.Button(
            bottom_right_frame,
            text="Experiment Info",
            command=show_experiment_info,
            cursor="hand2"
        )
        self.experiment_info_button.pack(fill=tk.X, expand=False, pady=(10, 5))

        # ========== Pasek postępu na samym dole, pełna szerokość ==========
        progress_frame = tk.Frame(self.master, padx=10, pady=5)
        progress_frame.pack(fill=tk.X)

        # Etykieta statusu wyśrodkowana nad paskiem postępu
        self.status_label = tk.Label(
            progress_frame,
            text="Ready",
            font=("Arial", 10, "italic")
        )
        self.status_label.pack()

        self.progress = Progressbar(
            progress_frame,
            orient=tk.HORIZONTAL,
            length=100,
            mode='determinate',
            maximum=100
        )
        self.progress.pack(fill=tk.X, expand=True, pady=5)

        # Zamykanie okna
        self.master.protocol("WM_DELETE_WINDOW", self.on_closing)

    # =============================================================================
    # ============================= Metody pomocnicze =============================
    # =============================================================================

    def update_threads_number(self):
        try:
            self.threads_number = int(self.threads_spinbox.get())
        except ValueError:
            messagebox.showerror("Error", "Please enter a valid number of threads.")

    def toggle_threads_spinbox(self):
        if self.use_multithreading.get():
            self.threads_spinbox.config(state='normal')
        else:
            self.threads_spinbox.config(state='disabled')

    def load_coefficients(self):
        filename = load_file("Select Coefficients File", [("Coefficients Files", "*.coeffs"), ("All Files", "*.*")])
        if filename:
            self.coefficients_file = filename
            self.coeff_file_label.config(text=os.path.basename(filename))

    def load_points(self):
        filename = load_file("Select Points File", [("Points Files", "*.points"), ("All Files", "*.*")])
        if filename:
            self.points_file = filename
            self.points_file_label.config(text=os.path.basename(filename))

    def choose_output_file(self):
        filename = filedialog.asksaveasfilename(
            title="Select Output File",
            defaultextension=".out",
            filetypes=[("Output Files", "*.out"), ("All Files", "*.*")]
        )
        if filename:
            self.output_file = filename
            self.output_file_label.config(text=os.path.basename(filename))

    def generate_coefficients(self):
        generate_coefficients_file(self)

    def generate_points(self):
        generate_points_file(self)

    def start_computation(self):
        # Walidacja przed uruchomieniem
        if not self.coefficients_file or not os.path.exists(self.coefficients_file):
            messagebox.showerror("Error", "Please load or generate coefficients file first.")
            return

        if not self.points_file or not os.path.exists(self.points_file):
            messagebox.showerror("Error", "Please load or generate points file first.")
            return

        if self.save_results.get() and not self.output_file:
            messagebox.showerror("Error", "Please choose an output file.")
            return

        # Rozpocznij obliczenia
        self.compute_button.config(state='disabled')
        self.status_label.config(text="Computing...")
        self.progress['value'] = 0
        threading.Thread(target=self.compute_polynomial).start()

    def compute_polynomial(self):
        try:
            # Upewnij się, że katalog temp istnieje
            if not os.path.exists(conf.TEMP_DIR):
                os.makedirs(conf.TEMP_DIR)

            # Usuń poprzedni plik postępu
            progress_file = os.path.abspath(conf.PROGRESS_FILE)
            if os.path.exists(progress_file):
                os.remove(progress_file)

            # Zapisz config
            write_config_file(
                self.impl_choice.get(),
                self.use_multithreading.get(),
                self.threads_number,
                self.use_avx.get(),
                self.save_results.get(),
                self.generate_chart.get(),
                self.output_file,
                self.coefficients_file,
                self.points_file
            )

            # Wyzwól zdarzenie (uruchom program C++/ASM w tle)
            trigger_event(conf.EVENT_NAME)

            # Monitoruj postęp i zakończenie
            self.monitor_thread = threading.Thread(
                target=monitor_progress_and_completion,
                args=(self,)
            )
            self.monitor_thread.start()

        except Exception as e:
            messagebox.showerror("Error", f"An error occurred: {e}")
            self.compute_button.config(state='normal')
            self.status_label.config(text="Error")
            self.progress.stop()

    def display_results(self, computation_time, points, results):
        # Po zakończeniu obliczeń
        messagebox.showinfo("Computation Complete", f"Computation completed in {computation_time:.4f} seconds.")
        self.status_label.config(text=f"Finished in {computation_time:.4f}s")

        # Zakończ jeżeli wyniki nie są zapisane
        if results is None:
            if self.generate_chart.get():
                messagebox.showwarning(
                    "Warning",
                    'Cannot generate chart because results are not available. Ensure you have enabled option "Save Outpuut File".',
                    
                )
            return

        # Zrównaj długość listy punktów i wyników w razie nadmiaru
        if len(points) > len(results):
            points = points[:len(results)]

        plot_results(self, points, results)


    def on_closing(self):
        self.is_running = False
        self.master.destroy()
        if hasattr(self, 'monitor_thread') and self.monitor_thread.is_alive():
            self.monitor_thread.join(timeout=1)

