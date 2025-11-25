package entities;

// Clase abstracta base para todas las entidades del juego
// Define ID, posición y métodos de serializacion

public abstract class Entity {
    protected Integer id;
    protected Position position;
    
    public Entity(Integer id, Float x, Float y) {
        this.id = id;
        this.position = new Position(x, y);
    }
    
    // Getters
    public Integer getId() { return id; }
    public Position getPosition() { return position; }
    
    // Setters
    public void setPosition(Position position) {
        this.position = position;
    }
    
    public void setPosition(Float x, Float y) {
        this.position.setX(x);
        this.position.setY(y);
    }
    
    // Método abstracto que cada entidad implementará
    public abstract String toJson();
    
    public abstract String getType();
}