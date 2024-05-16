package com.example.iotent;

public class WifiItem {

    private int imageWifi;
    private String nameWifi;

    public WifiItem(int iconID, String name) {
        this.imageWifi = iconID;
        this.nameWifi = name;

    }

    public int getImageWifi() {
        return imageWifi;
    }

    public String getNameWifi() {
        return nameWifi;
    }
}

