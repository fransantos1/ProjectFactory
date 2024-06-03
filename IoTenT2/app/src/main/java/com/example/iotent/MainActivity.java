package com.example.iotent;
import android.os.Handler;
import android.content.Context;
import android.content.Intent;
import android.net.ConnectivityManager;
import android.net.Network;
import android.net.NetworkCapabilities;
import android.net.NetworkRequest;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.content.SharedPreferences;
import androidx.appcompat.app.AppCompatActivity;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.IOException;

import okhttp3.MediaType;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.RequestBody;
import okhttp3.Response;

public class MainActivity extends AppCompatActivity {

    Button btnlocation, btnrefresh, btnLight;
    private Handler handler;
    TextView value_temp, value_hum;
    String token;
    String ip = "http://192.168.1.72:8080";
    private OkHttpClient client = new OkHttpClient();
    public static final MediaType JSON = MediaType.get("application/json; charset=utf-8");

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.d("Debug: ","Started");
        SharedPreferences sharedPreferences = this.getSharedPreferences("MyAppPrefs", Context.MODE_PRIVATE);
        token = sharedPreferences.getString("auth_token", null);
        if(token == null){
            Log.d("DEBUG: ","not TOken");
            Intent switchActivity = new Intent(this, NfcActivity.class);
            startActivity(switchActivity);
        }else{
            Log.d("DEBUG: ",token);
        }


        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_main);

        handler = new Handler();

        ConnectivityManager connManager = (ConnectivityManager) getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkRequest.Builder builder = new NetworkRequest.Builder();
        builder.addTransportType(NetworkCapabilities.TRANSPORT_WIFI);
        builder.addCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET);

        NetworkRequest request = builder.build();

        connManager.requestNetwork(request, new ConnectivityManager.NetworkCallback() {
            @Override
            public void onAvailable(Network network) {
                connManager.bindProcessToNetwork(network);
            }
        });
        btnLight = findViewById(R.id.btn_light);
        btnlocation = findViewById(R.id.btn_location);
        btnrefresh = findViewById(R.id.btn_token);
        value_temp = findViewById(R.id.value_temp);
        value_hum = findViewById(R.id.value_hum);
        btnrefresh.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View view) {
                    saveToken(null);
                    Intent switchActivity = new Intent(MainActivity.this, NfcActivity.class);
                    startActivity(switchActivity);
                }
            });
        btnLight.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                TurnLight();
            }
        });

        btnlocation.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                FindNode();
            }
        });


        GetData();
       handler.postDelayed(sendHttpRequestRunnable, 30000);
    }

    private Runnable sendHttpRequestRunnable = new Runnable() {
        @Override
        public void run() {
            GetData();
            handler.postDelayed(this, 30000); // Send the request again after 30 seconds
        }
    };

    private void GetData() {
        final String api = ip+"/api/node/GetData"; // insert API IP
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    OkHttpClient client = new OkHttpClient();
                    Request request = new Request.Builder()
                            .url(api)
                            .addHeader("UserToken", token)
                            .build();
                    Response response = client.newCall(request).execute();
                    final String _response = response.body().string();
                    final int responseCode = response.code();
                    Log.d("Response Code", String.valueOf(responseCode));
                    Log.d("Debug", _response);
                    if(responseCode == 401){
                        handler.removeCallbacks(sendHttpRequestRunnable);
                        saveToken(null);
                        Intent switchActivity = new Intent(MainActivity.this, NfcActivity.class);
                        startActivity(switchActivity);
                        return;
                    }
                    JSONObject jsonObject = new JSONObject(_response);
                    JSONObject resultObject = jsonObject.getJSONObject("result");
                    final String temperature = resultObject.getString("temperature")+" Cº";
                    final String humidity = resultObject.getString("humidity") + " %";

                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            value_temp.setText(temperature);
                            value_hum.setText(humidity);
                        }
                    });
                } catch (IOException e) {
                    e.printStackTrace();
                } catch (JSONException e) {
                    throw new RuntimeException(e);
                }
            }
        }).start();
    }
    private void FindNode() {
        final String api = ip+"/api/node/localize"; // insert API IP
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    OkHttpClient client = new OkHttpClient();
                    Request request = new Request.Builder()
                            .url(api)
                            .addHeader("UserToken", token)
                            .build();
                    Response response = client.newCall(request).execute();
                    final int responseCode = response.code();
                    Log.d("Response Code", String.valueOf(responseCode));
                    if(responseCode == 401){
                        handler.removeCallbacks(sendHttpRequestRunnable);
                        saveToken(null);
                        Intent switchActivity = new Intent(MainActivity.this, NfcActivity.class);
                        startActivity(switchActivity);
                        return;
                    }
                    final String _response = response.body().string();

                    Log.d("Debug", _response);
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }).start();
    }
    private void TurnLight() {
        final String api = ip+"/api/node/Led"; // insert API IP
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    OkHttpClient client = new OkHttpClient();
                    RequestBody requestBody = RequestBody.create("", null);
                    Request request = new Request.Builder()
                            .url(api)
                            .post(requestBody)
                            .addHeader("UserToken", token)
                            .build();
                    Response response = client.newCall(request).execute();
                    final int responseCode = response.code();
                    Log.d("Response Code", String.valueOf(responseCode));
                    final String _response = response.body().string();
                    Log.d("Debug", _response);
                    if(responseCode == 401){
                        handler.removeCallbacks(sendHttpRequestRunnable);
                        saveToken(null);
                        Intent switchActivity = new Intent(MainActivity.this, NfcActivity.class);
                        startActivity(switchActivity);
                        return;
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }).start();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        // Remove callbacks to avoid memory leaks
        handler.removeCallbacks(sendHttpRequestRunnable);
    }
    private void saveToken(String token) {
        SharedPreferences sharedPreferences = getSharedPreferences("MyAppPrefs", MODE_PRIVATE);
        SharedPreferences.Editor editor = sharedPreferences.edit();
        editor.putString("auth_token", token);
        editor.apply();
    }


}