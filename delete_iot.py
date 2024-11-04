# delete_iot.py
from flask import request, jsonify
from database import get_connection

def delete_iot():
    data = request.json
    record_id = data.get('id')

    connection = get_connection()
    try:
        with connection.cursor() as cursor:
            # SQL para eliminar el registro
            sql = "DELETE FROM IoTCarStatus WHERE id = %s"
            cursor.execute(sql, (record_id,))
        connection.commit()
        return jsonify({'message': 'Registro eliminado exitosamente'}), 200
    except Exception as e:
        return jsonify({'error': str(e)}), 500
    finally:
        connection.close()
