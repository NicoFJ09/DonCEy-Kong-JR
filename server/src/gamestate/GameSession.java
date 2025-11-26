package server.src.gamestate;

import java.util.Map;
import java.util.HashMap;

/**
 * GameSession - Server-authoritative game state for a single player
 * 
 * This class maintains the authoritative state of a player's game session.
 * The client is responsible for all game logic, physics, and collision detection.
 * The server ONLY maintains score, lives, level, and speed multiplier.
 * 
 * Flow:
 * 1. Client detects event (fruit collected, death, level complete)
 * 2. Client shows immediate feedback (popup, animation)
 * 3. Client sends notification to server
 * 4. Server updates authoritative state
 * 5. Server sends confirmation to client
 * 6. Client updates HUD with server values
 */
public class GameSession {
    // Player identification
    private Integer playerId;
    
    // Server-authoritative state (ONLY these values)
    private Integer score;
    private Integer lives;
    private Integer level;
    private Float speedMultiplier;
    
    // Session state
    private SessionState state;
    
    // Admin-spawned fruit tracking
    // Maps fruit ID -> SpawnedFruit
    private Map<Integer, SpawnedFruit> adminFruits;
    private int nextFruitId;
    
    // Initial fruits spawned at session start (persist across level resets)
    private Map<Integer, SpawnedFruit> initialFruits;
    
    /**
     * Represents a fruit spawned by admin panel
     */
    public static class SpawnedFruit {
        public int id;
        public int vineId;
        public int positionY;
        public String type;  // "Mango- 200 pts", etc.
        
        public SpawnedFruit(int id, int vineId, int positionY, String type) {
            this.id = id;
            this.vineId = vineId;
            this.positionY = positionY;
            this.type = type;
        }
        
        @Override
        public String toString() {
            return type + " (Liana: " + vineId + ", PosY: " + positionY + ")";
        }
    }
    
    public enum SessionState {
        PLAYING,
        DEAD,          // Player died, waiting for respawn
        LEVEL_COMPLETE, // Level completed, waiting for next level
        GAME_OVER      // All lives lost
    }
    
    /**
     * Create new game session for player
     */
    public GameSession(Integer playerId) {
        this.playerId = playerId;
        this.score = 0;
        this.lives = 3;
        this.level = 1;
        this.speedMultiplier = 1.0f;
        this.state = SessionState.PLAYING;
        
        // Initialize fruit tracking
        this.adminFruits = new HashMap<>();
        this.initialFruits = new HashMap<>();
        this.nextFruitId = 1000;  // Start at 1000 to avoid conflicts with level fruits (0-999)
        
        System.out.println("[SESSION] Created for Player #" + playerId + 
                          " - Lives: 3, Score: 0, Level: 1");
    }
    
    // ========== EVENT HANDLERS ==========
    
    /**
     * Handle fruit collection event from client
     * Client has already shown popup and hidden fruit
     * Server updates score and confirms
     */
    public synchronized void onFruitCollected(int points) {
        if (state != SessionState.PLAYING) {
            System.out.println("[SESSION] Player #" + playerId + 
                              " collected fruit while not playing (ignored)");
            return;
        }
        
        score += points;
        
        System.out.println("[SESSION] Player #" + playerId + 
                          " collected fruit +" + points + " points (Total: " + score + ")");
    }
    
    /**
     * Handle player death event from client
     * Client has already shown death animation
     * Server updates lives and determines if game over
     */
    public synchronized void onPlayerDied(String cause) {
        if (state != SessionState.PLAYING) {
            System.out.println("[SESSION] Player #" + playerId + 
                              " died while not playing (ignored)");
            return;
        }
        
        lives--;
        
        System.out.println("[SESSION] Player #" + playerId + 
                          " died (" + cause + ") - Lives: " + lives);
        
        if (lives <= 0) {
            state = SessionState.GAME_OVER;
            System.out.println("[SESSION] Player #" + playerId + 
                              " ☠️ GAME OVER - Final Score: " + score);
        } else {
            state = SessionState.DEAD;
        }
    }
    
    /**
     * Handle level completion event from client
     * Client has already shown victory animation
     * Server increments level, adds bonus life, increases speed
     */
    public synchronized void onLevelCompleted() {
        if (state != SessionState.PLAYING) {
            System.out.println("[SESSION] Player #" + playerId + 
                              " completed level while not playing (ignored)");
            return;
        }
        
        level++;
        lives++;  // Bonus life
        speedMultiplier += 0.2f;
        state = SessionState.LEVEL_COMPLETE;
        
        System.out.println("[SESSION] Player #" + playerId + 
                          " completed level! Now Level " + level + 
                          ", Lives: " + lives + 
                          ", Speed: " + String.format("%.1fx", speedMultiplier));
    }
    
