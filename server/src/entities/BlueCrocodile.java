package entities;

// Cocodrilo Azul cae de liana cuando alcanza el final
// Baja hasta nivel del agua y muere al tocarla

public class BlueCrocodile extends Enemy {
    private Float waterLevel;  // Nivel del agua donde desaparece
    
    public BlueCrocodile(Integer id, Float x, Float y, Integer vineId, 
                         Float waterLevel, Float speed) {
        super(id, x, y, vineId, "BLUE");
        this.waterLevel = waterLevel;
        this.speed = speed;
    }
    
    public Float getWaterLevel() { return waterLevel; }
    
    @Override
    public void updatePosition(Float deltaTime) {
        if (!isActive) return;
        
        // Solo cae verticalmente
        position.setY(position.getY() + speed * deltaTime);
        
        // Si llega al agua, se muere
        if (position.getY() >= waterLevel) {
            isActive = false;
        }
    }
    
    @Override
    public String toJson() {
        return String.format(
            "{\"id\":%d,\"x\":%.2f,\"y\":%.2f,\"vineId\":%d,\"type\":\"BLUE\"," +
            "\"active\":%b,\"speed\":%.2f}",
            id, position.getX(), position.getY(), vineId, isActive, speed
        );
    }
    
    @Override
    public String toString() {
        return String.format("BlueCrocodile[id=%d, pos=%s, vine=%d, active=%b, falling]",
            id, position, vineId, isActive);
    }
}