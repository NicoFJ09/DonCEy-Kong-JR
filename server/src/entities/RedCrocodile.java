package entities;

// Cocodrilo Rojo que se mantiene en una sola liana hasta que muera
// Se mueve entre los limites de la liana

public class RedCrocodile extends Enemy {
    private Float topPatrolY;      // Límite superior de patrulla
    private Float bottomPatrolY;   // Límite inferior de patrulla
    private Boolean movingUp;      // true = sube, false = baja
    
    public RedCrocodile(Integer id, Float x, Float y, Integer vineId, 
                        Float topY, Float bottomY, Float speed) {
        super(id, x, y, vineId, "RED");
        this.topPatrolY = topY;
        this.bottomPatrolY = bottomY;
        this.speed = speed;
        this.movingUp = false;  // Empieza bajando
    }
    
    // Getters
    public Float getTopPatrolY() { return topPatrolY; }
    public Float getBottomPatrolY() { return bottomPatrolY; }
    public Boolean getMovingUp() { return movingUp; }
    
    // Setters
    public void setMovingUp(Boolean movingUp) { this.movingUp = movingUp; }
    
    @Override
    public void updatePosition(Float deltaTime) {
        if (!isActive) return;
        
        if (movingUp) {
            // Subir
            position.setY(position.getY() - speed * deltaTime);
            
            // Llegar arriba -> cambiar dirección
            if (position.getY() <= topPatrolY) {
                position.setY(topPatrolY);
                movingUp = false;
            }
        } else {
            // Bajar
            position.setY(position.getY() + speed * deltaTime);
            
            // Llegar abajo -> cambiar dirección
            if (position.getY() >= bottomPatrolY) {
                position.setY(bottomPatrolY);
                movingUp = true;
            }
        }
    }
    
    @Override
    public String toJson() {
        return String.format(
            "{\"id\":%d,\"x\":%.2f,\"y\":%.2f,\"vineId\":%d,\"type\":\"RED\"," +
            "\"active\":%b,\"speed\":%.2f}",
            id, position.getX(), position.getY(), vineId, isActive, speed
        );
    }
    
    @Override
    public String toString() {
        return String.format("RedCrocodile[id=%d, pos=%s, vine=%d, active=%b]",
            id, position, vineId, isActive);
    }
}