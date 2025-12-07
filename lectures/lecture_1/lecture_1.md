# Resumen de la primera clase del curso de Raspberry pi pico


La primera clase del curso, **ECE 4760** (co-listado con ECE 5730 para estudiantes de posgrado), sirvió como una introducción detallada a la estructura, filosofía y contenido práctico de la asignatura.

El instructor, **Hunter**, detalló que toda la información del curso---incluyendo materiales, asignaciones de laboratorio, el calendario de entregables y grabaciones de clases---se encuentra disponible en la página web del curso, que puede encontrarse buscando "ECE 4760". Las grabaciones de clases anteriores (de 2021 o 2022) se reemplazarán a lo largo del semestre, ya que los laboratorios han cambiado desde entonces.

**Objetivos y Estructura del Curso**

El curso es una clase rigurosa de **diseño y laboratorio**. Aproximadamente **el 90% del aprendizaje** tendrá lugar en el laboratorio mediante la construcción de dispositivos.

La estructura es la siguiente:

1\. **Tres asignaciones de laboratorio:** Cada laboratorio dura **tres semanas**.

2\. **Un proyecto de diseño final:** Dura **cuatro semanas** y es completamente abierto.

El objetivo de los laboratorios es exponer a los estudiantes a un conjunto diferente de **periféricos** del microcontrolador **rp240**, distintos **algoritmos**, y la interfaz con nuevos **sensores o dispositivos de entrada** (como teclados y potenciómetros), con la meta de aumentar las herramientas en la "caja de herramientas" del estudiante.

**Carga de Trabajo y Evaluación**

- **Evaluación:** **No hay exámenes**. La evaluación se basa principalmente en la demostración del funcionamiento de los dispositivos, los informes de laboratorio (uno por asignación) y la página web final del proyecto.

- **Tiempo de trabajo:** Al ser una clase de cuatro créditos, se debe presupuestar de **16 a 20 horas de trabajo por semana**, incluyendo el tiempo de clase y laboratorio. Este tiempo adicional se dedica a la **preparación del laboratorio** (lectura, y diseño de la aplicación y máquinas de estado antes de llegar a escribir el código) y la elaboración de los informes. El propósito de las clases magistrales es el **100% de apoyo al trabajo de laboratorio**.

**Trabajo en Grupo y Políticas**

- **Grupos:** Los laboratorios y el proyecto final se realizan en grupos. Se recomienda enfáticamente trabajar en **grupos de tres**, aunque los grupos de dos son aceptables si los números lo requieren. **No se permiten grupos de uno**.

- **Requisitos del Grupo:** Los miembros del grupo deben estar inscritos en el **mismo número de curso** (4760 o 5730). Los estudiantes de 5730 (posgrado) tienen requisitos de laboratorio ligeramente mayores, siguiendo una política estatal.

- **Asistencia al Laboratorio:** Se debe asistir a la sección de laboratorio en la que se está inscrito para las verificaciones (*check-offs*), aunque se puede asistir a secciones adicionales si hay espacio.

**Filosofía de Diseño de Laboratorio**

El instructor destacó varias metas al diseñar los laboratorios:

- **Auto-motivación:** Se pide construir cosas interesantes en sí mismas, separadas de los detalles técnicos, para fomentar una atmósfera de **"juego y experimentación"**.

- **Extensibilidad:** Cada laboratorio tiene una línea de meta, pero ofrece caminos interesantes para que los estudiantes tomen el proyecto más allá, quizás como proyecto final.

- **Intereses Variados:** La secuencia de laboratorios busca apelar a estudiantes con intereses diversos (Robótica, DSP, Electrónica Analógica, Gráficos por Computadora).

- **Conexión con el Mundo:** El instructor espera que, para algunos estudiantes, un laboratorio pueda **cambiar fundamentalmente su experiencia del mundo** (mencionado en el contexto de la síntesis de cantos de pájaros, que hace que el estudiante escuche las frecuencias en la naturaleza).

- **Nivel de Abstracción:** Se busca que el desarrollo se realice en el nivel de abstracción más productivo, pero siempre informando a los estudiantes sobre lo que sucede "bajo el capó" para facilitar la depuración.

**Política de IA (Nueva este Semestre)**

- **Informes de Laboratorio:** Se permite el uso de IA, tratándola como un recurso humano. Sin embargo, cualquier uso debe ser **citado**.

