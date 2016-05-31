package ch.santos_alves.nitroxanalyzer.commands;

import android.util.Log;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * The GET AVERAGE command. This command will make device to compute an average and return it
 *
 * Created by Sergio on 16.01.2016.
 */
public class GetAverageRequestCommand extends AbstractRequestCommand {
    private String responsePattern = "^RET AVERAGE (\\d*)$";
    private String command = "GET AVERAGE %1$d";
    private int iterations = 5;
    private long average;

    /**
     * The constructor
     * @param iterations the wished number of iterations
     */
    public GetAverageRequestCommand(int iterations) {
        this.setIterations(iterations);
    }

    /**
     * Gets command String
     *
     * @return A command string
     */
    @Override
    public String getCommand() {
        return String.format(command, getIterations());
    }

    /**
     * Parses received response
     * @param str
     */
    public void parseReceivedResponse(String str) {
        Pattern p = Pattern.compile(responsePattern);
        Matcher m = p.matcher(str);

        // TODO: 24.01.2016 What if We got an error???
        if(m.matches()) {
            Log.i("Nitrox Analyzer", "Matches found");
            average = Long.parseLong(m.group(1));
            Log.i("Nitrox Analyzer", "Group value : " + m.group(1));
            completed = true;
        }
    }

    /**
     * Gets the returned average
     *
     * @return the average
     */
    public long getAverage() {
        return average;
    }

    /**
     * gets the number of iterations
     *
     * @return iterations number
     */
    public int getIterations() {
        return iterations;
    }

    /**
     * The number of iterations wanted. Take care that each iterations is time consuming.
     * Actually 1 iteration == 150 ms => never set it higher than 6 iterations per second or
     * should change ADC precision from 23 to less to get higher conversion rate.
     *
     * @param iterations
     */
    public void setIterations(int iterations) {
        this.iterations = iterations;
    }
}
