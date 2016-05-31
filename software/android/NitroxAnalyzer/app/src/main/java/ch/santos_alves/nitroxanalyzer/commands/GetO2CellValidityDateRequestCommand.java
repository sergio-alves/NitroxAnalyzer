package ch.santos_alves.nitroxanalyzer.commands;

import android.util.Log;

import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * The GET O2 CELL VALIDITY DATE command. This command will make device to return the validity o2
 * cell installation date
 *
 * Created by Sergio on 16.01.2016.
 */
public class GetO2CellValidityDateRequestCommand extends AbstractRequestCommand {
    private String responsePattern = "^RET O2 CELL VALIDITY DATE (\\d+-\\d+-\\d{4})$";
    private String command = "GET O2 CELL VALIDITY DATE";
    protected Date date;
    protected String formattedDate;

    @Override
    public String getCommand() {
        return command;
    }

    /**
     * Get Date
     * @return the date
     */
    public Date getDate() {
        return date;
    }

    /**
     * Get formatted date
     * @return a dd-MM-yyyy date
     */
    public String getFormattedDate() {
        return formattedDate;
    }

    /**
     * Sets the formatted date
     * @param formattedDate string formatted date
     */
    public void setFormattedDate(String formattedDate) {this.formattedDate = formattedDate;}

    public void parseReceivedResponse(String str) {
        Pattern p = null;
        Matcher m = null;
        DateFormat df = new SimpleDateFormat("dd-MM-yyyy");

        p = Pattern.compile(responsePattern);
        m = p.matcher(str);
        if (m.matches()) {
            setFormattedDate(m.group(1));
            Log.i("OxygenDisplay", "parsed date : " + getFormattedDate());
        } else {
            setFormattedDate("01-01-1970");
            Log.w("OxygenDisplay", "Set default date : " + getFormattedDate());
        }
        
        completed = true;

        try {
            date = df.parse(getFormattedDate());
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