    /**
     * Client confirms ready for next level
     * Reset state to PLAYING
     */
    public synchronized void onLevelReady() {
        if (state == SessionState.LEVEL_COMPLETE) {
            state = SessionState.PLAYING;
            System.out.println("[SESSION] Player #" + playerId + " ready for level " + level);
        }
    }
    
    /**
     * Client respawned after death
     * Reset state to PLAYING
     */
    public synchronized void onRespawn() {
        if (state == SessionState.DEAD) {
            state = SessionState.PLAYING;
            System.out.println("[SESSION] Player #" + playerId + " respawned");
        }
    }
    
    /**
     * Reset session to initial state when player reconnects
     * This allows spectators to persist and watch multiple games from same player
     */
    public synchronized void reset() {
        this.score = 0;
        this.lives = 3;
        this.level = 1;
        this.speedMultiplier = 1.0f;
        this.state = SessionState.PLAYING;
        
        System.out.println("[SESSION] Player #" + playerId + 
                          " session reset - Lives: 3, Score: 0, Level: 1");
    }
    
    /**
     * Reset level (called when player dies but has lives remaining)
     * Re-spawns all admin fruits that were spawned during session
     */
    public synchronized void resetLevel() {
        // Restore all admin fruits (both initial + runtime spawned)
        adminFruits.clear();
        adminFruits.putAll(initialFruits);
        
        System.out.println("[SESSION] Player #" + playerId + 
                          " level reset - Restored " + adminFruits.size() + " admin fruits");
    }
    
    // ========== ADMIN FRUIT MANAGEMENT ==========
    
    /**
     * Spawn fruit via admin panel
     * @param isInitial If true, fruit is saved to initialFruits for respawning on level reset
     * @return Fruit ID assigned
     */
    public synchronized int spawnAdminFruit(int vineId, int positionY, String type, boolean isInitial) {
        int fruitId = nextFruitId++;
        SpawnedFruit fruit = new SpawnedFruit(fruitId, vineId, positionY, type);
        adminFruits.put(fruitId, fruit);
        
        // If initial fruit, save to initialFruits for respawning
        if (isInitial) {
            initialFruits.put(fruitId, fruit);
        }
        
        System.out.println("[SESSION] Player #" + playerId + 
                          " admin fruit spawned: ID=" + fruitId + 
                          ", vine=" + vineId + ", posY=" + positionY + 
                          ", type=" + type + 
                          (isInitial ? " [INITIAL]" : " [RUNTIME]"));
        
        return fruitId;
    }
    
    /**
     * Spawn fruit via admin panel (default: runtime spawned, not persistent)
     * @return Fruit ID assigned
     */
    public synchronized int spawnAdminFruit(int vineId, int positionY, String type) {
        return spawnAdminFruit(vineId, positionY, type, false);
    }
    
    /**
     * Remove fruit by ID
     */
    public synchronized boolean removeAdminFruit(int fruitId) {
        SpawnedFruit removed = adminFruits.remove(fruitId);
        if (removed != null) {
            System.out.println("[SESSION] Player #" + playerId + 
                              " admin fruit removed: ID=" + fruitId);
            return true;
        }
        return false;
    }
    
    /**
     * Get all spawned fruits for UI display
     */
    public synchronized Map<Integer, SpawnedFruit> getAdminFruits() {
        return new HashMap<>(adminFruits);
    }
    
    // ========== GETTERS ==========
    
    public Integer getPlayerId() { return playerId; }
    public Integer getScore() { return score; }
    public Integer getLives() { return lives; }
    public Integer getLevel() { return level; }
    public Float getSpeedMultiplier() { return speedMultiplier; }
    public SessionState getState() { return state; }
    
    public boolean isGameOver() {
        return state == SessionState.GAME_OVER;
    }
    
    public boolean isPlaying() {
        return state == SessionState.PLAYING;
    }
    
    // ========== SERIALIZATION ==========
    
    public String toJson() {
        return String.format(
            "{\"playerId\":%d,\"score\":%d,\"lives\":%d,\"level\":%d," +
            "\"speedMultiplier\":%.1f,\"state\":\"%s\"}",
            playerId, score, lives, level, speedMultiplier, state.name()
        );
    }
    
    @Override
    public String toString() {
        return String.format("GameSession[Player#%d, L%d, %d❤️, %d pts, %.1fx, %s]",
            playerId, level, lives, score, speedMultiplier, state);
    }
}
