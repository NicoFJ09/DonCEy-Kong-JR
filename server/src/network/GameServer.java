package server.src.network;

import server.src.utils.Config;
import server.src.game.MapData;
import server.src.game.DefaultMap;
import server.src.gamestate.GameSession;
import java.io.*;
import java.net.*;
import java.util.*;
import java.util.concurrent.ConcurrentHashMap;

public class GameServer {
    private ServerSocket serverSocket;
    private volatile boolean running;
    private Integer nextClientId;

    private Map<Integer, NetworkPlayer> players;
    private Map<Integer, Integer> spectators;
    private Map<Integer, ClientHandler> clientHandlers;
    private Map<Integer, GameSession> gameSessions;  // Player game state
    private MapData currentMap;

    public GameServer() {
        this.running = false;
        this.nextClientId = 1;
        this.players = new ConcurrentHashMap<>();
        this.spectators = new ConcurrentHashMap<>();
        this.clientHandlers = new ConcurrentHashMap<>();
        this.gameSessions = new ConcurrentHashMap<>();
        this.currentMap = DefaultMap.create();
        System.out.println("✓ Map initialized with " +
                         currentMap.getPlatforms().size() + " platforms, " +
                         currentMap.getColumns().size() + " columns, " +
                         currentMap.getVineGroups().size() + " vine groups");
    }
    
