package com.example.iotent;

import android.os.Bundle;
import android.util.Log;

import androidx.appcompat.app.AppCompatActivity;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.android.volley.Request;
import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.StringRequest;
import com.android.volley.toolbox.Volley;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.Objects;

public class SensorActivity extends AppCompatActivity {
    ArrayList<SensorItem> allSensorList;
    RecyclerView recyclerSensors;

    String api ="localhost:8080/api/data";
    private boolean mLogShown;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Objects.requireNonNull(getSupportActionBar()).hide();

        recyclerSensors.findViewById(R.id.recyclerSensors);
        recyclerSensors.setLayoutManager(new LinearLayoutManager(this));

        allSensorList = new ArrayList<>();

        getData();
    }

    private void getData(){
        RequestQueue queue = Volley.newRequestQueue(this);

        // Request a string response from the provided URL.
        StringRequest stringRequest = new StringRequest(Request.Method.GET, api,
                new Response.Listener<String>() {
                    @Override
                    public void onResponse(String response) {

                        try {
                            JSONArray array = new JSONArray(response);
                            for(int i = 0; i<array.length();i++){
                                JSONObject object = array.getJSONObject(i);

                                // nomes tem de coincidir com os atributos recebidos da api
                                SensorItem sensor = new SensorItem(
                                        object.getString("temperatura"),
                                        object.getString("tipo_de_Sensor")
                                );
                                allSensorList.add(sensor);
                            }
                            recyclerSensors.setAdapter(new CustomAdapter(SensorActivity.this, allSensorList));

                            Log.e("TAMANHO DA RESPOSTA", "onResponse"+ allSensorList.size());

                        }catch (JSONException e){
                            e.printStackTrace();
                            Log.e("API", "OnResponse "+e.getMessage());
                        }
                        // Display the first 500 characters of the response string.
                        Log.e("API", "OnErrorResponse" + response.toString());
                    }
                }, new Response.ErrorListener() {
            @Override
            public void onErrorResponse(VolleyError error) {
                Log.e("API", "OnErrorResponse" + error.getLocalizedMessage());
            }
        });

// Add the request to the RequestQueue.
        queue.add(stringRequest);

    }

    public void onClick(){

    }

}

