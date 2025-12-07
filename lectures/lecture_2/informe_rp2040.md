Informe Técnico: Arquitectura del Microcontrolador RP2040
=========================================================

1.0 Introducción a la Arquitectura del RP2040
---------------------------------------------

El microcontrolador RP2040 es un diseño de alto rendimiento que integra un procesador de doble núcleo Arm Cortex-M0+, memoria interna de gran capacidad y un subsistema de E/S programable (PIO) único en su clase. Esta combinación de características lo posiciona como una solución potente y flexible para una amplia gama de aplicaciones de sistemas embebidos. El propósito de este informe es proporcionar un análisis técnico exhaustivo de su arquitectura, destacando los elementos clave que definen su rendimiento y versatilidad para ingenieros y desarrolladores de sistemas.

Las características fundamentales del chip RP2040 se pueden resumir en los siguientes puntos:

-   **Procesador de Doble Núcleo:** Incorpora dos procesadores Arm Cortex-M0+ idénticos.
-   **Memoria Interna:** Cuenta con una SRAM interna de 264 kB, físicamente particionada en 6 bancos para permitir accesos paralelos simultáneos desde diferentes maestros del bus.
-   **Acceso a Memoria Externa:** Permite la ejecución de código directamente desde memoria Flash externa (Execute-In-Place o XIP) a través de una interfaz dedicada SPI, DSPI o QSPI, mejorada por una pequeña caché para optimizar el rendimiento.
-   **Periféricos y DMA:** Incluye un controlador de Acceso Directo a Memoria (DMA) y un conjunto de periféricos conectados a través de una matriz de bus AHB/APB.
-   **Regulador de Voltaje Integrado:** Dispone de un regulador de voltaje interno para suministrar la tensión del núcleo, simplificando el diseño de la fuente de alimentación externa, que solo necesita proveer el voltaje para las E/S.
-   **Depuración:** Ofrece capacidades de depuración a través de una interfaz Serial Wire Debug (SWD).

A continuación, se presenta un análisis detallado de la arquitectura del sistema que sustenta estas capacidades.

2.0 Arquitectura General del Sistema
------------------------------------

La estructura de alto nivel del RP2040 está centrada en una matriz de bus (*bus fabric*) de alto ancho de banda que interconecta de manera eficiente los procesadores, las memorias y los periféricos. Esta arquitectura, basada en una matriz de conmutación AHB-Lite, está diseñada para permitir múltiples transferencias de datos en paralelo, eliminando cuellos de botella y garantizando un alto rendimiento del sistema.

La siguiente representación textual ilustra la interconexión jerárquica de los componentes principales del sistema:

```
               MAESTROS DEL BUS
      +-----------------+  +-----------------+
      |   Procesador 0  |  |   Procesador 1  |
      +-----------------+  +-----------------+
      | Controlador DMA |  | Controlador DMA |
      | (Puerto Lectura)|  | (Puerto Escritura)|
      +-----------------+  +-----------------+
                 |                |
                 +----------------+
                        |
      +-----------------------------------------+
      |      Matriz de Bus (AHB-Lite Crossbar)  |
      +-----------------------------------------+
      |        |        |          |            |
      |        |        |          |            +-----> Puente APB
      |        |        |          |                      |
      |        |        |          |                      +--> Periféricos APB
      |        |        |          |                           (UART, SPI, I2C...)
      |        |        |          |
+-------+  +-------+  +-------+  +-------------------------+
|  ROM  |  | SRAM  |  |  XIP  |  | Periféricos Rápidos AHB |
+-------+  +-------+  +-------+  +-------------------------+
           (6 Bancos) (Flash Ext) (PIO, USB, DMA Ctrl...)

                  ESCLAVOS DEL BUS

```

La importancia estratégica de esta arquitectura de matriz de conmutación (*crossbar*) radica en su capacidad para facilitar el acceso concurrente a los recursos del sistema. Hasta cuatro maestros del bus (los dos núcleos del procesador y los dos puertos del DMA) pueden acceder simultáneamente a cuatro puertos esclavos diferentes (como bancos de SRAM, ROM o periféricos) sin introducir estados de espera. A una frecuencia de reloj del sistema de 125 MHz, esta configuración permite alcanzar un ancho de banda máximo sostenido de 2.0 GB/s, lo que es fundamental para aplicaciones que requieren un procesamiento de datos intensivo y una baja latencia.

