package org.devdiagon;

import java.util.concurrent.atomic.AtomicInteger;

public class Panaderia {
    private final int maxThreads;
    private final boolean[] eligiendo;
    private final int[] turno;
    private final AtomicInteger totalHilos = new AtomicInteger(0);

    public Panaderia(int maxThreads) {
        this.maxThreads = maxThreads;
        this.eligiendo = new boolean[maxThreads];
        this.turno = new int[maxThreads];

        for (int i = 0; i < maxThreads; i++) {
            eligiendo[i] = false;
            turno[i] = 0;
        }
    }

    public int registrarHilo() {
        int id = totalHilos.getAndIncrement();
        if (id >= maxThreads) {
            throw new RuntimeException("Máximo número de hilos excedido");
        }
        return id;
    }
    
    public void tomarNumero(int threadId, String threadName) {
        eligiendo[threadId] = true;

        // Encontrar el máximo turno actual y sumar 1
        int max = 0;
        for (int i = 0; i < totalHilos.get(); i++) {
            if (turno[i] > max) {
                max = turno[i];
            }
        }
        turno[threadId] = max + 1;
        eligiendo[threadId] = false;

        System.out.println(threadName + " se le asigno [ticket " + turno[threadId] + "].");
    }

    public void esperarTurno(int threadId, String threadName) {
        for (int i = 0; i < totalHilos.get(); i++) {
            if (i == threadId) continue;

            // Esperar si el hilo i está eligiendo número
            while (eligiendo[i]) {
                Thread.yield();
            }

            // El hilo actual, esperará según las condiciones
            while (turno[i] != 0 &&
                    // Si hay hilos con turnos anteriores al actual
                    (turno[i] < turno[threadId] ||
                            // Desempate, se prioriza el hilo con el menor ID
                            (turno[i] == turno[threadId] && i < threadId))) {
                Thread.yield();
            }
        }

        System.out.println(threadName + " esta siendo ATENDIDO ---> (ticket " + turno[threadId] + ").");
    }

    public void liberarTurno(int threadId) {
        turno[threadId] = 0;
    }
}
