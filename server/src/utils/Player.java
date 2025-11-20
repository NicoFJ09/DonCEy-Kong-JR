package server.src.utils;

import server.src.network.NetworkPlayer;
import java.util.List;
import java.util.ArrayList;

public class Player extends NetworkPlayer {
    private List<String> fruits;
    private List<String> crocodiles;

    public Player(int id, String address) {
        super(id, address);
        this.fruits = new ArrayList<>();
        this.crocodiles = new ArrayList<>();
    }

    public List<String> getFruits() {
        return fruits;
    }

    public List<String> getCrocodiles() {
        return crocodiles;
    }

    // Add fruit to player - stores in same format as displayed in UI
    public void addFruit(String fruitType, String vineCount, String positionY) {
        String fruitEntry = fruitType + " (Liana: " + vineCount + ", PosY: " + positionY + ")";
        fruits.add(fruitEntry);
    }

    // Remove fruit from player by exact match
    public void removeFruit(String fruit) {
        fruits.remove(fruit);
    }

    // Remove fruit by index
    public void removeFruit(int index) {
        if (index >= 0 && index < fruits.size()) {
            fruits.remove(index);
        }
    }

    // Add crocodile to player
    public void addCrocodile(String crocodileType, int vineCount) {
        String crocEntry = crocodileType + ", " + vineCount;
        crocodiles.add(crocEntry);
    }

    // Remove crocodile from player
    public void removeCrocodile(int index) {
        if (index >= 0 && index < crocodiles.size()) {
            crocodiles.remove(index);
        }
    }

    // Get all crocodiles for this player
    public String getCrocodileInfo() {
        if (crocodiles.isEmpty()) {
            return "No crocodiles";
        }
        return String.join(", ", crocodiles);
    }

    // Get all fruits for this player
    public String getFruitInfo() {
        if (fruits.isEmpty()) {
            return "No fruits";
        }
        return String.join(", ", fruits);
    }

    @Override
    public String toString() {
        return "Player{" +
                "id=" + getId() +
                ", address='" + getAddress() + '\'' +
                ", fruits=" + fruits.size() +
                ", crocodiles=" + crocodiles.size() +
                '}';
    }
}