El siguiente apartado profundiza en el subsistema que alberga los núcleos de procesamiento.

3.0 Subsistema del Procesador
-----------------------------

El subsistema del procesador del RP2040 está compuesto por dos procesadores Arm Cortex-M0+ idénticos, complementados por un bloque especializado de E/S de ciclo único (SIO) diseñado para la comunicación inter-núcleo y el acceso a los pines GPIO con una latencia mínima. Cada procesador opera de forma independiente, con sus propios periféricos internos estándar, pero comparte el acceso a recursos del sistema a través de la matriz de bus principal.

La estructura del subsistema se puede visualizar de la siguiente manera:

```
                               +-----------------------------+
                               | Serial Wire Debug (SWD)     |
                               | (Interfaz Compartida)       |
                               +--------------+--------------+
                                              |
      +--------------------------+------------+------------+--------------------------+
      |                          |                         |                          |
+-----V-----------+        +-----V-----------++------------V----+               +-----V-----------+
| Procesador 0    |        | Bloque de E/S de Ciclo Único (SIO) |               | Procesador 1    |
| (Arm Cortex-M0+)|<------IOPORT----|      (Recurso Compartido) |---IOPORT----->|(Arm Cortex-M0+)|
+-----^-----------+        +------------------------------------+                +-----^-----------+
      |                                                                                |
      | AHB-Lite                                                                       | AHB-Lite
      |                                                                                |
      +----------------------------> Hacia Matriz de Bus <-----------------------------+

```

Los procesadores se comunican con el resto del sistema a través de varias interfaces clave:

-   **Bus AHB-Lite:** Cada núcleo dispone de su propio bus AHB-Lite independiente de 32 bits para acceder a la memoria y a los periféricos mapeados en memoria.
-   **IOPORT:** Cada procesador se conecta al bloque SIO a través de un puerto de bus auxiliar (IOPORT) que permite realizar lecturas y escrituras de 32 bits en un único ciclo de reloj, garantizando un acceso determinista y de alta velocidad a los GPIO y otros periféricos locales del SIO.
-   **Interrupciones:** Un total de 26 interrupciones a nivel de sistema se enrutan a los controladores de interrupciones vectorizadas anidadas (NVIC) de ambos procesadores.
-   **Serial Wire Debug (SWD):** Una única interfaz SWD multipunto proporciona acceso de depuración a ambos procesadores desde un host externo.

### 3.1 El Bloque de E/S de Ciclo Único (SIO)

El bloque de E/S de ciclo único (SIO) es un componente arquitectónico fundamental para lograr un rendimiento determinista. Se accede a él a través del IOPORT de cada procesador, lo que permite operaciones de lectura y escritura en un solo ciclo de reloj. Es importante destacar que el SIO no está conectado al bus principal del sistema (AHB/APB), lo que lo aísla de las latencias variables del bus y garantiza un acceso de baja latencia predecible.

Los componentes clave del SIO son los siguientes:

#### Identificación de CPU (CPUID)

El SIO contiene un registro `CPUID` que permite al software diferenciar entre los dos núcleos. Al leer este registro, el núcleo 0 obtiene un valor de 0, mientras que el núcleo 1 obtiene un valor de 1. Este mecanismo es esencial durante la secuencia de arranque para asignar tareas específicas a cada núcleo.

#### Comunicación Inter-procesador

Para facilitar la cooperación entre los dos núcleos, el SIO proporciona mecanismos de hardware dedicados:

-   **FIFOs (Buzones):** Dos FIFOs de 8 entradas y 32 bits de ancho permiten el paso de datos y mensajes entre los núcleos. Una FIFO es para la comunicación del núcleo 0 al 1, y la otra para la dirección opuesta.
-   **Spinlocks de Hardware:** Se proporcionan 32 spinlocks de hardware para gestionar la sincronización y la exclusión mutua. Estos permiten que un núcleo reclame un recurso compartido de forma atómica, evitando que el otro núcleo acceda a él hasta que sea liberado.

