import os
import re
import tkinter
from tkinter import filedialog

def select_directory():
    dialog = tkinter.Tk()
    dialog.withdraw()
    folder = filedialog.askdirectory(title="Select Plots Folder")
    return folder

def parse_txt_file(text_capture_path):
    average_ms      = None
    number_of_calls = None
    with open(text_capture_path, "r") as f:
        for line in f:
            # \s       -> whitespace between string literal and next token.
            # (\d+)    -> any amount of characters that match a digit 0-9 and captures into first group  => This is number of calls.
            # ([\d.]+) -> any amount of characters that match a digit 0-9 and captures into second group => This is average.
            matches = re.search(r"calls\s(\d+)\s-\savg\s([\d.]+)\s*us", line)

            if matches:
                number_of_calls = int(matches.group(1))
                average_ms      = float(matches.group(2)) / 1000.0
            if average_ms is not None or number_of_calls is not None:
                break
    f_name = os.path.basename(txt_path)

    # (\d+)  -> any amount of characters that match a digit 0-9 and captures into first group => This is level.
    # (.+)   -> any amount of characters that aren't '.' and captures into second group       => This is the stack path for the capture.
    matches = re.match(r"level_(\d+)__(.+)\.txt", f_name)
    if not matches:
        return None
    level_from_path = int(matches.group(1))
    stack_path      = matches.group(2).replace("__", "/")
    return level_from_path, stack_path, average_ms, number_of_calls

plots_dir = select_directory()
if not plots_dir:
    print("No directory selected, exiting.")
    exit()

captures = [d for d in os.listdir(plots_dir) if os.path.isdir(os.path.join(plots_dir, d))]
data = {}

for cap in captures:
    cap_path = os.path.join(plots_dir, cap)
    for root_dir, _, files in os.walk(cap_path):
        for file in files:
            if file.endswith(".txt"):
                txt_path = os.path.join(root_dir, file)
                parsed = parse_txt_file(txt_path)
                if parsed:
                    level, path, avg_ms, num_calls = parsed
                    key = (level, path)
                    if key not in data:
                        data[key] = {}
                    data[key][cap] = (avg_ms, num_calls)

common_nodes = [k for k, v in data.items() if len(v) > 1]

print(f"Found {len(common_nodes)} nodes present in multiple captures:\n")

for level, path in sorted(common_nodes):
    print(f"Level {level}, {path}")
    captures_data = data[(level, path)]
    for cap_name, (avg, calls) in captures_data.items():
        print(f"-> '{cap_name}': {avg*1000:.3f} us ({calls} calls)")
    captures_sorted = sorted(captures_data.items())
    base_avg = captures_sorted[0][1][0]
    for cap_name, (avg, _) in captures_sorted[1:]:
        delta = avg - base_avg
        sign = "+" if delta > 0 else "-"
        print(f"'{cap_name}' change vs '{captures_sorted[0][0]}': {sign}{abs(delta*1000):.3f} us")

    best_capture, (best_avg, best_calls) = min(captures_data.items(), key=lambda x: x[1][0])

    min_calls = min(calls for _, (_, calls) in captures_data.items())
    time_saved_ms = (sum(avg for _, (avg, _) in captures_data.items()) / len(captures_data) - best_avg) * min_calls
    print(f" === Best: '{best_capture}' [Estimated saved {time_saved_ms * 1000:.3f} us] ===\n")
