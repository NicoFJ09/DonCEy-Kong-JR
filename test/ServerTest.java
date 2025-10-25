package test;
import java.io.*;
import java.net.*;

public class ServerTest {
    
    public static void main(String[] args){
        ServerSocket server = null;
        Socket client = null;
        BufferedReader input = null;
        PrintWriter output = null;

        try {
            // 1. Creamos servidor, port 12345
            server = new ServerSocket(12345);
            System.out.println("Servidor esperando conexiones en puerto 12345...");

            // 2. Esperar a que un cliente se conecte
            client = server.accept();
            System.out.println("Cliente Conectado");

            // 3. Crear herramientas para leer y escribir
            input = new BufferedReader(
                new InputStreamReader(client.getInputStream())
            );
            output = new PrintWriter(client.getOutputStream(), true);

            // 4. Loop simple: Leer el mensaje del cliente
            String msg;
            while ((msg = input.readLine()) != null) {
                System.out.println("Client: " + msg);

                // Responder a cliente
                output.println("Server received: " + msg);

                // Si el cliente escribe "bye", cerramos la conexión
                if (msg.trim().equalsIgnoreCase("bye")) {
                    System.out.println("Cerrando conexión por petición del cliente.");
                    break;
                }
            }
        } catch (IOException e) {
            System.out.println("Error: " + e.getMessage());
        } finally {
            // Cerramos recursos
            try { if (input != null) input.close(); } catch (IOException e) {}
            if (output != null) output.close();
            try { if (client != null) client.close(); } catch (IOException e) {}
            try { if (server != null) server.close(); } catch (IOException e) {}
        }
    }
}