#### Control de GPIO

El SIO ofrece un acceso directo y rápido a los pines GPIO a través de registros compartidos. Este acceso es seguro para la concurrencia gracias a los alias de registro atómicos:

-   **SET:** Permite establecer bits específicos en un registro de GPIO a 1.
-   **CLR:** Permite limpiar bits específicos a 0.
-   **XOR:** Permite invertir el estado de bits específicos.

El uso de estos alias elimina la necesidad de secuencias de lectura-modificación-escritura, lo que previene condiciones de carrera cuando ambos núcleos o un núcleo y una interrupción intentan modificar el estado de los GPIO simultáneamente.

#### Aceleradores de Hardware

El SIO integra aceleradores para descargar tareas computacionales comunes de los procesadores:

-   **Divisor de Enteros por Hardware:** Cada núcleo tiene acceso a un acelerador de división y módulo de 8 ciclos para enteros con y sin signo. Mientras el cálculo se realiza en segundo plano, el procesador puede ejecutar otras instrucciones.
-   **Interpoladores:** Cada núcleo cuenta con dos unidades de interpolación. Estas unidades están diseñadas para acelerar tareas como el renderizado de gráficos, el procesamiento de señales o la manipulación de datos, descargando a los procesadores de estas operaciones computacionalmente intensivas.

Este subsistema de procesamiento, con su acceso directo y rápido al SIO, se conecta al resto de los componentes a través de la estructura de bus central del chip.

4.0 Estructura del Bus (Bus Fabric)
-----------------------------------

El *bus fabric* es el corazón de la arquitectura del RP2040, funcionando como una matriz de conmutación AHB-Lite totalmente conectada que enruta direcciones y datos entre los dispositivos que inician transferencias (maestros) y los que responden (esclavos). Su diseño permite que hasta cuatro transferencias de bus se realicen simultáneamente en cada ciclo de reloj, sin añadir estados de espera a los accesos de los esclavos AHB-Lite, lo que maximiza el ancho de banda del sistema.

Los maestros y esclavos conectados a esta matriz son:

-   **Maestros del Bus (4):**
    -   Núcleo del procesador 0
    -   Núcleo del procesador 1
    -   Puerto de lectura del controlador DMA
    -   Puerto de escritura del controlador DMA
-   **Puertos Esclavos (10):**
    -   ROM
    -   Flash XIP (Execute-In-Place)
    -   SRAM 0 a 5 (un puerto dedicado para cada uno de los 6 bancos)
    -   Periféricos rápidos AHB-Lite (PIO0, PIO1, USB, registros de control DMA, XIP aux, etc., compartiendo un único puerto)
    -   Puente a los periféricos APB y registros de control del sistema

### Características de Rendimiento y Acceso

El bus implementa un esquema de prioridad de dos niveles (alto y bajo) para la arbitraje. Esto permite asignar una mayor prioridad a maestros críticos, como los núcleos del procesador, garantizando una latencia predecible para casos de uso de tiempo real, incluso bajo una alta carga del bus por parte de otros maestros de baja prioridad.

### Mecanismo de Acceso Atómico a Registros

Para evitar secuencias de lectura-modificación-escritura y las condiciones de carrera asociadas, el RP2040 implementa un ingenioso mecanismo de acceso atómico a registros a nivel de bus. Cada bloque de registros de periféricos se mapea en el espacio de direcciones con alias específicos que realizan operaciones atómicas en una sola escritura:

-   **Addr + 0x1000:** Realiza una operación XOR atómica.
-   **Addr + 0x2000:** Realiza una operación de establecimiento de bits (SET) atómica.
-   **Addr + 0x3000:** Realiza una operación de limpieza de bits (CLR) atómica.

Aunque esta funcionalidad está disponible a nivel de bus, los periféricos que no la soportan de forma nativa la implementan a través de un "interpuesto de bus" (*bus interposer*). Este componente traduce las escrituras atómicas en secuencias de lectura-modificación-escritura en la frontera del periférico, lo que añade una latencia de dos ciclos de reloj.

