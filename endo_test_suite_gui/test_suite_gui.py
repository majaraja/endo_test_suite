import tkinter as tk
from tkinter import ttk, filedialog, messagebox
import serial
import serial.tools.list_ports
import threading
import csv
import json
import time


class SerialGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("Serial JSON Sender & CSV Logger")

        self.ser = None
        self.running = False

        # COM Port selection with dropdown menu
        menubar = tk.Menu(root)
        root.config(menu=menubar)

        port_menu = tk.Menu(menubar, tearoff=0)
        menubar.add_cascade(label="COM Ports", menu=port_menu)

        def rebuild_port_menu():
            port_menu.delete(0, tk.END)
            for p in self.get_ports():
                port_menu.add_command(
                    label=p, command=lambda port=p: self.port_combo.set(port)
                )

        rebuild_port_menu()

        self.port_label = ttk.Label(root, text="Select COM Port:")
        self.port_label.pack()

        self.port_combo = ttk.Combobox(root, values=self.get_ports(), state="readonly")
        self.port_combo.pack(pady=5)

        self.refresh_button = ttk.Button(
            root,
            text="Refresh Ports",
            command=lambda: [self.refresh_ports(), rebuild_port_menu()],
        )
        self.refresh_button.pack(pady=5)

        # JSON input
        self.json_label = ttk.Label(root, text="JSON to Send:")
        self.json_label.pack()

        self.json_entry = tk.Text(root, height=5, width=40)
        self.json_entry.insert(
            "1.0",
            '{"insertion_speed": 1.0, "insertion_distance": 10.0, "rotation_speed": 50, "rotation_angle": 720, "roller_speed": 100.0}',
        )
        self.json_entry.pack(pady=5)

        # CSV file selection
        self.csv_button = ttk.Button(
            root, text="Select CSV File", command=self.select_csv
        )
        self.csv_button.pack(pady=5)

        self.csv_path_label = ttk.Label(root, text="No CSV selected")
        self.csv_path_label.pack()

        # tare button
        self.tare_button = ttk.Button(
            root, text="Tare Device", command=self.tare_device
        )
        self.tare_button.pack(pady=5)

        # Start / Stop buttons
        self.start_button = ttk.Button(root, text="Start", command=self.start)
        self.start_button.pack(pady=10)

        self.stop_button = ttk.Button(
            root, text="Stop", command=self.stop, state=tk.DISABLED
        )
        self.stop_button.pack(pady=5)

        # Log display
        self.log_box = tk.Text(root, height=10, width=60)
        self.log_box.pack(pady=10)

    def log(self, message):
        self.log_box.insert(tk.END, message + "\n")
        self.log_box.see(tk.END)

    def get_ports(self):
        return [p.device for p in serial.tools.list_ports.comports()]

    def refresh_ports(self):
        self.port_combo["values"] = self.get_ports()
        self.log("Ports refreshed.")

    def select_csv(self):
        filepath = filedialog.asksaveasfilename(
            defaultextension=".csv", filetypes=[("CSV Files", "*.csv")]
        )
        if filepath:
            self.csv_path = filepath
            self.csv_path_label.config(text=filepath)
            self.log(f"CSV selected: {filepath}")

    def start(self):
        if not hasattr(self, "csv_path"):
            messagebox.showerror("Error", "Select a CSV file first.")
            return

        port = self.port_combo.get()
        if not port:
            messagebox.showerror("Error", "Select a COM port.")
            return

        try:
            self.ser = serial.Serial(port, 115200, timeout=1)
        except Exception as e:
            messagebox.showerror("Error", f"Cannot open port: {e}")
            return

        time.sleep(2)

        # Send JSON
        try:
            json_text = self.json_entry.get("1.0", tk.END).strip()
            json.loads(json_text)  # Validate JSON
            self.ser.write((json_text + "\n").encode())
            self.log(f"Sent JSON: {json_text}")
        except Exception as e:
            messagebox.showerror("Error", f"Invalid JSON: {e}")
            return

        self.running = True
        self.start_button.config(state=tk.DISABLED)
        self.stop_button.config(state=tk.NORMAL)

        threading.Thread(target=self.read_serial, daemon=True).start()

    def read_serial(self):
        with open(self.csv_path, "a", newline="") as f:
            writer = csv.writer(f)

            while self.running:
                line = self.ser.readline().decode(errors="ignore").strip()
                if line:
                    self.log(f"RX: {line}")
                    fields = line.split(",")
                    writer.writerow(fields)
                    f.flush()

    def stop(self):
        self.running = False
        if self.ser:
            self.ser.close()
        self.start_button.config(state=tk.NORMAL)
        self.stop_button.config(state=tk.DISABLED)
        self.log("Stopped.")

    def tare_device(self):
        port = self.port_combo.get()
        if not port:
            messagebox.showerror("Error", "Select a COM port.")
            return

        try:
            with serial.Serial(port, 115200, timeout=1) as ser:
                time.sleep(2)
                ser.write(b'{"tare": true}\n')
                self.log("Sent TARE command.")
        except Exception as e:
            messagebox.showerror("Error", f"Cannot open port: {e}")


if __name__ == "__main__":
    root = tk.Tk()
    app = SerialGUI(root)
    root.mainloop()
