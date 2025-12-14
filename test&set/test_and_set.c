/**
 * Exclusión Mutua con Test & Set
 * 
 * Este programa implementa un spinlock usando la instrucción atómica
 * Test & Set (xchg en x86) para garantizar exclusión mutua entre
 * dos hilos concurrentes que acceden a una sección crítica.
 */

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

/* Variable de bloqueo compartida (0 = libre, 1 = ocupado) */
volatile int lock = 0;

/* Contador compartido para demostrar la exclusión mutua */
volatile int contador = 0;

/**
 * Función atómica Test & Set usando ensamblador inline x86-64
 * 
 * Funcionamiento:
 * 1. Carga el valor 1 en un registro
 * 2. Intercambia atómicamente ese 1 con el valor en memoria (lock)
 * 3. Retorna el valor anterior del lock
 * 
 * Si retorna 0: el lock estaba libre y ahora lo adquirimos
 * Si retorna 1: el lock estaba ocupado, debemos reintentar
 */
static inline int test_and_set(volatile int *lock) {
    int old_value = 1;
    
    __asm__ __volatile__(
        "xchg %0, %1"           /* Intercambio atómico entre registro y memoria */
        : "=r" (old_value),     /* %0: salida en registro (valor anterior) */
          "+m" (*lock)          /* %1: memoria (lock), lectura/escritura */
        : "0" (old_value)       /* entrada: el registro inicia con 1 */
        : "memory"              /* barrera de memoria para evitar reordenamiento */
    );
    
    return old_value;
}

/**
 * Adquirir el lock usando espera activa (spinlock)
 * 
 * El hilo gira en un bucle hasta que logra adquirir el lock.
 * Esto es "busy waiting" - consume CPU mientras espera.
 */
void acquire_lock(volatile int *lock) {
    printf("[Hilo %lu] Intentando adquirir el lock...\n", pthread_self() % 1000);
    
    /* Espera activa: girar mientras test_and_set retorne 1 */
    while (test_and_set(lock) == 1) {
        /* El lock está ocupado, seguir intentando */
        /* Pequeña pausa para reducir contención del bus */
        __asm__ __volatile__("pause" ::: "memory");
    }
    
    printf("[Hilo %lu] ¡Lock adquirido!\n", pthread_self() % 1000);
}

/**
 * Liberar el lock
 * 
 * Simplemente pone el lock en 0 (libre).
 * Esta operación es segura porque solo el dueño del lock la ejecuta.
 */
void release_lock(volatile int *lock) {
    __asm__ __volatile__(
        "movl $0, %0"           /* Escribir 0 en el lock */
        : "=m" (*lock)
        :
        : "memory"
    );
    
    printf("[Hilo %lu] Lock liberado.\n", pthread_self() % 1000);
}

/**
 * Sección crítica
 * 
 * Esta función representa el código que debe ejecutarse
 * con exclusión mutua. Solo un hilo puede estar aquí a la vez.
 */
void seccion_critica(int id_hilo) {
    printf("\n");
    printf("╔════════════════════════════════════════════╗\n");
    printf("║  SECCIÓN CRÍTICA - Hilo %d                  ║\n", id_hilo);
    printf("╠════════════════════════════════════════════╣\n");
    
    /* Operación sobre recurso compartido */
    int valor_anterior = contador;
    printf("║  Valor anterior del contador: %d            ║\n", valor_anterior);
    
    /* Simular trabajo dentro de la sección crítica */
    usleep(100000);  /* 100ms - hace visible la exclusión mutua */
    
    contador++;
    printf("║  Nuevo valor del contador: %d               ║\n", contador);
    printf("║  Hilo %d completó su trabajo                ║\n", id_hilo);
    printf("╚════════════════════════════════════════════╝\n");
    printf("\n");
}

/**
 * Función que ejecuta cada hilo
 */
void *funcion_hilo(void *arg) {
    int id_hilo = *((int *)arg);
    
    printf("[Hilo %d] Iniciado.\n", id_hilo);
    
    /* Ejecutar 3 veces para demostrar la alternancia */
    for (int i = 0; i < 3; i++) {
        printf("[Hilo %d] Iteración %d\n", id_hilo, i + 1);
        
        /* === ENTRADA A LA SECCIÓN CRÍTICA === */
        acquire_lock(&lock);
        
        /* === SECCIÓN CRÍTICA === */
        seccion_critica(id_hilo);
        
        /* === SALIDA DE LA SECCIÓN CRÍTICA === */
        release_lock(&lock);
        
        /* Pequeña pausa fuera de la sección crítica */
        usleep(50000);  /* 50ms */
    }
    
    printf("[Hilo %d] Terminado.\n", id_hilo);
    return NULL;
}

int main(void) {
    pthread_t hilo1, hilo2;
    int id1 = 1, id2 = 2;
    
    printf("═══════════════════════════════════════════════\n");
    printf("   EXCLUSIÓN MUTUA CON TEST & SET (SPINLOCK)\n");
    printf("═══════════════════════════════════════════════\n\n");
    
    printf("Creando dos hilos concurrentes...\n\n");
    
    /* Crear los dos hilos */
    if (pthread_create(&hilo1, NULL, funcion_hilo, &id1) != 0) {
        perror("Error creando hilo 1");
        return 1;
    }
    
    if (pthread_create(&hilo2, NULL, funcion_hilo, &id2) != 0) {
        perror("Error creando hilo 2");
        return 1;
    }
    
    /* Esperar a que ambos hilos terminen */
    pthread_join(hilo1, NULL);
    pthread_join(hilo2, NULL);
    
    printf("\n═══════════════════════════════════════════════\n");
    printf("   RESULTADOS FINALES\n");
    printf("═══════════════════════════════════════════════\n");
    printf("   Valor final del contador: %d\n", contador);
    printf("   Valor esperado: 6 (3 iteraciones x 2 hilos)\n");
    
    if (contador == 6) {
        printf("   ✓ ¡Exclusión mutua funcionó correctamente!\n");
    } else {
        printf("   ✗ Error: condición de carrera detectada\n");
    }
    printf("═══════════════════════════════════════════════\n");
    
    return 0;
}

