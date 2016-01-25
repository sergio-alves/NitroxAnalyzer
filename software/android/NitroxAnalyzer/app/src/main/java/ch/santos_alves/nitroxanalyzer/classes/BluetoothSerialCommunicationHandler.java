package ch.santos_alves.nitroxanalyzer.classes;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Handler;
import android.preference.PreferenceManager;
import android.util.Log;
import android.widget.Toast;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.UUID;

import ch.santos_alves.nitroxanalyzer.commands.Command;

/**
 * The class that allows a communication with bluetooth device
 *
 * Created by Sergio on 23.01.2016.
 */
public class BluetoothSerialCommunicationHandler implements SerialCommunicationInterfaceListener {
    private static BluetoothSerialCommunicationHandler handler;
    private UUID uuid = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");
    private BluetoothAdapter BA;
    private BluetoothSocket bts;
    private boolean connected = false;
    private InputStream inputStream;
    private OutputStream outputStream;
    private SerialCommandsConsumer dt;
    private SharedPreferences prefs;
    private Handler mHandler;

    /**
     * Adds an handler to allow communication with calling activity
     *
     * @param mHandler the handler
     */
    public void setHandler(Handler mHandler) {
        this.mHandler = mHandler;
    }

    /**
     * The default blueetooth serial communication handler constructor
     */
    protected BluetoothSerialCommunicationHandler() {
        BA = BluetoothAdapter.getDefaultAdapter();
    }

    /**
     * checks if adapter is on
     *
     * @return true if enabled false otherwise
     */
    public boolean isBluetoothAdapterEnabled() {
        return BA.isEnabled();
    }


    /**
     * Enqueue a new command.
     *
     * @param command the command to add
     */
    public void addNewCommand(Command command) {
        dt.addNewCommand(command);
    }

    /**
     * Return connection state
     * @return true if connected false otherwise
     */
    public boolean isConnected() {
        return connected;
    }

    /**
     * Start connection with given bluetooth device
     *
     * @param context the calling activity context
     */
    public void connect(Context context) {
        if (isConnected()) {
            Log.v("Nitrox Analyzer", "Connection already established with device");
            return;
        }
        //Load app preferences
        prefs = PreferenceManager.getDefaultSharedPreferences(context);

        //Quit if no bluetooth inferface selected in preferences
        if (prefs.getString("bluetooth_interface", "").isEmpty()) {
            String str = "No bluetooth interface selected. Please select one in preferences and try to connect again.";

            Log.v("Nitrox Analyzer", str);
            Toast.makeText(context, str, Toast.LENGTH_SHORT).show();
            return;
        }

        //test and enable bluetooth adapter
        if (!isBluetoothAdapterEnabled()) {
            Log.v("Nitrox Analyzer", "bt adapter not enable. enabling it");

            Intent turnOn = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            ((Activity) context).startActivityForResult(turnOn, 0);

            if (!isBluetoothAdapterEnabled()) {
                Log.v("Nitrox Analyzer", "User does not accepted to enable bluetooth adapter. Quitting execution.");
                Toast.makeText(context, "Application needs Bluetooth module to be turned on.", Toast.LENGTH_LONG).show();
                return;
            } else {
                Log.v("Nitrox Analyzer", "User accepted to enable bluetooth adapter");
                Toast.makeText(context, "Bluetooth adapter turned on.", Toast.LENGTH_LONG).show();
            }
        }

        //tells that connection started
        mHandler.sendEmptyMessage(States.CONNECTING.getValue());

        Thread connectionThread = new Thread(new Runnable() {
            @Override
            public void run() {
                startBluetoothConnection();
            }
        });

        connectionThread.start();
    }

    /**
     * Connects to given bluetooth device store in preferences and starts and instance of Serial Command Consumer
     */
    private void startBluetoothConnection() {
        try {
            BluetoothDevice btd = BA.getRemoteDevice(prefs.getString("bluetooth_interface", ""));
            bts = btd.createRfcommSocketToServiceRecord(uuid);
            Log.v("Nitrox Analyzer", "Bts created and is " + (bts == null ? " NULL" : " NOT NULL"));

            //Stops discovery cancel to avoid interruptions during connection
            BluetoothAdapter.getDefaultAdapter().cancelDiscovery();

            bts.connect();
            connected = true;

            //tells that connection is established
            mHandler.sendEmptyMessage(States.CONNECTED.getValue());

            if (bts.isConnected()) {
                Log.v("Nitrox Analyzer", "Socket created and connected");
            } else {
                Log.v("Nitrox Analyzer", "Socket created but connection failed");
            }

            inputStream = bts.getInputStream();
            outputStream = bts.getOutputStream();

            dt = new SerialCommandsConsumer(inputStream, outputStream);
            dt.addSerialCommunicationInterfaceListener(this);
            dt.start();

            //tells that connection is established
            mHandler.sendEmptyMessage(States.COMMAND_HANDLER_READY.getValue());
        } catch (IOException e) {
            e.printStackTrace();
            if (inputStream == null) {
                //tells that connection is established
                mHandler.sendEmptyMessage(States.TIMEOUT.getValue());
            }

            //execute disconnection
            disconnect();
        }
    }

    /**
     * disconnects and releases the connection to bluetooth device
     */
    public void disconnect() {
        try {
            if (dt != null) {
                dt.setThreadRunning(false);
            }
            bts.close();
        } catch (IOException ex) {
            ex.printStackTrace();
        }finally {
            connected=false;
            //tells that connection started
            mHandler.sendEmptyMessage(States.DISCONNECTED.getValue());
        }
    }

    /**
     * Returns a single instance of this object
     *
     * @return the single instance
     */
    public static synchronized BluetoothSerialCommunicationHandler getInstance() {
        if (handler == null)
            handler = new BluetoothSerialCommunicationHandler();
        return handler;
    }

    /**
     * Informs handler that consumer executed "send" the command to the analyzer. Waiting for answer.
     *
     * @param c The command
     */
    public void commandSent(Command c) {
        mHandler.sendEmptyMessage(States.DISCONNECTED.getValue());
    }

    /**
     * Informs handler that consumer received a response to a previously sent command.
     *
     * @param c the command executed with response
     */
    public void responseReceived(Command c) {
        mHandler.obtainMessage(States.RESPONSE_RECEIVED.getValue(), c).sendToTarget();
    }

    /**
     * Inform handler that consumer just dequeued a command to be executed
     *
     * @param c the command dequeued
     */
    public void dequeueCommand(Command c) {
        Log.d("Nitrox Analyzer", "Command dequeue :" + c.getCommand());
    }

    /**
     * Something went wrong
     *
     * @param c The returned command
     */
    @Override
    public void commandFailed(Command c) {
        Log.e("Nitrox Analyzer", "Command " + c.getCommand() + " failed!!");
        //is connection still up???
        Log.e("Nitrox Analyzer", "Bluetooth socket is disconnected. Disconnect Handler");
        //socket is closed => disconnect
        disconnect();
    }
}
