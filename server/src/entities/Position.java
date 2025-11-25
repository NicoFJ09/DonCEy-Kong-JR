package entities;

// Representa coordenadas dentro del plano del juego (x,y)
// Metodos para calcular distancias entre posiciones

public class Position {
    private Float x;
    private Float y;
    
    public Position(Float x, Float y) {
        this.x = x;
        this.y = y;
    }
    
    // Getters y setters
    public Float getX() { return x; }
    public void setX(Float x) { this.x = x; }
    public Float getY() { return y; }
    public void setY(Float y) { this.y = y; }
    
    // Métodos útiles
    public Float distanceTo(Position other) {
        Float dx = this.x - other.x;
        Float dy = this.y - other.y;
        return (float) Math.sqrt(dx * dx + dy * dy);
    }
    
    public Position copy() {
        return new Position(this.x, this.y);
    }
    
    @Override
    public String toString() {
        return String.format("(%.2f, %.2f)", x, y);
    }
}