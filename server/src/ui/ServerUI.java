package server.src.ui;

import server.src.network.GameServer;
import server.src.network.NetworkPlayer;

import javax.swing.*;
import javax.swing.border.LineBorder;

import java.awt.*;
import java.util.Map;

public class ServerUI extends JFrame {
    private GameServer server;
    private DefaultListModel<String> playerListModel;
    private JList<String> playerList;
    private Thread updateThread;
    private DefaultListModel<String> fruitListModel;
    private JList<String> fruitList;
    private String selectedPlayer = "Ninguno";
    
    public ServerUI(GameServer server) {
        this.server = server;
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
        playerSelectedLabel.setBounds(25, 160, 200, 25);
        mainPanel.add(playerSelectedLabel);

        playerListModel = new DefaultListModel<>();
        playerList = new JList<>(playerListModel);
        playerList.setFont(new Font("Arial", Font.PLAIN, 15));
        playerList.setBorder(new LineBorder(new Color(139, 69, 19), 3)); // Borde café de 3px
        playerList.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
        playerList.setBackground(new Color(240, 240, 240));
        
        // Listener para capturar selección de jugador
        playerList.addListSelectionListener(e -> {
            int selectedIndex = playerList.getSelectedIndex();
            if (selectedIndex != -1) {
                selectedPlayer = playerList.getSelectedValue();
                playerSelectedLabel.setText("Jugador Seleccionado: " + selectedPlayer);
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
        
        // Separador horizontal 1 que va desde el inicio hasta x=300
        JPanel separatorBottom = new JPanel();
        separatorBottom.setBackground(new Color(46,111,64));
        separatorBottom.setBounds(0, 200, 430, 8); // x=0, y=145, ancho=300, alto=8
        mainPanel.add(separatorBottom);
        
        // Separador vertical (usando JPanel) posicionado a la derecha
        JPanel separator = new JPanel();
        separator.setBackground(new Color(46,111,64));
        separator.setBounds(430, 0, 10, y); // Posicionar en coordenadas x=500, ancho=8
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

        JComboBox<String> vineCountCombo = new JComboBox<>(new String[]{"1", "2", "3", "4", "5"});
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
        mainPanel.add(addButton);

        JPanel cocodrileSeparator = new JPanel();
        cocodrileSeparator.setBackground(new Color(46,111,64));
        cocodrileSeparator.setBounds(0, 415, 430, 8); // x=0, y=310, ancho=400, alto=8
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
    
        JComboBox<String> vineCountCombo2 = new JComboBox<>(new String[]{"1", "2", "3", "4", "5"});
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
            String fruitType = (String) fruitPointsCombo.getSelectedItem();
            String vineCount = (String) vineCountCombo2.getSelectedItem();
            String positionY = positionYField.getText();
            
            if (!positionY.isEmpty()) {
                String fruitEntry = fruitType + " (Lianas: " + vineCount + ", PosY: " + positionY + ")";
                fruitListModel.addElement(fruitEntry);
                positionYField.setText("");
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
        fruitList.setBorder(new LineBorder(new Color(139, 69, 19), 3)); // Borde café de 3px
        
        JScrollPane fruitScrollPane = new JScrollPane(fruitList);
        fruitScrollPane.setBounds(20, 720, 280, 200);
        mainPanel.add(fruitScrollPane);

        JButton removeButton = new JButton("Eliminar");
        removeButton.setBorder(new LineBorder(new Color(139, 69, 19), 2)); // Borde café de 3px
        removeButton.setBackground(new Color(211, 182, 131));
        removeButton.setFont(new Font("Arial", Font.BOLD, 15));
        removeButton.setBounds(315, 790, 98, 40);
        removeButton.addActionListener(e -> {
            int selectedIndex = fruitList.getSelectedIndex();
            if (selectedIndex != -1) {
                fruitListModel.remove(selectedIndex);
            }
        });
        mainPanel.add(removeButton);

        outerPanel.add(mainPanel, BorderLayout.CENTER);
        add(outerPanel);
        setVisible(true);
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
        Map<Integer, NetworkPlayer> players = server.getPlayers();
        
        SwingUtilities.invokeLater(() -> {
            playerListModel.clear();
            
            for (Map.Entry<Integer, NetworkPlayer> entry : players.entrySet()) {
                Integer playerId = entry.getKey();
                NetworkPlayer player = entry.getValue();
                String playerInfo = "* " + "Jugador #" + playerId + " - " + player.getAddress();
                playerListModel.addElement(playerInfo);
            }
        });
    }
}