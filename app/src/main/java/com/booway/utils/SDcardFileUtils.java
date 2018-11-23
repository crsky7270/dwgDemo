package com.booway.utils;

import android.content.Context;
import android.os.Environment;
import android.os.StatFs;
import android.text.format.Formatter;
import android.util.Log;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;


/**
 * Created by booway on 2018/8/13.
 */

public class SDcardFileUtils {
    // 判断sd卡是否可用
    public boolean isSdSafe() {
        /**
         * 当equals结果为true时，代表sd卡可正常使用
         */
        return Environment.getExternalStorageState().equals(
                Environment.MEDIA_MOUNTED);
    }

    // 获取sd卡总大小
    public String getTotalSize(Context context) {
        // 获取SD卡根目录
        File path = Environment.getExternalStorageDirectory();
        // 获取指定目录下的内存存储状态
        StatFs stat = new StatFs(path.getPath());
        // 获取单个扇区的大小
        long blockSize = stat.getBlockSize();
        // 获取扇区的数量
        long totalBlocks = stat.getBlockCount();
        // 总空间 = 扇区的总数 * 扇区的大小
        long totalSize = blockSize * totalBlocks;
        // 格式化文件大小的格式
        Log.i("lyb", "总空间 = " + Formatter.formatFileSize(context, totalSize));
        return Formatter.formatFileSize(context, totalSize);
    }

    // 获取sd卡的可用大小
    public String getAvailableSize(Context context) {
        // 获取SD卡根目录
        File path = Environment.getExternalStorageDirectory();
        // 获取指定目录下的内存存储状态
        StatFs stat = new StatFs(path.getPath());
        // 获取单个扇区的大小
        long blockSize = stat.getBlockSize();
        // 获取可以使用的扇区数量
        long availableBlocks = stat.getAvailableBlocks();
        // 可用空间 = 扇区的大小 + 可用的扇区
        long availableSize = blockSize * availableBlocks;
        // 格式化文件大小的格式
        Log.i("lyb", "可用空间 = " + Formatter.formatFileSize(context, availableSize));
        return Formatter.formatFileSize(context, availableSize);
    }

    // 获取sd卡根目录字符串的路径
    public String getSdPath() {
        return Environment.getExternalStorageDirectory().getAbsolutePath();
    }

    public File getSdFile() {
        return Environment.getExternalStorageDirectory();
    }

    public File[] getSdFiles(String path) {
        return new File(path).listFiles();
    }

    /**
     * read file names from sd dictory
     * extenal prefix file filter
    */
    public List<String> getSdFileNames(String path) {
        List<String> fileNames = new ArrayList<>();
        File[] files = getSdFiles(path);
        for (File f : files) {
            fileNames.add(f.getName());
        }
        return fileNames;
    }

    /**
     * 读取指定文件中的数据，将数据读取为byte[]类型
     * 参数：要读取数据的文件路径
     */
    public byte[] getDataFromFile(String path) {
        FileInputStream fis = null;
        ByteArrayOutputStream bos = null;
        try {
            fis = new FileInputStream(path);
            byte[] b = new byte[1024];
            int num = -1;
            bos = new ByteArrayOutputStream();
            while ((num = fis.read(b)) != -1) {
                bos.write(b, 0, num);
            }
            return bos.toByteArray();
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            if (fis != null) {
                try {
                    fis.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
            if (bos != null) {
                try {
                    bos.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
        return null;
    }

    /**
     * 向指定路径中写入指定数据
     * 1. 设置写人数据要存储到的文件夹的路径
     * 2. 要写入的数据
     * 3. 文件名称
     */
    public void saveFile(String path, byte[] b, String fileName) {
        File file = new File(path);
        if (!file.exists()) {
            file.mkdirs();
        }
        try {
            FileOutputStream fos = new FileOutputStream(path + File.separator + fileName);
            fos.write(b);
            fos.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /*
     * 复制指定文件
     * 1. 要复制的原文件
     * 2. 复制文件的存储路径
     * 3. 复制文件的文件名称
     * */
    public void copyFile(File source, String path, String fileName) {
        try {
            FileInputStream fis = new FileInputStream(source);
            FileOutputStream fos = new FileOutputStream(path + File.separator + fileName);
            byte[] b = new byte[1024];
            int num = -1;
            while ((num = fis.read(b)) != -1) {
                fos.write(b, 0, num);
            }
            fos.close();
            fis.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
