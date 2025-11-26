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
        
        // Configure socket for high-load scenarios
        try {
            // Use larger buffers for better throughput under load
            socket.setSendBufferSize(65536);    // 64KB send buffer
            socket.setReceiveBufferSize(65536); // 64KB receive buffer
            
            // Performance optimizations
            socket.setPerformancePreferences(0, 1, 0); // latency > bandwidth > connection time
        } catch (SocketException e) {
            System.err.println("Warning: Could not optimize socket for client #" + id);
        }
    }
    
    @Override
    public void run() {
        try {
            // Use larger buffer for better performance under load
            input = new BufferedReader(new InputStreamReader(socket.getInputStream()), 8192);
            output = new PrintWriter(socket.getOutputStream(), true);
            
            sendMessage("CLIENT_ID:" + id);
            System.out.println("Sent initial CLIENT_ID:" + id);
            
            while (running) {
                type = handleClientSelection();
                if (type == null) {
                    System.out.println("Client #" + id + " disconnected");
                    break;
                }
                
                boolean returnToLobby = false;
                if (type == ClientType.PLAYER) {
                    returnToLobby = handlePlayerSession();
                } else {
                    returnToLobby = handleSpectatorSession();
                }
                
                if (type == ClientType.PLAYER) {
                    server.unregisterPlayerFromSession(id);
                } else if (type == ClientType.SPECTATOR && watchedPlayerId != null) {
                    server.unregisterSpectatorFromSession(id, watchedPlayerId);
                }
                
                if (!returnToLobby) {
                    break;
                }
                
                type = null;
                watchedPlayerId = null;
                System.out.println("Client #" + id + " returned to lobby");
            }
            
        } catch (SocketTimeoutException e) {
            System.err.println("Client #" + id + " timed out (no activity)");
        } catch (SocketException e) {
            if (running) {
                System.err.println("Client #" + id + " socket error: " + e.getMessage());
            }
        } catch (IOException e) {
            if (running) {
                System.err.println("Client #" + id + " I/O error: " + e.getMessage());
            }
        } catch (Exception e) {
            System.err.println("Client #" + id + " unexpected error: " + e.getMessage());
            e.printStackTrace();
        } finally {
            close();
            server.cleanup(id);
        }
    }
    
    private ClientType handleClientSelection() throws IOException {
        String selection;
        
        while (running && (selection = readLineWithTimeout()) != null) {
            selection = selection.trim();
            System.out.println("  Client #" + id + " sent: " + selection);
            
            if (selection.equals("LIST_PLAYERS")) {
                sendPlayerList();
                continue;
            }
            
            if (selection.equals("1")) {
                if (server.registerPlayer(id, address)) {
                    sendMessage("ACCEPTED:PLAYER");
                    sendMessage("CLIENT_ID:" + id);
                    sendMessage("SESSION_START");
                    sendMapData();
                    return ClientType.PLAYER;
                } else {
                    sendMessage("REJECTED:Players full");
                }
                continue;
            }
            
            if (selection.startsWith("JOIN_SPECTATOR:")) {
                try {
                    String[] parts = selection.split(":");
                    Integer playerId = Integer.parseInt(parts[1]);

                    if (server.registerSpectator(id, playerId)) {
                        watchedPlayerId = playerId;
                        sendMessage("ACCEPTED:SPECTATOR");
                        sendMessage("CLIENT_ID:" + id);
                        sendMessage("SESSION_START");
                        sendMapData();
                        return ClientType.SPECTATOR;
                    } else {
                        sendMessage("REJECTED:Cannot join - spectators full");
                    }
                } catch (Exception e) {
                    sendMessage("ERROR:Invalid spectator request");
                }
                continue;
            }
            
            if (selection.equals("DISCONNECT")) {
                System.out.println("Client #" + id + " disconnecting gracefully");
                sendMessage("BYE");
                return null;
            }
            
            if (selection.equalsIgnoreCase("exit")) {
                sendMessage("BYE");
                return null;
            }
        }
        
        return null;
    }
    
    private void sendPlayerList() {
        Map<Integer, NetworkPlayer> players = server.getPlayers();

        sendMessage("PLAYER_LIST_START");

        for (NetworkPlayer player : players.values()) {
            sendMessage("PLAYER:" + player.getId() + ":" +
                       player.getAddress() + ":" +
                       player.getSpectatorCount() + ":" +
                       Config.SPECTATORS_PER_PLAYER);
        }

        sendMessage("PLAYER_LIST_END");

        System.out.println("  Sent player list to client #" + id + " (" + players.size() + " players)");
    }

    private void sendMapData() {
        try {
            String jsonData = server.getMapData().toJson();
            sendMessage("MAP_DATA:" + jsonData);
            System.out.println("  Sent MAP_DATA to client #" + id + " (" + jsonData.length() + " bytes)");
        } catch (Exception e) {
            System.err.println("Error sending map data to client #" + id + ": " + e.getMessage());
        }
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
        String message;
        while (running && (message = readLineWithTimeout()) != null) {
            message = message.trim();
            
            if (message.equals("DISCONNECT")) {
                System.out.println("Client #" + id + " returning to lobby - cleaning up session");
                // Clean up player session to free slot for new players
                server.unregisterPlayerFromSession(id);
                return true;
            }
            
            if (message.equals("PLAY_AGAIN")) {
                System.out.println("Client #" + id + " wants to play again");
                sendMessage("INFO:Play again - starting new round");
                continue;
            }
            
            if (message.equalsIgnoreCase("exit")) {
                sendMessage("BYE");
                return false;
            }
            
            // Handle game events if player session
            if (type == ClientType.PLAYER) {
                if (handleGameEvent(message)) {
                    continue;
                }
            }
            
            sendMessage("ECHO:" + message);
        }
        
        return false;
    }
    
    /**
     * Handle game events from client
     * Returns true if message was a game event, false otherwise
     */
    private boolean handleGameEvent(String message) {
        server.src.gamestate.GameSession session = server.getGameSession(id);
        if (session == null) {
            return false;
        }
        
        // FRUIT_COLLECTED:fruitId:points
        if (message.startsWith("FRUIT_COLLECTED:")) {
            try {
                String[] parts = message.split(":");
                if (parts.length >= 3) {
                    int points = Integer.parseInt(parts[2]);
                    session.onFruitCollected(points);
                    sendMessage("SCORE_UPDATE:" + session.getScore());
                    return true;
                }
            } catch (Exception e) {
                System.err.println("Error parsing FRUIT_COLLECTED: " + e.getMessage());
            }
        }
        
        // PLAYER_DIED:cause
        if (message.startsWith("PLAYER_DIED:")) {
            try {
                String[] parts = message.split(":");
                if (parts.length >= 2) {
                    String cause = parts[1];
                    session.onPlayerDied(cause);
                    
                    // CRITICAL: If game over, send ONLY GAME_OVER (no lives update)
                    if (session.isGameOver()) {
                        String gameOverMsg = "GAME_OVER:" + session.getScore();
                        sendMessage(gameOverMsg);
                        System.out.println("[HANDLER] ☠️ Sent GAME_OVER to Player #" + id + ": " + gameOverMsg);
                    } else {
                        // Only send lives update if NOT game over
                        String livesMsg = "LIVES_UPDATE:" + session.getLives();
                        sendMessage(livesMsg);
                        System.out.println("[HANDLER] Sent LIVES_UPDATE to Player #" + id + ": " + livesMsg);
                    }
                    
                    return true;
                }
            } catch (Exception e) {
                System.err.println("Error parsing PLAYER_DIED: " + e.getMessage());
            }
        }
        
        // LEVEL_COMPLETED
        if (message.equals("LEVEL_COMPLETED")) {
            session.onLevelCompleted();
            sendMessage("LEVEL_UPDATE:" + session.getLevel() + ":" + 
                       session.getLives() + ":" + 
                       String.format("%.1f", session.getSpeedMultiplier()));
            session.onLevelReady();  // Auto-ready for next level
            return true;
        }
        
        // PLAYER_RESPAWN
        if (message.equals("PLAYER_RESPAWN")) {
            session.onRespawn();
            return true;
        }
        
        return false;
    }
    
    public void notifyPlayerDisconnected(Integer playerId) {
        sendMessage("PLAYER_DISCONNECTED:" + playerId);
        sendMessage("INFO:Player disconnected - connection closing");
        running = false;
        close();
    }
    
    public void notifyPlayerLeftSession(Integer playerId) {
        sendMessage("PLAYER_LEFT_SESSION:" + playerId);
        System.out.println("  Notified spectator #" + id + " that player #" + playerId + " left session");
    }
    
    public void sendMessageToPlayer(String message) {
        sendMessage(message);
    }
    
    /**
     * Read a line with timeout protection
     * Uses the socket's SO_TIMEOUT if set, or reads normally
     */
    private String readLineWithTimeout() throws IOException {
        try {
            // Check if socket is still connected before reading
            if (socket.isClosed() || !socket.isConnected()) {
                return null;
            }
            
            return input.readLine();
        } catch (SocketTimeoutException e) {
            // Timeout is expected for idle connections - just return null to check running flag
            if (running) {
                // If still supposed to be running, return empty to continue loop
                return "";
            }
            return null;
        } catch (SocketException e) {
            // Socket closed or reset - connection lost
            return null;
        }
    }
    
    private synchronized void sendMessage(String message) {
        if (output != null && !socket.isClosed()) {
            try {
                output.println(message);
                output.flush();
                // Check if output stream had an error
                if (output.checkError()) {
                    System.err.println("Warning: Error sending message to client #" + id);
                    running = false;
                }
            } catch (Exception e) {
                System.err.println("Error sending to client #" + id + ": " + e.getMessage());
                running = false;
            }
        }
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