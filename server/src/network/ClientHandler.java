package server.src.network;

import server.src.utils.Config;
import java.io.*;
import java.net.*;
import java.util.Map;

/**
 * ClientHandler - Manages individual client connection in separate thread
 * 
 * Lifecycle:
 * 1. Setup I/O streams
 * 2. Lobby: Client selects PLAYER or SPECTATOR
 * 3. Game Session: Handle based on type
 * 4. Cleanup: Close resources and notify server
 */

public class ClientHandler extends Thread {
    private Socket socket;
    private Integer id;
    private String address;
    private GameServer server;
    
    private BufferedReader input;
    private PrintWriter output;
    private volatile boolean running;  // volatile for thread visibility

    private enum ClientType {
    PLAYER,      // Active game participant
    SPECTATOR    // Read-only observer
    }
    private ClientType type;

    public ClientHandler(Socket socket, Integer id, String address, GameServer server) {
        this.socket = socket;
        this.id = id;
        this.address = address;
        this.server = server;
        this.running = true;
    }
    
    // === Thread Lifecycle ===
    
    /**
     * Main thread execution
     * Template Method pattern: defines client connection lifecycle
     */
    @Override
    public void run() {
        try {
            // Phase 1: Setup I/O streams
            input = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            output = new PrintWriter(socket.getOutputStream(), true);
            
            // Phase 2: Lobby - client type selection
            type = selectClientType();
            if (type == null) {
                System.out.println("Client #" + id + " disconnected during selection");
                return;
            }
            
            // Phase 3: Game session (behavior varies by type)
            if (type == ClientType.PLAYER) {
                handlePlayerSession();
            } else {
                handleSpectatorSession();
            }
            
        } catch (IOException e) {
            if (running) {
                System.err.println("Error with client #" + id + ": " + e.getMessage());
            }
        } finally {
            // Phase 4: Cleanup (always executes)
            close();
            server.cleanup(id);
        }
    }
    
    // === Lobby Phase ===
    
    /**
     * Handle lobby interactions until client selects type
     * Loops until valid selection or disconnect
     * @return Selected ClientType, or null if disconnected
     * @throws IOException on network error
     */
    private ClientType selectClientType() throws IOException {
        while (running) {
            displayLobby();
            
            String selection = input.readLine();
            if (selection == null) return null;  // Client disconnected
            
            selection = selection.trim();
            System.out.println("  Client #" + id + " selected: " + selection);
            
            switch (selection) {
                case "1":  // Join as player
                    if (server.registerPlayer(id, address)) {
                        return ClientType.PLAYER;
                    } else {
                        output.println("REJECTED:Players full");
                    }
                    break;
                    
                case "2":  // Join as spectator
                    Integer playerId = selectPlayer();
                    if (playerId != null && server.registerSpectator(id, playerId)) {
                        return ClientType.SPECTATOR;
                    }
                    // If failed, loop back to lobby menu
                    break;
                    
                case "exit":
                    output.println("BYE");
                    return null;
                    
                default:
                    output.println("ERROR:Invalid option");
            }
        }
        return null;
    }
    
    /**
     * Display lobby menu to client
     * Shows current players and spectator availability
     */
    private void displayLobby() {
        output.println("========================================");
        output.println("LOBBY");
        output.println("========================================");
        
        Map<Integer, NetworkPlayer> players = server.getPlayers();
        output.println("Players: " + players.size() + "/" + Config.MAX_PLAYERS);
        
        // Show active players with spectator slots
        if (!players.isEmpty()) {
            output.println("\nActive players:");
            for (NetworkPlayer player : players.values()) {
                String status = player.canAcceptSpectator() ? "JOIN" : "FULL";
                output.println("  Player #" + player.getId() +
                             " [" + player.getSpectatorCount() + "/" +
                             Config.SPECTATORS_PER_PLAYER + " spectators] " + status);
            }
        }
        
        output.println("\nOptions:");
        output.println("  1 - Join as PLAYER");
        output.println("  2 - Join as SPECTATOR");
        output.println("  exit - Quit");
        output.println();
        output.print("> ");
        output.flush();  // Force send prompt without waiting for newline
    }
    