- **Código:** Se permite usar IA para ayudar a escribir código bajo el propio riesgo del estudiante. **El código generado por IA debe estar claramente marcado con comentarios**, y el personal del curso **no ayudará a depurar** ese código si introduce errores, ya que la IA a menudo tiene dificultades para escribir código específico para hardware de microcontroladores.

**Vista Previa de los Laboratorios Principales**

El hardware central es el microcontrolador **rp240 de Raspberry Pi**.

**1\. Laboratorio 1: Síntesis de Canto de Pájaro**

- Se enfocará en el **Cardenal del Norte** (*Northern Cardinal*), ya que es nativo de Ítaca y su canto es simple, produciendo principalmente **un solo tono a la vez**.

- La tarea es construir un sintetizador de onda sinusoidal y modular su frecuencia en tiempo real para imitar los cambios de frecuencia del canto (basándose en el análisis del espectrograma).

- El objetivo final es lograr que la aplicación **Merlin** (desarrollada por el Laboratorio de Ornitología de Cornell) identifique el canto sintetizado como el de un Cardenal del Norte.

- En este laboratorio, la síntesis de audio se manejará mediante una interrupción de temporizador (*timer interrupt*).

**2\. Laboratorio 2: Tablero de Galton (Gráficos y Rendimiento)**

- Se construirá un simulador del **Tablero de Galton** (demostración física del **Teorema del Límite Central**), en el que las bolas que caen forman una aproximación a la distribución gaussiana en los contenedores inferiores.

- La simulación requiere implementar la **física de colisión real**.

- Habrá una competición amistosa para ver qué grupo puede animar la **mayor cantidad de bolas a 30 cuadros por segundo**.

- Para lograr un alto rendimiento, se requerirá el uso de **aritmética de punto fijo** (más rápida que la de punto flotante) y la utilización de los **dos núcleos** del rp240.

- La producción del sonido (*thwack*) que se escucha cuando las bolas golpean las clavijas se **descargará a un canal DMA** (*Direct Memory Access*) para no consumir tiempo de CPU necesario para la animación.

**3\. Laboratorio 3: Controlador de Alta Velocidad para Dron Restringido**

- Se construirá un sistema con un motor de dron unido al final de un brazo acrílico articulado, equipado con un **IMU** (acelerómetro y giroscopio).

- Los estudiantes deberán escribir un **filtro complementario** para estimar el ángulo del brazo y luego aplicar un **controlador PID** para mantener un ángulo de suspensión (*hover angle*) especificado por el usuario.

- Este laboratorio ilustra la necesidad de un controlador de alta velocidad, ya que el sistema se vuelve **no lineal** e inestable cuando el brazo sube por encima de la horizontal, siendo imposible de controlar por un ser humano.

**Proyecto Final**

El proyecto final es un **proyecto de diseño acumulativo** (*CDE*) y los estudiantes son libres de construir cualquier cosa que sea legal y razonable. El instructor sugirió que la ingeniería puede ser un **mecanismo para aprender sobre otros campos de interés** (literatura, arquitectura, historia del arte, etc.), y que el proyecto puede ser una exploración de estos temas. Se anima a los estudiantes a comenzar a pensar en ideas, aunque las propuestas se solicitarán en unas ocho semanas.

**Tarea Inicial**

La única tarea para la primera semana es **configurar el entorno de compilación** de Visual Studio Code para el microcontrolador. Las instrucciones detalladas se encuentran en la página de **Lab 1**. Los laboratorios comenzarán la próxima semana.

--------------------------------------------------------------------------------

**Analogía para el Enfoque del Curso:**

Podemos imaginar que el curso es como **un gimnasio de ingeniería avanzado, centrado en herramientas de hardware**. Los tres laboratorios son como las **sesiones de entrenamiento intensivo específicas** (cardio, pesas, flexibilidad), donde en cada sesión el instructor no solo te enseña una nueva máquina (un periférico, como el DMA), sino que te obliga a usarla de la manera más eficiente y productiva (optimizando código/física). El proyecto final abierto, de cuatro semanas, es como el **desafío de diseño personal**, donde el estudiante usa todas las herramientas y la fuerza acumulada para construir algo único y significativo, sin un plan de entrenamiento preestablecido.