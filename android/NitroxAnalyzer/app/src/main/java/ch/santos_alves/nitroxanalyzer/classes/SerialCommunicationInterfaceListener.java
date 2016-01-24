package ch.santos_alves.nitroxanalyzer.classes;

import ch.santos_alves.nitroxanalyzer.commands.Command;

/**
 * Serial Communication Interface used between SerialCommandsConsumer and the BlueetoothSerialCommunicationHandler
 *
 * Created by Sergio on 23.01.2016.
 */
public interface SerialCommunicationInterfaceListener {
    /**
     * Called when command was sent
     * @param c the command sent
     */
    void commandSent(Command c);
    /**
     * Called when command response was received
     * @param c the command sent
     */
    void responseReceived(Command c);
    /**
     * Called when command was dequeue
     * @param c the command sent
     */
    void dequeueCommand(Command c);
}
