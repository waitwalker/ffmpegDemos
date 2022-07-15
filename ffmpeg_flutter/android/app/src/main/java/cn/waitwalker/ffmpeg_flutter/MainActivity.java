package cn.waitwalker.ffmpeg_flutter;

import android.os.Bundle;

import androidx.annotation.Nullable;

import io.flutter.Log;
import io.flutter.embedding.android.FlutterActivity;

public class MainActivity extends FlutterActivity {

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        String str = JNI.stringFromJNI();
        Log.d("1", str);
    }

    @Override
    protected void onRestart() {
        super.onRestart();
    }
}
