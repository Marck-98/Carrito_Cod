from flask import Blueprint, jsonify
from database import get_connection

get_last_api = Blueprint('get_last_api', __name__)  # Corregido de _name_ a __name__

@get_last_api.route('/api/status/last', methods=['GET'])
def get_last_status():
    connection = get_connection()
    try:
        with connection.cursor() as cursor:
            sql = "SELECT * FROM IoTCarStatus ORDER BY date DESC LIMIT 1"
            cursor.execute(sql)
            result = cursor.fetchone()
            return jsonify(result) if result else jsonify({"error": "No records found"}), 404
    finally:
        connection.close()
