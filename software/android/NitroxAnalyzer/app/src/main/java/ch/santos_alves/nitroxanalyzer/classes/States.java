package ch.santos_alves.nitroxanalyzer.classes;

/**
 * The different states to be handled by Activities
 *
 * Created by Sergio on 23.01.2016.
 */
public enum States {
    CONNECTING(0),
    CONNECTED(1),
    DISCONNECTED(2),
    TIMEOUT(3),
    RESPONSE_RECEIVED(4),
    COMMAND_HANDLER_READY(5);
    private int value;

    States(int val) {
        value = val;
    }

    public int getValue() {
        return value;
    }

    public static States getState(int value) {
        switch (value) {
            case 0:
                return States.CONNECTING;
            case 1:
                return States.CONNECTED;
            case 2:
                return States.DISCONNECTED;
            case 3:
                return States.TIMEOUT;
            case 4:
                return States.RESPONSE_RECEIVED;
            case 5:
                return States.COMMAND_HANDLER_READY;
        }

        return null;
    }
}
