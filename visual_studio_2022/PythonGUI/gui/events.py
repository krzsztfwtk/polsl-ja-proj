import os
import time
import win32event
import win32api
import win32con
from tkinter import messagebox

import gui.config as conf
from gui.file_utils import read_results_and_display


def trigger_event(event_name):
    hEvent = win32event.OpenEvent(win32con.EVENT_MODIFY_STATE, False, event_name)
    if not hEvent:
        raise Exception("Failed to open event.")
    win32event.SetEvent(hEvent)
    win32api.CloseHandle(hEvent)


def monitor_progress_and_completion(app):
    try:
        progress_file = os.path.abspath(conf.PROGRESS_FILE)

        # Otwórz zdarzenie zakończenia
        hCompletionEvent = win32event.OpenEvent(win32con.SYNCHRONIZE, False, conf.COMPLETION_EVENT_NAME)
        if not hCompletionEvent:
            raise Exception("Failed to open completion event.")

        # Reset paska postępu
        app.progress['value'] = 0

        while app.is_running:
            # Sprawdź, czy obliczenia zostały zakończone
            result = win32event.WaitForSingleObject(hCompletionEvent, 100)  # Timeout ustawiony na 100 ms
            if result == win32con.WAIT_OBJECT_0:
                # Obliczenia zakończone
                app.master.after(0, app.status_label.config, {"text": "Computation Complete."})
                break  # Przerwij pętlę po wykryciu zakończenia

            # Odczytaj postęp
            if os.path.exists(progress_file):
                with open(progress_file, 'r') as f:
                    progress = f.read().strip()
                    if progress:
                        progress = int(progress)
                        app.master.after(0, lambda: app.progress.config(value=progress))
                        app.master.after(0, app.status_label.config, {"text": f"Computing... {progress}%"})

            time.sleep(0.1)

        win32api.CloseHandle(hCompletionEvent)

        if app.is_running:
            # Odczytaj czas obliczeń i wyniki tylko jeśli aplikacja nadal działa
            read_results_and_display(app)

    except Exception as e:
        if app.is_running:
            messagebox.showerror("Error", f"An error occurred during monitoring: {e}")
            app.compute_button.config(state='normal')
            app.status_label.config(text="Error")
            app.progress.stop()


def on_closing(self):
    self.is_running = False  # Ustaw flagę na False
    self.master.destroy()
    if hasattr(self, 'monitor_thread') and self.monitor_thread.is_alive():
        self.monitor_thread.join(timeout=1)