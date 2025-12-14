package org.devdiagon;

public class Hilo extends Thread {
    private static final int MAX_THREADS = 20;
    private static final Panaderia panaderia = new Panaderia(MAX_THREADS);
    private final int id;

    public Hilo(String str) {
        super(str);
        this.id = panaderia.registrarHilo();
    }

    @Override
    public void run() {
        try {
            Thread.sleep((int) (Math.random() * 20));

            panaderia.tomarNumero(id, getName());

            panaderia.esperarTurno(id, getName());

            // SECCIÓN CRÍTICA
            Thread.sleep((int) (Math.random() * 10));
            System.out.println(getName() + " ... YA FUE ATENDIDO ... !");

            panaderia.liberarTurno(id);

        } catch (InterruptedException e) {
            System.out.println(getName() + " fue interrumpido");
        }
    }
}