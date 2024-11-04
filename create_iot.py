from flask import request, jsonify
from database import get_connection

def create_iot():
    data = request.json

    # Validar que los campos requeridos están presentes
    status = data.get('status')
    id_device = data.get('id_device')

    if status is None or id_device is None:
        return jsonify({'error': 'Faltan campos requeridos: status y id_device'}), 400

    # Capturar la IP del cliente
    ip_client = request.remote_addr
    
    # Asignar un valor fijo al campo name
    name = "Marcos"

    connection = get_connection()
    try:
        with connection.cursor() as cursor:
            sql = "INSERT INTO IoTCarStatus (status, ip_cliente, name, id_device) VALUES (%s, %s, %s, %s)"
            cursor.execute(sql, (status, ip_client, name, id_device))
        connection.commit()
        
        # Respuesta exitosa
        return jsonify({
            'message': 'Registro creado exitosamente',
            'data': {
                'status': status,
                'id_device': id_device,
                'ip_client': ip_client
            }
        }), 201

    except Exception as e:
        # Manejo de errores de base de datos
        return jsonify({'error': str(e)}), 500

    finally:
        connection.close()

