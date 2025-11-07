package server.src.network;

import server.src.utils.Config;
import java.io.*;
import java.net.*;
import java.util.*;
import java.util.concurrent.ConcurrentHashMap;

/**
 * GameServer - Main server managing client connections
 * Accepts TCP connections and spawns ClientHandler threads
 * Thread-safe using ConcurrentHashMap + synchronized methods
 */
public class GameServer {
    private ServerSocket serverSocket;
    private boolean running;
    private Integer nextClientId;
    
    // ConcurrentHashMap provides thread-safe operations
    private Map<Integer, NetworkPlayer> players;        // id -> NetworkPlayer
    private Map<Integer, Integer> spectators;           // spectatorId -> playerId watching
    private Map<Integer, ClientHandler> clientHandlers; // id -> ClientHandler thread
    
    public GameServer() {
        this.running = false;
        this.nextClientId = 1;
        this.players = new ConcurrentHashMap<>();
        this.spectators = new ConcurrentHashMap<>();
        this.clientHandlers = new ConcurrentHashMap<>();
    }
    
    // === Server Lifecycle ===
    
    /**
     * Start server and accept connections
     * Blocks until server stops
     */
    public void start() {
        try {
            serverSocket = new ServerSocket(Config.SERVER_PORT);
            running = true;
            
            printServerHeader();
            
            // Main accept loop - runs until server stops
            while (running) {
                try {
                    Socket clientSocket = serverSocket.accept();
                    Integer clientId = nextClientId++;
                    String clientAddress = clientSocket.getInetAddress().getHostAddress();
                    
                    System.out.println("→ Client #" + clientId + " connected from " + clientAddress);
                    
                    // Spawn handler thread for this client
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
    
    /**
     * Stop server and close socket
     */
    public void stop() {
        running = false;
        try {
            if (serverSocket != null) serverSocket.close();
        } catch (IOException e) {
            System.err.println("Error stopping server: " + e.getMessage());
        }
    }
    
    // === Client Registration ===
    
    /**
     * Register new player
     * Called by ClientHandler after lobby selection
     * @param id Client ID
     * @param address Client IP address
     * @return true if registered, false if server full
     */
    public synchronized boolean registerPlayer(Integer id, String address) {
        if (players.size() >= Config.MAX_PLAYERS) {
            return false;
        }
        NetworkPlayer player = new NetworkPlayer(id, address);
        players.put(id, player);
        return true;
    }
    
    /**
     * Register spectator to watch a player
     * Called by ClientHandler after player selection
     * @param spectatorId Spectator's client ID
     * @param playerId Player to watch
     * @return true if registered, false if player doesn't exist or full
     */
    public synchronized boolean registerSpectator(Integer spectatorId, Integer playerId) {
        NetworkPlayer player = players.get(playerId);
        if (player == null || !player.canAcceptSpectator()) {
            return false;
        }
        player.addSpectator(spectatorId);
        spectators.put(spectatorId, playerId);
        return true;
    }
    
    // === Disconnection Handling ===
    
    /**
     * Cleanup when client disconnects
     * 
     * Two cases:
     * 1. Player disconnect: Remove player and notify all spectators (Observer pattern)
     * 2. Spectator disconnect: Remove from watched player's list
     * 
     * @param id Client ID that disconnected
     */
    public synchronized void cleanup(Integer id) {
        clientHandlers.remove(id);
        
        // Case 1: Disconnecting client is a player
        NetworkPlayer player = players.remove(id);
        if (player != null) {
            System.out.println("Player #" + id + " disconnected");
            
            // Find all spectators watching this player
            List<Integer> orphanedSpectators = new ArrayList<>();
            for (Map.Entry<Integer, Integer> entry : spectators.entrySet()) {
                if (entry.getValue().equals(id)) {
                    orphanedSpectators.add(entry.getKey());
                }
            }
            
            // Notify spectators that their player disconnected (Observer pattern)
            if (!orphanedSpectators.isEmpty()) {
                System.out.println("  Notifying " + orphanedSpectators.size() + " spectator(s)");
                
                for (Integer spectatorId : orphanedSpectators) {
                    spectators.remove(spectatorId);
                    
                    ClientHandler spectatorHandler = clientHandlers.get(spectatorId);
                    if (spectatorHandler != null) {
                        spectatorHandler.notifyPlayerDisconnected(id);
                    }
                }
            }
            
            printStatus();
            return;
        }
        
        // Case 2: Disconnecting client is a spectator
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
    
    // === Data Access ===
    
    /**
     * Get copy of players map
     * Returns copy for thread safety
     * @return Snapshot of current players
     */
    public synchronized Map<Integer, NetworkPlayer> getPlayers() {
        return new HashMap<>(players);
    }
    
    /**
     * Get specific player by ID
     * @param playerId Player ID to find
     * @return NetworkPlayer or null if not found
     */
    public synchronized NetworkPlayer getPlayer(Integer playerId) {
        return players.get(playerId);
    }
    
    // === Logging ===
    
    private void printServerHeader() {
        System.out.println("===========================================");
        System.out.println("🎮 DonCEy Kong Jr - Server");
        System.out.println("===========================================");
        
        // Get the actual network IP (not loopback)
        String serverIP = getNetworkIP();
        if (serverIP != null) {
            System.out.println("Server IP: " + serverIP);
        } else {
            System.out.println("Server IP: Could not determine (check network connection)");
        }
        
        System.out.println("Port: " + Config.SERVER_PORT);
        System.out.println("Max Players: " + Config.MAX_PLAYERS);
        System.out.println("Spectators per Player: " + Config.SPECTATORS_PER_PLAYER);
        System.out.println("\nWaiting for connections...\n");
    }
    
    /**
     * Get the actual network IP address (not loopback)
     * Returns the first non-loopback IPv4 address found
     */
    private String getNetworkIP() {
        try {
            Enumeration<NetworkInterface> interfaces = NetworkInterface.getNetworkInterfaces();
            
            while (interfaces.hasMoreElements()) {
                NetworkInterface iface = interfaces.nextElement();
                
                // Skip loopback and inactive interfaces
                if (iface.isLoopback() || !iface.isUp()) {
                    continue;
                }
                
                Enumeration<InetAddress> addresses = iface.getInetAddresses();
                while (addresses.hasMoreElements()) {
                    InetAddress addr = addresses.nextElement();
                    
                    // Only return IPv4 addresses (not IPv6)
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