package server.src.network;

import server.src.utils.Config;
import java.io.*;
import java.net.*;
import java.util.*;
import java.util.concurrent.ConcurrentHashMap;

public class GameServer {
    private ServerSocket serverSocket;
    private boolean running;
    private Integer nextClientId;
    
    // Active players: key=playerId, value=Player
    private Map<Integer, Player> players;
    
    // Active spectators: key=spectatorId, value=playerId watching
    private Map<Integer, Integer> spectators;
    
    // Active client handlers: key=clientId, value=ClientHandler
    private Map<Integer, ClientHandler> clientHandlers;
    
    public GameServer() {
        this.running = false;
        this.nextClientId = 1;
        this.players = new ConcurrentHashMap<>();
        this.spectators = new ConcurrentHashMap<>();
        this.clientHandlers = new ConcurrentHashMap<>();
    }
    
    public void start() {
        try {
            serverSocket = new ServerSocket(Config.SERVER_PORT);
            running = true;
            
            System.out.println("===========================================");
            System.out.println("🎮 DonCEy Kong Jr - Server");
            System.out.println("===========================================");
            System.out.println("Port: " + Config.SERVER_PORT);
            System.out.println("Max Players: " + Config.MAX_PLAYERS);
            System.out.println("Spectators per Player: " + Config.SPECTATORS_PER_PLAYER);
            System.out.println("\nWaiting for connections...\n");
            
            while (running) {
                try {
                    Socket clientSocket = serverSocket.accept();
                    Integer clientId = nextClientId++;
                    String clientAddress = clientSocket.getInetAddress().getHostAddress();
                    
                    System.out.println("→ Client #" + clientId + " connected from " + clientAddress);
                    
                    // Create and start client handler thread
                    ClientHandler handler = new ClientHandler(clientSocket, clientId, clientAddress, this);
                    clientHandlers.put(clientId, handler);
                    handler.start();
                    
                } catch (IOException e) {
                    if (running) {
                        System.err.println("Error accepting client: " + e.getMessage());
                    }
                }
            }
            
        } catch (IOException e) {
            System.err.println("Error starting server: " + e.getMessage());
        }
    }
    
    // Called by ClientHandler when client type is selected
    public synchronized boolean registerPlayer(Integer id, String address) {
        if (players.size() >= Config.MAX_PLAYERS) {
            return false;
        }
        Player player = new Player(id, address);
        players.put(id, player);
        return true;
    }
    
    public synchronized boolean registerSpectator(Integer spectatorId, Integer playerId) {
        Player player = players.get(playerId);
        if (player == null || !player.canAcceptSpectator()) {
            return false;
        }
        player.addSpectator(spectatorId);
        spectators.put(spectatorId, playerId);
        return true;
    }
    
    public synchronized void cleanup(Integer id) {
        clientHandlers.remove(id);
        
        // Check if disconnecting client is a player
        Player player = players.remove(id);
        if (player != null) {
            System.out.println("✗ Player #" + id + " disconnected");
            
            // Find and notify orphaned spectators
            List<Integer> orphanedSpectators = new ArrayList<>();
            for (Map.Entry<Integer, Integer> entry : spectators.entrySet()) {
                if (entry.getValue().equals(id)) {
                    orphanedSpectators.add(entry.getKey());
                }
            }
            
            if (!orphanedSpectators.isEmpty()) {
                System.out.println("  ⚠️ Notifying " + orphanedSpectators.size() + " spectator(s)");
                
                for (Integer spectatorId : orphanedSpectators) {
                    spectators.remove(spectatorId);
                    
                    // Notify spectator that their player disconnected
                    ClientHandler spectatorHandler = clientHandlers.get(spectatorId);
                    if (spectatorHandler != null) {
                        spectatorHandler.notifyPlayerDisconnected(id);
                    }
                }
            }
            
            printStatus();
            return;
        }
        
        // Check if disconnecting client is a spectator
        Integer watchedPlayerId = spectators.remove(id);
        if (watchedPlayerId != null) {
            Player watchedPlayer = players.get(watchedPlayerId);
            if (watchedPlayer != null) {
                watchedPlayer.removeSpectator(id);
            }
            System.out.println("✗ Spectator #" + id + " disconnected");
            printStatus();
        }
    }
    
    public synchronized Map<Integer, Player> getPlayers() {
        return new HashMap<>(players);
    }
    
    public synchronized Player getPlayer(Integer playerId) {
        return players.get(playerId);
    }
    
    private void printStatus() {
        System.out.println("-------------------------------------------");
        System.out.println("Status: " + players.size() + " players, " + spectators.size() + " spectators");
        players.values().forEach(p -> System.out.println("  " + p));
        System.out.println("-------------------------------------------\n");
    }
    
    public void stop() {
        running = false;
        try {
            if (serverSocket != null) serverSocket.close();
        } catch (IOException e) {
            System.err.println("Error stopping server: " + e.getMessage());
        }
    }
}