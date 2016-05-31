package ch.santos_alves.nitroxanalyzer.commands;

/**
 * Command interface to be implemented by all commands
 *
 * Created by Sergio on 16.01.2016.
 */
public interface Command {
    Boolean isSent();

    void setSend(boolean val);
    String getCommand();

    void parseReceivedResponse(String str);

    boolean isCompleted();
}
