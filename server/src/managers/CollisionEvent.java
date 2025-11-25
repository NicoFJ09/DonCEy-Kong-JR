package managers;

// Representa el evento de colision detectado
// Colisiones con enemigos, frutas, bordes o agua

public class CollisionEvent {
    public enum Type {
        PLAYER_ENEMY,
        PLAYER_FRUIT,
        PLAYER_WATER,
        PLAYER_PLATFORM
    }
    
    private Type type;
    private Integer playerId;
    private Integer entityId;  // Id de entidad con la que colisiona
    private String details;
    
    public CollisionEvent(Type type, Integer playerId, Integer entityId, String details) {
        this.type = type;
        this.playerId = playerId;
        this.entityId = entityId;
        this.details = details;
    }
    
    // Getters
    public Type getType() { return type; }
    public Integer getPlayerId() { return playerId; }
    public Integer getEntityId() { return entityId; }
    public String getDetails() { return details; }
    
    @Override
    public String toString() {
        return String.format("CollisionEvent[%s, player=%d, entity=%d, %s]",
            type, playerId, entityId, details);
    }
}