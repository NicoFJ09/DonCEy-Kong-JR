package gamestate;

import entities.*;
import java.util.*;
import entities.Position;

// Contenedor principal de manejo de estados del juego
// Se encarga de gestionar a los jugadores, enemigos, frutas, los niveles y su dificultad.
// Serializa todos los datos a jsons para enviar a clientes

public class GameState {
    // Contenedores de entidades
    private Map<Integer, GamePlayer> players;  // key = playerId
    private Map<Integer, Enemy> enemies;       // key = enemyId
    private Map<Integer, Fruit> fruits;        // key = fruitId
    
    // Info del nivel
    private Integer level;
    private Float difficultyMultiplier;
    private Float waterLevel;
    private Boolean gameOver;
    
    // Contadores de IDs
    private Integer nextEnemyId;
    private Integer nextFruitId;
    
    // Constructor
    public GameState() {
        this.players = new HashMap<>();
        this.enemies = new HashMap<>();
        this.fruits = new HashMap<>();
        this.level = 1;
        this.difficultyMultiplier = 1.0f;
        this.waterLevel = 550.0f;  
        this.gameOver = false;
        this.nextEnemyId = 1;
        this.nextFruitId = 1;
    }
    
    // ========== PLAYER MANAGEMENT ==========
    
    public void addPlayer(Integer playerId, Position startPos) {
        if (!players.containsKey(playerId)) {
            GamePlayer player = new GamePlayer(playerId, startPos.getX(), startPos.getY());
            players.put(playerId, player);
            System.out.println("Added player: " + player);
        }
    }
    
    public void removePlayer(Integer playerId) {
        GamePlayer removed = players.remove(playerId);
        if (removed != null) {
            System.out.println("Removed player: " + removed);
        }
    }
    
    public GamePlayer getPlayer(Integer playerId) {
        return players.get(playerId);
    }
    
    public Collection<GamePlayer> getAllPlayers() {
        return players.values();
    }
    
    public void updatePlayerPosition(Integer playerId, Position newPos) {
        GamePlayer player = players.get(playerId);
        if (player != null) {
            player.setPosition(newPos);
        }
    }
    
    // ========== ENEMY MANAGEMENT ==========
    
    public Integer addRedCrocodile(Float x, Float y, Integer vineId, 
                                   Float topY, Float bottomY, Float speed) {
        Integer id = nextEnemyId++;
        RedCrocodile croc = new RedCrocodile(id, x, y, vineId, topY, bottomY, 
                                            speed * difficultyMultiplier);
        enemies.put(id, croc);
        System.out.println("Added red crocodile: " + croc);
        return id;
    }
    
    public Integer addBlueCrocodile(Float x, Float y, Integer vineId, Float speed) {
        Integer id = nextEnemyId++;
        BlueCrocodile croc = new BlueCrocodile(id, x, y, vineId, waterLevel, 
                                              speed * difficultyMultiplier);
        enemies.put(id, croc);
        System.out.println("Added blue crocodile: " + croc);
        return id;
    }
    
    public void removeEnemy(Integer enemyId) {
        Enemy removed = enemies.remove(enemyId);
        if (removed != null) {
            System.out.println("Removed enemy: " + removed);
        }
    }
    
    public Enemy getEnemy(Integer enemyId) {
        return enemies.get(enemyId);
    }
    
    public Collection<Enemy> getAllEnemies() {
        return enemies.values();
    }
    
    // ========== FRUIT MANAGEMENT ==========
    
    public Integer addFruit(Float x, Float y, Integer vineId, Float height, 
                           Integer points, String fruitType) {
        Integer id = nextFruitId++;
        Fruit fruit = new Fruit(id, x, y, vineId, height, points, fruitType);
        fruits.put(id, fruit);
        System.out.println("Added fruit: " + fruit);
        return id;
    }
    
    public void removeFruit(Integer fruitId) {
        Fruit removed = fruits.remove(fruitId);
        if (removed != null) {
            System.out.println("Removed fruit: " + removed);
        }
    }
    
    // Método para encontrar fruta por liana y altura (para el admin)
    public Fruit findFruit(Integer vineId, Float height) {
        for (Fruit fruit : fruits.values()) {
            if (fruit.getVineId().equals(vineId) && 
                Math.abs(fruit.getHeight() - height) < 5.0f) {
                return fruit;
            }
        }
        return null;
    }
    
    public Fruit getFruit(Integer fruitId) {
        return fruits.get(fruitId);
    }
    
    public Collection<Fruit> getAllFruits() {
        return fruits.values();
    }
    
    // ========== LEVEL MANAGEMENT ==========
    
    public void nextLevel() {
        level++;
        difficultyMultiplier += 0.2f;  
        System.out.println("Level up! Now at level " + level + 
                          " (difficulty x" + difficultyMultiplier + ")");
    }
    
    public void resetLevel() {
        // Limpiar enemigos y frutas, pero mantener jugadores
        enemies.clear();
        fruits.clear();
        nextEnemyId = 1;
        nextFruitId = 1;
        System.out.println("Level reset");
    }
    
    // ========== GAME STATE QUERIES ==========
    
    public Boolean isGameOver() {
        // Game over si todos los jugadores están muertos
        for (GamePlayer player : players.values()) {
            if (!player.isDead()) {
                return false;
            }
        }
        return players.size() > 0;  // Solo si hay jugadores
    }
    
    public Integer getPlayerCount() {
        return players.size();
    }
    
    public Integer getActiveEnemyCount() {
        int count = 0;
        for (Enemy enemy : enemies.values()) {
            if (enemy.getIsActive()) count++;
        }
        return count;
    }
    
    // ========== SERIALIZATION ==========
    
    public String serializeState() {
        StringBuilder json = new StringBuilder();
        json.append("{");
        
        // Players
        json.append("\"players\":[");
        boolean first = true;
        for (GamePlayer player : players.values()) {
            if (!first) json.append(",");
            json.append(player.toJson());
            first = false;
        }
        json.append("],");
        
        // Enemies
        json.append("\"enemies\":[");
        first = true;
        for (Enemy enemy : enemies.values()) {
            if (enemy.getIsActive()) {  // Solo activos
                if (!first) json.append(",");
                json.append(enemy.toJson());
                first = false;
            }
        }
        json.append("],");
        
        // Fruits
        json.append("\"fruits\":[");
        first = true;
        for (Fruit fruit : fruits.values()) {
            if (!fruit.getCollected()) {  // Solo no recolectadas
                if (!first) json.append(",");
                json.append(fruit.toJson());
                first = false;
            }
        }
        json.append("],");
        
        // Level info
        json.append(String.format("\"level\":%d,\"waterLevel\":%.2f,\"gameOver\":%b",
            level, waterLevel, isGameOver()));
        
        json.append("}");
        return json.toString();
    }
    
    // Getters
    public Integer getLevel() { return level; }
    public Float getDifficultyMultiplier() { return difficultyMultiplier; }
    public Float getWaterLevel() { return waterLevel; }
    
    @Override
    public String toString() {
        return String.format("GameState[level=%d, players=%d, enemies=%d, fruits=%d]",
            level, players.size(), enemies.size(), fruits.size());
    }
}