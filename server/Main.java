package server;

import server.src.network.GameServer;

public class Main {
    public static void main(String[] args) {
        GameServer server = new GameServer();
        server.start();
    }
}