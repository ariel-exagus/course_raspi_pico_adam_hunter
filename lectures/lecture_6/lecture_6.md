
# Resumen Detallado: Clase 6 — Interfaz con Teclado Matricial y Síntesis de Audio (RP2040)

En esta lección, el profesor V. Hunter Adams explica cómo conectar y escanear un **teclado matricial** (*keypad*) con la Raspberry Pi Pico (RP2040) [1, 2], cómo implementar un filtrado de **anti-rebote por software** (*debouncing*) mediante una máquina de estados finitos [3, 4], y cómo **integrar la lectura del teclado con el sistema de síntesis de audio** por tiempo real (DDS) [1, 5].

---

## 1. Arquitectura del Hardware del Teclado (*Switch Matrix*)

El teclado utilizado en el laboratorio es internamente una **matriz de interruptores** [2]:
* **Conexiones físicas:** Posee **7 pines** de conexión: **4 filas** (*rows*) y **3 columnas** (*columns*) [2].
* **Mecanismo de conmutación:** Al presionar un botón, el hardware genera un cortocircuito eléctrico directo entre el pin de su fila y el pin de su columna asociada (por ejemplo, presionar el botón 2 cortocircuita GP9 con GP14) [2].
* **Configuración GPIO en el C SDK:**
  * **Filas (Salidas / Outputs):** Se configuran como salidas digitales cuya tensión es controlada directamente por el código del microcontrolador [6].
  * **Columnas (Entradas / Inputs):** Se configuran como entradas de alta impedancia [6, 7].

### Protecciones de Hardware y Consideraciones Físicas
1. **Resistencias *Pull-Down* Internas:** Se activan mediante el C SDK en todos los pines de las columnas [7]. Evitan que las entradas de alta impedancia queden en un estado flotante (actuando como pequeñas antenas) cuando no hay botones presionados, garantizando lecturas estables de 0 V [7, 8].
2. **Resistencias de Protección de $330\,\Omega$:** Se colocan en serie en las salidas de las filas [9]. Protegen los pines GPIO del RP2040 contra cortocircuitos destructivos si un usuario presiona simultáneamente dos botones de diferentes filas (lo que conectaría una salida en nivel ALTO con una en nivel BAJO) [9].
3. **Efecto de la Capacitancia en la *Breadboard*:** La *breadboard* genera capacitancia parásita en paralelo hacia tierra [10, 11]. Por ello, tras poner una fila en nivel ALTO, es **obligatorio esperar un pequeño retardo** (un microsegundo) antes de leer las columnas para permitir que la carga se descargue a través de las resistencias *pull-down* [10, 11].

---

## 2. Lógica de Escaneo de Filas y Columnas

El hilo de escaneo procesa el teclado iterando fila por fila [6]:
1. **Patrón de Bit Único (*One-Hot*):** Se coloca en nivel ALTO (1) únicamente la fila que se está evaluando, manteniendo el resto de las filas en nivel BAJO (0) [6, 12, 13].
2. **Lectura de Columnas:** Se lee el registro de los GPIO de las columnas. Si una columna pasa a nivel ALTO, se confirma que la fila activa está conectada a esa columna [6, 14].
3. **Traducción a *Scan Codes* y *Key Codes*:** Se convierten las combinaciones binarias de las filas y columnas en códigos hexadecimales (*scan codes* y *key codes*) [14-16].
4. **Validación de Teclas Múltiples:** Si se detecta que dos o más columnas pasan a nivel ALTO simultáneamente dentro de una misma fila, el escáner interpreta la entrada como **inválida** y devuelve un código de `-1` [4, 15, 17].

---

## 3. Anti-rebote por Software (*Debouncing*)

Dado que los botones son sistemas mecánicos (modelados como masa-resorte-amortiguador), los contactos oscilan ("rebotan") abriéndose y cerrándose velozmente antes de estabilizarse [3, 18]. Para evitar múltiples falsos disparos, se implementa una **máquina de estados finitos** dentro del hilo de Protothreads que se evalúa cada **30 ms** [4, 18, 19]:


