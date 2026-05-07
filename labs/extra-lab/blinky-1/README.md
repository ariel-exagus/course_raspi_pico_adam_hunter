# Lab 1 - week 1

## I. Propósito del Primer Laboratorio (Lab 1)

El objetivo fundamental de Lab 1 es instalar el entorno de desarrollo y crear su primer programa embebido: hacer parpadear un LED (Blinky).
Aunque el programa es simple, te exige dominar la configuración de software necesaria para compilar y cargar código en el microcontrolador RP2040.

## II. Componentes Clave del Programa Blinky

- **Inclusión de la Librería:** Se incluye la librería de alto nivel `pico_standard_lib`. Esta librería jala otras librerías de soporte de bajo nivel, como la interfaz UART, permitiendo la comunicación serial.

- **Definición del Pin:** Se asocia el pin 25 (donde se encuentra el LED integrado en la placa) con la etiqueta `LED_PIN` usando `#define`.

- **Configuración del GPIO:** Se usan las funciones del SDK para inicializar el pin (configurarlo para uso de E/S desde el procesador, `gpio_init`) y luego establecer su dirección como salida (`gpio_set_dir`).

- **Lógica del Bucle:** Se utiliza un bucle `while(true)` que alternativamente pone un nivel alto (`gpio_put(LED_PIN, 1)`) y luego un nivel bajo (`gpio_put(LED_PIN, 0)`) en el pin, con una función de retardo (`sleep`) entre cada cambio para crear el parpadeo.

## III. Plan de Implementación Sugerido

Para abordar Lab 1, seguiremos estos tres pasos principales:

1. **Configuración del Entorno:** Instalar la extensión de VS Code y el driver necesario (especialmente importante para máquinas Windows).

2. **Construcción del Ejemplo:** Crear un nuevo proyecto a partir del ejemplo Blinky utilizando la extensión de VS Code.

3. **Compilación y Ejecución:** Compilar el código y cargarlo en la placa Pico para verificar su funcionamiento.