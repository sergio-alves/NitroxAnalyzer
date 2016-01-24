package ch.santos_alves.nitroxanalyzer.activities;

import android.content.Intent;
import android.content.SharedPreferences;
import android.graphics.drawable.AnimationDrawable;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.preference.PreferenceManager;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.Toast;

import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Timer;
import java.util.TimerTask;

import ch.santos_alves.nitroxanalyzer.R;
import ch.santos_alves.nitroxanalyzer.classes.BluetoothSerialCommunicationHandler;
import ch.santos_alves.nitroxanalyzer.commands.GetAverageCommand;
import ch.santos_alves.nitroxanalyzer.commands.GetO2CellIntallDateCommand;
import ch.santos_alves.nitroxanalyzer.commands.GetO2CellValidityDateCommand;
import ch.santos_alves.nitroxanalyzer.classes.NitroxAnalyzerConfiguration;
import ch.santos_alves.nitroxanalyzer.classes.States;
import ch.santos_alves.nitroxanalyzer.views.OxyGenPercentDisplayer;

public class OxygenDisplay extends AppCompatActivity {
    public static final int CALIBRATION_ITERATIONS = 100;
    public static final int AVERAGE_ITERATIONS = 5;
    private AnimationDrawable myAnimationDrawable;
    private SharedPreferences prefs;
    private BluetoothSerialCommunicationHandler btHandler = BluetoothSerialCommunicationHandler.getInstance();
    private long calibre = 0;
    private NitroxAnalyzerConfiguration configuration = new NitroxAnalyzerConfiguration();

    @Override
    protected void onDestroy() {
        super.onDestroy();
        btHandler.disconnect();
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (prefs.getBoolean("automatic_calibration_at_connection", false)) {
            findViewById(R.id.btnCalibrate).setVisibility(Button.INVISIBLE);
        } else {
            findViewById(R.id.btnCalibrate).setVisibility(Button.VISIBLE);
        }

        if (prefs.getBoolean("continuous_o2_measurement", false)) {
            findViewById(R.id.btnGetO2).setVisibility(Button.INVISIBLE);
            /*if (calibrated)
                readingO2 = true;*/
        } else {
            findViewById(R.id.btnGetO2).setVisibility(Button.VISIBLE);
            /*if (readingO2)
                readingO2 = false;*/
        }
    }

