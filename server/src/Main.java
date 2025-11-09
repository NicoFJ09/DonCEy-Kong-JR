package server.src;

import server.src.network.GameServer;
import server.src.ui.ServerUI;
import javax.swing.SwingUtilities;

public class Main {
    public static void main(String[] args) {
        GameServer server = new GameServer();
        
        SwingUtilities.invokeLater(() -> new ServerUI(server));
        
        Thread serverThread = new Thread(server::start);
        serverThread.setName("GameServer-Thread");
        serverThread.start();
    }
}