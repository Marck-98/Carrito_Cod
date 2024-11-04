const apiUrl = 'http://54.89.198.38:5000/iot';// URL de la API
const idDevice = "19982610";

const acciones = {
    curvaIzquierda: 1,
    avanzar: 2,
    curvaDerecha: 3,
    girarIzquierda: 4,
    detener: 5,
    girarDerecha: 6,
    rutina: 7,
    retroceder: 8,
    obstaculos: 9
};

// Función para enviar la acción al servidor
async function enviarAccion(accion) {
    const statusValue = acciones[accion];
    const boton = document.getElementById(accion);

    // Cambiar temporalmente el color del botón
    boton.classList.add("active");
    setTimeout(() => boton.classList.remove("active"), 200);

    try {
        console.log(`Enviando acción: ${accion}, status: ${statusValue}`);
        const response = await fetch(apiUrl, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({ 
                status: statusValue, 
                id_device: idDevice
            })
        });

        if (response.ok) {
            document.getElementById('mensaje').textContent = 'Registro agregado exitosamente a la BD';
            console.log('Registro agregado exitosamente a la BD');

            // Actualizar la tabla con el último registro
            await obtenerUltimoRegistro(); 

            setTimeout(() => {
                document.getElementById('mensaje').textContent = '';
            }, 3000);
        } else {
            console.error('Error al enviar la acción:', response.statusText);
        }
    } catch (error) {
        console.error('Error en la solicitud:', error);
    }
}

// Función para obtener el último registro de la API y mostrarlo en la tabla
async function obtenerUltimoRegistro() {
    try {
        console.log('Obteniendo el último registro de la API...');
        const response = await fetch(apiUrl);
        if (response.ok) {
            const data = await response.json();
            console.log("Datos obtenidos de la API:", data);

            // Verificar si hay registros
            if (data.length === 0) {
                console.warn("No se encontraron registros en la base de datos.");
                document.getElementById('mensaje').textContent = 'No se encontraron registros en la base de datos.';
                return;
            }

            // Seleccionar el último registro
            const ultimoRegistro = data[data.length - 1];
            console.log("Último registro:", ultimoRegistro);

            // Seleccionar el tbody de la tabla y limpiar su contenido previo
            const tbody = document.getElementById('tablaUltimoRegistro').querySelector('tbody');
            tbody.innerHTML = '';

            // Crear una nueva fila con los datos del último registro
            const fila = document.createElement('tr');
            fila.innerHTML = `
                <td>${ultimoRegistro.id}</td>
                <td>${ultimoRegistro.name || 'N/A'}</td>
                <td>${ultimoRegistro.id_device || 'N/A'}</td>
                <td>${obtenerNombreAccion(ultimoRegistro.status)}</td>
                <td>${ultimoRegistro.status}</td>
                <td>${new Date(ultimoRegistro.date).toLocaleString()}</td>
            `;
            tbody.appendChild(fila); // Añadir la fila a la tabla
        } else {
            console.error('Error al obtener el último registro:', response.statusText);
        }
    } catch (error) {
        console.error('Error en la solicitud:', error);
    }
}

// Función para obtener el nombre de la acción basada en el código de estado
function obtenerNombreAccion(status) {
    const acciones = {
        1: 'Curva Izquierda',
        2: 'Avanzar',
        3: 'Curva Derecha',
        4: 'Girar Izquierda',
        5: 'Detener',
        6: 'Girar Derecha',
        7: 'Rutina',
        8: 'Retroceder',
        9: 'Obstáculos'
    };
    return acciones[status] || 'Desconocida';
}


// Llamar a la función al cargar la página para mostrar el último registro
window.onload = obtenerUltimoRegistro;
