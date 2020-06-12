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
    private TextView stop;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        surfaceView = findViewById(R.id.surfaceView);
        stop =findViewById(R.id.stop);
        dnPlayer = new DNPlayer();
        dnPlayer.setSurfaceView(surfaceView);
        String url = getIntent().getStringExtra("url");
        Log.e("xain","url="+url);
        dnPlayer.setDataSource(url);
//        dnPlayer.setDataSource("http://hls3a.douyucdn.cn/live/5324159vs7009686x/playlist.m3u8?wsSecret=bf50915b4b9d027d6e94433e07a211be&wsTime=1591608558&token=h5-douyu-0-5324159-56d7fa7236f6b767db97641c4d7deae1&did=57c80714ddf676b079cda4ef00021531&origin=ws&vhost=play4&mix=1&st=1");
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

    public void stop(View view) {
        dnPlayer.stop();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        dnPlayer.release();
    }

    @Override
    protected void onStop() {
        super.onStop();
        stop(stop);
    }
}
