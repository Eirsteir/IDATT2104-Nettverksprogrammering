import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;

public class ConcurrentServer {

    static final int PORT = 1250;

    private ServerSocket serverSocket;

    public void start(int port) {
        try {
            serverSocket = new ServerSocket(port);
            System.out.println("Awaiting incoming connections...");

            while (true) {
                new ClientHandler(serverSocket.accept()).start();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void stop() throws IOException {
        serverSocket.close();
    }

    private static class ClientHandler extends Thread {

        private Socket connection;
        private PrintWriter writer;
        private BufferedReader reader;

        public ClientHandler(Socket connection) {
            this.connection = connection;
        }

        public void run() {
            try {
                handle();
                shutdown();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        private void handle() throws IOException {
            writer = new PrintWriter(connection.getOutputStream(), true);
            reader = new BufferedReader(new InputStreamReader(connection.getInputStream()));

            String input = reader.readLine();
            while (!input.equals("q")){
                String response = processEquation(input);
                writer.println(response);
                input = reader.readLine();
            }
        }

        private String processEquation(String equation) {
            String[] components = equation.split(" ");

            int firstNumber = Integer.parseInt(components[0]);
            String operator = components[1];
            int secondNumber = Integer.parseInt(components[2]);

            Integer result = calculate(firstNumber, operator, secondNumber);
            return getResponse(equation, result);
        }

        private Integer calculate(int firstNumber, String operator, int secondNumber) {
            switch (operator) {
                case "+":
                    return firstNumber + secondNumber;
                case "-":
                    return firstNumber - secondNumber;
                case "*":
                    return firstNumber * secondNumber;
                case "/":
                    return firstNumber / secondNumber;
                default:
            }
            return null;
        }

        private String getResponse(String equation, Integer result) {
            if (result == null)
                return "Invalid operand, try again";

            return equation + " = " + result;
        }

        public void shutdown() throws IOException {
            reader.close();
            writer.close();
            connection.close();
        }
    }

    public static void main(String[] args) throws IOException {
        ConcurrentServer server = new ConcurrentServer();
        server.start(PORT);
        server.stop();
    }
}