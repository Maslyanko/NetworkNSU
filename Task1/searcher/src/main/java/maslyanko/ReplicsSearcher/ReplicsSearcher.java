package maslyanko.ReplicsSearcher;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.MulticastSocket;
import java.net.SocketException;
import java.util.HashMap;
import java.util.Map;

import maslyanko.MessageType.MessageType;

public class ReplicsSearcher {
    private String groupIP;
    private int port;
    private Map<String, Long> replics;
    private DatagramSocket sendSocket = null;
    private MulticastSocket receiveSocket = null;
    
    private boolean timeToStop = false;
    private Thread sender;
    private Thread printer;
    private Thread receiver;

    public ReplicsSearcher(String groupIP, int port) {
        this.groupIP = groupIP;
        this.port = port;
        replics = new HashMap<>();
    }

    public void searche() {
        if (timeToStop) {
            return;
        }

        timeToStop = false;

        try {
            sendSocket = new DatagramSocket();

            sender = new Thread(() -> {
                while (!timeToStop) {
                    sendMessage(MessageType.LIVE);
                    try {
                        Thread.sleep(1000);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            });

            printer = new Thread(() -> {
                while (!timeToStop) {
                    printReplics();
                    try {
                        Thread.sleep(5000);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            });

            receiver = new Thread(() -> {
                while (!timeToStop) {
                    recieveMulticast();
                }
            });

            sender.start();
            printer.start();
            receiver.start();
        } catch (SocketException e) {
            System.out.println("Failed to create DatagramSocket");
        }

        try {
            receiveSocket = new MulticastSocket(port);
            receiveSocket.joinGroup(InetAddress.getByName(groupIP));
        } catch (IOException e) {
            System.out.println("Failed to create MulticastSocket");
            return;
        }

        recieveMulticast();
    }

    public void shutDown() {
        if (!timeToStop) {
            return;
        }

        timeToStop = true;

        try {
            sender.join();
            printer.join();
            receiver.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
            return;
        }

        sendMessage(MessageType.LEAVE);

        sendSocket.close();
    }

    private void recieveMulticast() {
        byte[] buffer = new byte[1024];
        DatagramPacket packet = new DatagramPacket(buffer, buffer.length);

        try {
            receiveSocket.receive(packet);

            String senderIP = packet.getAddress().getHostAddress();
            long timestamp = System.currentTimeMillis();
            MessageType messge = MessageType.identifyMessageType(packet.getData()[0]);

            if (messge == MessageType.LIVE) {
                replics.put(senderIP, timestamp);
            } else if (messge == MessageType.LEAVE) {
                replics.remove(senderIP);
            } else {
                System.out.println("Unknown message type;");
                return;
            }

        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void sendMessage(MessageType messageType) {
        try {
            InetAddress group = InetAddress.getByName(groupIP);
            byte[] message = {messageType.getValue()};

            DatagramPacket packet = new DatagramPacket(message, message.length, group, port);
            sendSocket.send(packet);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void printReplics() {
        System.out.println("Replics:");
        for (Map.Entry<String, Long> entry : replics.entrySet()) {
            long timeSinceLastLive = System.currentTimeMillis() - entry.getValue();
            if (timeSinceLastLive < 10000) {
                System.out.println(entry.getKey());
            } else {
                replics.remove(entry);
            }
        }
    }
}
