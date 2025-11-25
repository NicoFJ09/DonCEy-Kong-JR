package entities;

// Fruta recolectrable que da puntos al jugador
// Tiene posicion y en x liana y puede ser recolectada por jugador.

public class Fruit extends Entity {
    private Integer vineId;
    private Float height;  // Altura en la liana 
    private Integer points;
    private String fruitType;  
    private Boolean collected;
    
    public Fruit(Integer id, Float x, Float y, Integer vineId, Float height,
                 Integer points, String fruitType) {
        super(id, x, y);
        this.vineId = vineId;
        this.height = height;
        this.points = points;
        this.fruitType = fruitType;
        this.collected = false;
    }
    
    // Getters
    public Integer getVineId() { return vineId; }
    public Float getHeight() { return height; }
    public Integer getPoints() { return points; }
    public String getFruitType() { return fruitType; }
    public Boolean getCollected() { return collected; }
    
    // Métodos
    public void collect() {
        collected = true;
    }
    
    @Override
    public String getType() {
        return "FRUIT";
    }
    
    @Override
    public String toJson() {
        return String.format(
            "{\"id\":%d,\"x\":%.2f,\"y\":%.2f,\"vineId\":%d,\"height\":%.2f," +
            "\"points\":%d,\"type\":\"%s\",\"collected\":%b}",
            id, position.getX(), position.getY(), vineId, height, 
            points, fruitType, collected
        );
    }
    
    @Override
    public String toString() {
        return String.format("Fruit[id=%d, type=%s, vine=%d, height=%.2f, points=%d, collected=%b]",
            id, fruitType, vineId, height, points, collected);
    }
}