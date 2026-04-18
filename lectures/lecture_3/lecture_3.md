# Resumen tercera clase curso Raspberry pi pico

Esta clase se centra en preparar el Laboratorio 1, cubriendo herramientas de depuración, la configuración de CMake y el uso de periféricos avanzados como el SPI y el Timer para la síntesis de audio.


1\. **Herramientas de Desarrollo y Depuración**

* **Depurador (Debugger):** Es una herramienta crítica para cargar programas, establecer puntos de interrupción (breakpoints) y observar el estado del sistema

* **Ventana de Periféricos:** Permite ver el valor exacto de cada registro de hardware en tiempo real (por ejemplo, verificar si un bit de un GPIO está en 1 o 0)

* **sleep vs. busy_wait:** Al depurar, se prefiere busy_wait porque sleep pone al RP2040 en modo de bajo consumo, lo que puede colgar el depurador

* **Solución en Windows:** Si el entorno falla en Windows, suele solucionarse instalando un controlador específico

2\. **Configuración de CMake (CMakeLists.txt)**

Es el archivo que indica al compilador cómo construir el proyecto
Sus comandos principales son:

* **add_executable:** Define el nombre del proyecto final

* **target_sources:** Enumera todos los archivos .c asociados al proyecto

* **target_link_libraries:** Vincula las bibliotecas del SDK (como hardware_spi o hardware_timer). Si se olvida, el compilador dará una advertencia de "definición implícita"

* **pico_add_extra_outputs:** Genera archivos (como .uf2) para flashear la placa vía USB

3\. **Comunicación SPI (Serial Peripheral Interface)**

Se utiliza para hablar con periféricos externos, como el DAC MCP4822 del laboratorio

* **Protocolo:** Es un sistema full-duplex (envía y recibe datos simultáneamente) que usa cuatro cables: Reloj (SCLK), MOSI (datos al periférico), MISO (datos al RP2040) y Chip Select (CS)

* **Configuración:** Se define la velocidad (bits por segundo), la polaridad del reloj y la fase según la hoja de datos del dispositivo externo. Para el DAC MCP4822, se usan transferencias de 16 bits
.
4\. **Periférico de Temporizador (Timer) e Interrupciones**

El RP2040 tiene un temporizador de hardware de 64 bits que aumenta cada microsegundo

* **Alarmas:** Tiene 4 alarmas (0-3). Se puede programar una alarma para que dispare una interrupción (ISR) cuando el temporizador alcance un valor específico

* **Limitación:** Las alarmas solo ven los 32 bits inferiores, por lo que solo pueden programarse con hasta ~70 minutos de antelación

* **Repetición:** A diferencia de otros micros, para que una tarea se repita (como en la síntesis de audio a 50kHz), se debe reprogramar la alarma manualmente dentro de la propia función de interrupción

5\. **Conceptos de Programación**

* **volatile:** Se usa para variables que cambian dentro de una interrupción. Esto le dice al compilador que no "cachee" el valor y que lo busque siempre en la memoria real

* **Síntesis de Audio:** Se usará un algoritmo llamado DDS (Direct Digital Synthesis) dentro de una interrupción de temporizador para generar tonos puros