    /**
     * called when on activity creation
     * @param savedInstanceState instance state
     */
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_oxygen_display);
        prefs = PreferenceManager.getDefaultSharedPreferences(this);

        //Handles all events coming from BluetoothSerialCommunicationHandler ... atchumm ... need to find smaller name :
        btHandler.setHandler(new Handler(new Handler.Callback() {
            private void startingToConnect() {
                startBtButtonAnimation();
            }

            private void connected() {
                myAnimationDrawable.stop();
                ((ImageButton) findViewById(R.id.imageButton)).setImageDrawable(getDrawable(R.drawable.bt_anim_006));
                findViewById(R.id.btnCalibrate).setEnabled(true);
            }

            private void disconnected() {
                myAnimationDrawable.stop();
                ((ImageButton) findViewById(R.id.imageButton)).setImageDrawable(getDrawable(R.drawable.bt_anim_001));
            }

            private void timeout() {
                myAnimationDrawable.stop();
                ((ImageButton) findViewById(R.id.imageButton)).setImageDrawable(getDrawable(R.drawable.bt_anim_001));
            }

            private void commandHandlerReady() {
                //after successful connection and only if
                onCalibrate(findViewById(R.id.btnCalibrate));

                //get device stored configuration first
                btHandler.addNewCommand(new GetO2CellIntallDateCommand());
                btHandler.addNewCommand(new GetO2CellValidityDateCommand());

                //and finally ask for calibration
                btHandler.addNewCommand(new GetAverageCommand(CALIBRATION_ITERATIONS));
            }
            
            @Override
            public boolean handleMessage(Message msg) {
                switch (States.getState(msg.what)) {
                    case DISCONNECTED:
                        disconnected();
                        break;
                    case TIMEOUT:
                        timeout();
                        break;
                    case CONNECTED:
                        connected();
                        break;
                    case CONNECTING:
                        startingToConnect();
                        break;
                    case RESPONSE_RECEIVED:
                        onResponseReceived(msg);
                        break;
                    case COMMAND_HANDLER_READY:
                        commandHandlerReady();
                        break;
                }
                return true;
            }
        }));


        if (prefs.getBoolean("bluetooth_automatic_connection", true)) {
            // start automatic connection
            onConnect(findViewById(R.id.imageButton));
        }

        if (prefs.getBoolean("automatic_calibration_at_connection", true)) {
            // start automatic calibration and hide button
            findViewById(R.id.btnCalibrate).setVisibility(View.INVISIBLE);
        }

        if (prefs.getBoolean("continuous_o2_measurement", true)) {
            // start automatic calibration and hide button
            findViewById(R.id.btnGetO2).setVisibility(View.INVISIBLE);
        }
    }

    /**
     * Handle all messages types received from BluetoothSerialCommunicationHandler
     *
     * @param msg The message received
     */
    private void onResponseReceived(Message msg) {
        if (msg.obj instanceof GetAverageCommand) {
            GetAverageCommand gac = (GetAverageCommand) msg.obj;
            if (gac.getIterations() == CALIBRATION_ITERATIONS) {
                //Calibration
                calibre = gac.getAverage();
                Log.d("Nitrox Analyzer", "Calibration terminated. Caliber : " + calibre);
                findViewById(R.id.btnGetO2).setEnabled(true);

                Toast.makeText(getApplicationContext(), "Calibration terminated", Toast.LENGTH_SHORT).show();

                //After calibration check if should measure o2 continuously
                if (prefs.getBoolean("continuous_o2_measurement", true)) {
                    //Start a measurement each 1 second
                    new Timer().schedule(new TimerTask() {
                        @Override
                        public void run() {
                            btHandler.addNewCommand(new GetAverageCommand(AVERAGE_ITERATIONS));
                        }
                    }, 50, 1000);
                }
            } else {
                //Simple continuous iteration
                Log.d("Nitrox Analyzer", "Data treceived in BTDeviceActivity + obj ? " + msg.obj);
                OxyGenPercentDisplayer og = (OxyGenPercentDisplayer) findViewById(R.id.nitroxTank);
                og.setOxyrate((int) Math.round(gac.getAverage() * 20.95 / calibre));
            }
        }else if (msg.obj instanceof GetO2CellIntallDateCommand) {
            configuration.getO2Cell().setInstallDate(((GetO2CellIntallDateCommand) msg.obj).getDate());
        }else if (msg.obj instanceof GetO2CellValidityDateCommand) {
            configuration.getO2Cell().setValidityDate(((GetO2CellValidityDateCommand) msg.obj).getDate());
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        MenuInflater inflater = getMenuInflater();
        inflater.inflate(R.menu.menu_oxygen_display, menu);
        return true;
    }

    /**
     * called on calibrate button click or called by any sub function
     *
     * @param view the view
     */
    public void onCalibrate(View view) {
        Toast.makeText(this, "Calibration will take about 10 seconds. " +
                "Please do not approach the device to a O2 source.", Toast.LENGTH_LONG).show();
    }

    /**
     * called when click on bt connect button
     *
     * @param view the view
     */
    public void onConnect(View view) {
        btHandler.connect(getApplicationContext());
    }

    /**
     * Starts the animation for the bt connection button
     */
    private void startBtButtonAnimation() {
        //Bluetooth activated -> start connection animation
        ((ImageButton) findViewById(R.id.imageButton)).setImageResource(R.drawable.bt);
        ImageButton myAnimation = (ImageButton) findViewById(R.id.imageButton);
        myAnimationDrawable = (AnimationDrawable) myAnimation.getDrawable();

        myAnimation.post(
                new Runnable() {
                    @Override
                    public void run() {
                        myAnimationDrawable.start();
                    }
                }
        );
    }

    /**
     * on menu click
     * @param item the menu item
     * @return true if handled
     */
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        final Intent intent;
        switch (item.getItemId()) {
            case R.id.action_settings:
                intent = new Intent(getApplicationContext(), SettingsActivity.class);
                intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_REORDER_TO_FRONT);
                startActivity(intent);
                return true;
            case R.id.O2_cell_settings:
                intent = new Intent(getApplicationContext(), O2CellActivity.class);
                intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_REORDER_TO_FRONT);
                DateFormat df = new SimpleDateFormat("dd-MM-yyyy");
                intent.putExtra("installDate", df.format(configuration.getO2Cell().getInstallDate()));
                intent.putExtra("validityDate", df.format(configuration.getO2Cell().getValidityDate()));
                startActivity(intent);
                return true;
            default:
                return super.onOptionsItemSelected(item);
        }
    }
}
