package org.devdiagon;

import java.util.ArrayList;

public class Main {
    public static void main(String[] args) {
        ArrayList<Hilo> P_lamport = new ArrayList<>();

        for(int i=0;  i < 10; i++){
            Hilo h1 = new Hilo("Cliente " + (i+1));
            P_lamport.add(h1);
            P_lamport.get(i).start();
        }
    }
}