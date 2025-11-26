package server.src;

import server.src.network.GameServer;
import server.src.ui.ServerUI;
import javax.swing.*;
import java.io.File;
import java.io.IOException;
import java.nio.channels.FileChannel;
import java.nio.channels.FileLock;
import java.nio.file.StandardOpenOption;

public class Main {
    private static final String LOCK_FILE = System.getProperty("java.io.tmpdir") + "/doncey-kong-server.lock";
    private static FileChannel lockChannel;
    private static FileLock serverLock;
    
    public static void main(String[] args) {
        // Intentar adquirir el lock antes de crear cualquier componente
        if (!acquireServerLock()) {
            System.err.println("ERROR: Ya hay una instancia del servidor ejecutándose en esta máquina.");
            SwingUtilities.invokeLater(() -> {
                JOptionPane.showMessageDialog(null, 
                    "Ya hay una instancia del servidor ejecutándose en esta máquina.\n" +
                    "Solo se permite una instancia del servidor a la vez.",
                    "Error - Servidor Ya Ejecutándose",
                    JOptionPane.ERROR_MESSAGE);
                System.exit(1);
            });
            return;
        }
        
        // Agregar shutdown hook para liberar el lock
        Runtime.getRuntime().addShutdownHook(new Thread(() -> releaseServerLock()));
        
        // Ahora sí, crear el servidor y la UI
        GameServer server = new GameServer();
        
        SwingUtilities.invokeLater(() -> {
            ServerUI ui = new ServerUI(server);
            server.setUI(ui);  // Set UI reference for updates
        });
        
        Thread serverThread = new Thread(server::start);
        serverThread.setName("GameServer-Thread");
        serverThread.start();
    }
    
    private static boolean acquireServerLock() {
        try {
            File lockFile = new File(LOCK_FILE);
            lockChannel = FileChannel.open(lockFile.toPath(), 
                StandardOpenOption.CREATE, 
                StandardOpenOption.WRITE);
            
            serverLock = lockChannel.tryLock();
            
            if (serverLock == null) {
                lockChannel.close();
                return false;
            }
            
            return true;
        } catch (IOException e) {
            System.err.println("Error al adquirir el lock del servidor: " + e.getMessage());
            return false;
        }
    }
    
    private static void releaseServerLock() {
        try {
            if (serverLock != null && serverLock.isValid()) {
                serverLock.release();
            }
            if (lockChannel != null && lockChannel.isOpen()) {
                lockChannel.close();
            }
            File lockFile = new File(LOCK_FILE);
            if (lockFile.exists()) {
                lockFile.delete();
            }
        } catch (IOException e) {
            System.err.println("Error al liberar el lock: " + e.getMessage());
        }
    }
}