package com.example.iotent;

public class SensorItem {


    private String nameSensor;
    private String valueSensor;

    public SensorItem( String name, String value){

        this.nameSensor = name;
        this.valueSensor = value;

    }

    public String getNameSensor() {
        return nameSensor;
    }

    public String getValueSensor() {
        return valueSensor;
    }
}