### Puente APB

El puente APB (Advanced Peripheral Bus) conecta los periféricos de menor ancho de banda (como UART, SPI, I2C) a la matriz principal AHB-Lite. Actúa como un esclavo en el bus AHB-Lite y como el único maestro en el bus APB, desacoplando eficazmente los subsistemas de alta y baja velocidad.

Esta eficiente estructura de bus proporciona acceso a la organización de memoria del sistema, que se detalla a continuación.

5.0 Organización de la Memoria y Mapa de Direcciones
----------------------------------------------------

La arquitectura de memoria del RP2040 combina memorias internas de alta velocidad, como ROM y SRAM, con la capacidad de acceder a grandes cantidades de memoria flash externa. Cada tipo de memoria y periférico ocupa una región específica en el mapa de direcciones de 32 bits del sistema, permitiendo un acceso unificado desde los procesadores y el DMA.

El siguiente es un resumen del mapa de direcciones del sistema:

|Región de Memoria | Dirección Base |
|------------------|----------------|
| ROM|`0x00000000`|
|XIP (Flash Externa)|`0x10000000`|
|SRAM|`0x20000000`|
|Periféricos APB|`0x40000000`|
|Periféricos AHB-Lite|`0x50000000`|
|Registros IOPORT (SIO)|`0xd0000000`|
|Registros internos Cortex-M0+|`0xe0000000`|

### 5.1 ROM

El RP2040 incluye una memoria de solo lectura (ROM) interna de **16 kB**. Esta memoria, programada durante la fabricación del chip, es crucial para el funcionamiento del sistema, ya que contiene:

-   La rutina de arranque inicial del procesador.
-   Un gestor de arranque (bootloader) USB que cumple con la especificación UF2, permitiendo la programación del chip arrastrando y soltando archivos.
-   Bibliotecas de utilidades, como rutinas de punto flotante rápido y funciones optimizadas para la manipulación de memoria, que pueden ser llamadas por el código del usuario.

### 5.2 SRAM

La memoria SRAM total es de **264 kB**, particionada físicamente en **6 bancos** para optimizar el rendimiento mediante el acceso paralelo. La distribución es la siguiente:

-   Cuatro bancos de 64 kB cada uno.
-   Dos bancos de 4 kB cada uno.

Esta partición física permite que hasta cuatro maestros del bus (por ejemplo, ambos núcleos y el DMA) accedan a diferentes bancos de SRAM simultáneamente sin conflictos. Para maximizar este paralelismo en casos de uso comunes, los primeros 256 kB de SRAM (correspondientes a los cuatro bancos grandes) están mapeados en un modo entrelazado (*striped*). Esto significa que palabras de memoria consecutivas en el espacio de direcciones se asignan a bancos físicos diferentes. Esta asignación distribuye las cargas de acceso a memoria secuencial entre los cuatro bancos principales, reduciendo la probabilidad de contención del bus y maximizando el paralelismo de acceso, lo cual es clave para el alto ancho de banda del sistema.

### 5.3 Flash Externa (XIP)

El subsistema de ejecución en el lugar (Execute-In-Place o XIP) permite a los procesadores ejecutar código directamente desde una memoria flash externa conectada a través de la interfaz QSPI, como si fuera memoria interna. Esta capacidad es fundamental para soportar programas de gran tamaño.

-   **Caché XIP:** Para mitigar la latencia del acceso a la flash externa, el RP2040 incorpora una **caché XIP de 16 kB** y dos vías asociativas (*two-way set-associative*). Esta caché acelera significativamente la ejecución de código al almacenar las instrucciones y datos más recientemente utilizados.
-   **Modos de Acceso:** El espacio de direcciones de la flash externa tiene diferentes alias que controlan el comportamiento de la caché (cacheable, no cacheable, etc.), proporcionando flexibilidad para gestionar la coherencia de datos.
-   **Caché como SRAM:** Una característica notable es que, si la caché XIP se deshabilita, su memoria de 16 kB puede ser utilizada como un banco de SRAM adicional de acceso rápido.

