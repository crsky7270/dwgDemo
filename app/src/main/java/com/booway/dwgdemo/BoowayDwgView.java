package com.booway.dwgdemo;

import android.content.Context;
import android.graphics.PixelFormat;
import android.graphics.PointF;
import android.opengl.GLES10;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.opengl.GLU;
import android.opengl.GLUtils;
import android.os.Environment;
import android.util.AttributeSet;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.MotionEvent;

import com.booway.testSharpes.Line;
import com.booway.utils.VaryTools;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.opengles.GL10;

import static com.booway.utils.CalUtils.midPoint;
import static com.booway.utils.CalUtils.rotation;
import static com.booway.utils.CalUtils.spacing;

/**
 * Created by booway on 2018/8/30.
 */

public class BoowayDwgView extends GLSurfaceView {
    private static String TAG = "DwgView";
    private static int GLESVER = 2;
    public Renderer mRenderer;
    private Context dwgViewCtx;

    public void onLoad() {
        mRenderer.onLoad(dwgViewCtx);
    }

    public BoowayDwgView(Context context) {
        super(context);
        init(false, 24, 2);
        dwgViewCtx = context;
//        setRenderMode(RENDERMODE_CONTINUOUSLY);
    }

    private void init(boolean translucent, int depth, int stencil) {
        if (translucent)
            this.getHolder().setFormat(PixelFormat.TRANSLUCENT);

        setEGLContextFactory(new ContextFactory());

        setEGLConfigChooser(translucent ?
                new ConfigChooser(8, 8, 8, 8, depth, stencil) :
                new ConfigChooser(5, 6, 5, 0, depth, stencil));

        mRenderer = new Renderer();
        setEGLContextClientVersion(2);
        setRenderer(mRenderer);
//        setRenderMode(RENDERMODE_WHEN_DIRTY);
    }

    private static class ConfigChooser implements GLSurfaceView.EGLConfigChooser {

        public ConfigChooser(int r, int g, int b, int a, int depth, int stencil) {
            mRedSize = r;
            mGreenSize = g;
            mBlueSize = b;
            mAlphaSize = a;
            mDepthSize = depth;
            mStencilSize = stencil;
        }

        private static int EGL_OPENGL_ES2_BIT = 4;
        private static int[] s_configAttribs1 =
                {
                        EGL10.EGL_RED_SIZE, 4,
                        EGL10.EGL_GREEN_SIZE, 4,
                        EGL10.EGL_BLUE_SIZE, 4,
                        // EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                        EGL10.EGL_NONE
                };
        private static int[] s_configAttribs2 =
                {
                        EGL10.EGL_RED_SIZE, 4,
                        EGL10.EGL_GREEN_SIZE, 4,
                        EGL10.EGL_BLUE_SIZE, 4,
                        EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                        EGL10.EGL_NONE
                };

        public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display) {

            int[] num_config = new int[1];
            if (GLESVER != 2)
                egl.eglChooseConfig(display, s_configAttribs1, null, 0, num_config);
            else
                egl.eglChooseConfig(display, s_configAttribs2, null, 0, num_config);

            int numConfigs = num_config[0];

            if (numConfigs <= 0) {
                throw new IllegalArgumentException("No configs match configSpec");
            }

            EGLConfig[] configs = new EGLConfig[numConfigs];
            if (GLESVER != 2)
                egl.eglChooseConfig(display, s_configAttribs1, configs, numConfigs, num_config);
            else
                egl.eglChooseConfig(display, s_configAttribs2, configs, numConfigs, num_config);

            return chooseConfig(egl, display, configs);
        }

