# read_iot.py
from flask import jsonify
from database import get_connection

def read_iot():
    connection = get_connection()
    try:
        with connection.cursor() as cursor:
            sql = "SELECT * FROM IoTCarStatus" 
            cursor.execute(sql)
            result = cursor.fetchall()
        return jsonify(result), 200
    finally:
        connection.close()
