package ch.santos_alves.nitroxanalyzer.commands;

import android.util.Log;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * The command abstraction.
 *
 * Created by Sergio on 23.01.2016.
 */
public abstract class AbstractResponseCommand implements Command {
    protected String errorPattern = "^ERROR";
    protected boolean completed = false;
    //private boolean commandReceived = false;

    /**
     * Check that response is an error.
     * @Todo Should throw Exceptions
     * @param str The string to test
     * @return true if error false otherwise
     */
    public  boolean isResponseError(String str) {
        Pattern p = Pattern.compile(errorPattern);
        Matcher m = p.matcher(str);
        Log.v("Nitrox Analyzer", "Error match found ??? " + m.matches()+" on received text " + str);

        if (m.matches()) {
            //oups error
            Log.e("Nitrox Analyzer", "Found error");
            return true;
        }

        return false;
    }

    /**
     * check if command was sent
     * @return sent state
     */
    public Boolean isSent() {
        throw new RuntimeException("Unimplemented function");
    }

    /**
     * set send state
     * @param val the send state
     */
    public void setSend(boolean val) {
        throw new RuntimeException("Unimplemented function");
    }

    /**
     * check if command is completed => send request, received response, parsed response.
     * @return true if true, false otherwise
     */
    @Override
    public boolean isCompleted() {
        return completed;
    }

    @Override
    public String getCommand() {
        throw new RuntimeException("Unimplemented function");
    }
}
