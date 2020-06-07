package com.axin.player;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceView;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends AppCompatActivity {
    private DNPlayer dnPlayer;

    //rtmp://58.200.131.2:1935/livetv/hunantv

    private SurfaceView surfaceView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        surfaceView = findViewById(R.id.surfaceView);
        dnPlayer = new DNPlayer();
        dnPlayer.setSurfaceView(surfaceView);
        dnPlayer.setDataSource("rtmp://58.200.131.2:1935/livetv/hunantv");
//        dnPlayer.setDataSource("http://hls1a.douyucdn.cn/live/288016rlols5_2000/playlist.m3u8?wsSecret=c70ce3b1bc7fb4c180206a0f4cc2699c&wsTime=1591254089&token=h5-douyu-0-288016-36ed2eaedf38a81d04b854c6430211a7&did=57c80714ddf676b079cda4ef00021531&origin=all&vhost=play2");
        dnPlayer.setOnPrepareListener(new DNPlayer.OnPrepareListener() {
            @Override
            public void onPrepare() {
                dnPlayer.start();
                Log.e("axin333","可以开始播发了");
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        Toast.makeText(MainActivity.this, "可以开始播发了", Toast.LENGTH_SHORT).show();

                    }
                });
            }
        });
    }

    public void start(View view) {
        dnPlayer.prepare();
    }
}
