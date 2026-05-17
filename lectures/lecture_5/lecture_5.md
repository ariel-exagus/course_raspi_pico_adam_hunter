# Resumen: Protothreads y Gestión de Memoria (Lecture 5)

## 1. Introducción a los Protothreads
Los Protothreads son una biblioteca de hilos extremadamente ligera y *stackless* (sin pila), escrita enteramente mediante macros de C.  
En la jerarquía de software, se ubican en el nivel más alto, por encima del SDK y los registros.

### Características Fundamentales
- **Multitarea Cooperativa**: No hay *preemption*. Un hilo nunca interrumpe a otro; el cambio de contexto ocurre solo cuando un hilo ejecuta voluntariamente una sentencia `yield`. Si un hilo no cede el control, bloquea a todos los demás.
- **Sin Pila (Stackless)**: A diferencia de los hilos tradicionales, no tienen una pila propia. Funcionan bajo el capó como funciones de C gracias a un truco llamado *Duff's Device*, que permite saltar dentro de un bucle `while` usando un `switch case`.
- **Eficiencia**: El tiempo de cambio entre hilos es menor a 1 microsegundo.

---

## 2. Estructura de un Programa con Hilos
Un programa con hilos se diferencia de uno convencional en su organización:

- **Hilos**: Cada uno es una función con su propio bucle infinito `while(1)`.
- **Scheduler (Planificador)**: Por defecto usa un método *Round-Robin* (pasa de un hilo a otro en orden). También existe un planificador basado en prioridades donde el primer hilo añadido es el de mayor prioridad.
- **Main**: Ya no contiene lógica repetitiva. Se encarga de configurar el hardware, añadir los hilos con `pt_add()` e iniciar el planificador con `pt_schedule_start`.

---

## 3. Palabras Clave: static, volatile y const
El profesor enfatiza tres modificadores esenciales para el manejo de memoria y la estabilidad del sistema:

- **static (Uso obligatorio en hilos)**: Debido a que los hilos no tienen pila, las variables locales automáticas se pierden al ceder el control (`yield`). Al declarar una variable como `static`, el compilador le da ámbito local (solo se ve en ese hilo) pero la almacena como una variable global, permitiendo que retenga su valor entre ejecuciones.
- **volatile**: Indica al compilador que no optimice la variable. Es crucial para cualquier dato que se actualice de forma asíncrona, como variables modificadas dentro de una Interrupción (ISR) o compartidas entre núcleos, obligando al sistema a leer siempre el valor real desde la memoria.
- **const**: Indica que la variable no cambiará. El beneficio práctico en el RP2040 es que el compilador almacena estas variables en la Memoria Flash (XIP, que empieza en la dirección `0x1`) en lugar de la SRAM (RAM, que empieza en `0x2`). Esto ahorra RAM, que es un recurso mucho más limitado.

---

## 4. Ejemplos de Implementación

### A. Estructura Básica de un Hilo
Todo hilo debe comenzar con `PT_BEGIN` y terminar con `PT_END`.

```c
static PT_THREAD (nombre_del_hilo(struct pt *pt)) {
    PT_BEGIN(pt);
    // Variable estática para preservar valor entre yields
    static int contador = 0; 

    while(1) {
        contador++;
        printf("Hilos ejecutado %d veces\n", contador);
        // Cede el control por 1 segundo (1,000,000 us)
        PT_YIELD_usec(1000000); 
    }
    PT_END(pt);
}
```

### B. Sincronización con Semáforos (Core-Safe)
Para que un hilo espere a que otro le dé una señal (útil para comunicación entre el Núcleo 0 y el Núcleo 1), se utilizan semáforos protegidos por spinlocks de hardware.

```c
// En el Hilo A (Emisor)
if (condicion_cumplida) {
    PT_SEM_SDK_SIGNAL(pt, &mi_semaforo); // Envía señal "Go"
}

// En el Hilo B (Receptor)
PT_SEM_SDK_WAIT(pt, &mi_semaforo); // Se bloquea hasta recibir la señal
// Una vez recibida, el semáforo se limpia automáticamente a cero
```

### C. Yield Condicional
Es posible hacer que un hilo no se ejecute hasta que se cumpla una condición lógica específica.

```c
// El hilo cede el control hasta que la variable 'incremento' llegue a 10
PT_WAIT_UNTIL(pt, incremento == 10);
```

---

## 5. Consejos de Depuración

- **Reinicio constante:** Si el programa se reinicia varias veces por segundo, es probable que hayas salido de un hilo programado (*scheduled thread*) sin usar un bucle infinito, lo que corrompe la pila.

- **Variables que cambian solas:** Revisa si olvidaste el modificador `static` en las variables locales del hilo.

- **Bloqueo total:** Asegúrate de que cada hilo tenga al menos un `PT_YIELD` o similar dentro de su `while(1)`.