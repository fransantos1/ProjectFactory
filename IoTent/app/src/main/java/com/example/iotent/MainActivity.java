package com.example.iotent;


import android.Manifest;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothManager;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.text.InputType;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.Toast;

import androidx.annotation.RequiresApi;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import java.util.Objects;
import java.util.Set;
import java.util.UUID;
import java.util.concurrent.atomic.AtomicReference;

import io.reactivex.Observable;
import io.reactivex.android.schedulers.AndroidSchedulers;
import io.reactivex.disposables.Disposable;
import io.reactivex.schedulers.Schedulers;

public class MainActivity extends AppCompatActivity {

    private static final String TAG = "IoTent";
    //We will use a Handler to get the BT Connection statys
    public static Handler handler;
    private final static int ERROR_READ = 0; // used in bluetooth handler to identify message update
    BluetoothDevice ESP32 = null;
    UUID arduinoUUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB"); //We declare a default UUID to create the global variable
    @RequiresApi(api = Build.VERSION_CODES.M)

    public int REQUEST_ENABLE_BT = 0;
    ListView lView;
    ListAdapterWifi lAdapter;
    WifiItems wifiItems;
    private ConnectedThread connectedThread;
    private ConnectThread connectThread;
    AtomicReference<ConnectedThread> connectedThreadRef = new AtomicReference<>();
    public void handleMessage(Message msg) {
        switch (msg.what) {

            case ERROR_READ:
                String arduinoMsg = msg.obj.toString(); // Read message from Arduino
                Toast.makeText(MainActivity.this,arduinoMsg, Toast.LENGTH_SHORT).show();
                break;
        }
    }



