from flask import Flask, request, render_template_string

app = Flask(__name__)

analog_value = 0
ppm_value = 0
o2_value = 0

@app.route('/')
def index():
    global analog_value, ppm_value, o2_value
    try:
        # Abre el archivo con codificación utf-8
        with open('index.html', 'r', encoding='utf-8') as file:
            html_content = file.read()
        return render_template_string(html_content, analog_value=analog_value, ppm_value=ppm_value, o2_value=o2_value)
    except Exception as e:
        # En caso de error, muestra el mensaje de error
        return f"Error al cargar la página: {str(e)}"

@app.route('/data', methods=['POST'])
def receive_data():
    global analog_value, ppm_value, o2_value
    try:
        analog_value = float(request.form.get('analog_value', 0))
        ppm_value = int(request.form.get('ppm_value', 0))
        o2_value = int(request.form.get('o2_value', 0))
        print(f"Valor analógico recibido: {analog_value}")
        print(f"PPM recibido: {ppm_value}")
        print(f"SpO2 recibido: {o2_value}")
    except ValueError:
        print("Error al convertir los datos recibidos")
    return "Datos recibidos"

@app.route('/data.json')
def data_json():
    global analog_value, ppm_value, o2_value
    return {
        'analog_value': analog_value,
        'ppm_value': ppm_value,
        'o2_value': o2_value
    }

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)
