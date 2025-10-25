package server.src.utils;

public class Config {
    // Network settings
    public static final Integer SERVER_PORT = 12345;
    public static final String SERVER_HOST = "0.0.0.0";
    
    // Connection limits
    public static final Integer MAX_PLAYERS = 2;
    public static final Integer SPECTATORS_PER_PLAYER = 2;
    
    private Config() {}
}