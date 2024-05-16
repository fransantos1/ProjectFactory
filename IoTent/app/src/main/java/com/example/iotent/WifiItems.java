package com.example.iotent;

import java.util.ArrayList;

public class WifiItems {

    ArrayList<WifiItem> wifiItemsList;
    int[] images = {
            R.drawable.wifi
    };

    String[] names = {

    };

    public WifiItems() {
        wifiItemsList = new ArrayList<>();
    }
    public void addWifiItems(WifiItem item){
        wifiItemsList.add(item);
    }
    public String[] getNames() {
        String[] names = new String[wifiItemsList.size()];
        for (int i = 0; i < wifiItemsList.size(); i++) {
            names[i] = wifiItemsList.get(i).getNameWifi();
        }
        return names;
    }
    public int[] getImages() {
        int[] images = new int[wifiItemsList.size()];
        for (int i = 0; i < wifiItemsList.size(); i++) {
            images[i] = wifiItemsList.get(i).getImageWifi();
        }
        return images;
    }

}