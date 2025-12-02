import json
import os
import tkinter
import plotly.express as px
from tkinter import filedialog
from Node import Node

def select_json_file():
    dialog = tkinter.Tk()
    dialog.withdraw()
    file_path = filedialog.askopenfilename(
        title    = "Select profile results",
        filetypes=[("JSON Files", "*.json"), ("All Files", "*.*")])
    return file_path

# Get json filepath and open file.
json_file = select_json_file()
if not json_file:
    print("No file selected, exiting.")
    exit()

print(f"Opening capture at {json_file}...\nThis might take a while...")

with open(json_file) as f:
    data = json.load(f)

events = sorted(data.get("traceEvents", []), key=lambda trace_event: trace_event['ts'])

root = Node("root")
stack = []

for e in events:
    name    = e['name']
    start   = e['ts']
    dur_ms  = e.get('dur', 0) / 1000.0
    end     = start + e.get('dur', 0)

    while stack and stack[-1][1] <= start:
        stack.pop()

    parent_node = stack[-1][0] if stack else root
    node = parent_node.add_child(name)

    node.total += dur_ms
    node.num_calls += 1
    node.min = min(node.min, dur_ms)
    node.max = max(node.max, dur_ms)

    stack.append((node, end))

history = []
def clear_console():
    os.system('cls' if os.name == 'nt' else 'clear')

def print_bar(value, total_value, length=30):
    bar_length = int((value / total_value) * length)
    return "[" + ("#" * bar_length) + ("-" * (length - bar_length)) + "]"

def plot_and_choose(plot_node):
    children = list(plot_node.children.values())
    if not children:
        print(f"{plot_node.name} has no children.")
        if history:
            plot_node = history.pop()
            plot_and_choose(plot_node)
        return

    labels = [c.name for c in children]
    values = [c.total for c in children]
    hover_text = [
        f"Total: {c.total:.2f} ms | Avg: {c.total/c.num_calls:.2f} ms | "
        f"Min: {c.min:.2f} ms | Max: {c.max:.2f} ms | Calls: {c.num_calls}"
        for c in children
    ]

    fig = px.pie(
        names=labels,
        values=values,
        title=f"Functions called by '{plot_node.name}'",
        hover_data={'hover': hover_text}
    )
    fig.update_traces(hovertext=hover_text, hoverinfo="label+text+percent")
    fig.show()

    clear_console()
    print(f"=== Functions under '{plot_node.name}' ===\n")

    max_name_len = max(len(c.name) for c in children)
    total_sum = sum(c.total for c in children)

    for i, c in enumerate(children):
        bar = print_bar(c.total, total_sum)
        percent = (c.total / total_sum) * 100
        choice_str = f"[{i:02d}]"
        print(f"{choice_str} {c.name:{max_name_len}} {bar} {percent:5.1f}% ({c.total:.2f} ms, calls {c.num_calls} - avg {c.total / c.num_calls * 1000:.2f} us)")

    print("\n[b] Go back | [q] Quit")

    choice = input("Enter choice: ").strip()
    if choice == "b" and history:
        plot_node = history.pop()
        plot_and_choose(plot_node)
    elif choice == "q":
        return
    elif choice.isdigit() and int(choice) < len(children):
        history.append(plot_node)
        plot_and_choose(children[int(choice)])
    else:
        print("Invalid choice, try again.")
        plot_and_choose(plot_node)

plot_and_choose(root)
