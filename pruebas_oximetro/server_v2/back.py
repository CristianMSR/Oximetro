from flask import Flask, request, jsonify, render_template_string

app = Flask(__name__)

analog_values = []
ppm_value = 0
o2_value = 0

@app.route('/')
def index():
    global ppm_value, o2_value
    try:
        with open('index.html', 'r', encoding='utf-8') as file:
            html_content = file.read()
        return render_template_string(html_content, ppm_value=ppm_value, o2_value=o2_value)
    except Exception as e:
        return f"Error al cargar la página: {str(e)}"

@app.route('/data', methods=['POST'])
def receive_data():
    global analog_values, ppm_value, o2_value
    try:
        data = request.get_json()  # Obtiene los datos como JSON
        analog_values = data.get('analog_values', [])
        ppm_value = data.get('ppm_value', 0)
        o2_value = data.get('o2_value', 0)

        if not isinstance(analog_values, list) or not all(isinstance(val, (float, int)) for val in analog_values):
            raise ValueError("Formato de 'analog_values' incorrecto")
        
        print(f"Valores analógicos recibidos: {analog_values[:10]}...")  # Muestra los primeros 10 valores para verificar
        print(f"PPM recibido: {ppm_value}")
        print(f"SpO2 recibido: {o2_value}")
    except (ValueError, TypeError) as e:
        print(f"Error al convertir los datos recibidos: {e}")
    return "Datos recibidos"

@app.route('/data.json')
def data_json():
    global analog_values, ppm_value, o2_value  # `analog_values` es el arreglo de 100 datos en el backend
    return {
        'analog_values': analog_values,  # Envía el arreglo de datos analógicos
        'ppm_value': ppm_value,
        'o2_value': o2_value
    }

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)
