package server.src.network;

import server.src.utils.Config;
import java.util.*;

public class Player {
    private Integer id;
    private String address;
    private List<Integer> spectatorIds;
    
    public Player(Integer id, String address) {
        this.id = id;
        this.address = address;
        this.spectatorIds = new ArrayList<>();
    }
    
    public Integer getId() {
        return id;
    }
    
    public String getAddress() {
        return address;
    }
    
    public Integer getSpectatorCount() {
        return spectatorIds.size();
    }
    
    public boolean canAcceptSpectator() {
        return spectatorIds.size() < Config.SPECTATORS_PER_PLAYER;
    }
    
    public void addSpectator(Integer spectatorId) {
        spectatorIds.add(spectatorId);
    }
    
    public void removeSpectator(Integer spectatorId) {
        spectatorIds.remove(spectatorId);
    }
    
    @Override
    public String toString() {
        return "Player #" + id + " [" + address + "] - " + 
               spectatorIds.size() + "/" + Config.SPECTATORS_PER_PLAYER + " spectators";
    }
}