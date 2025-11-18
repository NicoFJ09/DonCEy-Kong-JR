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
    private volatile boolean running;

    private enum ClientType {
        PLAYER,
        SPECTATOR
    }
    private ClientType type;
    private Integer watchedPlayerId;

    public ClientHandler(Socket socket, Integer id, String address, GameServer server) {
        this.socket = socket;
        this.id = id;
        this.address = address;
        this.server = server;
        this.running = true;
        this.watchedPlayerId = null;
    }
    
    @Override
    public void run() {
        try {
            input = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            output = new PrintWriter(socket.getOutputStream(), true);
            
            // Send initial CLIENT_ID immediately upon connection
            output.println("CLIENT_ID:" + id);
            System.out.println("Sent initial CLIENT_ID:" + id + " to new connection");
            
            // Main lobby loop - client can join/leave sessions multiple times
            while (running) {
                type = handleClientSelection();
                if (type == null) {
                    System.out.println("Client #" + id + " disconnected");
                    break;
                }
                
                // Enter session
                boolean returnToLobby = false;
                if (type == ClientType.PLAYER) {
                    returnToLobby = handlePlayerSession();
                } else {
                    returnToLobby = handleSpectatorSession();
                }
                
                // Clean up session registration
                if (type == ClientType.PLAYER) {
                    server.unregisterPlayerFromSession(id);
                } else if (type == ClientType.SPECTATOR && watchedPlayerId != null) {
                    server.unregisterSpectatorFromSession(id, watchedPlayerId);
                }
                
                // If not returning to lobby, exit
                if (!returnToLobby) {
                    break;
                }
                
                // Reset session state for lobby
                type = null;
                watchedPlayerId = null;
                
                System.out.println("Client #" + id + " returned to lobby");
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
    
    private ClientType handleClientSelection() throws IOException {
        String selection;
        
        while (running && (selection = input.readLine()) != null) {
            selection = selection.trim();
            System.out.println("  Client #" + id + " sent: " + selection);
            
            if (selection.equals("LIST_PLAYERS")) {
                sendPlayerList();
                continue;
            }
            
            if (selection.equals("1")) {
                if (server.registerPlayer(id, address)) {
                    output.println("ACCEPTED:PLAYER");
                    output.println("CLIENT_ID:" + id);
                    output.println("SESSION_START");
                    return ClientType.PLAYER;
                } else {
                    output.println("REJECTED:Players full");
                }
                continue;
            }
            
            if (selection.startsWith("JOIN_SPECTATOR:")) {
                try {
                    String[] parts = selection.split(":");
                    Integer playerId = Integer.parseInt(parts[1]);
                    
                    if (server.registerSpectator(id, playerId)) {
                        watchedPlayerId = playerId;
                        output.println("ACCEPTED:SPECTATOR");
                        output.println("CLIENT_ID:" + id);
                        output.println("SESSION_START");
                        return ClientType.SPECTATOR;
                    } else {
                        output.println("REJECTED:Cannot join - spectators full");
                    }
                } catch (Exception e) {
                    output.println("ERROR:Invalid spectator request");
                }
                continue;
            }
            
            if (selection.equals("DISCONNECT")) {
                System.out.println("Client #" + id + " disconnecting gracefully");
                output.println("BYE");
                return null;
            }
            
            if (selection.equalsIgnoreCase("exit")) {
                output.println("BYE");
                return null;
            }
        }
        
        return null;
    }
    
    private void sendPlayerList() {
        Map<Integer, NetworkPlayer> players = server.getPlayers();
        
        output.println("PLAYER_LIST_START");
        
        for (NetworkPlayer player : players.values()) {
            output.println("PLAYER:" + player.getId() + ":" + 
                          player.getAddress() + ":" + 
                          player.getSpectatorCount() + ":" + 
                          Config.SPECTATORS_PER_PLAYER);
        }
        
        output.println("PLAYER_LIST_END");
        
        System.out.println("  Sent player list to client #" + id + " (" + players.size() + " players)");
    }
    
    private boolean handlePlayerSession() throws IOException {
        System.out.println("Client #" + id + " joined as PLAYER");
        return placeholderGameLoop();
    }
    
    private boolean handleSpectatorSession() throws IOException {
        System.out.println("Client #" + id + " joined as SPECTATOR (watching Player #" + watchedPlayerId + ")");
        return placeholderGameLoop();
    }
    
    private boolean placeholderGameLoop() throws IOException {
        // Don't send welcome message - it interferes with lobby protocol
        
        String message;
        while (running && (message = input.readLine()) != null) {
            message = message.trim();
            
            if (message.equals("DISCONNECT")) {
                System.out.println("Client #" + id + " returning to lobby");
                // Don't send BYE - client is returning to lobby
                return true; // Return to lobby
            }
            
            if (message.equals("PLAY_AGAIN")) {
                System.out.println("Client #" + id + " wants to play again");
                output.println("INFO:Play again - starting new round");
                continue;
            }
            
            if (message.equalsIgnoreCase("exit")) {
                output.println("BYE");
                return false; // Close connection
            }
            
            output.println("ECHO:" + message);
        }
        
        return false; // Connection lost or error
    }
    
    public void notifyPlayerDisconnected(Integer playerId) {
        if (output != null) {
            output.println("PLAYER_DISCONNECTED:" + playerId);
            output.println("INFO:Player disconnected - connection closing");
        }
        running = false;
        close();
    }
    
    public void notifyPlayerLeftSession(Integer playerId) {
        if (output != null) {
            output.println("PLAYER_LEFT_SESSION:" + playerId);
        }
        // Don't close connection - spectator stays connected
        System.out.println("  Notified spectator #" + id + " that player #" + playerId + " left session");
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