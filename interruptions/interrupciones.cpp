#include <iostream>

// Simulación de una variable compartida
volatile int counter = 0;


inline void disableInterrupts() {
    asm volatile("cli");  // Clear Interrupt Flag
}

inline void enableInterrupts() {
    asm volatile("sti");  // Set Interrupt Flag
}

void incrementCounter() {
    disableInterrupts();         // BLOQUEA TODAS LAS INTERRUPCIONES
    counter = counter + 1;       // Sección crítica
    enableInterrupts();          // HABILITA INTERRUPCIONES
}

int main() {
    incrementCounter();
    std::cout << "Counter: " << counter << std::endl;
    return 0;
}
