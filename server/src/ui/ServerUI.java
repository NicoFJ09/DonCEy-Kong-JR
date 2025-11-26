package server.src.ui;

import server.src.network.GameServer;
import server.src.network.NetworkPlayer;
import server.src.utils.Player;

import javax.swing.*;
import javax.swing.border.LineBorder;

import java.awt.*;
import java.util.Map;
import java.util.HashMap;

public class ServerUI extends JFrame {
    private GameServer server;
    private DefaultListModel<String> playerListModel;
    private JList<String> playerList;
    private Thread updateThread;
    private DefaultListModel<String> fruitListModel;
    private JList<String> fruitList;
    private String selectedPlayer = "Ninguno";
    private Integer selectedPlayerId = -1;
    private Map<Integer, Player> players;
    
    public ServerUI(GameServer server) {
        this.server = server;
        this.players = new HashMap<>();
        initializeUI();
        startPlayerUpdateThread();
    }
    
    private void initializeUI() {
        setTitle("DonCEy Kong Jr - Server");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        int x = 1600;
        int y = 1000;
        setSize(x, y);
        setResizable(false);
        setLocationRelativeTo(null);
        
        // Panel externo que actúa como borde
        JPanel outerPanel = new JPanel();
        outerPanel.setBackground(new Color(139, 69, 19)); // Color café (borde)
        outerPanel.setLayout(new BorderLayout());
        outerPanel.setBorder(BorderFactory.createEmptyBorder(15, 15, 15, 15)); // Grosor del borde
        
        JPanel mainPanel = new JPanel();
        mainPanel.setBackground(Color.WHITE);
        mainPanel.setLayout(null); // Usar null layout para posicionar con coordenadas
        
        JPanel leftPanel = new JPanel(new BorderLayout(5, 5));
        leftPanel.setBackground(Color.WHITE);
        leftPanel.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
        leftPanel.setBounds(0, 20, 395, 120); // Posicionar con coordenadas

        JLabel titleLabel = new JLabel("Jugadores Conectados a [IP: " + server.getNetworkIP() + "]");
        titleLabel.setFont(new Font("SansSerif", Font.BOLD, 18));
        titleLabel.setBounds(25, 10, 400, 25); // Posicionar en x=200
        mainPanel.add(titleLabel);

        JLabel playerSelectedLabel = new JLabel("Jugador Seleccionado:");
        playerSelectedLabel.setFont(new Font("SansSerif", Font.BOLD, 16));
        playerSelectedLabel.setBounds(25, 160, 400, 25);
        mainPanel.add(playerSelectedLabel);

        playerListModel = new DefaultListModel<>();
        playerList = new JList<>(playerListModel);
        playerList.setFont(new Font("Arial", Font.PLAIN, 15));
        playerList.setBorder(new LineBorder(new Color(139, 69, 19), 3));
        playerList.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
        playerList.setBackground(new Color(240, 240, 240));
        
        // Listener para capturar selección de jugador
        playerList.addListSelectionListener(e -> {
            int selectedIndex = playerList.getSelectedIndex();
            if (selectedIndex != -1) {
                selectedPlayer = playerList.getSelectedValue();
                playerSelectedLabel.setText("Jugador Seleccionado: " + selectedPlayer);
                
                // Extract player ID and update selectedPlayerId
                try {
                    String[] parts = selectedPlayer.split("#");
                    if (parts.length > 1) {
                        String idPart = parts[1].split(" ")[0];
                        selectedPlayerId = Integer.parseInt(idPart);
                    }
                } catch (Exception ex) {
                    selectedPlayerId = -1;
                }
            }
        });
        
        //centrar el texto en cada celda
        playerList.setCellRenderer(new DefaultListCellRenderer() {
            @Override
            public Component getListCellRendererComponent(JList<?> list, Object value, int index, boolean isSelected, boolean cellHasFocus) {
                Component comp = super.getListCellRendererComponent(list, value, index, isSelected, cellHasFocus);
                ((JLabel) comp).setHorizontalAlignment(SwingConstants.CENTER);
                return comp;
            }
        });
        
        JScrollPane scrollPane = new JScrollPane(playerList);
        leftPanel.add(scrollPane, BorderLayout.CENTER);
        leftPanel.setBounds(20, 40, 395, 120);
        mainPanel.add(leftPanel);
        
        // Separador horixontal 1
        JPanel separatorBottom = new JPanel();
        separatorBottom.setBackground(new Color(46,111,64));
        separatorBottom.setBounds(0, 200, 430, 8);
        mainPanel.add(separatorBottom);
        
        // Separador vertical posicionado a la derecha
        JPanel separator = new JPanel();
        separator.setBackground(new Color(46,111,64));
        separator.setBounds(430, 0, 10, y);
        mainPanel.add(separator);        

        //_____________________________________________________________
        //PARA LAS PARTES DE ABAJO DE EL PRIMER SEPARADOR HORIZONTAL
        //                      COCODRILOS
        //_____________________________________________________________
        JLabel secondLabel = new JLabel("Agregue Cocodrilos");
        secondLabel.setFont(new Font("SansSerif", Font.BOLD, 18));
        secondLabel.setBounds(120, 220, 200, 25);
        mainPanel.add(secondLabel);
        
        //para los tipos de cocodrilos
        JLabel typeLabel = new JLabel("Tipo de Cocodrilo:");
        typeLabel.setFont(new Font("Arial", Font.BOLD, 16));
        typeLabel.setBounds(25, 265, 200, 25);
        mainPanel.add(typeLabel);

        JComboBox<String> crocodileTypeCombo = new JComboBox<>(new String[]{"Cocodrilo Rojo", "Cocodrilo Azul"});
        crocodileTypeCombo.setFont(new Font("Arial", Font.PLAIN, 16));
        crocodileTypeCombo.setBorder(new LineBorder(new Color(139, 69, 19), 2));
        crocodileTypeCombo.setBackground(Color.WHITE);
        crocodileTypeCombo.setBounds(230, 265, 150, 30);
        mainPanel.add(crocodileTypeCombo);

        //para el numero de liana
        JLabel vineLabel = new JLabel("Numero de Lianas:");
        vineLabel.setFont(new Font("Arial", Font.BOLD, 16));
        vineLabel.setBounds(25, 310, 200, 25);
        mainPanel.add(vineLabel);

        // Map DefaultMap vine numbers to actual client IDs
        // DefaultMap shows vines 1-11, but client creates invisible laterals between them
        // Snapjaws can only spawn on 7 specific vines (from DefaultMap numbering)
        Map<String, Integer> snapjawVineMapping = new HashMap<>();
        snapjawVineMapping.put("Liana 1", 1);    // DefaultMap Vine 1 → Client ID 1
        snapjawVineMapping.put("Liana 2", 3);    // DefaultMap Vine 2 → Client ID 3
        snapjawVineMapping.put("Liana 4", 6);    // DefaultMap Vine 4 → Client ID 6
        snapjawVineMapping.put("Liana 5", 7);    // DefaultMap Vine 5 → Client ID 7
        snapjawVineMapping.put("Liana 6", 9);    // DefaultMap Vine 6 → Client ID 9
        snapjawVineMapping.put("Liana 7", 10);   // DefaultMap Vine 7 → Client ID 10
        snapjawVineMapping.put("Liana 9", 14);   // DefaultMap Vine 9 → Client ID 14

        // Dropdown shows DefaultMap vine numbers (what admin sees visually)
        JComboBox<String> vineCountCombo = new JComboBox<>(new String[]{"Liana 1", "Liana 2", "Liana 4", "Liana 5", "Liana 6", "Liana 7", "Liana 9"});
        vineCountCombo.setFont(new Font("Arial", Font.PLAIN, 16));
        vineCountCombo.setBorder(new LineBorder(new Color(139, 69, 19), 2));
        vineCountCombo.setBackground(Color.WHITE);
        vineCountCombo.setBounds(230, 310, 150, 30);
        mainPanel.add(vineCountCombo);

        JButton addButton = new JButton("Agregar Cocodrilo");
        addButton.setBorder(new LineBorder(new Color(139, 69, 19), 2));
        addButton.setBackground(new Color(211, 182, 131));
        addButton.setFont(new Font("Arial", Font.BOLD, 15));
        addButton.setBounds(120, 360, 180, 40);
        addButton.setOpaque(true);
        addButton.setBorderPainted(true);
        addButton.setContentAreaFilled(true);
        addButton.setFocusPainted(false);
        
        // Add button press visual feedback with hover effect
        addButton.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mouseEntered(java.awt.event.MouseEvent evt) {
                addButton.setBackground(new Color(190, 161, 110)); // Hover shade
            }
            public void mouseExited(java.awt.event.MouseEvent evt) {
                addButton.setBackground(new Color(211, 182, 131)); // Original color
            }
            public void mousePressed(java.awt.event.MouseEvent evt) {
                addButton.setBackground(new Color(150, 121, 70)); // Pressed shade (darker)
            }
            public void mouseReleased(java.awt.event.MouseEvent evt) {
                addButton.setBackground(new Color(190, 161, 110)); // Back to hover
            }
        });
        
        addButton.addActionListener(e -> {
            if (selectedPlayerId != -1) {
                try {
                    // Get player object (already created in updatePlayerList)
                    Player player = players.get(selectedPlayerId);
                    if (player != null) {
                        // Get crocodile data
                        String crocodileType = (String) crocodileTypeCombo.getSelectedItem();
                        String selectedVine = (String) vineCountCombo.getSelectedItem();
                        
                        // Map DefaultMap vine number to actual client ID
                        int clientVineId = snapjawVineMapping.get(selectedVine);
                        
                        // Add crocodile to player
                        player.addCrocodile(crocodileType, clientVineId);
                        
                        // Determine enemy type (RED or BLUE)
                        String enemyType = crocodileType.contains("Rojo") ? "RED" : "BLUE";
                        
                        // Send SPAWN_ENEMY command with client vine ID
                        String command = "SPAWN_ENEMY:" + enemyType + ":" + clientVineId;
                        server.sendMessageToClient(selectedPlayerId, command);
                        
                        System.out.println("✓ Sent to Player #" + selectedPlayerId + ": " + command);
                    }
                } catch (Exception ex) {
                    System.err.println("Error spawning enemy: " + ex.getMessage());
                    ex.printStackTrace();
                }
            } else {
                System.out.println("✗ No player selected");
            }
        });
        mainPanel.add(addButton);

        JPanel cocodrileSeparator = new JPanel();
        cocodrileSeparator.setBackground(new Color(46,111,64));
        cocodrileSeparator.setBounds(0, 415, 430, 8);
        mainPanel.add(cocodrileSeparator);

        //_____________________________________________________________
        //PARA LAS PARTES DE ABAJO DE EL SEGUNDO SEPARADOR HORIZONTAL
        //                      FRUTAS
        //_____________________________________________________________

        JLabel fruitsLabel = new JLabel("Agregue Frutas");
        fruitsLabel.setFont(new Font("SansSerif", Font.BOLD, 18));
        fruitsLabel.setBounds(145, 435, 200, 25);
        mainPanel.add(fruitsLabel);

        //para los tipos de frutas
        JLabel tipoFruta = new JLabel("Tipo de Fruta:");
        tipoFruta.setFont(new Font("Arial", Font.BOLD, 16));
        tipoFruta.setBounds(25, 480, 200, 25);
        mainPanel.add(tipoFruta);

        JComboBox<String> fruitPointsCombo = new JComboBox<>(new String[]{"Mango 800 pts", "Banana 400 pts", "Manzana 200 pts"});
        fruitPointsCombo.setBorder(new LineBorder(new Color(139, 69, 19), 2));
        fruitPointsCombo.setFont(new Font("Arial", Font.PLAIN, 16));
        fruitPointsCombo.setBackground(Color.WHITE);
        fruitPointsCombo.setBounds(230, 475, 165, 30);
        mainPanel.add(fruitPointsCombo);
    
        // Map ALL DefaultMap vine numbers (1-11) to actual client IDs
        Map<String, Integer> fruitVineMapping = new HashMap<>();
        fruitVineMapping.put("Liana 1", 1);     // DefaultMap Vine 1 → Client ID 1
        fruitVineMapping.put("Liana 2", 3);     // DefaultMap Vine 2 → Client ID 3
        fruitVineMapping.put("Liana 3", 5);     // DefaultMap Vine 3 → Client ID 5
        fruitVineMapping.put("Liana 4", 6);     // DefaultMap Vine 4 → Client ID 6
        fruitVineMapping.put("Liana 5", 7);     // DefaultMap Vine 5 → Client ID 7
        fruitVineMapping.put("Liana 6", 9);     // DefaultMap Vine 6 → Client ID 9
        fruitVineMapping.put("Liana 7", 10);    // DefaultMap Vine 7 → Client ID 10
        fruitVineMapping.put("Liana 8", 12);    // DefaultMap Vine 8 → Client ID 12
        fruitVineMapping.put("Liana 9", 14);    // DefaultMap Vine 9 → Client ID 14
        fruitVineMapping.put("Liana 10", 16);   // DefaultMap Vine 10 → Client ID 16
        fruitVineMapping.put("Liana 11", 18);   // DefaultMap Vine 11 → Client ID 18

        // Fruits: ALL visible vines (11 vines total - no physics restrictions)
        JComboBox<String> vineCountCombo2 = new JComboBox<>(new String[]{"Liana 1", "Liana 2", "Liana 3", "Liana 4", "Liana 5", "Liana 6", "Liana 7", "Liana 8", "Liana 9", "Liana 10", "Liana 11"});
        vineCountCombo2.setBorder(new LineBorder(new Color(139, 69, 19), 2));
        vineCountCombo2.setFont(new Font("Arial", Font.PLAIN, 15));
        vineCountCombo2.setBackground(Color.WHITE);
        vineCountCombo2.setBounds(230, 520, 150, 30);
        mainPanel.add(vineCountCombo2);

        JLabel vineLabel3 = new JLabel("Numero de Lianas:");
        vineLabel3.setFont(new Font("Arial", Font.BOLD, 16));
        vineLabel3.setBounds(25, 520, 200, 25);
        mainPanel.add(vineLabel3);

        //para el la posicion en y de la fruta
        JLabel positionYLabel = new JLabel("Posicion de Fruta:");
        positionYLabel.setFont(new Font("SansSerif", Font.BOLD, 16));
        positionYLabel.setBounds(25, 565, 200, 25);
        mainPanel.add(positionYLabel);

        // Map to store position ranges for ALL DefaultMap vines (used for fruits)
        // Based on heightBlocks from DefaultMap.java vine groups
        // Key = DefaultMap vine label, Value = height in blocks
        Map<String, Integer> vinePositionRanges = new HashMap<>();
        vinePositionRanges.put("Liana 1", 24);    // DefaultMap Vine 1 (height=24)
        vinePositionRanges.put("Liana 2", 21);    // DefaultMap Vine 2 (height=21)
        vinePositionRanges.put("Liana 3", 15);    // DefaultMap Vine 3 (height=15)
        vinePositionRanges.put("Liana 4", 18);    // DefaultMap Vine 4 (height=18)
        vinePositionRanges.put("Liana 5", 9);     // DefaultMap Vine 5 (height=9)
        vinePositionRanges.put("Liana 6", 9);     // DefaultMap Vine 6 (height=9)
        vinePositionRanges.put("Liana 7", 12);    // DefaultMap Vine 7 (height=12)
        vinePositionRanges.put("Liana 8", 6);     // DefaultMap Vine 8 (height=6)
        vinePositionRanges.put("Liana 9", 18);    // DefaultMap Vine 9 (height=18)
        vinePositionRanges.put("Liana 10", 21);   // DefaultMap Vine 10 (height=21)
        vinePositionRanges.put("Liana 11", 24);   // DefaultMap Vine 11 (height=24)


        JComboBox<String> positionYCombo = new JComboBox<>();
        positionYCombo.setBorder(new LineBorder(new Color(139, 69, 19), 2));
        positionYCombo.setFont(new Font("SansSerif", Font.PLAIN, 16));
        positionYCombo.setBackground(Color.WHITE);
        positionYCombo.setBounds(230, 565, 150, 30);
        mainPanel.add(positionYCombo);

        // Listener to update position options when vine count changes
        vineCountCombo2.addActionListener(e -> {
            String selectedVine = (String) vineCountCombo2.getSelectedItem();
            positionYCombo.removeAllItems();
            
            if (selectedVine != null && vinePositionRanges.containsKey(selectedVine)) {
                int maxPosition = vinePositionRanges.get(selectedVine);
                for (int i = 0; i <= maxPosition; i++) {
                    positionYCombo.addItem(String.valueOf(i));
                }
            }
        });

        // Initialize with first vine's positions
        String initialVine = (String) vineCountCombo2.getSelectedItem();
        if (initialVine != null && vinePositionRanges.containsKey(initialVine)) {
            int maxPosition = vinePositionRanges.get(initialVine);
            for (int i = 0; i <= maxPosition; i++) {
                positionYCombo.addItem(String.valueOf(i));
            }
        }

        JButton addFruitButton = new JButton("Agregar Fruta");
        addFruitButton.setBorder(new LineBorder(new Color(139, 69, 19), 2));
        addFruitButton.setBackground(new Color(211, 182, 131));
        addFruitButton.setFont(new Font("Arial", Font.BOLD, 15));
        addFruitButton.setBounds(120, 610, 180, 40);
        addFruitButton.setOpaque(true);
        addFruitButton.setBorderPainted(true);
        addFruitButton.setContentAreaFilled(true);
        addFruitButton.setFocusPainted(false);
        
        // Add button press visual feedback with hover effect
        addFruitButton.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mouseEntered(java.awt.event.MouseEvent evt) {
                addFruitButton.setBackground(new Color(190, 161, 110)); // Hover shade
            }
            public void mouseExited(java.awt.event.MouseEvent evt) {
                addFruitButton.setBackground(new Color(211, 182, 131)); // Original color
            }
            public void mousePressed(java.awt.event.MouseEvent evt) {
                addFruitButton.setBackground(new Color(150, 121, 70)); // Pressed shade (darker)
            }
            public void mouseReleased(java.awt.event.MouseEvent evt) {
                addFruitButton.setBackground(new Color(190, 161, 110)); // Back to hover
            }
        });

        addFruitButton.addActionListener(e -> {
            if (selectedPlayerId != -1) {
                try {
                    // Get game session for fruit tracking
                    server.src.gamestate.GameSession session = server.getGameSession(selectedPlayerId);
                    if (session != null) {
                        // Get fruit data
                        String fruitType = (String) fruitPointsCombo.getSelectedItem();
                        String selectedVine = (String) vineCountCombo2.getSelectedItem();
                        String positionY = (String) positionYCombo.getSelectedItem();
                        
                        if (positionY != null && !positionY.isEmpty()) {
                            // Map DefaultMap vine number to actual client ID
                            int clientVineId = fruitVineMapping.get(selectedVine);
                            int posY = Integer.parseInt(positionY);
                            
                            // Register fruit in server session (FASE 3)
                            int fruitId = session.spawnAdminFruit(clientVineId, posY, fruitType);
                            
                            // Add to UI list with fruit ID (show DefaultMap vine label)
                            String listEntry = fruitType + " (" + selectedVine + ", PosY: " + positionY + ", ID: " + fruitId + ")";
                            fruitListModel.addElement(listEntry);
                            
                            // FASE 3: Send SPAWN_FRUIT command with client vine ID
                            // Format: "SPAWN_FRUIT:vineId:positionY:type:fruitId"
                            String command = "SPAWN_FRUIT:" + clientVineId + ":" + posY + ":" + fruitType + ":" + fruitId;
                            server.sendMessageToClient(selectedPlayerId, command);
                            
                            System.out.println("✓ Sent to Player #" + selectedPlayerId + ": " + command);
                        }
                    }
                } catch (Exception ex) {
                    System.err.println("Error spawning fruit: " + ex.getMessage());
                    ex.printStackTrace();
                }
            } else {
                System.out.println("✗ No player selected");
            }
        });
        mainPanel.add(addFruitButton);

        JPanel fruitSeparator = new JPanel();
        fruitSeparator.setBackground(new Color(46,111,64));
        fruitSeparator.setBounds(0, 662, 430, 8);
        mainPanel.add(fruitSeparator);

        //_____________________________________________________________
        //PARA LAS PARTES DE ABAJO DE EL TERCER SEPARADOR HORIZONTAL
        //                       ELIMINAR ELEMENTOS
        //_____________________________________________________________

        JLabel removeLabel = new JLabel("Elimine Frutas");
        removeLabel.setFont(new Font("SansSerif", Font.BOLD, 18));
        removeLabel.setBounds(145, 682, 200, 25);
        mainPanel.add(removeLabel);

        // Lista de frutas dinámicas
        fruitListModel = new DefaultListModel<>();
        fruitList = new JList<>(fruitListModel);
        fruitList.setFont(new Font("Arial", Font.PLAIN, 14));
        fruitList.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
        fruitList.setBackground(new Color(240, 240, 240));
        fruitList.setBorder(new LineBorder(new Color(139, 69, 19), 3));

        JScrollPane fruitScrollPane = new JScrollPane(fruitList);
        fruitScrollPane.setBounds(20, 720, 290, 200);
        mainPanel.add(fruitScrollPane);

        JButton removeButton = new JButton("Eliminar");
        removeButton.setBorder(new LineBorder(new Color(139, 69, 19), 2));
        removeButton.setBackground(new Color(211, 182, 131));
        removeButton.setFont(new Font("Arial", Font.BOLD, 15));
        removeButton.setBounds(320, 790, 98, 40);
        removeButton.setOpaque(true);
        removeButton.setBorderPainted(true);
        removeButton.setContentAreaFilled(true);
        removeButton.setFocusPainted(false);
        
        // Add button press visual feedback with hover effect
        removeButton.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mouseEntered(java.awt.event.MouseEvent evt) {
                removeButton.setBackground(new Color(190, 161, 110)); // Hover shade
            }
            public void mouseExited(java.awt.event.MouseEvent evt) {
                removeButton.setBackground(new Color(211, 182, 131)); // Original color
            }
            public void mousePressed(java.awt.event.MouseEvent evt) {
                removeButton.setBackground(new Color(150, 121, 70)); // Pressed shade (darker)
            }
            public void mouseReleased(java.awt.event.MouseEvent evt) {
                removeButton.setBackground(new Color(190, 161, 110)); // Back to hover
            }
        });
        
        removeButton.addActionListener(e -> {
            int selectedIndex = fruitList.getSelectedIndex();
            if (selectedIndex != -1 && selectedPlayerId != -1) {
                try {
                    // Get the selected fruit text from UI
                    String fruitEntry = fruitListModel.get(selectedIndex);
                    
                    // Extract fruit ID from entry (format: "Type (Liana X, PosY: Y, ID: XXXX)")
                    int idIndex = fruitEntry.lastIndexOf("ID: ");
                    if (idIndex != -1) {
                        String idStr = fruitEntry.substring(idIndex + 4).replace(")", "").trim();
                        int fruitId = Integer.parseInt(idStr);
                        
                        // Get game session and remove fruit
                        server.src.gamestate.GameSession session = server.getGameSession(selectedPlayerId);
                        if (session != null && session.removeAdminFruit(fruitId)) {
                            // Remove from UI list
                            fruitListModel.remove(selectedIndex);
                            
                            // Send REMOVE_FRUIT command to client
                            String command = "REMOVE_FRUIT:" + fruitId;
                            server.sendMessageToClient(selectedPlayerId, command);
                            
                            System.out.println("✓ Sent to Player #" + selectedPlayerId + ": " + command);
                        } else {
                            System.err.println("✗ Failed to remove fruit ID " + fruitId + " from session");
                        }
                    } else {
                        System.err.println("✗ Could not extract fruit ID from: " + fruitEntry);
                    }
                } catch (Exception ex) {
                    System.err.println("Error removing fruit: " + ex.getMessage());
                    ex.printStackTrace();
                }
            }
        });
        mainPanel.add(removeButton);

        //__________________________________________________________
        //              PARA LAS PARTES DEL JUEGO 
        //           EN LA IZQUIERDA DE LA PANTALLA
        //__________________________________________________________

        String currentDir = System.getProperty("user.dir");
        String projectRoot = currentDir.endsWith("server") ? currentDir + "/.." : currentDir;
        
        
        // plataformas - Primera fila de plataformas
        for (int i = 0; i < 25; i++) {
            addPlatform(mainPanel, projectRoot, 440 + (i * 25), 180, 25, 25);
        }
        for (int i = 0; i < 13; i++) {
            addPlatform(mainPanel, projectRoot, 1065 + (i * 25), 200, 25, 25);
        }

        for (int i = 0; i < 13; i++) {
            addPlatform(mainPanel, projectRoot, 1240 + (i * 25), 470, 25, 25);
        }
        for (int i = 0; i < 12; i++) {
            addPlatform(mainPanel, projectRoot, 440 + (i * 25), 890, 25, 25);
        }
        //las del medio
        for (int i = 0; i < 5; i++) {
            addPlatform(mainPanel, projectRoot, 670 + (i * 25), 420, 25, 25);
        }
        for (int i = 0; i < 8; i++) {
            addPlatform(mainPanel, projectRoot, 670 + (i * 25), 595, 25, 25);
        }
        

        //bases
        addBase(mainPanel, projectRoot, 1445, 795, 110, 30);
        addStem(mainPanel, projectRoot, 1475, 820, 50, 95);

        addBase(mainPanel, projectRoot, 1270, 830, 105, 30);
        addStem(mainPanel, projectRoot, 1296, 835, 50, 80);

        addBase(mainPanel, projectRoot, 1092, 860, 85, 30);
        addStem(mainPanel, projectRoot, 1122, 885, 30, 30);

        addBase(mainPanel, projectRoot, 910, 820, 105, 30);
        addStem(mainPanel, projectRoot, 937, 835, 50, 80);


        //lianas despues de plataformas
        for (int i = 0; i < 8; i++) {
            int baseY = 205 + (i * 81);
            addVine1(mainPanel, projectRoot, 580, baseY, 30, 27);
            addVine2(mainPanel, projectRoot, 580, baseY + 27, 30, 27);
            addVine3(mainPanel, projectRoot, 580, baseY + 54, 30, 27);
            addVineBox(mainPanel, 580, baseY, 30, 27);
            addVineBox(mainPanel, 580, baseY + 27, 30, 27);
            addVineBox(mainPanel, 580, baseY + 54, 30, 27);
        }

        JLabel vineNumber1 = new JLabel("[1]");
        vineNumber1.setFont(new Font("Arial", Font.BOLD, 12));
        vineNumber1.setHorizontalAlignment(SwingConstants.CENTER);
        vineNumber1.setBounds(535, 854, 120, 20);
        mainPanel.add(vineNumber1);

        for (int i = 0; i < 7; i++) {
            int baseY = 205 + (i * 81);
            addVine1(mainPanel, projectRoot, 640, baseY, 30, 27);
            addVine2(mainPanel, projectRoot, 640, baseY + 27, 30, 27);
            addVine3(mainPanel, projectRoot, 640, baseY + 54, 30, 27);
            addVineBox(mainPanel, 640, baseY, 30, 27);
            addVineBox(mainPanel, 640, baseY + 27, 30, 27);
            addVineBox(mainPanel, 640, baseY + 54, 30, 27);
        }

        JLabel vineNumber2 = new JLabel("[2]");
        vineNumber2.setFont(new Font("Arial", Font.BOLD, 12));
        vineNumber2.setHorizontalAlignment(SwingConstants.CENTER);
        vineNumber2.setBounds(595, 780, 120, 20);
        mainPanel.add(vineNumber2);

        for (int i = 0; i < 6; i++) {
            int baseY = 205 + (i * 81);
            addVine1(mainPanel, projectRoot, 900, baseY, 30, 27);
            addVine2(mainPanel, projectRoot, 900, baseY + 27, 30, 27);
            addVine3(mainPanel, projectRoot, 900, baseY + 54, 30, 27);
            addVineBox(mainPanel, 900, baseY, 30, 27);
            addVineBox(mainPanel, 900, baseY + 27, 30, 27);
            addVineBox(mainPanel, 900, baseY + 54, 30, 27);
        }

        JLabel vineNumber3 = new JLabel("[4]");
        vineNumber3.setFont(new Font("Arial", Font.BOLD, 12));
        vineNumber3.setHorizontalAlignment(SwingConstants.CENTER);
        vineNumber3.setBounds(855, 700, 120, 20);
        mainPanel.add(vineNumber3);

        for (int i = 0; i < 3; i++) {
            int baseY = 205 + (i * 81);
            addVine1(mainPanel, projectRoot, 1010, baseY, 30, 27);
            addVine2(mainPanel, projectRoot, 1010, baseY + 27, 30, 27);
            addVine3(mainPanel, projectRoot, 1010, baseY + 54, 30, 27);
            addVineBox(mainPanel, 1010, baseY, 30, 27);
            addVineBox(mainPanel, 1010, baseY + 27, 30, 27);
            addVineBox(mainPanel, 1010, baseY + 54, 30, 27);
        }

        JLabel vineNumber4 = new JLabel("[5]");
        vineNumber4.setFont(new Font("Arial", Font.BOLD, 12));
        vineNumber4.setHorizontalAlignment(SwingConstants.CENTER);
        vineNumber4.setBounds(966, 454, 120, 20);
        mainPanel.add(vineNumber4);

        for (int i = 0; i < 3; i++) {
            int baseY = 225 + (i * 81);
            addVine1(mainPanel, projectRoot, 1090, baseY, 30, 27);
            addVine2(mainPanel, projectRoot, 1090, baseY + 27, 30, 27);
            addVine3(mainPanel, projectRoot, 1090, baseY + 54, 30, 27);
            addVineBox(mainPanel, 1090, baseY, 30, 27);
            addVineBox(mainPanel, 1090, baseY + 27, 30, 27);
            addVineBox(mainPanel, 1090, baseY + 54, 30, 27);
        }

        JLabel vineNumber5 = new JLabel("[6]");
        vineNumber5.setFont(new Font("Arial", Font.BOLD, 12));
        vineNumber5.setHorizontalAlignment(SwingConstants.CENTER);
        vineNumber5.setBounds(1045, 473, 120, 20);
        mainPanel.add(vineNumber5);

        for (int i = 0; i < 4; i++) {
            int baseY = 225 + (i * 81);
            addVine1(mainPanel, projectRoot, 1200, baseY, 30, 27);
            addVine2(mainPanel, projectRoot, 1200, baseY + 27, 30, 27);
            addVine3(mainPanel, projectRoot, 1200, baseY + 54, 30, 27);
            addVineBox(mainPanel, 1200, baseY, 30, 27);
            addVineBox(mainPanel, 1200, baseY + 27, 30, 27);
            addVineBox(mainPanel, 1200, baseY + 54, 30, 27);
        }
        JLabel vineNumber6 = new JLabel("[7]");
        vineNumber6.setFont(new Font("Arial", Font.BOLD, 12));
        vineNumber6.setHorizontalAlignment(SwingConstants.CENTER);
        vineNumber6.setBounds(1155, 554, 120, 20);
        mainPanel.add(vineNumber6);

        for (int i = 0; i < 5; i++) {
            int baseY = 445 + (i * 81);
            addVine1(mainPanel, projectRoot, 700, baseY, 30, 27);
            addVine2(mainPanel, projectRoot, 700, baseY + 27, 30, 27);
            addVine3(mainPanel, projectRoot, 700, baseY + 54, 30, 27);
            addVineBox(mainPanel, 700, baseY, 30, 27);
            addVineBox(mainPanel, 700, baseY + 27, 30, 27);
            addVineBox(mainPanel, 700, baseY + 54, 30, 27);
        }
        JLabel vineNumber7 = new JLabel("[3]");
        vineNumber7.setFont(new Font("Arial", Font.BOLD, 12));
        vineNumber7.setHorizontalAlignment(SwingConstants.CENTER);
        vineNumber7.setBounds(655, 855, 120, 20);
        mainPanel.add(vineNumber7);        

        for (int i = 0; i < 2; i++) {
            int baseY = 495 + (i * 81);
            addVine1(mainPanel, projectRoot, 1273, baseY, 30, 27);
            addVine2(mainPanel, projectRoot, 1273, baseY + 27, 30, 27);
            addVine3(mainPanel, projectRoot, 1273, baseY + 54, 30, 27);
            addVineBox(mainPanel, 1273, baseY, 30, 27);
            addVineBox(mainPanel, 1273, baseY + 27, 30, 27);
            addVineBox(mainPanel, 1273, baseY + 54, 30, 27);
        }
        
        JLabel vineNumber11 = new JLabel("[8]");
        vineNumber11.setFont(new Font("Arial", Font.BOLD, 12));
        vineNumber11.setHorizontalAlignment(SwingConstants.CENTER);
        vineNumber11.setBounds(1229, 660, 120, 20);
        mainPanel.add(vineNumber11);

        for (int i = 0; i < 6; i++) {
            int baseY = 225 + (i * 81);
            addVine1(mainPanel, projectRoot, 1360, baseY, 30, 27);
            addVine2(mainPanel, projectRoot, 1360, baseY + 27, 30, 27);
            addVine3(mainPanel, projectRoot, 1360, baseY + 54, 30, 27);
            addVineBox(mainPanel, 1360, baseY, 30, 27);
            addVineBox(mainPanel, 1360, baseY + 27, 30, 27);
            addVineBox(mainPanel, 1360, baseY + 54, 30, 27);
        }
        JLabel vineNumber10 = new JLabel("[9]");
        vineNumber10.setFont(new Font("Arial", Font.BOLD, 12));
        vineNumber10.setHorizontalAlignment(SwingConstants.CENTER);
        vineNumber10.setBounds(1315, 715, 120, 20);
        mainPanel.add(vineNumber10);


        for (int i = 0; i < 7; i++) {
            int baseY = 0 + (i * 81);
            addVine1(mainPanel, projectRoot, 1440, baseY, 30, 27);
            addVine2(mainPanel, projectRoot, 1440, baseY + 27, 30, 27);
            addVine3(mainPanel, projectRoot, 1440, baseY + 54, 30, 27);
            addVineBox(mainPanel, 1440, baseY, 30, 27);
            addVineBox(mainPanel, 1440, baseY + 27, 30, 27);
            addVineBox(mainPanel, 1440, baseY + 54, 30, 27);
        }
        JLabel vineNumber9 = new JLabel("[10]");
        vineNumber9.setFont(new Font("Arial", Font.BOLD, 12));
        vineNumber9.setHorizontalAlignment(SwingConstants.CENTER);
        vineNumber9.setBounds(1395, 570, 120, 20);
        mainPanel.add(vineNumber9);

        for (int i = 0; i < 8; i++) {
            int baseY = 0 + (i * 81);
            addVine1(mainPanel, projectRoot, 1500, baseY, 30, 27);
            addVine2(mainPanel, projectRoot, 1500, baseY + 27, 30, 27);
            addVine3(mainPanel, projectRoot, 1500, baseY + 54, 30, 27);
            addVineBox(mainPanel, 1500, baseY, 30, 27);
            addVineBox(mainPanel, 1500, baseY + 27, 30, 27);
            addVineBox(mainPanel, 1500, baseY + 54, 30, 27);
        }
        JLabel vineNumber8 = new JLabel("[11]");
        vineNumber8.setFont(new Font("Arial", Font.BOLD, 12));
        vineNumber8.setHorizontalAlignment(SwingConstants.CENTER);
        vineNumber8.setBounds(1455, 650, 120, 20);
        mainPanel.add(vineNumber8);


        //aditional 
        for (int i = 0; i < 46; i++) {
            addWater(mainPanel, projectRoot, 440 + (i * 25), 915, 25, 25);
        }
        addDonkeyJr(mainPanel, projectRoot, 430, 840, 90, 50);


        outerPanel.add(mainPanel, BorderLayout.CENTER);
        add(outerPanel);
        setVisible(true);
    }
    

    private void addVine1(JPanel panel, String projectRoot, int x, int y, int width, int height) {
        String vinePath1 = projectRoot + "/client/assets/environment/vines/vine1.png";
        ImageIcon vineIcon1 = new ImageIcon(vinePath1);
        Image vineImage = vineIcon1.getImage().getScaledInstance(width, height, Image.SCALE_SMOOTH);
        ImageIcon scaledIcon1 = new ImageIcon(vineImage);
        JLabel vineLabel1 = new JLabel(scaledIcon1);
        vineLabel1.setBounds(x, y, width, height);
        panel.add(vineLabel1);
    }

    private void addDonkeyJr(JPanel panel, String projectRoot, int x, int y, int width, int height) {
        String donkeyJrPath = projectRoot + "/client/assets/characters/junior/idle.png";
        ImageIcon donkeyJrIcon = new ImageIcon(donkeyJrPath);
        Image donkeyJrImage = donkeyJrIcon.getImage().getScaledInstance(width, height, Image.SCALE_SMOOTH);
        ImageIcon scaledIcon1 = new ImageIcon(donkeyJrImage);
        JLabel donkeyJrLabel = new JLabel(scaledIcon1);
        donkeyJrLabel.setBounds(x, y, width, height);
        panel.add(donkeyJrLabel);
    }

    private void addVine2(JPanel panel, String projectRoot, int x, int y, int width, int height) {
        String vinePath2 = projectRoot + "/client/assets/environment/vines/vine2.png";
        ImageIcon vineIcon2 = new ImageIcon(vinePath2);
        Image vineImage = vineIcon2.getImage().getScaledInstance(width, height, Image.SCALE_SMOOTH);
        ImageIcon scaledIcon2 = new ImageIcon(vineImage);
        JLabel vineLabel2 = new JLabel(scaledIcon2);
        vineLabel2.setBounds(x, y, width, height);
        panel.add(vineLabel2);
    }
    private void addVine3(JPanel panel, String projectRoot, int x, int y, int width, int height) {
        String vinePath3 = projectRoot + "/client/assets/environment/vines/vine3.png";
        ImageIcon vineIcon3 = new ImageIcon(vinePath3);
        Image vineImage = vineIcon3.getImage().getScaledInstance(width, height, Image.SCALE_SMOOTH);
        ImageIcon scaledIcon3 = new ImageIcon(vineImage);
        JLabel vineLabel3 = new JLabel(scaledIcon3);
        vineLabel3.setBounds(x, y, width, height);
        panel.add(vineLabel3);
    }


    private void addPlatform(JPanel panel, String projectRoot, int x, int y, int width, int height) {
        String platformPath = projectRoot + "/client/assets/environment/platforms/platform.png";
        ImageIcon platformIcon = new ImageIcon(platformPath);
        Image platformImage = platformIcon.getImage().getScaledInstance(width, height, Image.SCALE_SMOOTH);
        ImageIcon scaledIcon = new ImageIcon(platformImage);
        JLabel platformLabel = new JLabel(scaledIcon);
        platformLabel.setBounds(x, y, width, height);
        panel.add(platformLabel);
    }

    private void addBase(JPanel panel, String projectRoot, int x, int y, int width, int height) {
        String vinePath = projectRoot + "/client/assets/environment/grass/wide_grass.png";
        ImageIcon vineIcon = new ImageIcon(vinePath);
        Image vineImage = vineIcon.getImage().getScaledInstance(width, height, Image.SCALE_SMOOTH);
        ImageIcon scaledIcon = new ImageIcon(vineImage);
        JLabel vineLabel = new JLabel(scaledIcon);
        vineLabel.setBounds(x, y, width, height);
        panel.add(vineLabel);
    }

    private void addStem(JPanel panel, String projectRoot, int x, int y, int width, int height) {
        String platformPath = projectRoot + "/client/assets/environment/grass/wide_stem.png";
        ImageIcon platformIcon = new ImageIcon(platformPath);
        Image platformImage = platformIcon.getImage().getScaledInstance(width, height, Image.SCALE_SMOOTH);
        ImageIcon scaledIcon = new ImageIcon(platformImage);
        JLabel platformLabel = new JLabel(scaledIcon);
        platformLabel.setBounds(x, y, width, height);
        panel.add(platformLabel);
    }

    private void addWater(JPanel panel, String projectRoot, int x, int y, int width, int height) {
        String waterPath = projectRoot + "/client/assets/environment/water/water.png";
        ImageIcon waterIcon = new ImageIcon(waterPath);
        Image waterImage = waterIcon.getImage().getScaledInstance(width, height, Image.SCALE_SMOOTH);
        ImageIcon scaledWaterIcon = new ImageIcon(waterImage);
        JLabel waterLabel = new JLabel(scaledWaterIcon);
        waterLabel.setBounds(x, y, width, height);
        panel.add(waterLabel);
    }

    private void addVineBox(JPanel panel, int x, int y, int width, int height) {
        JPanel boxPanel = new JPanel() {
            @Override
            protected void paintComponent(Graphics g) {
                super.paintComponent(g);
                g.setColor(Color.WHITE);
                g.drawRect(0, 0, getWidth() - 1, getHeight() - 1);
            }
        };
        boxPanel.setOpaque(false);
        boxPanel.setBorder(new LineBorder(Color.LIGHT_GRAY, 1));
        boxPanel.setBounds(x, y, width, height);
        panel.add(boxPanel);
    }
    
    private void startPlayerUpdateThread() {
        updateThread = new Thread(() -> {
            while (true) {
                try {
                    updatePlayerList();
                    Thread.sleep(1000);
                } catch (InterruptedException e) {
                    break;
                }
            }
        });
        updateThread.setDaemon(true);
        updateThread.start();
    }
    
    private void updatePlayerList() {
        Map<Integer, NetworkPlayer> networkPlayers = server.getPlayers();
        
        SwingUtilities.invokeLater(() -> {
            playerListModel.clear();
            
            // Sync with our Player objects
            for (Map.Entry<Integer, NetworkPlayer> entry : networkPlayers.entrySet()) {
                Integer playerId = entry.getKey();
                NetworkPlayer networkPlayer = entry.getValue();
                
                // Create Player object if it doesn't exist
                players.computeIfAbsent(playerId, k -> new Player(playerId, networkPlayer.getAddress()));
                
                String playerInfo = "* " + "Jugador #" + playerId + " - " + networkPlayer.getAddress();
                playerListModel.addElement(playerInfo);
            }
        });
    }
    

    public Map<Integer, Player> getPlayersData() {
        return players;
    }
    
    /**
     * Remove fruit from UI list by ID (called when player collects fruit)
     * @param playerId Player whose fruit list to update
     * @param fruitId ID of fruit to remove
     */
    public void removeFruitFromUI(Integer playerId, int fruitId) {
        // Only update if this is the currently selected player
        if (playerId.equals(selectedPlayerId)) {
            SwingUtilities.invokeLater(() -> {
                // Find and remove fruit with matching ID from list
                for (int i = 0; i < fruitListModel.size(); i++) {
                    String entry = fruitListModel.get(i);
                    if (entry.contains("ID: " + fruitId + ")")) {
                        fruitListModel.remove(i);
                        System.out.println("[UI] Removed fruit ID=" + fruitId + " from admin list");
                        break;
                    }
                }
            });
        }
    }
}