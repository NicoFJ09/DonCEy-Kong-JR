package managers;

import gamestate.*;
import entities.*;
import java.util.*;

// Se encarga de detectar y aplicar las consecuencias de las colisiones de jugador
// Verifica las distancias entre entidades y aplica los cambios.

public class CollisionManager {
    private static final Float COLLISION_RADIUS = 20.0f;  // Radio de colisión
    
    public List<CollisionEvent> checkCollisions(GameState state) {
        List<CollisionEvent> events = new ArrayList<>();
        
        for (GamePlayer player : state.getAllPlayers()) {
            // 1. Colisiones con enemigos
            for (Enemy enemy : state.getAllEnemies()) {
                if (enemy.getIsActive() && isColliding(player, enemy)) {
                    events.add(new CollisionEvent(
                        CollisionEvent.Type.PLAYER_ENEMY,
                        player.getId(),
                        enemy.getId(),
                        "Hit by " + enemy.getEnemyType() + " crocodile"
                    ));
                }
            }
            
            // 2. Colisiones con frutas
            for (Fruit fruit : state.getAllFruits()) {
                if (!fruit.getCollected() && isColliding(player, fruit)) {
                    events.add(new CollisionEvent(
                        CollisionEvent.Type.PLAYER_FRUIT,
                        player.getId(),
                        fruit.getId(),
                        "Collected " + fruit.getFruitType() + " (+" + fruit.getPoints() + ")"
                    ));
                }
            }
            
            // 3. Caída al agua
            if (player.getPosition().getY() >= state.getWaterLevel()) {
                events.add(new CollisionEvent(
                    CollisionEvent.Type.PLAYER_WATER,
                    player.getId(),
                    -1,
                    "Fell into water"
                ));
            }
        }
        
        return events;
    }
    
    private Boolean isColliding(GamePlayer player, Entity entity) {
        Float distance = player.getPosition().distanceTo(entity.getPosition());
        return distance < COLLISION_RADIUS;
    }
    
    // Método para aplicar las colisiones al estado
    public void applyCollisions(GameState state, List<CollisionEvent> events) {
        for (CollisionEvent event : events) {
            switch (event.getType()) {
                case PLAYER_ENEMY:
                case PLAYER_WATER:
                    // Jugador pierde vida
                    GamePlayer player = state.getPlayer(event.getPlayerId());
                    if (player != null) {
                        player.loseLife();
                        System.out.println(event);
                    }
                    break;
                    
                case PLAYER_FRUIT:
                    // Jugador gana puntos
                    player = state.getPlayer(event.getPlayerId());
                    Fruit fruit = state.getFruit(event.getEntityId());
                    if (player != null && fruit != null) {
                        player.addScore(fruit.getPoints());
                        fruit.collect();
                        System.out.println(event);
                    }
                    break;
            }
        }
    }
}