Además de las memorias, el mapa de direcciones también alberga un rico conjunto de periféricos.

6.0 Periféricos Clave
---------------------

El RP2040 está equipado con un completo conjunto de periféricos que va más allá de los componentes estándar. Además de interfaces de comunicación comunes, el microcontrolador integra bloques de hardware potentes y flexibles como un controlador de DMA multicanal y el innovador subsistema de E/S Programable (PIO), que lo diferencian de otros dispositivos de su clase.

Los periféricos del RP2040 se pueden agrupar en las siguientes categorías funcionales:

-   **Comunicaciones:**
    -   2 × UART
    -   2 × SPI
    -   2 × I2C
    -   1 × Controlador USB 1.1 (con soporte para Host y Dispositivo)
-   **Temporización y Control:**
    -   16 × canales PWM
    -   1 × Temporizador (con 4 alarmas de 32 bits)
    -   1 × Watchdog
    -   1 × Reloj de Tiempo Real (RTC)
-   **E/S Programable:**
    -   2 × bloques PIO (con 4 máquinas de estado cada uno)
-   **Transferencia de Datos:**
    -   1 × Controlador DMA (con 12 canales)
-   **Analógico:**
    -   1 × Conversor Analógico-Digital (ADC) de 4 canales

### 6.1 Controlador DMA

El controlador de Acceso Directo a Memoria (DMA) es un componente esencial para el alto rendimiento del sistema, ya que permite transferir grandes volúmenes de datos entre la memoria y los periféricos sin intervención de la CPU. Sus capacidades principales incluyen:

-   **12 canales** configurables de forma independiente.
-   **Encadenamiento de canales (*****chaining*****):** Permite que la finalización de una transferencia en un canal active automáticamente otro, creando secuencias de transferencia complejas.
-   **Ritmo controlado por DREQ:** Las transferencias pueden ser sincronizadas por las señales de solicitud de datos (DREQ) de los periféricos, asegurando que los datos se muevan solo cuando el periférico está listo para enviar o recibir.

### 6.2 E/S Programable (PIO)

El subsistema de E/S Programable (PIO) es una de las características más distintivas y potentes del RP2040. A diferencia de los periféricos de función fija, el PIO proporciona un hardware flexible para implementar protocolos de E/S personalizados y de alta velocidad.

-   **Arquitectura:** El RP2040 contiene dos bloques PIO, cada uno con **cuatro máquinas de estado** (*State Machines*). Cada máquina de estado es un procesador minimalista capaz de ejecutar un pequeño conjunto de instrucciones especializadas para la manipulación de GPIOs y la transferencia de datos.
-   **Componentes por Máquina de Estado:** Cada una incluye registros de desplazamiento de entrada y salida (ISR/OSR), FIFOs para la transferencia de datos con el sistema, y un divisor de reloj fraccionario para un control preciso de la temporización.
-   **Flexibilidad:** Los programas PIO pueden implementar una amplia variedad de interfaces digitales, como I2S, SDIO, VGA, o protocolos serie no estándar, descargando completamente a la CPU de la manipulación de bits a alta velocidad. El mapeo flexible de pines permite que estas interfaces se puedan asignar a diferentes GPIOs según las necesidades del diseño. Adicionalmente, todas las máquinas de estado tienen acceso independiente y simultáneo a cualquier GPIO, lo que permite una asignación de pines extremadamente flexible.

Una vez que el sistema está configurado, el proceso de arranque inicializa estos componentes.

7.0 Proceso de Arranque y Bootrom
---------------------------------

Tras un reinicio, los procesadores del RP2040 comienzan a ejecutar código desde una ROM interna de 16 kB. Esta ROM, conocida como Bootrom, contiene un gestor de arranque robusto y una serie de rutinas esenciales que configuran el sistema y determinan el origen del código de la aplicación.

La secuencia de arranque, controlada por el procesador, sigue un orden predefinido para garantizar una inicialización segura y flexible:

