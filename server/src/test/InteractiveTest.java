package test;

import entities.*;
import gamestate.GameState;
import managers.*;

import java.util.*;

/**
 * InteractiveTest
 * --------------------------------------------
 * Herramienta de consola para probar el GameState
 * sin necesidad del cliente ni del servidor real.
 *
 * Permite:
 *  - Crear jugadores, frutas y enemigos
 *  - Mover entidades
 *  - Simular colisiones
 *  - Subir de nivel
 *  - Ver estado completo o en JSON
 *
 * IMPORTANTE:
 *  Este archivo es SOLO para pruebas de lógica.
 *  No prueba ni sockets, ni admin UI, ni cliente C.
 */
public class InteractiveTest {

    private static final String BLUE = "\u001B[36m";
    private static final String GREEN = "\u001B[32m";
    private static final String YELLOW = "\u001B[33m";
    private static final String RED = "\u001B[31m";
    private static final String RESET = "\u001B[0m";

    public static void main(String[] args) {

        Scanner scanner = new Scanner(System.in);
        GameState state = new GameState();
        CollisionManager collisionMgr = new CollisionManager();

        printHeader();
        printCommands();

        while (true) {
            System.out.print(BLUE + "> " + RESET);
            String input = scanner.nextLine().trim();
            if (input.isEmpty()) continue;

            String[] parts = input.split(" ");

            try {
                switch (parts[0].toLowerCase()) {

                    case "addplayer":
                        createPlayer(parts, state);
                        break;

                    case "addred":
                        createRedCroc(parts, state);
                        break;

                    case "addblue":
                        createBlueCroc(parts, state);
                        break;

                    case "addfruit":
                        createFruit(parts, state);
                        break;

                    case "move":
                        movePlayer(parts, state);
                        break;

                    case "update":
                        updateEnemies(parts, state);
                        break;

                    case "collisions":
                        runCollisionStep(state, collisionMgr);
                        break;

                    case "show":
                        showState(state);
                        break;

                    case "json":
                        System.out.println(state.serializeState());
                        break;

                    case "levelup":
                        state.nextLevel();
                        break;

                    case "exit":
                        System.out.println(GREEN + "¡Adiós!" + RESET);
                        return;

                    default:
                        System.out.println(RED + "Comando desconocido." + RESET);
                }

            } catch (Exception e) {
                System.out.println(RED + "Error: " + e.getMessage() + RESET);
            }
        }
    }

    // --------------------------------------------------------------------
    //  MÉTODOS AUXILIARES
    // --------------------------------------------------------------------

    private static void printHeader() {
        System.out.println(GREEN +
                "\n=== INTERACTIVE GAME STATE TEST ===" + RESET);
    }

    private static void printCommands() {
        System.out.println(YELLOW + "Comandos disponibles:" + RESET);
        System.out.println("""
          addplayer <id> <x> <y>
          addred <vineId> <x> <y> <topY> <botY> <speed>
          addblue <vineId> <x> <y> <speed>
          addfruit <vineId> <x> <y> <height> <points> <type>
          move <playerId> <x> <y>
          update <deltaTime>
          collisions
          show
          json
          levelup
          exit
        """);
    }

    private static void createPlayer(String[] p, GameState state) {
        int id = Integer.parseInt(p[1]);
        float x = Float.parseFloat(p[2]);
        float y = Float.parseFloat(p[3]);

        state.addPlayer(id, new Position(x, y));
        System.out.println(GREEN + "Jugador creado." + RESET);
    }

    private static void createRedCroc(String[] p, GameState state) {
        int vine = Integer.parseInt(p[1]);
        float x = Float.parseFloat(p[2]);
        float y = Float.parseFloat(p[3]);
        float top = Float.parseFloat(p[4]);
        float bot = Float.parseFloat(p[5]);
        float speed = Float.parseFloat(p[6]);

        state.addRedCrocodile(x, y, vine, top, bot, speed);
        System.out.println(GREEN + "Cocodrilo rojo creado." + RESET);
    }

    private static void createBlueCroc(String[] p, GameState state) {
        int vine = Integer.parseInt(p[1]);
        float x = Float.parseFloat(p[2]);
        float y = Float.parseFloat(p[3]);
        float speed = Float.parseFloat(p[4]);

        state.addBlueCrocodile(x, y, vine, speed);
        System.out.println(GREEN + "Cocodrilo azul creado." + RESET);
    }

    private static void createFruit(String[] p, GameState state) {
        int vine = Integer.parseInt(p[1]);
        float x = Float.parseFloat(p[2]);
        float y = Float.parseFloat(p[3]);
        float height = Float.parseFloat(p[4]);
        int points = Integer.parseInt(p[5]);
        String type = p[6];

        state.addFruit(x, y, vine, height, points, type);
        System.out.println(GREEN + "Fruta creada." + RESET);
    }

    private static void movePlayer(String[] p, GameState state) {
        int id = Integer.parseInt(p[1]);
        float x = Float.parseFloat(p[2]);
        float y = Float.parseFloat(p[3]);
        state.updatePlayerPosition(id, new Position(x, y));

        System.out.println(GREEN + "Jugador movido." + RESET);
    }

    private static void updateEnemies(String[] p, GameState state) {
        float dt = Float.parseFloat(p[1]);
        for (Enemy e : state.getAllEnemies()) {
            e.updatePosition(dt);
        }
        System.out.println(GREEN + "Enemigos actualizados." + RESET);
    }

    private static void runCollisionStep(GameState state,
                                         CollisionManager mgr) {

        List<CollisionEvent> events = mgr.checkCollisions(state);
        System.out.println(YELLOW + "Eventos detectados: " + events.size() + RESET);

        for (CollisionEvent e : events)
            System.out.println("  " + e);

        mgr.applyCollisions(state, events);
    }

    private static void showState(GameState state) {
        System.out.println("\n" + state);

        System.out.println("\nPlayers:");
        for (GamePlayer p : state.getAllPlayers())
            System.out.println("  " + p);

        System.out.println("\nEnemies:");
        for (Enemy e : state.getAllEnemies())
            System.out.println("  " + e);

        System.out.println("\nFruits:");
        for (Fruit f : state.getAllFruits())
            System.out.println("  " + f);

        System.out.println();
    }
}
