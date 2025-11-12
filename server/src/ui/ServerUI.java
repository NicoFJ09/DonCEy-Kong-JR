package server.src.ui;

import server.src.network.GameServer;
import server.src.network.NetworkPlayer;

import javax.swing.*;
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
        int x = 1200;
        int y = 900;
        setSize(x, y);
        setResizable(false);
        setLocationRelativeTo(null);
        
        JPanel mainPanel = new JPanel();
        mainPanel.setBackground(Color.WHITE);
        mainPanel.setLayout(null); // Usar null layout para posicionar con coordenadas
        
        JPanel leftPanel = new JPanel(new BorderLayout(5, 5));
        leftPanel.setBackground(Color.WHITE);
        leftPanel.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
        leftPanel.setBounds(0, 20, 395, 120); // Posicionar con coordenadas
        
        JLabel titleLabel = new JLabel("Jugadores Conectados");
        titleLabel.setFont(new Font("Arial", Font.BOLD, 18));
        titleLabel.setBounds(100, 0, 200, 25); // Posicionar en x=200
        mainPanel.add(titleLabel);

        JLabel playerSelectedLabel = new JLabel("Jugador Seleccionado:");
        playerSelectedLabel.setFont(new Font("Arial", Font.PLAIN, 16));
        playerSelectedLabel.setBounds(500, 30, 200, 25);
        mainPanel.add(playerSelectedLabel);

        playerListModel = new DefaultListModel<>();
        playerList = new JList<>(playerListModel);
        playerList.setFont(new Font("Arial", Font.PLAIN, 15));
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
        mainPanel.add(leftPanel);
        
        // Separador horizontal 1 que va desde el inicio hasta x=300
        JPanel separatorBottom = new JPanel();
        separatorBottom.setBackground(Color.ORANGE);
        separatorBottom.setBounds(0, 145, 400, 8); // x=0, y=145, ancho=300, alto=8
        mainPanel.add(separatorBottom);
        
        // Separador vertical (usando JPanel) posicionado a la derecha
        JPanel separator = new JPanel();
        separator.setBackground(Color.ORANGE);
        separator.setBounds(400, 0, 8, y); // Posicionar en coordenadas x=400, ancho=8
        mainPanel.add(separator);
        

        //_____________________________________________________________
        //PARA LAS PARTES DE ABAJO DE EL PRIMER SEPARADOR HORIZONTAL
        //                      COCODRILOS
        //_____________________________________________________________
        JLabel secondLabel = new JLabel("Agregue Cocodrilos");
        secondLabel.setFont(new Font("Arial", Font.BOLD, 18));
        secondLabel.setBounds(120, 160, 200, 25);
        mainPanel.add(secondLabel);
        
        //para los tipos de cocodrilos
        JLabel typeLabel = new JLabel("Tipo de Cocodrilo:");
        typeLabel.setFont(new Font("Arial", Font.PLAIN, 18));
        typeLabel.setBounds(20, 210, 200, 25);
        mainPanel.add(typeLabel);

        JComboBox<String> crocodileTypeCombo = new JComboBox<>(new String[]{"Cocodrilo Rojo", "Cocodrilo Azul"});
        crocodileTypeCombo.setFont(new Font("Arial", Font.PLAIN, 16));
        crocodileTypeCombo.setBounds(220, 210, 150, 30);
        mainPanel.add(crocodileTypeCombo);

        //para el numero de liana
        JLabel vineLabel = new JLabel("Numero de Lianas:");
        vineLabel.setFont(new Font("Arial", Font.PLAIN, 18));
        vineLabel.setBounds(20, 260, 200, 25);
        mainPanel.add(vineLabel);

        JComboBox<String> vineCountCombo = new JComboBox<>(new String[]{"1", "2", "3", "4", "5"});
        vineCountCombo.setFont(new Font("Arial", Font.PLAIN, 16));
        vineCountCombo.setBounds(220, 258, 150, 30);
        mainPanel.add(vineCountCombo);

        JButton addButton = new JButton("Agregar Cocodrilo");
        addButton.setFont(new Font("Arial", Font.BOLD, 15));
        addButton.setBackground(Color.LIGHT_GRAY);
        addButton.setBounds(120, 300, 180, 40);
        mainPanel.add(addButton);

        JPanel cocodrileSeparator = new JPanel();
        cocodrileSeparator.setBackground(Color.ORANGE);
        cocodrileSeparator.setBounds(0, 350, 400, 8); // x=0, y=310, ancho=400, alto=8
        mainPanel.add(cocodrileSeparator);


        //_____________________________________________________________
        //PARA LAS PARTES DE ABAJO DE EL SEGUNDO SEPARADOR HORIZONTAL
        //                      FRUTAS
        //_____________________________________________________________

        JLabel fruitsLabel = new JLabel("Agregue Frutas");
        fruitsLabel.setFont(new Font("Arial", Font.BOLD, 18));
        fruitsLabel.setBounds(120, 360, 200, 25);
        mainPanel.add(fruitsLabel);

        //para los tipos de frutas
        JLabel tipoFruta = new JLabel("Tipo de Fruta:");
        tipoFruta.setFont(new Font("Arial", Font.PLAIN, 18));
        tipoFruta.setBounds(20, 410, 200, 25);
        mainPanel.add(tipoFruta);

        JComboBox<String> fruitPointsCombo = new JComboBox<>(new String[]{"Mango", "Manzana", "Banana"});
        fruitPointsCombo.setFont(new Font("Arial", Font.PLAIN, 16));
        fruitPointsCombo.setBounds(220, 410, 150, 30);
        mainPanel.add(fruitPointsCombo);

        //para el numero de lianas
        JLabel vineLabel2 = new JLabel("Numero de Lianas:");
        vineLabel2.setFont(new Font("Arial", Font.PLAIN, 16));
        vineLabel2.setBounds(20, 460, 200, 25);
        mainPanel.add(vineLabel2);

        JComboBox<String> vineCountCombo2 = new JComboBox<>(new String[]{"1", "2", "3", "4", "5"});
        vineCountCombo2.setFont(new Font("Arial", Font.PLAIN, 16));
        vineCountCombo2.setBounds(220, 460, 150, 30);
        mainPanel.add(vineCountCombo2);

        JLabel vineLabel3 = new JLabel("Numero de Lianas:");
        vineLabel3.setFont(new Font("Arial", Font.PLAIN, 16));
        vineLabel3.setBounds(20, 460, 200, 25);
        mainPanel.add(vineLabel3);

        //para el la posicion en y de la fruta
        JLabel positionYLabel = new JLabel("Posicion Y de Fruta:");
        positionYLabel.setFont(new Font("Arial", Font.PLAIN, 16));
        positionYLabel.setBounds(20, 510, 200, 25);
        mainPanel.add(positionYLabel);

        JTextField positionYField = new JTextField();
        positionYField.setFont(new Font("Arial", Font.PLAIN, 16));
        positionYField.setBounds(220, 510, 150, 30);
        mainPanel.add(positionYField);

        JButton addFruitButton = new JButton("Agregar Fruta");
        addFruitButton.setFont(new Font("Arial", Font.BOLD, 15));
        addFruitButton.setBackground(Color.LIGHT_GRAY);
        addFruitButton.setBounds(120, 550, 180, 40);
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
        fruitSeparator.setBackground(Color.ORANGE);
        fruitSeparator.setBounds(0, 600, 400, 8);
        mainPanel.add(fruitSeparator);

        //_____________________________________________________________
        //PARA LAS PARTES DE ABAJO DE EL TERCER SEPARADOR HORIZONTAL
        //                       ELIMINAR ELEMENTOS
        //_____________________________________________________________

        JLabel removeLabel = new JLabel("Eliminar Elemento");
        removeLabel.setFont(new Font("Arial", Font.BOLD, 18));
        removeLabel.setBounds(120, 615, 200, 25);
        mainPanel.add(removeLabel);

        // Lista de frutas dinámicas
        fruitListModel = new DefaultListModel<>();
        fruitList = new JList<>(fruitListModel);
        fruitList.setFont(new Font("Arial", Font.PLAIN, 14));
        fruitList.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
        fruitList.setBackground(new Color(240, 240, 240));
        
        JScrollPane fruitScrollPane = new JScrollPane(fruitList);
        fruitScrollPane.setBounds(10, 650, 280, 200);
        mainPanel.add(fruitScrollPane);

        JButton removeButton = new JButton("Eliminar");
        removeButton.setFont(new Font("Arial", Font.BOLD, 15));
        removeButton.setBackground(Color.LIGHT_GRAY);
        removeButton.setBounds(298, 700, 98, 40);
        removeButton.addActionListener(e -> {
            int selectedIndex = fruitList.getSelectedIndex();
            if (selectedIndex != -1) {
                fruitListModel.remove(selectedIndex);
            }
        });
        mainPanel.add(removeButton);

        add(mainPanel);
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
