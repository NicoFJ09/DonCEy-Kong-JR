package server.src.network;

import server.src.utils.Config;
import java.util.*;

/**
 * NetworkPlayer - Connection metadata for a player
 * Stores network info only (IP, spectators)
 * Game state (position, lives, score) goes in separate GamePlayer class
 */
public class NetworkPlayer {
    private Integer id;
    private String address;
    private List<Integer> spectatorIds;
    
    public NetworkPlayer(Integer id, String address) {
        this.id = id;
        this.address = address;
        this.spectatorIds = new ArrayList<>();
    }
    
    // === Getters ===
    
    public Integer getId() {
        return id;
    }
    
    public String getAddress() {
        return address;
    }
    
    public Integer getSpectatorCount() {
        return spectatorIds.size();
    }
    
    // === Spectator Management ===
    
    public boolean canAcceptSpectator() {
        return spectatorIds.size() < Config.SPECTATORS_PER_PLAYER;
    }

    public void addSpectator(Integer spectatorId) {
        spectatorIds.add(spectatorId);
    }
    
    public void removeSpectator(Integer spectatorId) {
        spectatorIds.remove(spectatorId);
    }
    
    // Used for server logging
    @Override
    public String toString() {
        return "Player #" + id + " [" + address + "] - " + 
               spectatorIds.size() + "/" + Config.SPECTATORS_PER_PLAYER + " spectators";
    }
}