    /**
     * Handle spectator player selection
     * Shows available players and gets user choice
     * @return Selected player ID, or null if cancelled/invalid
     * @throws IOException on network error
     */
    private Integer selectPlayer() throws IOException {
        Map<Integer, NetworkPlayer> players = server.getPlayers();
        
        if (players.isEmpty()) {
            output.println("REJECTED:No players to spectate");
            return null;
        }
        
        // Show available players
        output.println("\nSelect player to spectate:");
        for (NetworkPlayer player : players.values()) {
            output.println("  Player #" + player.getId() +
                         " [" + player.getSpectatorCount() + "/" +
                         Config.SPECTATORS_PER_PLAYER + " spectators]");
        }
        output.println();
        output.print("Enter player ID (or 'back'): ");
        output.flush();
        
        String response = input.readLine();
        if (response == null || response.trim().equalsIgnoreCase("back")) {
            return null;  // Return to lobby menu
        }
        
        // Parse and validate player ID
        try {
            Integer playerId = Integer.parseInt(response.trim());
            NetworkPlayer player = server.getPlayer(playerId);
            
            if (player == null) {
                output.println("ERROR:Player not found");
                return null;
            }
            
            if (!player.canAcceptSpectator()) {
                output.println("REJECTED:Player spectators full");
                return null;
            }
            
            return playerId;
            
        } catch (NumberFormatException e) {
            output.println("ERROR:Invalid ID");
            return null;
        }
    }
    
    // === Game Session Phase ===
    
    /**
     * Handle player game session
     * TODO: Replace placeholderGameLoop() with actual game logic:
     * - Parse movement commands (WASD, space)
     * - Update GamePlayer state
     * - Broadcast state to spectators
     * @throws IOException on network error
     */

    private void handlePlayerSession() throws IOException {
        output.println("ACCEPTED:PLAYER");
        output.println("CLIENT_ID:" + id);
        output.println("SESSION_START");
        
        System.out.println("Client #" + id + " joined as PLAYER");
        
        placeholderGameLoop();
    }
    
    /**
     * Handle spectator game session
     * TODO: Replace placeholderGameLoop() with spectator logic:
     * - Receive game state updates from observed player
     * - Display read-only view
     * - Handle watched player disconnect
     * @throws IOException on network error
     */

    private void handleSpectatorSession() throws IOException {
        output.println("ACCEPTED:SPECTATOR");
        output.println("CLIENT_ID:" + id);
        output.println("SESSION_START");
        
        System.out.println("Client #" + id + " joined as SPECTATOR");
        
        placeholderGameLoop();
    }
    
    /**
     * Placeholder game loop for testing
     * Simple echo server until game logic implemented
     * @throws IOException on network error
     */
    private void placeholderGameLoop() throws IOException {
        output.println("INFO:Connected. Type 'exit' to disconnect");
        
        String message;
        while (running && (message = input.readLine()) != null) {
            if (message.trim().equalsIgnoreCase("exit")) {
                output.println("BYE");
                break;
            }
            output.println("ECHO:" + message);
        }
    }
    
    // === Observer Pattern ===
    
    /**
     * Notify spectator that watched player disconnected
     * Called by GameServer.cleanup() when player leaves
     * @param playerId ID of disconnected player
     */
    public void notifyPlayerDisconnected(Integer playerId) {
        if (output != null) {
            output.println("PLAYER_DISCONNECTED:" + playerId);
            output.println("INFO:Connection closing...");
        }
        running = false;
        close();
    }
    
    // === Cleanup ===
    
    /**
     * Close all resources
     * Called in finally block to ensure cleanup
     */
    private void close() {
        running = false;
        try {
            if (input != null) input.close();
            if (output != null) output.close();
            if (socket != null) socket.close();
        } catch (IOException e) {
            // Ignore cleanup errors
        }
    }
}