package com.maslyanko.ReplicsSearcher;

import java.net.*;
import java.io.IOException;
import java.util.Collections;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.*;

import com.maslyanko.MessageType.MessageType;

public class ReplicsSearcher {
    private String groupIP;
    private int port;
    private DatagramSocket sendSocket;
    private MulticastSocket recieveSocket;
    private NetworkInterface netInterface;

    private Map<InetAddress, Long> replics = Collections.synchronizedMap(new HashMap<InetAddress, Long>());
    private ScheduledExecutorService scheduler;

    public class SearcherException extends Exception {
        public SearcherException(String message) {
            super(message);
        }
    }

    public void searche() throws SearcherException {
        searche("eth0", "224.0.0.1", 12345);
    }

    public void searche(String IP, int port) throws SearcherException{
        searche("eth0", IP, port);
    }

    public void searche(String networkInterfaceName, String IP, int port) throws SearcherException {
        if (scheduler != null && !scheduler.isShutdown()) {
            return;
        }

        this.groupIP = IP;
        this.port = port;

        try {
            sendSocket = new DatagramSocket();
        } catch (SocketException e) {
            throw new SearcherException("Failed to create sending Datagramsocket");
        }

        try {
            recieveSocket = new MulticastSocket(port);
            netInterface = NetworkInterface.getByName(networkInterfaceName);
            
            if (netInterface == null) {
                throw new SearcherException("Network interface not found");
            }

            recieveSocket.setNetworkInterface(netInterface);
            recieveSocket.joinGroup(new InetSocketAddress(groupIP, port), netInterface);
        } catch (IOException e) {
            throw new SearcherException("Failed to create receiving MulticastSocket");
        }

        scheduler = Executors.newScheduledThreadPool(3);

        scheduler.scheduleAtFixedRate(this::sendLiveMessage, 0, 5, TimeUnit.SECONDS);
        scheduler.scheduleAtFixedRate(this::recieveMulticast, 0, 1, TimeUnit.SECONDS);
        scheduler.scheduleAtFixedRate(this::printReplics, 2, 2, TimeUnit.SECONDS);
    }

    public void shutDown() {
        if (scheduler != null && scheduler.isShutdown()) {
            return;
        }

        scheduler.shutdown();

        sendMessage(MessageType.LEAVE);

        try {
            recieveSocket.leaveGroup(new InetSocketAddress(groupIP, port), netInterface);
        } catch (IOException e) {
            return;
        }
        sendSocket.close();
        recieveSocket.close();
    }

    private void recieveMulticast() {
        byte[] buffer = new byte[1024];
        DatagramPacket packet = new DatagramPacket(buffer, buffer.length);

        try {
            recieveSocket.receive(packet);
            long timestamp = System.currentTimeMillis();

            InetAddress senderIP = packet.getAddress();
            MessageType messge = MessageType.identifyMessageType(packet.getData()[0]);

            if (messge == MessageType.LIVE) {
                synchronized (replics) {
                    replics.put(senderIP, timestamp);
                }
            } else if (messge == MessageType.LEAVE) {
                synchronized (replics) {
                    replics.remove(senderIP);
                }
            } else {
                System.out.println("Unknown message type;");
                return;
            }

        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void sendLiveMessage() {
        sendMessage(MessageType.LIVE);
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
        synchronized (replics) {
            if (replics.isEmpty()) {
                return;
            }
    
            System.out.println("Replics:");
    
            for (Map.Entry<InetAddress, Long> entry : replics.entrySet()) {
                long timeSinceLastLive = System.currentTimeMillis() - entry.getValue();
                if (timeSinceLastLive < 7000) {
                    System.out.println(entry.getKey().getHostAddress());
                } else {
                    replics.remove(entry);
                }
            }
        }
    }
}
