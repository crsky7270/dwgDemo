package com.booway.testSharpes;

import android.opengl.GLES20;

import com.booway.utils.OpenGLUtils;
import com.booway.utils.VaryTools;

import java.nio.FloatBuffer;

/**
 * Created by booway on 2018/8/28.
 */

public class Line extends OpenGLUtils {

    float factor = -0.000f;

    float baseScaleValue = 1.0f;

    float baseOffsetX = 0.0f;

    float baseOffsety = 0.0f;

    private VaryTools varyTools = new VaryTools();

    private FloatBuffer pointVB;

    private int mProgram;

    private int maPositionHandle;

    private final String vertexShaderCode =
            "attribute vec4 vPosition;    \n"
                    + "void main()                  \n"
                    + "{                            \n"
                    + "   gl_Position = vPosition;  \n"
//                    + "   gl_PointSize = 20.0;  \n"
//                    + "   gl_LineWidth = 20.0;  \n"
                    + "}                            \n";

    private final String vertexCameraShaderCode =
            // This matrix member variable provides a hook to manipulate
            // the coordinates of the objects that use this vertex shader
            "uniform mat4 uMVPMatrix;   \n" +

                    "attribute vec4 vPosition;  \n" +
                    "void main(){               \n" +

                    // the matrix must be included as a modifier of gl_Position
                    " gl_Position = uMVPMatrix * vPosition; \n" +

                    "}  \n";

    private final String fragmentShaderCode =
            "precision mediump float;    \n"
                    + "void main()                  \n"
                    + "{                            \n"
                    + "   gl_FragColor = vec4 ( 1, 1, 1, 1 );    \n"
                    + "}                            \n";

    //设置每个顶点的坐标数
    static final int COORDS_PER_VERTEX = 2;
    //设置三角形顶点数组
    static float lineCoords[] = {   //默认按逆时针方向绘制
//            -0.5f, -0.25f, 0,
//            0.5f, -0.25f, 0,
//            0.0f, 0.559016994f, 0
            // 第一个三角形
            0.5f, 0.5f,    // 右上角
            0.5f, -0.5f,   // 右下角
            -0.5f, 0.5f,   // 左上角
            // 第二个三角形
            0.5f, -0.5f,  // 右下角
            -0.5f, -0.5f,  // 左下角
            -0.5f, 0.5f,   // 左上角
    };

    public Line() {

        pointVB = getFloatbuffer(lineCoords);
        CreateShadow();
    }

    public void CreateShadow() {
        // 创建空的OpenGL ES Program
        mProgram = GLES20.glCreateProgram();

        // 编译shader代码
        int vertexShader = loadShader(GLES20.GL_VERTEX_SHADER,
                vertexCameraShaderCode);
        int fragmentShader = loadShader(GLES20.GL_FRAGMENT_SHADER,
                fragmentShaderCode);

        // 将vertex shader添加到program
        GLES20.glAttachShader(mProgram, vertexShader);

        // 将fragment shader添加到program
        GLES20.glAttachShader(mProgram, fragmentShader);

        // 创建可执行的 OpenGL ES program
        GLES20.glLinkProgram(mProgram);

        maPositionHandle = GLES20.glGetAttribLocation(mProgram, "vPosition");
    }

    private int muMVPMatrixHandle;

    public void setCamara(int width, int height) {
        float ratio = (float) width / height;
        varyTools.frustum(-ratio, ratio, -1, 1, 3, 7);
        muMVPMatrixHandle = GLES20.glGetUniformLocation(mProgram, "uMVPMatrix");
        varyTools.setCamera(0, 0, -3, 0f, 0f, 0f, 0f, 1.0f, 0.0f);
    }

    public void draw() {
        GLES20.glUseProgram(mProgram);

        GLES20.glVertexAttribPointer(maPositionHandle, COORDS_PER_VERTEX, GLES20.GL_FLOAT,
                false, 0, pointVB);
        GLES20.glEnableVertexAttribArray(maPositionHandle);

//        long time = SystemClock.uptimeMillis() % 4000L;
//        float angle = 0.090f * ((int) time);

        //varyTools.rotate(angle, 0, 0, 1.0f);
        varyTools.scale(baseScaleValue + factor, baseScaleValue + factor, baseScaleValue + factor);
        varyTools.translate(-baseOffsetX, -baseOffsety, 0f);
        float[] finalMatrix = varyTools.getFinalMatrix();

        GLES20.glUniformMatrix4fv(muMVPMatrixHandle, 1, false, finalMatrix, 0);

//        GLES20.glLineWidth(20);
        //GLES20.glDrawArrays(GLES20.GL_TRIANGLES, 0, 3);

        GLES20.glDrawArrays(GLES20.GL_LINE_LOOP,0,6);
        //GLES20.glDrawArrays(GLES20.GL_LINE_LOOP,3,3);

    }

    public int loadShader(int type, String shaderCode) {

        //创建一个vertex shader类型(GLES20.GL_VERTEX_SHADER)
        //或一个fragment shader类型(GLES20.GL_FRAGMENT_SHADER)
        int shader = GLES20.glCreateShader(type);

        // 将源码添加到shader并编译它
        GLES20.glShaderSource(shader, shaderCode);
        GLES20.glCompileShader(shader);

        return shader;
    }

}
