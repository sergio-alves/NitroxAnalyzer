package ch.santos_alves.nitroxanalyzer.classes;

import android.util.Log;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.util.concurrent.Callable;

import ch.santos_alves.nitroxanalyzer.commands.Command;

/**
 * A callable object that will be executed through an executor returning a Future object, the command
 * with its response, to the caller.
 *
 * Created by Sergio on 23.01.2016.
 */
public class CallableCommandImpl<V extends Command> implements Callable<V> {
    private OutputStream outputStream;
    private InputStream inputStream;
    private V command;

    /**
     * the constructor
     * @param is input stream
     * @param os output stream
     * @param c the command to send
     */
    public CallableCommandImpl(InputStream is, OutputStream os, V c) {
        inputStream = is;
        outputStream = os;
        command = c;
    }

    /**
     * Set the command to be executed, allows re-use of the callable
     * @param command the command to execute
     */
    public void setCommand(V command) {this.command = command;}

    /**
     * Call function implementation
     *
     * @return A future object, with the command filled with the answer
     */
    @Override
    public  V call() {
        String line;
        BufferedReader br = new BufferedReader(new InputStreamReader(inputStream));

        while (!command.isCompleted()) {
            try {

                //If any command to execute . do
                if (command != null && !command.isSent()) {
                    Log.d("Nitrox Analyzer", "Sending command > " + command.getCommand());
                    outputStream.write(command.getCommand().getBytes());
                    outputStream.write(10);
                    outputStream.flush();
                    command.setSend(true);
                }
                //If any answer comming process
                if (command != null && command.isSent() && (line = br.readLine()) != null) {
                    Log.d("Nitrox Analyzer", "Received response > " + line);
                    command.parseReceivedResponse(line);
                }

                Thread.sleep(10);
            } catch (IOException e) {
                e.printStackTrace();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

        return  command;
    }
}
