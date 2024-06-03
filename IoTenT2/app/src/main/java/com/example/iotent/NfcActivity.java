package com.example.iotent;

import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.nfc.FormatException;
import android.nfc.NfcAdapter;
import android.nfc.Tag;
import android.nfc.TagLostException;
import android.nfc.tech.MifareClassic;
import android.nfc.tech.Ndef;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.content.Context;
import android.content.SharedPreferences;
import android.widget.Toast;


import androidx.appcompat.app.AppCompatActivity;

import java.io.IOException;
import java.nio.charset.StandardCharsets;

public class NfcActivity extends AppCompatActivity {

    Button btn_refresh_NFC;
@Override
        protected void onCreate(Bundle savedInstanceState){
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_nfc);

        Intent intent = getIntent();
        onNewIntent(intent);
    }
    public void onNewIntent(Intent intent) {
        super.onNewIntent(intent);
        Tag tag = intent.getParcelableExtra(NfcAdapter.EXTRA_TAG);
        if (tag == null) {
            Log.d("NFC", "No NFC tag detected");
            return;
        }
        MifareClassic mfc = MifareClassic.get(tag);
        if (mfc == null) {
            Log.d("NFC", "MifareClassic tag not supported");
            return;
        }
        byte[] key = { (byte) 0xD3, (byte) 0xF7, (byte) 0xD3, (byte) 0xF7, (byte) 0xD3, (byte) 0xF7 };
        try {
            mfc.connect();

            boolean isAuth = mfc.authenticateSectorWithKeyA(1, key); // Authenticating sector 1 to read block 4

            if (isAuth) {
                byte[] data = mfc.readBlock(4); // Reading block 4 directly
                if (data != null) {
                    String dataString = new String(data, StandardCharsets.UTF_8).trim(); // Convert byte array to string with UTF-8 encoding
                    Log.d("NFC", "Data: " + dataString);

                    SharedPreferences sharedPreferences = this.getSharedPreferences("MyAppPrefs", Context.MODE_PRIVATE);
                    SharedPreferences.Editor editor = sharedPreferences.edit();
                    editor.putString("auth_token", dataString);
                    editor.apply();
                    Intent switchActivity = new Intent(this, MainActivity.class);
                    startActivity(switchActivity);
                } else {
                    Log.d("NFC", "Failed to read block");
                }
            } else {
                Log.d("NFC", "Authentication failed");
            }
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            try {
                mfc.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }
    @Override
    protected void onResume() {
        super.onResume();
        NfcAdapter nfcAdapter = NfcAdapter.getDefaultAdapter(this);
        if (nfcAdapter != null) {
            PendingIntent pendingIntent = PendingIntent.getActivity(this, 0,
                    new Intent(this, getClass()).addFlags(Intent.FLAG_ACTIVITY_SINGLE_TOP), 0);
            IntentFilter[] intentFiltersArray = new IntentFilter[]{};
            String[][] techListsArray = new String[][]{new String[]{MifareClassic.class.getName()}};
            nfcAdapter.enableForegroundDispatch(this, pendingIntent, intentFiltersArray, techListsArray);
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        NfcAdapter nfcAdapter = NfcAdapter.getDefaultAdapter(this);
        if (nfcAdapter != null) {
            nfcAdapter.disableForegroundDispatch(this);
        }
    }





}
