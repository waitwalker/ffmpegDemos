package cn.waitwalker.ffmpeg_flutter;

// java 调用对应的C代码
public class JNI {
    static{
        System.loadLibrary("sample");
    }

    public static native String sayHello();
    public static native String stringFromJNI();
}
