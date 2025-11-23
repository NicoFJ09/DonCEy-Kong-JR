package server.src.game;

import java.util.ArrayList;
import java.util.List;

/**
 * MapData - Single source of truth for level geometry
 * Stores all map elements in BLOCK coordinates (not pixels)
 * Client converts blocks to pixels using PLATFORM_BLOCK_SIZE (24px)
 */
public class MapData {
    private List<Platform> platforms;
    private List<Column> columns;
    private List<VineGroup> vineGroups;

    public MapData() {
        this.platforms = new ArrayList<>();
        this.columns = new ArrayList<>();
        this.vineGroups = new ArrayList<>();
    }

    // ============================================================
    // DATA CONTAINER CLASSES
    // ============================================================

    /**
     * Platform - Horizontal floating platforms
     * Matches client's PlatformDef struct
     */
    public static class Platform {
        public int xBlock;
        public int yBlock;
        public int widthBlocks;

        public Platform(int xBlock, int yBlock, int widthBlocks) {
            this.xBlock = xBlock;
            this.yBlock = yBlock;
            this.widthBlocks = widthBlocks;
        }
    }

    /**
     * Column - Grass platforms with stems extending to water
     * Matches client's ColumnDef struct
     */
    public static class Column {
        public int xBlock;
        public int yBlock;
        public boolean wide;

        public Column(int xBlock, int yBlock, boolean wide) {
            this.xBlock = xBlock;
            this.yBlock = yBlock;
            this.wide = wide;
        }
    }

    /**
     * VineHeight - Individual vine height definition
     * Matches client's VineHeight struct
     */
    public static class VineHeight {
        public int yTopBlock;
        public int heightBlocks;

        public VineHeight(int yTopBlock, int heightBlocks) {
            this.yTopBlock = yTopBlock;
            this.heightBlocks = heightBlocks;
        }
    }

    /**
     * VineGroup - Group of vines at same X position
     * Matches client's VineGroupDef struct
     */
    public static class VineGroup {
        public int xBlock;
        public List<VineHeight> heights;

        public VineGroup(int xBlock, List<VineHeight> heights) {
            this.xBlock = xBlock;
            this.heights = heights;
        }
    }

    // ============================================================
    // BUILDER METHODS
    // ============================================================

    /**
     * Add a platform to the map
     * @param xBlock X position in blocks
     * @param yBlock Y position in blocks
     * @param widthBlocks Width in blocks
     */
    public void addPlatform(int xBlock, int yBlock, int widthBlocks) {
        platforms.add(new Platform(xBlock, yBlock, widthBlocks));
    }

    /**
     * Add a column to the map
     * @param xBlock X position in blocks (centered on grass)
     * @param yBlock Y position in blocks (top of grass)
     * @param wide true for wide grass/stem, false for normal
     */
    public void addColumn(int xBlock, int yBlock, boolean wide) {
        columns.add(new Column(xBlock, yBlock, wide));
    }

    /**
     * Add a vine group to the map
     * @param xBlock X position in blocks
     * @param heights List of vine heights for this group
     */
    public void addVineGroup(int xBlock, List<VineHeight> heights) {
        vineGroups.add(new VineGroup(xBlock, heights));
    }

    // ============================================================
    // JSON SERIALIZATION
    // ============================================================

    /**
     * Convert map data to JSON string
     * Manual serialization to avoid external dependencies
     * @return JSON string representation of map
     */
    public String toJson() {
        StringBuilder json = new StringBuilder();
        json.append("{");

        // Platforms array
        json.append("\"platforms\":[");
        for (int i = 0; i < platforms.size(); i++) {
            Platform p = platforms.get(i);
            json.append("{");
            json.append("\"xBlock\":").append(p.xBlock).append(",");
            json.append("\"yBlock\":").append(p.yBlock).append(",");
            json.append("\"widthBlocks\":").append(p.widthBlocks);
            json.append("}");
            if (i < platforms.size() - 1) json.append(",");
        }
        json.append("],");

        // Columns array
        json.append("\"columns\":[");
        for (int i = 0; i < columns.size(); i++) {
            Column c = columns.get(i);
            json.append("{");
            json.append("\"xBlock\":").append(c.xBlock).append(",");
            json.append("\"yBlock\":").append(c.yBlock).append(",");
            json.append("\"wide\":").append(c.wide);
            json.append("}");
            if (i < columns.size() - 1) json.append(",");
        }
        json.append("],");

        // Vine groups array
        json.append("\"vineGroups\":[");
        for (int i = 0; i < vineGroups.size(); i++) {
            VineGroup vg = vineGroups.get(i);
            json.append("{");
            json.append("\"xBlock\":").append(vg.xBlock).append(",");
            json.append("\"heights\":[");
            for (int j = 0; j < vg.heights.size(); j++) {
                VineHeight vh = vg.heights.get(j);
                json.append("{");
                json.append("\"yTopBlock\":").append(vh.yTopBlock).append(",");
                json.append("\"heightBlocks\":").append(vh.heightBlocks);
                json.append("}");
                if (j < vg.heights.size() - 1) json.append(",");
            }
            json.append("]");
            json.append("}");
            if (i < vineGroups.size() - 1) json.append(",");
        }
        json.append("]");

        json.append("}");
        return json.toString();
    }

    // ============================================================
    // GETTERS
    // ============================================================

    public List<Platform> getPlatforms() {
        return platforms;
    }

    public List<Column> getColumns() {
        return columns;
    }

    public List<VineGroup> getVineGroups() {
        return vineGroups;
    }
}
