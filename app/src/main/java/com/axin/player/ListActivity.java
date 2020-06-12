package com.axin.player;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;

public class ListActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_list);
    }

    public void play_one(View view) {
        Intent intent = new Intent(this,MainActivity.class);
        intent.putExtra("url","rtmp://58.200.131.2:1935/livetv/hunantv");
        startActivity(intent);
    }
    public void play_two(View view) {
        Intent intent = new Intent(this,MainActivity.class);
        intent.putExtra("url","http://hls3a.douyucdn.cn/live/5324159vs7009686x/playlist.m3u8?wsSecret=bf50915b4b9d027d6e94433e07a211be&wsTime=1591608558&token=h5-douyu-0-5324159-56d7fa7236f6b767db97641c4d7deae1&did=57c80714ddf676b079cda4ef00021531&origin=ws&vhost=play4&mix=1&st=1");
        startActivity(intent);
    }
}
