package com.maslyanko.MessageType;

public enum MessageType{
    LIVE(1),
    LEAVE(2);

    private byte type;
    MessageType(int type) {
        this.type = (byte) type;
    }

    public byte getValue() {
        return type;
    }

    public static MessageType identifyMessageType(byte value) {
        for (MessageType type : MessageType.values()) {
            if (type.getValue() == value) {
                return type;
            }
        }
        return null;
    }

}