```
                 +--------------+
                 |  NOT_PRESSED |
                 +--------------+
                  /            ^
Tecla válida    /                \ Tecla inválida (-1)
              v                    \
      +---------------+         +--------------------+
      | MAYBE_PRESSED |         | MAYBE_NOT_PRESSED  |
      +---------------+         +--------------------+
              |                    ^
Misma tecla   | Transición de      | Tecla = -1
confirmada    v Disparo de Audio   |
         +---------+---------------+
         | PRESSED |
         +---------+

```

### Los 4 Estados de la Máquina:
1. **`NOT_PRESSED`:** Estado de reposo inicial [20]. Si se lee un código de tecla válido ($I \neq -1$), la tecla se guarda en una variable temporal (`possible`) y transiciona a `MAYBE_PRESSED` [20, 21].
2. **`MAYBE_PRESSED`:** En la siguiente iteración (30 ms después), si se confirma exactamente la misma tecla ($I == \text{possible}$), el sistema asume estabilidad y transiciona a `PRESSED` [21, 22].
   * **¡Disparo de Audio!:** La acción que inicia el sonido (*swoop* o *chirp*) se ejecuta **únicamente en esta transición** (`MAYBE_PRESSED` $\rightarrow$ `PRESSED`) [23].
3. **`PRESSED`:** Permanece en este estado mientras la tecla siga presionada [22]. Esto garantiza que **mantener un botón apretado produzca exactamente un sonido** y no una repetición continua [23, 24]. Si se lee `-1`, pasa a `MAYBE_NOT_PRESSED` [22].
4. **`MAYBE_NOT_PRESSED`:** Si tras 30 ms adicionales se vuelve a leer `-1`, se confirma que el botón fue liberado y se retorna a `NOT_PRESSED` [19, 22].

---

## 4. Integración con la Síntesis de Audio (DDS)

Para conectar el teclado con el generador de sonido [5, 25]:
* **Estructura Recomendada:** Se recomienda importar el código de audio (*beep_beep*) dentro del proyecto base del teclado (*keypad*) [5].
* **Modificación de la Rutina de Interrupción (ISR):** Se modifica la ISR desactivando la condición que reinicia automáticamente el pitido, dejando la ISR en un estado pasivo o silencioso tras reproducir una señal [25, 26].
* **Disparo desde el Hilo del Teclado:** En la transición de estado `MAYBE_PRESSED` $\rightarrow$ `PRESSED`, el hilo del teclado reconfigura las variables de la ISR:
  * Reinicia la amplitud de modulación a cero (`amplitude = 0`) [27].
  * Cambia el estado de la ISR a modo de producción de sonido (`isr_state = 0`) [27].
  * Reinicia el contador de muestras/tiempo a cero (`counter = 0`) [27].
* **Rampas de Amplitud (Prevención de *Clicks*):** La onda senoidal sintetizada se multiplica por un factor de amplitud entre 0 y 1 para realizar una transición suave [28, 29]. Esto evita movimientos impulsivos e instantáneos de la membrana física del parlante, eliminando los "clicks" audibles generados por armónicos de alta frecuencia [29, 30].

---

## 5. Optimización Matemáticas y Tiempo Real

* **Prohibición de `pow()`:** La función `pow()` de la librería `math.h` es extremadamente lenta en microcontroladores y destruye el tiempo de ejecución en tiempo real [31].
* **Alternativas Eficientes de Cálculo:**
  1. Multiplicaciones directas (`x * x`) en lugar de `pow()` [31, 32].
  2. **Segundo sintetizador DDS:** Para generar modulaciones senoidales de frecuencia (*swoops*), es mucho más eficiente usar un **segundo acumulador de fase DDS** en lugar de evaluar funciones trigonométricas con `sin()` [33].
  3. **Integrador de Euler:** Para curvas parabólicas, integrar la derivada lineal del polinomio ahorra multiplicaciones complejas, convirtiendo el cálculo en simples operaciones de desplazamiento y suma (*shift and add*) [31, 32].
* **Síntesis en Tiempo Real:** Todos los sonidos deben generarse muestra a muestra dentro de la ISR y **no precalcularse en memoria Flash/RAM** [32, 33].