1.  **Verificación de reinicio por "Rescue DP":** El Bootrom primero comprueba si el reinicio fue iniciado a través del puerto de depuración de rescate (Rescue Debug Port). Si es así, el procesador se detiene para permitir que un depurador externo tome el control, un mecanismo crucial para recuperar un dispositivo bloqueado.
2.  **Verificación de arranque por "Watchdog":** A continuación, verifica si el watchdog fue configurado para arrancar desde una dirección específica en SRAM. Esto permite que una aplicación cargue su propio gestor de arranque en RAM y lo ejecute tras un reinicio por software.
3.  **Intento de arranque desde Flash externa:** Si las condiciones anteriores no se cumplen, el Bootrom intenta arrancar desde una memoria Flash externa conectada a través de la interfaz QSPI.
4.  **Modo de arranque por dispositivo USB:** Si el arranque desde la Flash externa falla (por ejemplo, si no se encuentra una imagen válida), el sistema entra en modo de arranque USB. El RP2040 se presenta como un dispositivo de almacenamiento masivo (Mass Storage Device) que soporta el formato de archivo UF2, permitiendo al usuario programar la Flash simplemente arrastrando y soltando un archivo.

### La Segunda Etapa de Arranque desde Flash (Flash Second Stage)

Debido a la gran variedad de memorias Flash QSPI disponibles en el mercado, cada una con sus propios requisitos de configuración y comandos, el Bootrom no puede configurar de forma óptima el modo XIP (Execute-In-Place) para todas ellas. Para solucionar esto, el Bootrom carga un pequeño programa de 256 bytes desde el inicio de la Flash a la SRAM. Este programa, conocido como "segunda etapa de arranque", es responsable de configurar correctamente la interfaz QSPI y el modo XIP para la memoria Flash específica utilizada en la placa, antes de ceder el control al código principal de la aplicación.

### Contenido Útil del Bootrom

Además de la secuencia de arranque, el Bootrom contiene bibliotecas optimizadas que pueden ser llamadas por el código de usuario. Esto incluye rutinas de punto flotante rápido y funciones de manipulación de memoria (como `memcpy` y `memset`) altamente optimizadas, lo que permite a los desarrolladores ahorrar espacio en la Flash y mejorar el rendimiento sin necesidad de incluir sus propias implementaciones.

8.0 Conclusión
--------------

La arquitectura del microcontrolador RP2040 representa un diseño innovador que equilibra rendimiento, flexibilidad y bajo coste de una manera excepcional. El análisis técnico revela una serie de decisiones estratégicas que lo hacen particularmente adecuado para una nueva generación de aplicaciones de sistemas embebidos.

Las fortalezas clave de su diseño se pueden recapitular en los siguientes puntos:

-   **Rendimiento del Sistema Dual-Core:** La inclusión de dos procesadores Arm Cortex-M0+ permite una verdadera computación paralela, ideal para tareas que requieren separar la lógica de control en tiempo real de la gestión de protocolos de comunicación o interfaces de usuario.
-   **Alto Ancho de Banda del Bus Fabric:** La matriz de conmutación AHB-Lite es el pilar del rendimiento del sistema, eliminando cuellos de botella al permitir el acceso concurrente y sin estados de espera a memorias y periféricos.
-   **Flexibilidad del Sistema de Memoria:** La combinación de una SRAM interna particionada y un robusto subsistema XIP con caché ofrece una solución de memoria escalable, que combina el acceso rápido a datos críticos con la capacidad de ejecutar programas de gran tamaño desde una flash externa de bajo coste.
-   **Capacidad Única del Subsistema PIO:** La E/S Programable es la característica más distintiva, proporcionando una flexibilidad sin precedentes para implementar protocolos de E/S personalizados a alta velocidad sin sobrecargar la CPU, algo que tradicionalmente requeriría hardware dedicado o FPGA.

En conjunto, estas características confirman la idoneidad del RP2040 para una amplia gama de aplicaciones que demandan un alto rendimiento computacional, flexibilidad de E/S y un coste competitivo, desde la automatización industrial y la robótica hasta la electrónica de consumo y los dispositivos IoT.