import os

EVENT_NAME = "Global\\ComputeEvent"
COMPLETION_EVENT_NAME = "Global\\CompletionEvent"
TEMP_DIR = os.path.abspath('temp')
CONFIG_FILE = os.path.join('config.ini')
PROGRESS_FILE = os.path.join(TEMP_DIR, 'progress.tmp')
COMPUTATION_TIME_FILE = os.path.join(TEMP_DIR, 'computation.time')
TIMEOUT_SECONDS = 600