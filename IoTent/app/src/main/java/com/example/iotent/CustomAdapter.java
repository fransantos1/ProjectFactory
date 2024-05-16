package com.example.iotent;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import java.util.ArrayList;

/**
 * Provide views to RecyclerView with data from mDataSet.
 */
public class CustomAdapter extends RecyclerView.Adapter<CustomAdapter.sensorHolder> {
    SensorActivity sensorActivity;
    ArrayList<SensorItem> allSensorList;
    public CustomAdapter(SensorActivity sensorActivity, ArrayList<SensorItem> allSensorList){
        this.sensorActivity = sensorActivity;
        this.allSensorList = allSensorList;
    }


    @NonNull
    @Override
    public sensorHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        return new sensorHolder(LayoutInflater.from(sensorActivity).inflate(R.layout.item_sensor, parent, false));
    }

    @Override
    public void onBindViewHolder(@NonNull CustomAdapter.sensorHolder holder, int position) {

    }

    @Override
    public int getItemCount() {
        return allSensorList.size();
    }

    class sensorHolder extends RecyclerView.ViewHolder{
        TextView textView;

        public sensorHolder(@NonNull View itemView){
            super(itemView);
            itemView = itemView.findViewById(R.id.itemTxt);

        }
    }
}