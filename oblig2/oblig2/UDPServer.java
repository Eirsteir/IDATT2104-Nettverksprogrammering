import javax.xml.crypto.Data;
import java.io.BufferedReader;
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;

public class UDPServer {

    public static void main(String[] args) throws IOException {
        //new UDPServerThread().start();
        UDPServerThread udpServerThread = new UDPServerThread();
        udpServerThread.start();
    }

}
