package ch.santos_alves.nitroxanalyzer.classes;

import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.List;
import java.util.Queue;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.ScheduledExecutorService;

import ch.santos_alves.nitroxanalyzer.commands.Command;

/**
 * The commands consumer class. Has a queue for commands storage. Since device does not handle multiples
 * request responses, we can only send a request, wait for a response before executing next command.
 *
 * Created by Sergio on 10.12.2015.
 */
public class SerialCommandsConsumer extends Thread {
    private InputStream inputStream;
    private OutputStream outputStream;
    private Queue<Command> queue = new ArrayDeque<>();
    private boolean threadRunning = true;
    private Command command;

    //An executor object that will handle the command execution
    ScheduledExecutorService executor =  Executors.newSingleThreadScheduledExecutor();

    //Listener to communicate with calling class about execution progress
    private List<SerialCommunicationInterfaceListener> listeners;

    /**
     * add listener
     * @param listener The listener to add
     */
    public void addSerialCommunicationInterfaceListener(SerialCommunicationInterfaceListener listener) {
        listeners.add(listener);
    }

    /**
     * Constructs a serial commands consumer using previously openned in out streams
     * @param is input stream
     * @param os output stream
     */
    public SerialCommandsConsumer(InputStream is, OutputStream os) {
        this.inputStream = is;
        this.outputStream = os;
        listeners = new ArrayList<>();
    }

    /**
     * Enqueue a new command.
     *
     * @param command The command to enqueue
     */
    public void addNewCommand(Command command) {
        queue.add(command);
    }

    /**
     * Extract next command, submit and wait for result and redo the same ad vitam æternam or till
     * threadRunning == false
     */
    @Override
    public void run() {
        CallableCommandImpl<Command> cci = new CallableCommandImpl<>(inputStream, outputStream, command);
        Future<Command> response;

        while (threadRunning) {
            try {
                if(command != null) {
                    cci.setCommand(command);
                    response = executor.submit(cci);

                    //notify listeners
                    for (SerialCommunicationInterfaceListener listener : listeners) {
                        listener.commandSent(command);
                    }

                    //block until response reception
                    if(response.get() == null) {
                        for (SerialCommunicationInterfaceListener listener : listeners) {
                            listener.commandFailed(command);
                        }
                        //// TODO: 24.01.2016 But what????
                    } else {
                        command = response.get();
                        //notify listeners
                        for (SerialCommunicationInterfaceListener listener : listeners) {
                            listener.responseReceived(command);
                        }
                    }
                    
                    command = null;
                }

                sleep(50);

                //If no command being executed or waiting for response peak new command if any
                if (command == null && !queue.isEmpty()) {
                    command = queue.remove();
                    //notify listeners
                    for (SerialCommunicationInterfaceListener listener : listeners) {
                        listener.dequeueCommand(command);
                    }
                }
            } catch (InterruptedException | ExecutionException ex) {
                ex.printStackTrace();
            }
        }

        //clear remaining commands
        queue.clear();
    }

    /**
     * Sets this execution loop state. If set to false exits run function and stops thread
     *
     * @param threadRunning The wanted thread running state
     */
    public void setThreadRunning(boolean threadRunning) {
        this.threadRunning = threadRunning;
    }
}
