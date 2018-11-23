package com.booway.utils;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.IntBuffer;

/**
 * Created by booway on 2018/8/23.
 */

public abstract class OpenGLUtils {

    public FloatBuffer getFloatbuffer(float[] ver) {
        ByteBuffer vbb = ByteBuffer.allocateDirect(ver.length * 4);
        vbb.order(ByteOrder.nativeOrder());
        FloatBuffer buffer = vbb.asFloatBuffer();
        buffer.put(ver);
        buffer.position(0);
        return buffer;
    }

    public ByteBuffer getByteBuffer(byte[] indices) {
        //创建三角形构造索引数据缓冲
        ByteBuffer indexBuffer = ByteBuffer.allocateDirect(indices.length);
        indexBuffer.put(indices);
        indexBuffer.position(0);
        return indexBuffer;
    }

    public IntBuffer getIntBuffer(int[] ver) {
        //创建顶点坐标数据缓存，由于不同平台字节顺序不同，数据单元不是字节的
        // 一定要经过ByteBuffer转换，关键是通过ByteOrder设置nativeOrder()
        //一个整数四个字节，根据最新分配的内存块来创建一个有向的字节缓冲
        ByteBuffer vbb = ByteBuffer.allocateDirect(ver.length * 4);
        vbb.order(ByteOrder.nativeOrder());//设置这个字节缓冲的字节顺序为本地平台的字节顺序
        IntBuffer intBuffer = vbb.asIntBuffer();//转换为int型缓冲
        intBuffer.put(ver);//向缓冲区中放入顶点坐标数据
        intBuffer.position(0);//设置缓冲区的起始位置
        return intBuffer;
    }


}