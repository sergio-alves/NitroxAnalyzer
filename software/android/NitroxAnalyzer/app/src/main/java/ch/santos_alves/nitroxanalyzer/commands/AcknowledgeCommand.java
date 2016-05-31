package ch.santos_alves.nitroxanalyzer.commands;

import android.util.Log;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * Created by Sergio on 12.02.2016.
 */
public class AcknowledgeCommand {
    private static final String requestPattern = "^GET ACKNOWLEDGE$";
    private static final String response = "RET ACKNOWLEDGE";

    public static boolean parseReceivedRequest(String str) {
        Pattern p = Pattern.compile(requestPattern);
        Matcher m = p.matcher(str);
        return m.matches();
    }

    public static String getResponse() {
        return response;
    }
}
