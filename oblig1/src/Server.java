import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;

public class Server {

    static final int PORT = 1250;

    private ServerSocket serverSocket;
    private Socket connection;
    private PrintWriter writer;
    private BufferedReader reader;

    public void start(int port) throws IOException {
        serverSocket = new ServerSocket(port);
        connection = serverSocket.accept();
        writer = new PrintWriter(connection.getOutputStream(), true);
        reader = new BufferedReader(new InputStreamReader(connection.getInputStream()));

        String input = reader.readLine();
        while (!input.equals("q")){
            processEquation(input);
            input = reader.readLine();
        }
    }

    private void processEquation(String equation) {
        String[] components = equation.split(" ");

        int firstNumber = Integer.parseInt(components[0]);
        String operator = components[1];
        int secondNumber = Integer.parseInt(components[2]);

        Integer result = calculate(firstNumber, operator, secondNumber);
        String response = getResponse(equation, result);
        writer.println(response);
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
            return "Ugyldig operator, prøv igjen";

        return equation + " = " + result;
    }

    public void stopConnection() throws IOException {
        reader.close();
        writer.close();
        connection.close();
        serverSocket.close();
    }

    public static void main(String[] args) throws IOException {
        Server server = new Server();
        server.start(PORT);
        server.stopConnection();
    }

}