    public void start() {
        try {
            serverSocket = new ServerSocket(Config.SERVER_PORT, 50); // Increased backlog to 50 connections
            
            // Optimize server socket for high-load scenarios
            serverSocket.setPerformancePreferences(0, 1, 0); // latency > bandwidth > connection time
            serverSocket.setReuseAddress(true); // Allow quick restart without TIME_WAIT issues
            
            running = true;
            
            printServerHeader();
            
            while (running) {
                try {
                    Socket clientSocket = serverSocket.accept();
                    
                    // Configure socket for reliability and dead connection detection
                    clientSocket.setTcpNoDelay(true); // Disable Nagle's algorithm for lower latency
                    clientSocket.setKeepAlive(true); // Enable TCP keepalive to detect dead connections
                    clientSocket.setSoLinger(true, 0); // Close immediately without waiting for unsent data
                    
                    Integer clientId = nextClientId++;
                    String clientAddress = clientSocket.getInetAddress().getHostAddress();
                    
                    System.out.println("→ Client #" + clientId + " connected from " + clientAddress);
                    
                    ClientHandler handler = new ClientHandler(clientSocket, clientId, clientAddress, this);
                    clientHandlers.put(clientId, handler);
                    handler.start();
                    
                } catch (SocketException e) {
                    if (running) {
                        System.err.println("Socket error accepting client: " + e.getMessage());
                    }
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
    
    public void stop() {
        running = false;
        try {
            if (serverSocket != null) serverSocket.close();
        } catch (IOException e) {
            System.err.println("Error stopping server: " + e.getMessage());
        }
    }
    
    // CRITICAL: synchronized prevents TWO clients from both passing the size check at same time
    public synchronized boolean registerPlayer(Integer id, String address) {
        if (players.size() >= Config.MAX_PLAYERS) {
            System.out.println("  ✗ Player #" + id + " registration failed - server full (" + players.size() + "/" + Config.MAX_PLAYERS + ")");
            return false;
        }
        
        // Check if player already has a session (reconnecting after game over)
        GameSession existingSession = gameSessions.get(id);
        if (existingSession != null && existingSession.isGameOver()) {
            // Reset existing session instead of creating new one
            // This keeps spectators connected across multiple games
            existingSession.reset();
            System.out.println("  ✓ Player #" + id + " session reset (play again after game over)");
        } else if (existingSession == null) {
            // Create new session only if none exists
            GameSession newSession = new GameSession(id);
            gameSessions.put(id, newSession);
            System.out.println("  ✓ Player #" + id + " new session created");
        } else {
            // Session exists but not in GAME_OVER state (shouldn't happen, but log it)
            System.out.println("  ⚠ Player #" + id + " already has active session: " + existingSession.getState());
        }
        
        NetworkPlayer player = new NetworkPlayer(id, address);
        players.put(id, player);
        
        System.out.println("  ✓ Player #" + id + " registered (" + players.size() + "/" + Config.MAX_PLAYERS + ")");
        return true;
    }
    
    public synchronized boolean registerSpectator(Integer spectatorId, Integer playerId) {
        NetworkPlayer player = players.get(playerId);
        if (player == null) {
            System.out.println("  ✗ Spectator #" + spectatorId + " registration failed - player #" + playerId + " not found");
            return false;
        }
        if (!player.canAcceptSpectator()) {
            System.out.println("  ✗ Spectator #" + spectatorId + " registration failed - player #" + playerId + " full");
            return false;
        }
        player.addSpectator(spectatorId);
        spectators.put(spectatorId, playerId);
        System.out.println("  ✓ Spectator #" + spectatorId + " registered to Player #" + playerId);
        return true;
    }
    
    public synchronized void unregisterPlayerFromSession(Integer id) {
        NetworkPlayer player = players.remove(id);
        if (player != null) {
            System.out.println("  Player #" + id + " left session (staying connected)");
            
            List<Integer> watchingSpectators = new ArrayList<>();
            for (Map.Entry<Integer, Integer> entry : spectators.entrySet()) {
                if (entry.getValue().equals(id)) {
                    watchingSpectators.add(entry.getKey());
                }
            }
            
            if (!watchingSpectators.isEmpty()) {
                System.out.println("  Notifying " + watchingSpectators.size() + " spectator(s) that player left");
                
                // Notify in parallel to avoid blocking on slow connections
                for (Integer spectatorId : watchingSpectators) {
                    spectators.remove(spectatorId);
                    
                    ClientHandler spectatorHandler = clientHandlers.get(spectatorId);
                    if (spectatorHandler != null) {
                        // Fire and forget - don't block if notification fails
                        new Thread(() -> {
                            try {
                                spectatorHandler.notifyPlayerLeftSession(id);
                            } catch (Exception e) {
                                System.err.println("Failed to notify spectator #" + spectatorId + ": " + e.getMessage());
                            }
                        }, "Notify-Spectator-" + spectatorId).start();
                    }
                }
            }
        }
    }
    
    public synchronized void unregisterSpectatorFromSession(Integer spectatorId, Integer playerId) {
        spectators.remove(spectatorId);
        NetworkPlayer player = players.get(playerId);
        if (player != null) {
            player.removeSpectator(spectatorId);
            System.out.println("  Spectator #" + spectatorId + " left session (staying connected)");
        }
    }
    
    public synchronized void cleanup(Integer id) {
        clientHandlers.remove(id);
        
        // Clean up game session
        GameSession session = gameSessions.remove(id);
        if (session != null) {
            System.out.println("  Cleaned up game session: " + session);
        }
        
        NetworkPlayer player = players.remove(id);
        if (player != null) {
            System.out.println("Player #" + id + " disconnected");
            
            List<Integer> orphanedSpectators = new ArrayList<>();
            for (Map.Entry<Integer, Integer> entry : spectators.entrySet()) {
                if (entry.getValue().equals(id)) {
                    orphanedSpectators.add(entry.getKey());
                }
            }
            
            if (!orphanedSpectators.isEmpty()) {
                System.out.println("  Kicking " + orphanedSpectators.size() + " spectator(s)");
                
                // Notify in parallel to avoid blocking on slow connections
                for (Integer spectatorId : orphanedSpectators) {
                    spectators.remove(spectatorId);
                    
                    ClientHandler spectatorHandler = clientHandlers.get(spectatorId);
                    if (spectatorHandler != null) {
                        // Fire and forget - don't block if notification fails
                        new Thread(() -> {
                            try {
                                spectatorHandler.notifyPlayerDisconnected(id);
                            } catch (Exception e) {
                                System.err.println("Failed to notify spectator #" + spectatorId + ": " + e.getMessage());
                            }
                        }, "Kick-Spectator-" + spectatorId).start();
                    }
                }
            }
            
            printStatus();
            return;
        }
        
        Integer watchedPlayerId = spectators.remove(id);
        if (watchedPlayerId != null) {
            NetworkPlayer watchedPlayer = players.get(watchedPlayerId);
            if (watchedPlayer != null) {
                watchedPlayer.removeSpectator(id);
            }
            System.out.println("Spectator #" + id + " disconnected");
            printStatus();
        }
    }
    
    public synchronized Map<Integer, NetworkPlayer> getPlayers() {
        return new HashMap<>(players);
    }
    
    public synchronized NetworkPlayer getPlayer(Integer playerId) {
        return players.get(playerId);
    }

    public MapData getMapData() {
        return currentMap;
    }
    
    public synchronized GameSession getGameSession(Integer playerId) {
        return gameSessions.get(playerId);
    }

    public synchronized void sendMessageToClient(Integer clientId, String message) {
        ClientHandler handler = clientHandlers.get(clientId);
        if (handler != null) {
            handler.sendMessageToPlayer(message);
        } else {
            System.err.println("Client #" + clientId + " not found");
        }
    }
    
    private void printServerHeader() {
        System.out.println("===========================================");
        System.out.println("🎮 DonCEy Kong Jr - Server");
        System.out.println("===========================================");
        
        String serverIP = getNetworkIP();
        if (serverIP != null) {
            System.out.println("Server IP: " + serverIP);
        } else {
            System.out.println("Server IP: Could not determine");
        }
        
        System.out.println("Port: " + Config.SERVER_PORT);
        System.out.println("Max Players: " + Config.MAX_PLAYERS);
        System.out.println("Spectators per Player: " + Config.SPECTATORS_PER_PLAYER);
        System.out.println("\nWaiting for connections...\n");
    }
    
    public String getNetworkIP() {
        try {
            Enumeration<NetworkInterface> interfaces = NetworkInterface.getNetworkInterfaces();
            
            while (interfaces.hasMoreElements()) {
                NetworkInterface iface = interfaces.nextElement();
                
                if (iface.isLoopback() || !iface.isUp()) {
                    continue;
                }
                
                Enumeration<InetAddress> addresses = iface.getInetAddresses();
                while (addresses.hasMoreElements()) {
                    InetAddress addr = addresses.nextElement();
                    
                    if (addr instanceof Inet4Address) {
                        return addr.getHostAddress();
                    }
                }
            }
        } catch (SocketException e) {
            System.err.println("Error retrieving network interfaces: " + e.getMessage());
        }
        
        return null;
    }
    
    private void printStatus() {
        System.out.println("-------------------------------------------");
        System.out.println("Status: " + players.size() + " players, " + spectators.size() + " spectators");
        players.values().forEach(p -> System.out.println("  " + p));
        System.out.println("-------------------------------------------\n");
    }
}