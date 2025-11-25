package entities;

// Clase del jugador
// Maneja vidas, puntos, movimiento, estados de direccion y trepamiento.

public class GamePlayer extends Entity {
    private Float velocityX;
    private Float velocityY;
    private Integer lives;
    private Integer score;
    private Boolean isClimbing;
    private Boolean isOnPlatform;
    private Integer currentVineId;  // -1 si no está en liana
    private String state;  // "IDLE", "RUNNING", "CLIMBING", "JUMPING", "FALLING"
    private String facingDirection;  // "LEFT", "RIGHT"
    
    // Constructor
    public GamePlayer(Integer id, Float x, Float y) {
        super(id, x, y);
        this.velocityX = 0.0f;
        this.velocityY = 0.0f;
        this.lives = 3;
        this.score = 0;
        this.isClimbing = false;
        this.isOnPlatform = false;
        this.currentVineId = -1;
        this.state = "IDLE";
        this.facingDirection = "RIGHT";
    }
    
    // Getters
    public Float getVelocityX() { return velocityX; }
    public Float getVelocityY() { return velocityY; }
    public Integer getLives() { return lives; }
    public Integer getScore() { return score; }
    public Boolean getIsClimbing() { return isClimbing; }
    public Boolean getIsOnPlatform() { return isOnPlatform; }
    public Integer getCurrentVineId() { return currentVineId; }
    public String getState() { return state; }
    public String getFacingDirection() { return facingDirection; }
    
    // Setters
    public void setVelocityX(Float velocityX) { this.velocityX = velocityX; }
    public void setVelocityY(Float velocityY) { this.velocityY = velocityY; }
    public void setLives(Integer lives) { this.lives = lives; }
    public void setScore(Integer score) { this.score = score; }
    public void setIsClimbing(Boolean isClimbing) { this.isClimbing = isClimbing; }
    public void setIsOnPlatform(Boolean isOnPlatform) { this.isOnPlatform = isOnPlatform; }
    public void setCurrentVineId(Integer vineId) { this.currentVineId = vineId; }
    public void setState(String state) { this.state = state; }
    public void setFacingDirection(String direction) { this.facingDirection = direction; }
    
    // Métodos de juego
    public void loseLife() {
        if (lives > 0) {
            lives--;
        }
    }
    
    public void gainLife() {
        lives++;
    }
    
    public void addScore(Integer points) {
        score += points;
    }
    
    public Boolean isDead() {
        return lives <= 0;
    }
    
    // Métodos de movimiento
    public void moveLeft(Float speed) {
        velocityX = -speed;
        facingDirection = "LEFT";
        if (!isClimbing) {
            state = "RUNNING";
        }
    }
    
    public void moveRight(Float speed) {
        velocityX = speed;
        facingDirection = "RIGHT";
        if (!isClimbing) {
            state = "RUNNING";
        }
    }
    
    public void stopHorizontal() {
        velocityX = 0.0f;
        if (!isClimbing && isOnPlatform) {
            state = "IDLE";
        }
    }
    
    public void jump(Float jumpForce) {
        if (isOnPlatform && !isClimbing) {
            velocityY = -jumpForce;
            isOnPlatform = false;
            state = "JUMPING";
        }
    }
    
    public void climbUp(Float speed) {
        if (isClimbing) {
            velocityY = -speed;
            state = "CLIMBING";
        }
    }
    
    public void climbDown(Float speed) {
        if (isClimbing) {
            velocityY = speed;
            state = "CLIMBING";
        }
    }
    
    public void grabVine(Integer vineId) {
        isClimbing = true;
        currentVineId = vineId;
        velocityX = 0.0f;
        velocityY = 0.0f;
        state = "CLIMBING";
    }
    
    public void releaseVine() {
        isClimbing = false;
        currentVineId = -1;
        state = "FALLING";
    }
    
    public void updatePosition(Float deltaTime) {
        position.setX(position.getX() + velocityX * deltaTime);
        position.setY(position.getY() + velocityY * deltaTime);
    }
    
    @Override
    public String getType() {
        return "PLAYER";
    }
    
    @Override
    public String toJson() {
        return String.format(
            "{\"id\":%d,\"x\":%.2f,\"y\":%.2f,\"velx\":%.2f,\"vely\":%.2f," +
            "\"lives\":%d,\"score\":%d,\"climbing\":%b,\"vineId\":%d,\"state\":\"%s\",\"facing\":\"%s\"}",
            id, position.getX(), position.getY(), velocityX, velocityY,
            lives, score, isClimbing, currentVineId, state, facingDirection
        );
    }
    
    @Override
    public String toString() {
        return String.format("Player[id=%d, pos=%s, lives=%d, score=%d, state=%s]",
            id, position, lives, score, state);
    }
}