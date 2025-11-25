package server.src.game;

import server.src.game.MapData.VineHeight;
import java.util.ArrayList;
import java.util.List;

/**
 * DefaultMap - Hardcoded default map initialization
 * Data copied directly from client/src/game/level.c
 * This is the single source of truth - client will receive this data via JSON
 */
public class DefaultMap {

    /**
     * Create the default map with all platforms, columns, and vines
     * Data matches PLATFORM_DEFS, COLUMN_DEFS, and VINE_GROUP_DEFS from level.c
     * @return Fully initialized MapData
     */
    public static MapData create() {
        MapData map = new MapData();

        // ============================================================
        // PLATFORMS
        // Copied from PLATFORM_DEFS[] in level.c
        // ============================================================

        map.addPlatform(0, 36, 15);   // Spawn platform
        map.addPlatform(0, 9, 30);    // Top left platform
        map.addPlatform(30, 10, 12);  // Top right platform
        map.addPlatform(36, 20, 14);  // Right platform
        map.addPlatform(12, 18, 6);   // Platform above third vine
        map.addPlatform(12, 24, 9);   // Platform below

        // ============================================================
        // COLUMNS
        // Copied from COLUMN_DEFS[] in level.c
        // ============================================================

        map.addColumn(25, 34, true);   // Wide column
        map.addColumn(32, 35, false);  // Normal column
        map.addColumn(39, 34, true);   // Wide column
        map.addColumn(47, 33, true);   // Wide column

        // ============================================================
        // VINE GROUPS
        // Copied from VINE_GROUP_DEFS[] and height arrays in level.c
        // ============================================================

        // Group 1 - GROUP1_HEIGHTS
        List<VineHeight> group1Heights = new ArrayList<>();
        group1Heights.add(new VineHeight(10, 24));  // Vine 1
        group1Heights.add(new VineHeight(10, 21));  // Vine 2
        group1Heights.add(new VineHeight(19, 15));  // Vine 3
        map.addVineGroup(8, group1Heights);

        // Group 2 - GROUP2_HEIGHTS
        List<VineHeight> group2Heights = new ArrayList<>();
        group2Heights.add(new VineHeight(10, 18));
        map.addVineGroup(23, group2Heights);

        // Group 3A - GROUP3A_HEIGHTS
        List<VineHeight> group3aHeights = new ArrayList<>();
        group3aHeights.add(new VineHeight(10, 9));
        group3aHeights.add(new VineHeight(11, 9));
        map.addVineGroup(28, group3aHeights);

        // Group 3B - GROUP3B_HEIGHTS
        List<VineHeight> group3bHeights = new ArrayList<>();
        group3bHeights.add(new VineHeight(11, 12));
        group3bHeights.add(new VineHeight(21, 6));
        group3bHeights.add(new VineHeight(11, 18));
        group3bHeights.add(new VineHeight(6, 21));
        group3bHeights.add(new VineHeight(6, 24));
        map.addVineGroup(35, group3bHeights);

        return map;
    }

    // Test main method to verify JSON output
    public static void main(String[] args) {
        MapData map = DefaultMap.create();
        String json = map.toJson();
        System.out.println("Generated JSON (" + json.length() + " bytes):");
        System.out.println(json);
        System.out.println("\nMap summary:");
        System.out.println("  Platforms: " + map.getPlatforms().size());
        System.out.println("  Columns: " + map.getColumns().size());
        System.out.println("  Vine Groups: " + map.getVineGroups().size());
    }
}
