package server.src.ui;

import server.src.network.GameServer;

import javax.swing.*;
import java.awt.*;

public class ServerUI extends JFrame {
    
    public ServerUI(GameServer server) {
        initializeUI();
    }
    
    private void initializeUI() {
        setTitle("DonCEy Kong Jr - Server");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setSize(400, 300);
        setLocationRelativeTo(null);
        
        JPanel panel = new JPanel();
        panel.setBackground(Color.WHITE);
        add(panel);
        
        setVisible(true);
    }
}