    @Override
    protected void onCreate(Bundle savedInstance) {
        super.onCreate(savedInstance);
        setContentView(R.layout.activity_wifi_selection);
        Objects.requireNonNull(getSupportActionBar()).hide();

        wifiItems = new WifiItems();

        lView = (ListView)findViewById(R.id.listview_wifi);
        BluetoothManager bluetoothManager = getSystemService(BluetoothManager.class);
        BluetoothAdapter bluetoothAdapter = bluetoothManager.getAdapter();

        handler = new Handler(Looper.getMainLooper());


        if (bluetoothAdapter == null) {
            // Device doesn't support Bluetooth
            Toast.makeText(MainActivity.this,"Device doesn't support Bluetooth", Toast.LENGTH_SHORT).show();
            Log.d(TAG, "Device doesn't support Bluetooth");
        } else {
            Toast.makeText(MainActivity.this,"Device support Bluetooth", Toast.LENGTH_SHORT).show();
            Log.d(TAG, "Device support Bluetooth");
            //Check BT enabled. If disabled, we ask the user to enable BT
            if (!bluetoothAdapter.isEnabled()) {
                Toast.makeText(MainActivity.this, "Bluetooth is disabled", Toast.LENGTH_SHORT).show();
                Log.d(TAG, "Bluetooth is disabled");
                Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                if (ActivityCompat.checkSelfPermission(getApplicationContext(), Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {

                    Log.d(TAG, "We don't BT Permissions");
                    startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
                    Log.d(TAG, "Bluetooth is enabled now");
                    Toast.makeText(MainActivity.this, "Bluetooth is enabled now", Toast.LENGTH_SHORT).show();
                } else {
                    Log.d(TAG, "We have BT Permissions");
                    startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
                    Toast.makeText(MainActivity.this, "Bluetooth enabled now ", Toast.LENGTH_SHORT).show();
                }
            }



            String btDevicesString="";
            Set<BluetoothDevice> pairedDevices = bluetoothAdapter.getBondedDevices();

            if (pairedDevices.size() > 0) {

                for (BluetoothDevice device: pairedDevices) {
                    String deviceName = device.getName();
                    String deviceHardwareAddress = device.getAddress(); // MAC address

                    btDevicesString=btDevicesString+deviceName+" || "+deviceHardwareAddress+"\n";

                    if (deviceName.equals("ESP32")) {
                        arduinoUUID = device.getUuids()[0].getUuid();
                        ESP32 = device;
                        Toast.makeText(MainActivity.this,"ESP32 FOUND ", Toast.LENGTH_SHORT).show();


                        try {
                            final Observable<String> connectToBTObservable = Observable.create(emitter -> {

                                connectThread = new ConnectThread(ESP32, arduinoUUID, handler);
                                connectThread.run();
                                //Check if Socket connected
                                Log.d("DEBUGGGG", "VERIFICA SE ESTÁ CONECTADO");
                                if (connectThread.getMmSocket().isConnected()) {

                                    connectedThread = new ConnectedThread(connectThread.getMmSocket());
                                    connectedThread.run();
                                    connectedThreadRef.set(connectedThread);
                                    String valueRead = connectedThread.getValueRead();

                                    if (valueRead != null) {
                                        emitter.onNext(connectedThread.getValueRead());
                                    }
                                    Log.d("content", " " + valueRead);

                                }



                                emitter.onComplete();

                            });
                            Toast.makeText(MainActivity.this,"ESP32 CONECTADO", Toast.LENGTH_SHORT).show();
                            if (ESP32 != null) {
                                Disposable disposable =
                                        connectToBTObservable.observeOn(AndroidSchedulers.mainThread()).subscribeOn(Schedulers.io()).subscribe(valueRead -> {
                                            String[] wifiArray = valueRead.split(" \\| ");
                                            WifiItems wifiItems = new WifiItems();
                                            for(String wifi: wifiArray){
                                                wifiItems.addWifiItems(new WifiItem(R.drawable.wifi, wifi));
                                            }
                                            lAdapter = new ListAdapterWifi(MainActivity.this, wifiItems.getNames(), wifiItems.getImages());
                                            lView.setAdapter(lAdapter);
                                            Log.d("content"," "+valueRead);


                                            lView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
                                                @Override
                                                public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                                                    AlertDialog.Builder builder = new AlertDialog.Builder(MainActivity.this);
                                                    builder.setTitle("Enter Password");


                                                    final EditText input = new EditText(MainActivity.this);
                                                    input.setInputType(InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_VARIATION_PASSWORD);
                                                    builder.setView(input);


                                                    builder.setPositiveButton("OK", new DialogInterface.OnClickListener() {
                                                        @Override
                                                        public void onClick(DialogInterface dialog, int which) {
                                                            String wifiName = wifiItems.getNames()[position];
                                                            String wifiPassword = input.getText().toString();

                                                            Log.d("content"," "+wifiName+" "+wifiPassword);
                                                            String wifiInfo = wifiName + " " + wifiPassword;

                                                            try {
                                                                connectedThread.write(wifiInfo);
                                                                Toast.makeText(MainActivity.this,"A conectar...", Toast.LENGTH_SHORT).show();
                                                                Thread.sleep(15000);
                                                                if(valueRead.contains("Conectado")){
                                                                    connectThread.cancel();
                                                                    Toast.makeText(MainActivity.this,"Wifi conectado", Toast.LENGTH_SHORT).show();
                                                                    //TODO:mudar para prox pagina.
                                                                }else{
                                                                    Toast.makeText(MainActivity.this,"Falha na conexão", Toast.LENGTH_SHORT).show();
                                                                }


                                                            }catch (Exception e) {
                                                                throw new RuntimeException(e);
                                                            }
                                                        }
                                                    });
                                                    builder.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                                                        @Override
                                                        public void onClick(DialogInterface dialog, int which) {
                                                            dialog.cancel();
                                                        }
                                                    });
                                                    builder.show();
                                                }
                                            });


                            });
                        }
                        }catch (Exception e) {
                            throw new RuntimeException(e);
                            }
                        }
                    }
                }

        }
    }
}