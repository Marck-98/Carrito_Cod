from flask import Flask, render_template, request, jsonify
from flask_cors import CORS
from flask_socketio import SocketIO  # Importa SocketIO
from create_iot import create_iot
from read_iot import read_iot
from update_iot import update_iot
from delete_iot import delete_iot
from database import get_connection  # Asegúrate de importar esto

app = Flask(__name__)
CORS(app)  # Permitir solicitudes CORS para toda la aplicación
socketio = SocketIO(app)  # Inicializa SocketIO

# Ruta para servir el archivo HTML en la ruta raíz
@app.route('/')
def home():
    return render_template('index.html')  # Carga el archivo HTML en la ruta raíz

@app.route('/iot', methods=['POST'])
def create():
    return create_iot()

@app.route('/iot', methods=['GET'])
def read():
    return read_iot()

@app.route('/iot', methods=['PUT'])
def update():
    return update_iot()

@app.route('/iot', methods=['DELETE'])
def delete():
    return delete_iot()

@app.route('/iot/last', methods=['GET'])
def get_last_record():
    connection = get_connection()
    try:
        with connection.cursor() as cursor:
            sql = "SELECT * FROM IoTCarStatus ORDER BY date DESC LIMIT 1"
            cursor.execute(sql)
            result = cursor.fetchone()
            return jsonify(result) if result else jsonify({"error": "No records found"}), 404
    finally:
        connection.close()

# Evento para el socket
@socketio.on('connect')
def handle_connect():
    print('Cliente conectado')

# Evento para enviar el último registro
def send_last_record():
    connection = get_connection()
    try:
        with connection.cursor() as cursor:
            sql = "SELECT * FROM IoTCarStatus ORDER BY date DESC LIMIT 1"
            cursor.execute(sql)
            result = cursor.fetchone()
            if result:
                socketio.emit('update', result)  # Envía el último registro al cliente
    finally:
        connection.close()

# Llama a la función `send_last_record` cada vez que se conecte un nuevo cliente
@socketio.on('request_last_record')
def request_last_record():
    send_last_record()

if __name__ == '__main__':
    socketio.run(app, host='0.0.0.0', port=5000, debug=True)  # Usa socketio.run en lugar de app.run
