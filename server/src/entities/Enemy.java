package entities;

public abstract class Enemy extends Entity {
    protected Integer vineId;
    protected Float speed;
    protected Boolean isActive;
    protected String enemyType;  // "RED" o "BLUE"
    
    public Enemy(Integer id, Float x, Float y, Integer vineId, String enemyType) {
        super(id, x, y);
        this.vineId = vineId;
        this.isActive = true;
        this.enemyType = enemyType;
    }
    
    // Getters
    public Integer getVineId() { return vineId; }
    public Float getSpeed() { return speed; }
    public Boolean getIsActive() { return isActive; }
    public String getEnemyType() { return enemyType; }
    
    // Setters
    public void setSpeed(Float speed) { this.speed = speed; }
    public void setIsActive(Boolean active) { this.isActive = active; }
    
    // Método abstracto que cada tipo de cocodrilo implementará
    public abstract void updatePosition(Float deltaTime);
    
    public void deactivate() {
        isActive = false;
    }
    
    @Override
    public String getType() {
        return "ENEMY";
    }
}