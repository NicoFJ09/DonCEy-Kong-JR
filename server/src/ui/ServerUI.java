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
    private Map<Integer, Player> players;  // Map of Player ID -> Player object
    
    public ServerUI(GameServer server) {
        this.server = server;
        this.players = new HashMap<>();
        initializeUI();
        startPlayerUpdateThread();
    }
    
    private void initializeUI() {
        setTitle("DonCEy Kong Jr - Server");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        int x = 1400;
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

        JComboBox<String> vineCountCombo = new JComboBox<>(new String[]{"1", "2", "3", "4", "5", "6"});
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
        addButton.addActionListener(e -> {
            if (selectedPlayerId != -1) {
                try {
                    // Get player object (already created in updatePlayerList)
                    Player player = players.get(selectedPlayerId);
                    if (player != null) {
                        // Get crocodile data
                        String crocodileType = (String) crocodileTypeCombo.getSelectedItem();
                        int vineCount = Integer.parseInt((String) vineCountCombo.getSelectedItem());
                        
                        // Add crocodile to player
                        player.addCrocodile(crocodileType, vineCount);
                                                
                        // Send message to the player
                        String message = crocodileType + ", " + vineCount;
                        server.sendMessageToClient(selectedPlayerId, "Agregar:" + message);
                        System.out.println("Message sent to Player #" + selectedPlayerId);
                    }
                } catch (Exception ex) {
                    System.err.println("Error: " + ex.getMessage());
                }
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

        JComboBox<String> fruitPointsCombo = new JComboBox<>(new String[]{"Mango", "Manzana", "Banana"});
        fruitPointsCombo.setBorder(new LineBorder(new Color(139, 69, 19), 2));
        fruitPointsCombo.setFont(new Font("Arial", Font.PLAIN, 16));
        fruitPointsCombo.setBackground(Color.WHITE);
        fruitPointsCombo.setBounds(230, 475, 150, 30);
        mainPanel.add(fruitPointsCombo);
    
        JComboBox<String> vineCountCombo2 = new JComboBox<>(new String[]{"1", "2", "3", "4", "5", "6"});
        vineCountCombo2.setBorder(new LineBorder(new Color(139, 69, 19), 2));
        vineCountCombo2.setFont(new Font("Arial", Font.PLAIN, 16));
        vineCountCombo2.setBackground(Color.WHITE);
        vineCountCombo2.setBounds(230, 520, 150, 30);
        mainPanel.add(vineCountCombo2);

        JLabel vineLabel3 = new JLabel("Numero de Lianas:");
        vineLabel3.setFont(new Font("Arial", Font.BOLD, 16));
        vineLabel3.setBounds(25, 520, 200, 25);
        mainPanel.add(vineLabel3);

        //para el la posicion en y de la fruta
        JLabel positionYLabel = new JLabel("Posicion Y de Fruta:");
        positionYLabel.setFont(new Font("SansSerif", Font.BOLD, 16));
        positionYLabel.setBounds(25, 565, 200, 25);
        mainPanel.add(positionYLabel);

        JTextField positionYField = new JTextField();
        positionYField.setBorder(new LineBorder(new Color(139, 69, 19), 2));
        positionYField.setFont(new Font("SansSerif", Font.PLAIN, 16));
        positionYField.setBounds(230, 565, 150, 30);
        mainPanel.add(positionYField);

        JButton addFruitButton = new JButton("Agregar Fruta");
        addFruitButton.setBorder(new LineBorder(new Color(139, 69, 19), 2));
        addFruitButton.setBackground(new Color(211, 182, 131));
        addFruitButton.setFont(new Font("Arial", Font.BOLD, 15));
        addFruitButton.setBounds(120, 610, 180, 40);

        addFruitButton.addActionListener(e -> {
            if (selectedPlayerId != -1) {
                try {
                    // Get player object (already created in updatePlayerList)
                    Player player = players.get(selectedPlayerId);
                    if (player != null) {
                        // Get fruit data
                        String fruitType = (String) fruitPointsCombo.getSelectedItem();
                        String vineCount = (String) vineCountCombo2.getSelectedItem();
                        String positionY = positionYField.getText();
                        
                        if (!positionY.isEmpty()) {
                            // Add fruit to player
                            player.addFruit(fruitType, vineCount, positionY);
                            fruitListModel.addElement(fruitType + " (Liana: " + vineCount + ", PosY: " + positionY + ")");
                            positionYField.setText("");
                            
                            // Send message to the player
                            String message = fruitType + ", " + vineCount + ", " + positionY;
                            server.sendMessageToClient(selectedPlayerId, "Agregar: " + message);
                            System.out.println("Message sent to Player #" + selectedPlayerId);
                        }
                    }
                } catch (Exception ex) {
                    System.err.println("Error: " + ex.getMessage());
                }
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
        fruitScrollPane.setBounds(20, 720, 280, 200);
        mainPanel.add(fruitScrollPane);

        JButton removeButton = new JButton("Eliminar");
        removeButton.setBorder(new LineBorder(new Color(139, 69, 19), 2));
        removeButton.setBackground(new Color(211, 182, 131));
        removeButton.setFont(new Font("Arial", Font.BOLD, 15));
        removeButton.setBounds(315, 790, 98, 40);
        removeButton.addActionListener(e -> {
            int selectedIndex = fruitList.getSelectedIndex();
            if (selectedIndex != -1 && selectedPlayerId != -1) {
                try {
                    // Get the selected fruit text from UI
                    String removedFruit = fruitListModel.remove(selectedIndex);
                    
                    // Remove from player object using the exact same text
                    Player player = players.get(selectedPlayerId);
                    if (player != null) {
                        player.removeFruit(removedFruit);
                        System.out.println("Fruit removed from Player #" + selectedPlayerId + ": " + removedFruit);
                    }
                    
                    // Send message to selected player
                    server.sendMessageToClient(selectedPlayerId, "Eliminar Fruta:" + removedFruit);
                    System.out.println("Message sent to Player #" + selectedPlayerId);
                } catch (Exception ex) {
                    System.err.println("Error removing fruit: " + ex.getMessage());
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
        
        // lianas
        addVine(mainPanel, projectRoot, 450, 200, 120, 200);
        addVine(mainPanel, projectRoot, 450, 400, 120, 200);
        addVine(mainPanel, projectRoot, 450, 600, 120, 200);

        JLabel vineNumber1 = new JLabel("[1]");
        vineNumber1.setFont(new Font("Arial", Font.BOLD, 12));
        vineNumber1.setHorizontalAlignment(SwingConstants.CENTER);
        vineNumber1.setBounds(450, 810, 120, 20);
        mainPanel.add(vineNumber1);

        addVine(mainPanel, projectRoot, 530, 200, 120, 100);
        addVine(mainPanel, projectRoot, 530, 300, 120, 200);
        addVine(mainPanel, projectRoot, 530, 500, 120, 250);

        JLabel vineNumber2 = new JLabel("[2]");
        vineNumber2.setFont(new Font("Arial", Font.BOLD, 12));
        vineNumber2.setHorizontalAlignment(SwingConstants.CENTER);
        vineNumber2.setBounds(530, 760, 120, 20);
        mainPanel.add(vineNumber2);

        
        // plataformas
        addPlatform(mainPanel, projectRoot, 440, 150, 200, 80);
        addPlatform(mainPanel, projectRoot, 640, 150, 200, 80);
        addPlatform(mainPanel, projectRoot, 790, 150, 200, 80);
        addPlatform(mainPanel, projectRoot, 930, 170, 230, 80);
        addPlatform(mainPanel, projectRoot, 1100, 600, 270, 80);

        //bases
        addBase(mainPanel, projectRoot, 440, 900, 230, 30);
        addBase(mainPanel, projectRoot, 1180, 810, 125, 30);
        addStem(mainPanel, projectRoot, 1216, 840, 55, 95);

        addBase(mainPanel, projectRoot, 1010, 850, 135, 30);
        addStem(mainPanel, projectRoot, 1050, 860, 55, 80);

        addBase(mainPanel, projectRoot, 890, 880, 100, 30);
        addStem(mainPanel, projectRoot, 925, 905, 30, 30);
        
        addBase(mainPanel, projectRoot, 730, 850, 135, 30);
        addStem(mainPanel, projectRoot, 770, 860, 55, 80);

        addPlatform(mainPanel, projectRoot, 615, 600, 150, 80);
        addPlatform(mainPanel, projectRoot, 625, 430, 110, 80);

        //lianas despues de plataformas
        addVine(mainPanel, projectRoot, 750, 200, 120, 200);
        addVine(mainPanel, projectRoot, 750, 400, 120, 200);
        addVine(mainPanel, projectRoot, 750, 600, 120, 100);

        JLabel vineNumber3 = new JLabel("[3]");
        vineNumber3.setFont(new Font("Arial", Font.BOLD, 12));
        vineNumber3.setHorizontalAlignment(SwingConstants.CENTER);
        vineNumber3.setBounds(750, 710, 120, 20);
        mainPanel.add(vineNumber3);

        addVine(mainPanel, projectRoot, 830, 200, 120, 100);
        addVine(mainPanel, projectRoot, 830, 300, 120, 150);
        addVine(mainPanel, projectRoot, 830, 450, 120, 150);

        JLabel vineNumber4 = new JLabel("[4]");
        vineNumber4.setFont(new Font("Arial", Font.BOLD, 12));
        vineNumber4.setHorizontalAlignment(SwingConstants.CENTER);
        vineNumber4.setBounds(830, 610, 120, 20);
        mainPanel.add(vineNumber4);

        addVine(mainPanel, projectRoot, 920, 200, 120, 180);
        addVine(mainPanel, projectRoot, 920, 380, 120, 180);
        addVine(mainPanel, projectRoot, 920, 560, 120, 220);

        JLabel vineNumber5 = new JLabel("[5]");
        vineNumber5.setFont(new Font("Arial", Font.BOLD, 12));
        vineNumber5.setHorizontalAlignment(SwingConstants.CENTER);
        vineNumber5.setBounds(920, 790, 120, 20);
        mainPanel.add(vineNumber5);

        addVine(mainPanel, projectRoot, 1020, 200, 120, 150);
        addVine(mainPanel, projectRoot, 1020, 350, 120, 150);
        addVine(mainPanel, projectRoot, 1020, 500, 120, 200);

        JLabel vineNumber6 = new JLabel("[6]");
        vineNumber6.setFont(new Font("Arial", Font.BOLD, 12));
        vineNumber6.setHorizontalAlignment(SwingConstants.CENTER);
        vineNumber6.setBounds(1020, 710, 120, 20);
        mainPanel.add(vineNumber6);


        outerPanel.add(mainPanel, BorderLayout.CENTER);
        add(outerPanel);
        setVisible(true);
    }
    

    private void addVine(JPanel panel, String projectRoot, int x, int y, int width, int height) {
        String vinePath = projectRoot + "/client/assets/environment/vines/vine.png";
        ImageIcon vineIcon = new ImageIcon(vinePath);
        Image vineImage = vineIcon.getImage().getScaledInstance(width, height, Image.SCALE_SMOOTH);
        ImageIcon scaledIcon = new ImageIcon(vineImage);
        JLabel vineLabel = new JLabel(scaledIcon);
        vineLabel.setBounds(x, y, width, height);
        panel.add(vineLabel);
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
}