        public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display,
                                      EGLConfig[] configs) {
            int checkStencil = 2;
            while ((checkStencil--) != 0) {
                for (EGLConfig config : configs) {
                    int d = findConfigAttrib(egl, display, config,
                            EGL10.EGL_DEPTH_SIZE, 0);
                    int s = findConfigAttrib(egl, display, config,
                            EGL10.EGL_STENCIL_SIZE, 0);
                    if (d < mDepthSize || ((checkStencil != 0) && (s < mStencilSize)))
                        continue;
                    int r = findConfigAttrib(egl, display, config,
                            EGL10.EGL_RED_SIZE, 0);
                    int g = findConfigAttrib(egl, display, config,
                            EGL10.EGL_GREEN_SIZE, 0);
                    int b = findConfigAttrib(egl, display, config,
                            EGL10.EGL_BLUE_SIZE, 0);
                    int a = findConfigAttrib(egl, display, config,
                            EGL10.EGL_ALPHA_SIZE, 0);

                    if (r == mRedSize && g == mGreenSize && b == mBlueSize && a == mAlphaSize)
                        return config;
                }
            }
            return null;
        }

        private int findConfigAttrib(EGL10 egl, EGLDisplay display,
                                     EGLConfig config, int attribute, int defaultValue) {

            if (egl.eglGetConfigAttrib(display, config, attribute, mValue)) {
                return mValue[0];
            }
            return defaultValue;
        }

        protected int mRedSize;
        protected int mGreenSize;
        protected int mBlueSize;
        protected int mAlphaSize;
        protected int mDepthSize;
        protected int mStencilSize;
        private int[] mValue = new int[1];
    }

    private static class ContextFactory implements GLSurfaceView.EGLContextFactory {
        private static int EGL_CONTEXT_CLIENT_VERSION = 0x3098;

        public EGLContext createContext(EGL10 egl, EGLDisplay display, EGLConfig eglConfig) {
            if (GLESVER != 2)
                Log.w(TAG, "creating OpenGL ES 1.1 context");
            else
                Log.w(TAG, "creating OpenGL ES 2.0 context");
            checkEglError("Before eglCreateContext", egl);
            int[] attrib_list = {EGL_CONTEXT_CLIENT_VERSION, GLESVER, EGL10.EGL_NONE};
            EGLContext context = egl.eglCreateContext(display, eglConfig, EGL10.EGL_NO_CONTEXT, attrib_list);
            checkEglError("After eglCreateContext", egl);
            return context;
        }

        public void destroyContext(EGL10 egl, EGLDisplay display, EGLContext context) {
            egl.eglDestroyContext(display, context);
        }
    }

    private static void checkEglError(String prompt, EGL10 egl) {
        int error;
        while ((error = egl.eglGetError()) != EGL10.EGL_SUCCESS) {
            Log.e(TAG, String.format("%s: EGL error: 0x%x", prompt, error));
        }
    }

    private static class Renderer implements GLSurfaceView.Renderer {
        private int mWidth = 0;
        private int mHeight = 0;
        private boolean mLoaded = false;
        private boolean mContextCreated = false;
        private Context dwgViewCtx;
        //        private Line line;
        public float scale = 1.0f;
        private String fontPath;

        @Override
        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
            fontPath = Environment.getExternalStorageDirectory() + "/Download/simsun.ttc";
            BoowayDwgJni.init(fontPath);
        }

        @Override
        public void onSurfaceChanged(GL10 gl, int width, int height) {
            mWidth = width;
            mHeight = height;
        }

        @Override
        public void onDrawFrame(GL10 gl) {
            if (mContextCreated == true) {
                BoowayDwgJni.renderFrame();
            } else {
                if (mLoaded == true && mWidth != 0 && mHeight != 0) {
                    mContextCreated = true;
                    BoowayDwgJni.createRenderer(mWidth, mHeight);

                } else {
                    GLES20.glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
                    GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);
                }
            }
        }

        public void onLoad(Context ctx) {
            mLoaded = true;
            dwgViewCtx = ctx;
        }

        public void onDestroyContext() {
            if (mContextCreated == true) {
                mContextCreated = false;
                BoowayDwgJni.destroyRenderer();
            }
        }
    }

    public void onDestroy() {
        mRenderer.onDestroyContext();
    }

    // we can be in one of these 3 states
    private static final int NONE = 0;
    private static final int DRAG = 1;
    private static final int ZOOM = 2;
    private static final int ORBIT = 3;
    private int mTouchMode = NONE;
    // remember some things for zooming
    private PointF mTouchStart = new PointF();
    private PointF mTouchMid = new PointF();
    private float mTouchOldDist = 1f;
    private float mTouchOldRot = 0f;
    private float[] mTouchLastEvent = null;
    private long mTouchLastTime = -1;


    @Override
    public boolean onTouchEvent(MotionEvent event) {
        /*触碰后设置为，当数据发生变动才刷新 view*/
        setRenderMode(RENDERMODE_WHEN_DIRTY);
        switch (event.getAction() & MotionEvent.ACTION_MASK) {
            case MotionEvent.ACTION_DOWN:
                long thisTime = System.currentTimeMillis();
                mTouchStart.set(event.getX(), event.getY());
                if (thisTime - mTouchLastTime < 250 && mTouchMode == NONE) {
                    // Double click
                    mTouchMode = ORBIT;
                    mTouchLastTime = -1;
                } else {
                    mTouchMode = DRAG;
                    mTouchLastTime = thisTime;
                }
                mTouchLastEvent = null;
                break;
            case MotionEvent.ACTION_POINTER_DOWN:
                mTouchOldDist = spacing(event);
                if (mTouchOldDist > 10f) {
                    midPoint(mTouchMid, event);
                    mTouchMode = ZOOM;
                }
                mTouchLastEvent = new float[4];
                mTouchLastEvent[0] = event.getX(0);
                mTouchLastEvent[1] = event.getX(1);
                mTouchLastEvent[2] = event.getY(0);
                mTouchLastEvent[3] = event.getY(1);
                mTouchOldRot = rotation(event);
                break;
            case MotionEvent.ACTION_UP:
            case MotionEvent.ACTION_POINTER_UP:
                mTouchMode = NONE;
                mTouchLastEvent = null;
//                BoowayDwgJni.viewTranslate(0, 0);
//                BoowayDwgJni.viewScale(1.0f);
                break;
            case MotionEvent.ACTION_MOVE:
                if (mTouchMode == DRAG) {
                    float dx = event.getX() - mTouchStart.x;
                    float dy = event.getY() - mTouchStart.y;
                    //TeighaDWGJni.viewTranslate(dx, dy);
//                    Log.w(TAG, "sx:" + mTouchStart.x + ";sy:" + mTouchStart.y);
//                    mRenderer.offsetX = dx / event.getX();
//                    mRenderer.offsetY = dy / event.getY();
                    BoowayDwgJni.viewTranslate(dx, dy);
                    requestRender();
                    mTouchStart.x += dx;
                    mTouchStart.y += dy;
//                    Log.w(TAG, "dx:" + dx + ";dy:" + dy);
                } else if (mTouchMode == ORBIT) {
                    float dx = event.getX() - mTouchStart.x;
                    float dy = event.getY() - mTouchStart.y;

                    final DisplayMetrics displayMetrics = new DisplayMetrics();
                    ((MainActivity) this.getContext()).getWindowManager()
                            .getDefaultDisplay().getMetrics(displayMetrics);
                    float density = displayMetrics.density;
                    //TeighaDWGJni.viewOrbit((float)Math.toRadians(dx / density / 2), (float)Math.toRadians(dy / density / 2));
                    mTouchStart.x += dx;
                    mTouchStart.y += dy;
                } else if (mTouchMode == ZOOM) {
                    float newDist = spacing(event);
                    if (newDist > 10f) {
                        float scale = (newDist / mTouchOldDist);
                        BoowayDwgJni.viewScale(scale);
//                        mRenderer.scale = scale;
                        requestRender();
                        mTouchOldDist = newDist;
                    }
                    if (mTouchLastEvent != null && event.getPointerCount() == 3) {
                        float newRot = rotation(event);
                        float r = newRot - mTouchOldRot;
//                        if (TeighaDWGJni.viewCanRotate())
//                            TeighaDWGJni.viewRotate((float)Math.toRadians(r));
                        mTouchOldRot = newRot;
                    }
                    break;
                }
        }
        return true;
    }
}
