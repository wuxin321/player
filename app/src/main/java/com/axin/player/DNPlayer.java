package com.axin.player;

import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

/**
 * 提供java进行 播放 停止等函数
 */
public class DNPlayer implements SurfaceHolder.Callback {
    static {
        System.loadLibrary("native-lib");
    }

    private OnPrepareListener listener;
    private String dataSource;
    private SurfaceView mSurfaceView;
    private SurfaceHolder holder;

    /**
     * 让使用者设置播放的文件或者直播地址
     */
    public void setDataSource(String dataSource){
        this.dataSource = dataSource;
    }

    /**
     * 設置播放顯示的画布
     * @param surfaceView
     */
    public void setSurfaceView(SurfaceView surfaceView) {
        mSurfaceView = surfaceView;
        holder = surfaceView.getHolder();
        holder.addCallback(this);
    }

    public void onError(int errorCode){
        Log.e("axin","onError = "+errorCode);
    }

    public void onPrepare(){
        if (listener != null){
            listener.onPrepare();
        }
    }

    public void setOnPrepareListener(OnPrepareListener listener){
        this.listener = listener;
    }
    public interface OnPrepareListener{
        void onPrepare();
    }

    /**
     * 开始播放
     */
    public void start(){
        native_start();
    }

    /**
     * 停止播放
     */
    public void stop(){

    }


    public void release(){

        holder.removeCallback(this);
    }

    /**
     * 准备好 要播放的视频
     */
    public void prepare() {
        native_prepare(dataSource);
    }

    /**
     * 画布创建
     * @param holder
     */
    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        Log.e("axin","surfaceCreated");
    }

    /**
     * 画布发生了变化(横竖屏切换 按了home键都会回调这个函数)
     * @param holder
     * @param format
     * @param width
     * @param height
     */
    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        Log.e("axin","surfaceChanged");
        native_setSurface(holder.getSurface());
    }


    /**
     * 画布销毁 （按了home/退出应用）
     * @param holder
     */
    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        Log.e("axin","surfaceDestroyed");
    }

    native void native_prepare(String dataSource);
    native void native_start();

    native void native_setSurface(Surface surface);
}
