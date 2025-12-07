# Resumen segunda clase curso Raspberry pi pico

El video se centra en proporcionar una visión general del hardware interno del microcontrolador RP2040 y la infraestructura de software utilizada para configurarlo y controlarlo.

Aquí está el resumen de los temas, ejemplos prácticos, la explicación del SDK y las herramientas utilizadas:

Resumen de Temas Cubiertos

1\. **Hardware Interno (Periféricos) del RP2040:** Se describió el hardware interno, que los programas de la clase no solo incluyen instrucciones para los procesadores, sino también comandos de configuración y control para este hardware adicional. La lista de periféricos incluye:

* **Dos procesadores** ARM Cortex M0+ (que pueden ejecutar código de forma independiente).

* **12 canales DMA** (Acceso Directo a Memoria), que actúan como coprocesadores simples moviendo información de una ubicación de memoria a otra.

* **Dos canales UART**, utilizados para la conexión serial (por ejemplo, con la PC).

* **Dos canales SPI**.

* **Dos canales I2C**.

* **16 canales PWM** (Modulación por Ancho de Pulso).

* Un **controlador USB** (no usado en laboratorios, pero disponible para proyectos finales).

* **30 pines GPIO** (pines de propósito general).

* Un **ADC** (Convertidor Analógico a Digital) de 12 bits (con un multiplexor de entrada de 5 vías).

* **Ocho máquinas de estado PIO** (IO Programable), que son coprocesadores separados con su propio lenguaje de ensamblador de nueve instrucciones. Estos tienen conexión directa a todos los puertos IO.

* Un divisor de enteros de **ocho ciclos** optimizado por hardware.

* Un **interpolador** de hardware.

* 264 KB de SRAM en chip y una interfaz Quad SPI para la memoria flash externa (donde reside el código del programa).

* La arquitectura del sistema es **concurrente**, lo que significa que los periféricos pueden estar haciendo cosas al mismo tiempo que el código se ejecuta en los núcleos ARM.



2\. **Niveles de Abstracción Hardware/Software:** Se presentó un modelo jerárquico para el sistema.

* El nivel más bajo es el **Hardware** (los periféricos reales).

* El siguiente nivel es la manipulación de **Registros**, que son ubicaciones de memoria con direcciones conocidas utilizadas para la configuración y control del hardware.

* El nivel intermedio es el **C SDK** (Software Development Kit).

* Por encima están las **Hardware Support Libraries** (que manejan *structs* de hardware) y las **High-Level Libraries** (que implementan funcionalidades superiores).

3\. **Depuración y Documentación:** Se explicó la importancia de entender la jerarquía para depurar eficazmente. Se mostró cómo investigar funciones desconocidas del SDK utilizando la **guía de API de hardware** y explorando la implementación del SDK hasta llegar a la manipulación de registros en la **hoja de datos**.

Ejemplos Prácticos

El video proporcionó varios ejemplos de cómo los periféricos pueden colaborar, a menudo sin la intervención de la CPU:

- **Audio sin CPU (DMA y SPI/ADC):** Se puede configurar un canal DMA para tomar muestras del ADC (conectado a un micrófono) y moverlas directamente a un búfer de transmisión SPI. Si un DAC externo está conectado al canal SPI, la voz puede salir por un altavoz sin actividad de la CPU.

- **Radio AM (ADC y PWM):** Se puede muestrear el ADC y enviar ese dato a un registro que controla el ciclo de trabajo de un canal PWM. Si el PWM opera a una frecuencia de banda AM, el cable del PWM actúa como una antena, y la voz (modulada a través de la expansión de Taylor del término fundamental de la onda cuadrada) puede escucharse en una radio AM.

- **Gráficos de Alta Velocidad (PIO y DMA):** Se puede utilizar el coprocesador PIO, en colaboración con los canales DMA, para comunicarse con una pantalla VGA (que requiere una línea de datos de aproximadamente 25 MHz) sin utilizar tiempo de la CPU. Esto permite que el tiempo de la CPU se dedique completamente a la animación.

- **El Programa "Blinky" (Ejemplo Fundamental):** El primer programa que se escribe para un microcontrolador es "blink an LED". El código Blinky utiliza funciones del SDK como `gpio_init(LED_PIN)`, `gpio_set_dir(LED_PIN, GPIO_OUT)`, y un bucle que alterna `gpio_put(LED_PIN, 1)` y `gpio_put(LED_PIN, 0)`, con pausas (sleep), para hacer parpadear el LED integrado que está asociado al pin GP25.

¿Qué es el SDK según el Video?

El **SDK** (Software Development Kit o Kit de Desarrollo de Software) es una infraestructura de software proporcionada por los fabricantes de microcontroladores (en este caso, Raspberry Pi) para facilitar la configuración y el control del hardware interno.

Dado que el desarrollo directo manipulando miles de registros con direcciones numéricas sería extremadamente engorroso y propenso a errores, el C SDK proporciona esencialmente una biblioteca completa de funciones que **abstraen estas manipulaciones de registros**.

Por ejemplo, en lugar de configurar manualmente los bits de un registro específico para inicializar un canal SPI, el SDK ofrece una función de alto nivel, como `SPI_init`, que toma argumentos de configuración sencillos. Sin embargo, es fundamental entender que la implementación de estas funciones del SDK **generalmente solo está modificando los valores de los registros**. Entender esta conexión directa entre el SDK y los registros es clave para la depuración efectiva, ya que los recursos de hardware son agotables y el SDK puede consumir recursos subyacentes (como un temporizador o un *spin lock* de hardware) que el código de la aplicación podría necesitar.

Herramientas para el Trabajo

Para el desarrollo y la depuración se utilizarán las siguientes herramientas:

1\. **Visual Studio Code (VS Code):** Es el entorno de compilación y desarrollo utilizado para la clase.

2\. **Extensión de VS Code:** Una extensión específica debe ser instalada para facilitar la creación de proyectos a partir de ejemplos (como "new project from example") y la compilación.

3\. **Hoja de Datos (Data Sheet):** Documento oficial para consultar miles de registros, sus direcciones de memoria base y los *offsets*, y para entender la función de cada bit en la configuración del hardware.

4\. **Guía API de Hardware (Manual del SDK):** Se utiliza para investigar la funcionalidad de las funciones del C SDK.

5\. **Interfaz de Depuración (Debugging Interface):** Permite cargar el programa, establecer puntos de interrupción (*breakpoints*) y visualizar el valor de las variables, el mapa de memoria y la configuración de los registros en un entorno de depuración embebido.