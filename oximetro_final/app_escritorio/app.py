import tkinter as tk
from tkinter import messagebox
from PIL import ImageTk, Image
import serial
import os
import sys

# Configuración del puerto serie
SERIAL_PORT = 'COM3'  # Cambia este puerto según el tuyo
BAUD_RATE = 115200    # Velocidad del puerto serie

# Función para enviar datos a la ESP32
def send_to_esp32(event=None):  # Se agrega `event=None` para que funcione con la tecla Enter
    ssid = ssid_entry.get()
    password = password_entry.get()
    
    if not ssid or not password:
        messagebox.showwarning("Advertencia", "Por favor, complete ambos campos.")
        return

    try:
        # Abre la conexión serial
        with serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1) as ser:
            # Envía el SSID y la contraseña
            ser.write(f"{ssid}\n".encode())
            ser.write(f"{password}\n".encode())
            messagebox.showinfo("Éxito", "Datos enviados correctamente a la ESP32")
    except serial.SerialException as error:
        messagebox.showerror("Error", f"No se pudo abrir el puerto serie: {error}")

# Crear la ventana principal
root = tk.Tk()
root.title("Enviar SSID y Password a ESP32")

def resource_path(relative_path):
    """ Obtiene la ruta absoluta del recurso, usa PyInstaller """
    try:
        base_path = sys._MEIPASS
    except Exception:
        base_path = os.path.abspath(".")

    return os.path.join(base_path, relative_path)

# Cargar la imagen de fondo
image_path = resource_path("fondo.jpg")
bg_image = Image.open(image_path)
bg_photo = ImageTk.PhotoImage(bg_image)

# Obtener el tamaño de la imagen
img_width, img_height = bg_image.size

# Crear un label para la imagen de fondo
bg_label = tk.Label(root, image=bg_photo)
bg_label.place(x=0, y=0, relwidth=1, relheight=1)  # Expande la imagen al tamaño de la ventana

# Crear un marco para centrar los widgets
frame = tk.Frame(root, bg='white')
frame.place(relx=0.5, rely=0.5, anchor='center')  # Centrar el frame

# Etiqueta y campo de entrada para SSID
ssid_label = tk.Label(frame, text="SSID:", bg='white')
ssid_label.grid(row=0, column=0, padx=10, pady=10)
ssid_entry = tk.Entry(frame)
ssid_entry.grid(row=0, column=1, padx=10, pady=10)

# Etiqueta y campo de entrada para Password
password_label = tk.Label(frame, text="Password:", bg='white')
password_label.grid(row=1, column=0, padx=10, pady=10)
password_entry = tk.Entry(frame, show="*")  # Oculta el texto con asteriscos
password_entry.grid(row=1, column=1, padx=10, pady=10)

# Botón para enviar los datos
send_button = tk.Button(frame, text="Enviar", command=send_to_esp32)
send_button.grid(row=2, column=0, columnspan=2, padx=10, pady=10)

# Ajustar el tamaño de la ventana al tamaño de la imagen
root.geometry(f"{img_width}x{img_height}")

# Deshabilitar el redimensionado de la ventana
root.resizable(False, False)

# Vincular la tecla Enter para enviar los datos
root.bind('<Return>', send_to_esp32)  # Vincula la tecla Enter a la función send_to_esp32

# Iniciar el bucle principal de la interfaz gráfica
root.mainloop()
