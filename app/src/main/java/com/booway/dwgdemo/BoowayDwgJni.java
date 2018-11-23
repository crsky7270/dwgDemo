package com.booway.dwgdemo;

/**
 * Created by booway on 2018/8/30.
 */

import android.util.Log;

public class BoowayDwgJni {
    // Used to load the 'native-lib' library on application startup.
    static {

        //System.loadLibrary("libDwgRead");
        try {
            System.loadLibrary("native-lib");
        } catch (Exception ex) {
            String msg = ex.getMessage();
        }

    }

    public static native boolean init(String file);

    public static native boolean open(String file);

    public static native boolean createRenderer(int width, int height);

    public static native boolean change(int width, int height);

    public static native boolean renderFrame();

    public static native boolean destroyRenderer();

    public static native boolean close();

    public static native boolean finit();


    public static native boolean viewTranslate ( float xAxis, float yAxis );
//
    public static native boolean viewScale ( float sCoef );
//
//    public static native boolean viewCanRotate ();
//
//    public static native boolean viewRotate ( float rAngle );
//
//    public static native boolean viewOrbit ( float xAxis, float yAxis );
//
//    public static native int viewGetRenderMode ();
//
//    public static native boolean viewSetRenderMode ( int nMode );
//
//    public static native boolean viewRegen ();
}
