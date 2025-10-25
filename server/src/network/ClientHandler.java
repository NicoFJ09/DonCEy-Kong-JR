package server.src.network;

import server.src.utils.Config;
import java.io.*;
import java.net.*;
import java.util.Map;

public class ClientHandler extends Thread {
    private Socket socket;
    private Integer id;
    private String address;
    private GameServer server;
    
    private BufferedReader input;
    private PrintWriter output;
    private ClientType type;
    private volatile boolean running;
    
    public ClientHandler(Socket socket, Integer id, String address, GameServer server) {
        this.socket = socket;
        this.id = id;
        this.address = address;
        this.server = server;
        this.running = true;
    }
    
    @Override
    public void run() {
        try {
            input = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            output = new PrintWriter(socket.getOutputStream(), true);
            
            type = selectClientType();
            if (type == null) {
                System.out.println("✗ Client #" + id + " disconnected during selection");
                return;
            }
            
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
            close();
            server.cleanup(id);
        }
    }
    
    private ClientType selectClientType() throws IOException {
        while (running) {
            displayLobby();
            
            String selection = input.readLine();
            if (selection == null) return null;
            
            selection = selection.trim();
            System.out.println("  Client #" + id + " selected: " + selection);
            
            switch (selection) {
                case "1":
                    if (server.registerPlayer(id, address)) {
                        return ClientType.PLAYER;
                    } else {
                        output.println("REJECTED:Players full");
                    }
                    break;
                    
                case "2":
                    Integer playerId = selectPlayer();
                    if (playerId != null && server.registerSpectator(id, playerId)) {
                        return ClientType.SPECTATOR;
                    }
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
    
    private void displayLobby() {
        output.println("========================================");
        output.println("LOBBY");
        output.println("========================================");
        
        Map<Integer, Player> players = server.getPlayers();
        output.println("Players: " + players.size() + "/" + Config.MAX_PLAYERS);
        
        if (!players.isEmpty()) {
            output.println("\nActive players:");
            for (Player player : players.values()) {
                String status = player.canAcceptSpectator() ? "✓" : "✗";
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
        output.flush();  // Explicit flush
    }
    
    private Integer selectPlayer() throws IOException {
        Map<Integer, Player> players = server.getPlayers();
        
        if (players.isEmpty()) {
            output.println("REJECTED:No players to spectate");
            return null;
        }
        
        output.println("\nSelect player to spectate:");
        for (Player player : players.values()) {
            output.println("  Player #" + player.getId() +
                         " [" + player.getSpectatorCount() + "/" +
                         Config.SPECTATORS_PER_PLAYER + " spectators]");
        }
        output.println();
        output.print("Enter player ID (or 'back'): ");
        output.flush();  // Explicit flush
        
        String response = input.readLine();
        if (response == null || response.trim().equalsIgnoreCase("back")) {
            return null;
        }
        
        try {
            Integer playerId = Integer.parseInt(response.trim());
            Player player = server.getPlayer(playerId);
            
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
    
    private void handlePlayerSession() throws IOException {
        output.println("ACCEPTED:PLAYER");
        output.println("CLIENT_ID:" + id);
        output.println("SESSION_START");
        
        System.out.println("✓ Client #" + id + " joined as PLAYER");
        
        placeholderGameLoop();
    }
    
    private void handleSpectatorSession() throws IOException {
        output.println("ACCEPTED:SPECTATOR");
        output.println("CLIENT_ID:" + id);
        output.println("SESSION_START");
        
        System.out.println("✓ Client #" + id + " joined as SPECTATOR");
        
        placeholderGameLoop();
    }
    
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
    
    public void notifyPlayerDisconnected(Integer playerId) {
        if (output != null) {
            output.println("PLAYER: " + playerId + " DISCONECTED");
            output.println("INFO: Connection closing...");
        }
        running = false;
        close();
    }
    
    private void close() {
        running = false;
        try {
            if (input != null) input.close();
            if (output != null) output.close();
            if (socket != null) socket.close();
        } catch (IOException e) {
            // Ignore
        }
    }
}