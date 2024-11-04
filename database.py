# database.py
import pymysql

def get_connection():
    return pymysql.connect(
        host='instancia-db-iot.cb20k2iyo0q0.us-east-1.rds.amazonaws.com',
        user='admin',  # Reemplaza con tu usuario de la base de datos
        password='Admin12345#!',  # Reemplaza con tu contraseña
        db='db_iot',
        charset='utf8mb4',
        cursorclass=pymysql.cursors.DictCursor
    )

# Prueba de conexión
try:
    connection = get_connection()
    print("Conexión exitosa a la base de datos")
    connection.close()  # Cierra la conexión cuando termines
except pymysql.MySQLError as e:
    print(f"Error al conectarse a la base de datos: {e}")