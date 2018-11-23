package com.booway.dwgdemo;

import android.os.Environment;
import android.os.Handler;
import android.os.Message;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;



public class MainActivity extends AppCompatActivity {
    private String filePath;
    private BoowayDwgView mView;
    private Handler mPdHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            mView.onLoad();
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mView = new BoowayDwgView(getApplication());
        setContentView(mView);
        filePath = Environment.getExternalStorageDirectory() + "/Download/1.dwg";

        new Thread(new Runnable() {
            @Override
            public void run() {
                BoowayDwgJni.open(filePath);
                mPdHandler.sendEmptyMessage(0);
            }
        }).start();
    }

    @Override
    public void finalize() {
    }

    @Override
    protected void onPause() {
        super.onPause();
        mView.onPause();
    }

    @Override
    protected void onResume() {
        super.onResume();
        mView.onResume();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (isFinishing()) {
            mPdHandler.removeCallbacksAndMessages(null);
            mView.onDestroy();
            BoowayDwgJni.close();
            BoowayDwgJni.finit();
        }
    }

//    private String getPath() {
//        Runtime runtime = Runtime.getRuntime();
//        try {
//            Process mProcess = runtime.exec("ls /storage");
//            BufferedReader mReader = new BufferedReader(new InputStreamReader(mProcess.getInputStream()));
//            StringBuffer mRespBuff = new StringBuffer();
//            char[] buff = new char[1024];
//            int ch = 0;
//            while ((ch = mReader.read(buff)) != -1) {
//                mRespBuff.append(buff, 0, ch);
//            }
//            mReader.close();
//            String[] result = mRespBuff.toString().trim().split("\n");
//            for (String str : result) {
//                if (str.equals("emulate") || str.equals("self"))
//                    continue;
//                return str;
//            }
//        } catch (IOException e) {
//// TODO Auto-generated catch block
//            e.printStackTrace();
//        }
//
//
//        return null;
//    }

//    private  String ExternalPath1;
//    private  String ExternalPath2;
//
//
//    private  int getMountedSDCardCount1(Context context) {
//        ExternalPath1 = null;
//        ExternalPath2 = null;
//        int readyCount = 0;
//        StorageManager storageManager = (StorageManager) context.getSystemService(Context.STORAGE_SERVICE);
//        if (storageManager == null)
//            return 0;
//        Method method;
//        Object obj;
//        try {
//            method = storageManager.getClass().getMethod("getVolumePaths", (Class[]) null);
//            obj = method.invoke(storageManager, (Object[]) null);
//
//
//            String[] paths = (String[]) obj;
//            if (paths == null)
//                return 0;
//
//
//            method = storageManager.getClass().getMethod("getVolumeState", new Class[]{String.class});
//            for (String path : paths) {
//                obj = method.invoke(storageManager, new Object[]{path});
//                if (Environment.MEDIA_MOUNTED.equals(obj)) {
//                    readyCount++;
//                    if (2 == readyCount) {
//                        ExternalPath1 = path;
//                    }
//                    if (3 == readyCount) {
//                        ExternalPath2 = path;
//                    }
//                }
//            }
//        } catch (NoSuchMethodException ex) {
//            throw new RuntimeException(ex);
//        } catch (IllegalAccessException ex) {
//            throw new RuntimeException(ex);
//        } catch (InvocationTargetException ex) {
//            if (Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED)) {
//                readyCount = 1;
//            }
//            Log.d("Test", ex.getMessage());
//            return readyCount;
//        }
//
//
//        Log.d("Test", "mounted sdcard unmber: " + readyCount);
//        return readyCount;
//    }

}
