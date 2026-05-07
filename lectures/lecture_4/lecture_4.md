# Algoritmo de Síntesis Digital Directa (DDS)

Este video se centra en el algoritmo de **Síntesis Digital Directa (DDS)**, una herramienta de alta precisión y bajo costo computacional para generar ondas senoidales en el RP2040.

## 1. Filosofía del Curso y Logística
- **Reducción de presión**: Se busca eliminar el estrés de tiempo permitiendo que los estudiantes trabajen en casa con sus kits y abriendo horas extra de laboratorio los domingos.
- **Metodología "Flipped"**: El profesor recomienda leer y experimentar antes de la clase para usar el tiempo presencial en clarificar dudas.

## 2. Fundamentos del Algoritmo DDS
- **El Fasor Rotatorio**: Una onda senoidal se entiende como la proyección de un fasor (vector unitario) que gira sobre el eje imaginario.
- **Nuevas Unidades de Ángulo**: El algoritmo define una unidad llamada *unidades de acumulador*, estableciendo que hay \(2^{32}\) unidades por rotación.
- **Uso del Desbordamiento (Overflow)**: Al usar un `unsigned int` de 32 bits, el desbordamiento automático del lenguaje C maneja la periodicidad del ángulo de forma perfecta, volviendo a cero justo al completar una rotación.

## 3. Implementación y Matemáticas
- **Pasos en la Interrupción (ISR)**: Cada 20 microsegundos (50 kHz), se incrementa el acumulador, se busca el valor en una tabla de senos, se envía al DAC vía SPI y se reprograma la alarma.
- **La Ecuación de Frecuencia**:  
  

\[
  F_{out} = \frac{n \cdot F_s}{2^{32}}
  \]

  
  donde *n* es el incremento que determina la frecuencia deseada.
- **Precisión Extrema**: Con un acumulador de 32 bits, se pueden generar frecuencias con una precisión de hasta \(10^{-5}\) Hz.

## 4. Tabla de Búsqueda (LUT) y Eficiencia
- **Indexación Fraccional**: Aunque el acumulador tiene 32 bits, la tabla de senos solo necesita 256 elementos. Se usan los 8 bits más significativos (MSB) para indexar la tabla mediante un desplazamiento a la derecha de 24 bits (`>> 24`).
- **Armónicos de Error**: Una tabla de 256 entradas es suficiente para que los errores de cuantización sean inaudibles para el oído humano (por encima de los 20 kHz).

## 5. Síntesis de Canto de Aves (Modulación)
- **Ajuste de Funciones**: Para los *barridos* (*swoops*) de frecuencia, se ajustan los datos a parábolas o funciones senoidales.
- **Prevención de "Clicks"**: Es vital realizar rampas de amplitud al encender o apagar el sonido. Si se usa una función escalón, el cambio brusco genera armónicos audibles como clics o pops molestos.

## 6. Aplicaciones Adicionales
- **Recreación de sonidos físicos**: Ejemplo de un estudiante que sintetizó una campana faltante de una torre de reloj analizando sus componentes de frecuencia.
- **Otros usos**: Control de motores para dibujar círculos y exploración de sistemas caóticos.
