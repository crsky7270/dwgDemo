#include <jni.h>
#include <string>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "../jni/libdxfrw/libdwgr.h"
#include "../jni/filters/rs_filterinterface.h"
#include "../jni/filters/rs_filterdxfrw.h"

#include <vector>
#include <cmath>
#include <android/log.h>
#include <GLES2/gl2.h>

#include "glm/mat4x4.hpp"
#include "glm/ext.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H
#include "../jni/engine/rs_font.h"
#include <wchar.h>
#include <sstream>
#include <fstream>

using namespace std;

#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR,"Dutil",__VA_ARGS__)

GLuint d_glprogram;
GLint mUMVPMatrixHandle;
GLint maPositionHandle;

#define WIDTH 640
#define HEIGHT 480

int screenWidth;
int screenHeight;

glm::mat4 mvpMatrix;
glm::mat4 projection;
glm::mat4 view;

GLfloat offsetX = 0.0f;
GLfloat offsetY = 0.0f;
GLfloat scale = 1.0f;
GLfloat *lineArrBegin;

const char *fontfile;

RS_Font *font;

float unit = 100;

/*dwg文字换行部分拆分*/
static vector<string> split(string s, char token) {
    istringstream iss(s);
    string word;
    vector<string> vs;
    while (getline(iss, word, token)) {
        vs.emplace_back(word);
    }
    return vs;
}

//获取图元最大位数
static long getUnit() {
    return 0;
}

//NDK获取外置存储卡位置方法
static jstring getExternalStoragePath(JNIEnv *env) {
    jclass envcls = env->FindClass("android/os/Environment");
    jmethodID id = env->GetStaticMethodID(envcls, "getExternalStorageDirectory",
                                          "()Ljava/io/File;");
    jobject fileobj = env->CallStaticObjectMethod(envcls, id, "");
    jclass fileclass = env->GetObjectClass(fileobj);
    jmethodID getPathId = env->GetMethodID(fileclass, "getPath", "()Ljava/lang/String;");
    return (jstring) env->CallObjectMethod(fileobj, getPathId, "");
}

class BoowayDwgContext {
public:
    std::vector<std::pair<RS_Vector, RS_Vector>> lineList;
    std::vector<std::pair<RS_Vector, double> > circleList;
    std::vector<std::pair<RS_Vector, RS_Vector>> arcList;
    std::vector<std::pair<RS_Vector, RS_Vector>> ellipseList;
    std::vector<std::pair<RS_Vector, double >> ellipseExList;
    std::vector<std::vector<std::pair<RS_Vector, double> >> lwLineList;
    std::vector<int> lwLineVertexNum;
    std::vector<std::pair<RS_Vector, std::string>> mTextList;
};

static BoowayDwgContext *context = NULL;

extern "C"
{
JNIEXPORT jboolean JNICALL
Java_com_booway_dwgdemo_BoowayDwgJni_open(JNIEnv *env, jclass type, jstring filename) {

    // TODO
    if (context != NULL)
        return JNI_FALSE;
    context = new BoowayDwgContext();
    const char *fname = NULL;
    fname = env->GetStringUTFChars(filename, 0);
    RS_Graphic g = new RS_Graphic();
    RS_FilterDXFRW *filter = static_cast<RS_FilterDXFRW *>(RS_FilterDXFRW::createFilter());
    bool flag = filter->fileImport(g, fname);
    if (!flag)
        return JNI_FALSE;
    context->lineList = filter->lineList;
    context->circleList = filter->circleList;
    context->arcList = filter->arcList;
    context->ellipseList = filter->ellipseList;
    context->ellipseExList = filter->ellipseExList;
    context->lwLineList = filter->lwLineList;
    context->lwLineVertexNum = filter->lwLineVertexNum;
    context->mTextList = filter->mTextList;

    //获取轻型线的最大坐标
    for (auto const &ilw :context->lwLineList) {
        for (vector<std::pair<RS_Vector, double> >::const_iterator it = ilw.cbegin();
             it != ilw.cend(); it++) {
            if (max(it->first.x, it->first.y) > unit) {
                unit = max(abs(it->first.x), abs(it->first.y));
            }
        }
    }
    //取dwg坐标缩放至OpenGL比例
    unit = unit * 2;

    font = new RS_Font(fontfile, unit);
    font->initVectexList(context->mTextList);
    delete fname;
    delete filter;
    return JNI_TRUE;
}

JNIEXPORT jboolean JNICALL
Java_com_booway_dwgdemo_BoowayDwgJni_createRenderer(JNIEnv *env, jclass type, jint width,
                                                    jint height) {
    if (context->lineList.size() == 0)
        return JNI_FALSE;
    //render
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    float angleUnit = 0.01f;
    screenWidth = width;
    screenHeight = height;

    std::vector<std::pair<RS_Vector, RS_Vector>> lineVectors = context->lineList;
    std::vector<std::pair<RS_Vector, double> > circleVectors = context->circleList;
    std::vector<std::pair<RS_Vector, RS_Vector>> arcVectors = context->arcList;
    std::vector<std::pair<RS_Vector, RS_Vector>> ellipseVectors = context->ellipseList;
    std::vector<std::pair<RS_Vector, double >> ellipseExVectors = context->ellipseExList;
    std::vector<std::vector<std::pair<RS_Vector, double> >> lwlineVectors = context->lwLineList;
    std::vector<int> lwlineVertexNum = context->lwLineVertexNum;
    std::vector<std::pair<RS_Vector, std::string>> mTextVectors = context->mTextList;

    projection = glm::ortho(-1.0f, 1.0f, -(float) height / width, (float) height / width, 5.0f,
                            7.0f);

    view = glm::lookAt(glm::vec3(0.0f, 0.0f, 6.0f),
                       glm::vec3(0.0f, 0.1f, 0.0f),
                       glm::vec3(0.0f, 1.0f, 0.0f));

    glViewport(0, 0, width, height);

    /*将模型矩阵转换为投影矩阵，保持正交投影*/
    mvpMatrix = projection * view/* * module*/;
    glm::mat4 trans;
    /*矩阵变化-平移*/
    trans = glm::translate(trans, glm::vec3(0.0f, 0.0f, 0.0f));
//    /*矩阵变化-放大*/
//    trans = glm::scale(trans, glm::vec3(1.5f, 1.5f, 1.5f));
    mvpMatrix = mvpMatrix * trans;
    float *mvp = glm::value_ptr(mvpMatrix);
    int i = 0;

    glUseProgram(d_glprogram);
//****************************开始测试简单元素绘制*************************

//    glUniformMatrix4fv(mUMVPMatrixHandle, 1, GL_FALSE, mvp);
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, mVertexArray);
//    glEnableVertexAttribArray(maPositionHandle);
//    glDrawArrays(GL_TRIANGLE_FAN, 0, 3);

//****************************结束测试简单元素绘制*************************

//****************************开始画线过程****************************

    lineArrBegin = new float[lineVectors.size() * 6];
    for (std::vector<std::pair<RS_Vector, RS_Vector>>::const_iterator i1 = lineVectors.cbegin();
         i1 != lineVectors.cend(); i1++) {
        *(lineArrBegin + i * 6) = (float) (i1->first.x / unit);
        *(lineArrBegin + 1 + i * 6) = (float) (i1->first.y / unit);
        *(lineArrBegin + 2 + i * 6) = 0.0f;
        *(lineArrBegin + 3 + i * 6) = (float) (i1->second.x / unit);
        *(lineArrBegin + 4 + i * 6) = (float) (i1->second.y / unit);
        *(lineArrBegin + 5 + i * 6) = 0.0f;
        i++;
    }
    glUniformMatrix4fv(mUMVPMatrixHandle, 1, GL_FALSE, mvp);
    //开始绘制线段
    glVertexAttribPointer(maPositionHandle, 3, GL_FLOAT, GL_FALSE, 0, lineArrBegin);
    glEnableVertexAttribArray(maPositionHandle);
    glDrawArrays(GL_LINES, 0, lineVectors.size() * 2);
    delete[]lineArrBegin;

//****************************画线过程结束****************************



//****************************开始曲线过程****************************

    for (std::vector<std::pair<RS_Vector, RS_Vector>>::const_iterator ia = arcVectors.cbegin();
         ia != arcVectors.cend(); ia++) {
        vector<float> arcPointVector;
        float centerX = ia->first.x / unit;
        float centerY = ia->first.y / unit;
        float r = ia->second.x / unit;
        float startAngle = ia->second.y;
        float endAngle = ia->second.z;
        GLfloat *arcArrBegin;
        i = 0;
        if (endAngle > startAngle) {
            while (startAngle <= endAngle) {
                arcPointVector.emplace_back(centerX + r * cos(startAngle));
                arcPointVector.emplace_back(centerY + r * sin(startAngle));
                arcPointVector.emplace_back(0.0f);
                startAngle += angleUnit;
            }

            if (arcPointVector.size() > 0) {
                arcArrBegin = new GLfloat[arcPointVector.size()];
                for (auto const &a:arcPointVector) {
                    *(arcArrBegin + i) = a;
                    i++;
                }
                glUniformMatrix4fv(mUMVPMatrixHandle, 1, GL_FALSE, mvp);
                glVertexAttribPointer(maPositionHandle, 3, GL_FLOAT, GL_FALSE, 0, arcArrBegin);
                glEnableVertexAttribArray(maPositionHandle);
                glDrawArrays(GL_LINE_STRIP, 0, i / 3);
                delete[]arcArrBegin;
            }
        } else {
            /*处理查过了2PI 的情况，先从起始点到2PI，
             * 在从2PI（0）到目标角度  4.00,1.00,2,5..*/
            while (startAngle <= M_PI * 2) {
                arcPointVector.emplace_back(centerX + r * cos(startAngle));
                arcPointVector.emplace_back(centerY + r * sin(startAngle));
                arcPointVector.emplace_back(0.0f);
                startAngle += angleUnit;
            }
            while ((startAngle - M_PI * 2) <= endAngle) {
                arcPointVector.emplace_back(centerX + r * cos(startAngle));
                arcPointVector.emplace_back(centerY + r * sin(startAngle));
                arcPointVector.emplace_back(0.0f);
                startAngle += angleUnit;
            }
            if (arcPointVector.size() > 0) {
                arcArrBegin = new float[arcPointVector.size()];
                for (auto const &a:arcPointVector) {
                    *(arcArrBegin + i) = a;
                    i++;
                }
                glUniformMatrix4fv(mUMVPMatrixHandle, 1, GL_FALSE, mvp);
                glVertexAttribPointer(maPositionHandle, 3, GL_FLOAT, GL_FALSE, 0, arcArrBegin);
                glEnableVertexAttribArray(maPositionHandle);
                glDrawArrays(GL_LINE_STRIP, 0, i / 3);
                delete[]arcArrBegin;
            }
        }
    }

//****************************结束曲线过程****************************

//    i = 0;
//****************************开始椭圆过程****************************

    /*椭圆测试代码，备份以供参考
    while ((angleLength) <= staangle * 3 / 4) {
        ellipsePointVector.emplace_back(centerX + abs(offsetCenterX) * cos(angleLength));
        ellipsePointVector.emplace_back(
                centerY + ratio * abs(offsetCenterX) * sin(angleLength));
        ellipsePointVector.emplace_back(0.0f);
        angleLength += angleUnit;
    }

    if (ellipsePointVector.size() > 0) {
        float *ellipseArrBegin = new float[ellipsePointVector.size()];
        for (auto const &a:ellipsePointVector) {
            *(ellipseArrBegin + i) = a;
            i++;
        }
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, ellipseArrBegin);
        glEnableVertexAttribArray(0);
        glDrawArrays(GL_LINE_STRIP, 0, i / 3 -1);
        delete[]ellipseArrBegin;
    }
    glDrawArrays(GL_LINE_STRIP, 0, 0);
    */

    int maxEllipasePoint = 50;
    for (std::vector<std::pair<RS_Vector, RS_Vector>>::const_iterator ie = ellipseVectors.cbegin();
         ie != ellipseVectors.cend(); ie++) {
        float centerX = ie->first.x / unit;
        float centerY = ie->first.y / unit;
        float offsetCenterX = ie->second.x / unit;
        float offsetCenterY = ie->second.y / unit;
        float staangle = ellipseExVectors[i].first.x;
        float endangle = ellipseExVectors[i].first.y;
        float ratio = ellipseExVectors[i].second;
        GLfloat *ellipseArrBegin;
        /*椭圆长轴带扩展*/
        float majorAxis = offsetCenterX >= offsetCenterY ? offsetCenterX : offsetCenterY;
        /*由于时间原因以下算法只适用于 X>Y 的扁椭圆，如果 y>x 需要判断后处理，
         * 这种情况需要修改是因为椭圆的长轴会导致椭圆形状的变化*/
        if (staangle == 0 && endangle == 0) {
            ellipseArrBegin = new GLfloat[(maxEllipasePoint + 1) * 3];
            for (int z = 0; z < maxEllipasePoint + 1; z++) {
                float angleInRadians =
                        ((float) z / (float) maxEllipasePoint) * ((float) M_PI * 2.0f);
                if (z != 0) {
                    *(ellipseArrBegin + 0 + (z - 1) * 3) =
                            centerX + abs(offsetCenterX) * cos(angleInRadians);
                    *(ellipseArrBegin + 1 + (z - 1) * 3) =
                            centerY + ratio * abs(offsetCenterX) * sin(angleInRadians);
                    *(ellipseArrBegin + 2 + (z - 1) * 3) = 0.0f;
                }
            }
            glUniformMatrix4fv(mUMVPMatrixHandle, 1, GL_FALSE, mvp);
            glVertexAttribPointer(maPositionHandle, 3, GL_FLOAT, GL_FALSE, 0, ellipseArrBegin);
            glEnableVertexAttribArray(maPositionHandle);
            glDrawArrays(GL_LINE_LOOP, 0, maxEllipasePoint);
            delete[]ellipseArrBegin;
        } else if (endangle > M_PI * 2) {
            vector<float> ellipsePointVector;
            float angleLength = endangle - staangle;// - M_PI_2;

            /*由于时间原因只处理不超过2PI 的情况*/
            for (int z = 0; z < maxEllipasePoint + 1; z++) {
                float angleInRadians =
                        ((float) z / (float) maxEllipasePoint) * ((float) M_PI * 2.0f);
                if (angleInRadians >= angleLength && angleInRadians <= (angleLength + endangle -
                                                                        staangle)) {
                    ellipsePointVector.emplace_back(
                            centerX + abs(offsetCenterX) * cos(angleInRadians));
                    ellipsePointVector.emplace_back(
                            centerY + ratio * abs(offsetCenterX) * sin(angleInRadians));
                    ellipsePointVector.emplace_back(0.0f);
                }

            }
            int x = 0;
            ellipseArrBegin = new GLfloat[ellipsePointVector.size()];
            for (auto const &e:ellipsePointVector) {
                *(ellipseArrBegin + x) = e;
                x++;
            }
            glUniformMatrix4fv(mUMVPMatrixHandle, 1, GL_FALSE, mvp);
            glVertexAttribPointer(maPositionHandle, 3, GL_FLOAT, GL_FALSE, 0, ellipseArrBegin);
            glEnableVertexAttribArray(maPositionHandle);
            glDrawArrays(GL_LINE_STRIP, 0, x / 3);
            delete[]ellipseArrBegin;
        }
        i++;
    }

//****************************结束椭圆过程****************************
//    i = 0;

//****************************开始轻型线过程**************************

    int i1 = 0;
    for (auto const &ilw :lwlineVectors) {
        GLfloat *lwLineArr = new GLfloat[ilw.size() * 3];
        for (vector<std::pair<RS_Vector, double> >::const_iterator it = ilw.cbegin();
             it != ilw.cend(); it++) {
            *(lwLineArr + 0 + i1 * 3) = it->first.x / unit;
            *(lwLineArr + 1 + i1 * 3) = it->first.y / unit;
            *(lwLineArr + 2 + i1 * 3) = 0.0f;
            i1++;
        }
        glUniformMatrix4fv(mUMVPMatrixHandle, 1, GL_FALSE, mvp);
        glVertexAttribPointer(maPositionHandle, 3, GL_FLOAT, GL_FALSE, 0, lwLineArr);
        glEnableVertexAttribArray(maPositionHandle);
        glDrawArrays(GL_LINE_STRIP, 0, i1);
        delete[]lwLineArr;
        i1 = 0;
    }
//
//****************************结束轻型线过程**************************


//****************************开始画点过程****************************


//****************************结束画点过程****************************

//****************************开始画字过程****************************

//    RS_Font font(fontfile);// = new RS_Font(&fontfile);
//    font.initVectexList(mTextVectors);
//    font.setMatrix(mUMVPMatrixHandle, maPositionHandle, mvp);
//    font.draw();

    font->initFreeType();
    font->setMatrix(mUMVPMatrixHandle, maPositionHandle, mvp);
    font->draw();


//****************************结束画字过程****************************


    return JNI_TRUE;

}

//****************************测试代码开始****************************
//    for (auto &v:vectors) {
//        vertexs[i] = v.first.x;
//        vertexs[i + 1] = v.first.y;
//        vertexs[i + 2] = 0.0f;
//        i = i + 3;
//
//    }
//    int count = vectors.size() * 3;
////    GLfloat vertexs2 []= new GLfloat[100];
//
//    i = 0;
//    const int num = vectors.size();
//    GLfloat vertexs1[10000];
//    for (std::vector<std::pair<RS_Vector, double> >::const_iterator iter = vectors.cbegin();
//         iter != vectors.cend(); iter++) {
//        vertexs1[i] = (float) (iter->first.x / 1000);
//        vertexs1[i + 1] = (float) (iter->first.y /
//                                   1000);//1.0f - 2.0f * (iter->first.y / screenHeight);
//        vertexs1[i + 2] = 0.0f;
//        i = i + 3;
//    }
//****************************测试代码结束****************************

JNIEXPORT jboolean JNICALL
Java_com_booway_dwgdemo_BoowayDwgJni_renderFrame(JNIEnv *env, jclass type) {

    if (context->lineList.size() == 0)
        return JNI_FALSE;

    //render
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    float angleUnit = 0.01f;
    std::vector<std::pair<RS_Vector, RS_Vector>> lineVectors = context->lineList;
    std::vector<std::pair<RS_Vector, double> > circleVectors = context->circleList;
    std::vector<std::pair<RS_Vector, RS_Vector>> arcVectors = context->arcList;
    std::vector<std::pair<RS_Vector, RS_Vector>> ellipseVectors = context->ellipseList;
    std::vector<std::pair<RS_Vector, double >> ellipseExVectors = context->ellipseExList;
    std::vector<std::vector<std::pair<RS_Vector, double> >> lwlineVectors = context->lwLineList;
    std::vector<int> lwlineVertexNum = context->lwLineVertexNum;
    std::vector<std::pair<RS_Vector, std::string>> mTextVectors = context->mTextList;

//    projection = glm::ortho(-1.0f, 1.0f, -(float) screenHeight / screenWidth,
//                            (float) screenHeight / screenWidth, 5.0f,
//                            7.0f);
//
//    view = glm::lookAt(glm::vec3(0.0f, 0.0f, 6.0f),
//                       glm::vec3(0.0f, 0.1f, 0.0f),
//                       glm::vec3(0.0f, 1.0f, 0.0f));

//    glViewport(0, 0, screenWidth, screenHeight);

    /*将模型矩阵转换为投影矩阵，保持正交投影*/
//    glm::mat4 mvpMatrix = projection * view/* * module*/;
    glm::mat4 trans;
//    /*矩阵变化-平移*/
    trans = glm::translate(trans, glm::vec3(offsetX, offsetY, 0.0f));
    /*矩阵变化-放大*/
    trans = glm::scale(trans, glm::vec3(scale, scale, scale));
    mvpMatrix = mvpMatrix * trans;
    float *mvp = glm::value_ptr(mvpMatrix);
    int i = 0;
    glUseProgram(d_glprogram);

//****************************开始测试简单元素绘制*************************

//    glUniformMatrix4fv(mUMVPMatrixHandle, 1, GL_FALSE, mvp);
//    glVertexAttribPointer(maPositionHandle, 3, GL_FLOAT, GL_FALSE, 0, mVertexArray);
//    glEnableVertexAttribArray(maPositionHandle);
//    glDrawArrays(GL_TRIANGLE_FAN, 0, 3);

//****************************结束测试简单元素绘制*************************


//****************************开始画线过程****************************

    GLfloat *lineArrBegin = new GLfloat[lineVectors.size() * 6];
    for (std::vector<std::pair<RS_Vector, RS_Vector>>::const_iterator i1 = lineVectors.cbegin();
         i1 != lineVectors.cend(); i1++) {
        *(lineArrBegin + i * 6) = (float) (i1->first.x / unit);
        *(lineArrBegin + 1 + i * 6) = (float) (i1->first.y / unit);
        *(lineArrBegin + 2 + i * 6) = 0.0f;
        *(lineArrBegin + 3 + i * 6) = (float) (i1->second.x / unit);
        *(lineArrBegin + 4 + i * 6) = (float) (i1->second.y / unit);
        *(lineArrBegin + 5 + i * 6) = 0.0f;
        i++;
    }
    glUniformMatrix4fv(mUMVPMatrixHandle, 1, GL_FALSE, mvp);
    //开始绘制线段
    glVertexAttribPointer(maPositionHandle, 3, GL_FLOAT, GL_FALSE, 0, lineArrBegin);
    glEnableVertexAttribArray(maPositionHandle);
    glDrawArrays(GL_LINES, 0, lineVectors.size() * 2);
    delete[]lineArrBegin;

//****************************画线过程结束****************************

//****************************开始圆过程****************************

    for (std::vector<std::pair<RS_Vector, double >>::const_iterator ic = circleVectors.cbegin();
         ic != circleVectors.cend(); ic++) {
        int maxCirclePoint = 50;
        GLfloat *circleArrBegin = new GLfloat[(maxCirclePoint + 1) * 3];

        float centerX = ic->first.x / unit;
        float centerY = ic->first.y / unit;
        float r = ic->second / unit;

        for (int y = 0; y < maxCirclePoint + 1; y++) {
            float angleInRadians = ((float) y / (float) maxCirclePoint) * ((float) M_PI * 2.0f);
            if (y != 0) {
                *(circleArrBegin + 0 + (y - 1) * 3) = centerX + r * cos(angleInRadians);
                *(circleArrBegin + 1 + (y - 1) * 3) = centerY + r * sin(angleInRadians);
                *(circleArrBegin + 2 + (y - 1) * 3) = 0.0f;
            }
        }
        glUniformMatrix4fv(mUMVPMatrixHandle, 1, GL_FALSE, mvp);
        glVertexAttribPointer(maPositionHandle, 3, GL_FLOAT, GL_FALSE, 0, circleArrBegin);
        glEnableVertexAttribArray(maPositionHandle);
        glDrawArrays(GL_LINE_LOOP, 0, maxCirclePoint);
    }

//****************************结束圆过程****************************

//****************************开始曲线过程****************************

    for (std::vector<std::pair<RS_Vector, RS_Vector>>::const_iterator ia = arcVectors.cbegin();
         ia != arcVectors.cend(); ia++) {
        vector<float> arcPointVector;
        float centerX = ia->first.x / unit;
        float centerY = ia->first.y / unit;
        float r = ia->second.x / unit;
        float startAngle = ia->second.y;
        float endAngle = ia->second.z;
        GLfloat *arcArrBegin;
        i = 0;
        if (endAngle > startAngle) {
            while (startAngle <= endAngle) {
                arcPointVector.emplace_back(centerX + r * cos(startAngle));
                arcPointVector.emplace_back(centerY + r * sin(startAngle));
                arcPointVector.emplace_back(0.0f);
                startAngle += angleUnit;
            }

            if (arcPointVector.size() > 0) {
                arcArrBegin = new GLfloat[arcPointVector.size()];
                for (auto const &a:arcPointVector) {
                    *(arcArrBegin + i) = a;
                    i++;
                }
                glUniformMatrix4fv(mUMVPMatrixHandle, 1, GL_FALSE, mvp);
                glVertexAttribPointer(maPositionHandle, 3, GL_FLOAT, GL_FALSE, 0, arcArrBegin);
                glEnableVertexAttribArray(maPositionHandle);
                glDrawArrays(GL_LINE_STRIP, 0, i / 3);
                delete[]arcArrBegin;
            }
        } else {
            /*处理查过了2PI 的情况，先从起始点到2PI，
             * 在从2PI（0）到目标角度  4.00,1.00,2,5..*/
            while (startAngle <= M_PI * 2) {
                arcPointVector.emplace_back(centerX + r * cos(startAngle));
                arcPointVector.emplace_back(centerY + r * sin(startAngle));
                arcPointVector.emplace_back(0.0f);
                startAngle += angleUnit;
            }
            while ((startAngle - M_PI * 2) <= endAngle) {
                arcPointVector.emplace_back(centerX + r * cos(startAngle));
                arcPointVector.emplace_back(centerY + r * sin(startAngle));
                arcPointVector.emplace_back(0.0f);
                startAngle += angleUnit;
            }
            if (arcPointVector.size() > 0) {
                arcArrBegin = new GLfloat[arcPointVector.size()];
                for (auto const &a:arcPointVector) {
                    *(arcArrBegin + i) = a;
                    i++;
                }
                glUniformMatrix4fv(mUMVPMatrixHandle, 1, GL_FALSE, mvp);
                glVertexAttribPointer(maPositionHandle, 3, GL_FLOAT, GL_FALSE, 0, arcArrBegin);
                glEnableVertexAttribArray(maPositionHandle);
                glDrawArrays(GL_LINE_STRIP, 0, i / 3);
                delete[]arcArrBegin;
            }
        }
    }

//****************************结束曲线过程****************************

//    i = 0;
//****************************开始椭圆过程****************************


    /*椭圆测试代码，备份以供参考
    while ((angleLength) <= staangle * 3 / 4) {
        ellipsePointVector.emplace_back(centerX + abs(offsetCenterX) * cos(angleLength));
        ellipsePointVector.emplace_back(
                centerY + ratio * abs(offsetCenterX) * sin(angleLength));
        ellipsePointVector.emplace_back(0.0f);
        angleLength += angleUnit;
    }

    if (ellipsePointVector.size() > 0) {
        float *ellipseArrBegin = new float[ellipsePointVector.size()];
        for (auto const &a:ellipsePointVector) {
            *(ellipseArrBegin + i) = a;
            i++;
        }
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, ellipseArrBegin);
        glEnableVertexAttribArray(0);
        glDrawArrays(GL_LINE_STRIP, 0, i / 3 -1);
        delete[]ellipseArrBegin;
    }
    glDrawArrays(GL_LINE_STRIP, 0, 0);
    */

    int maxEllipasePoint = 50;
    for (std::vector<std::pair<RS_Vector, RS_Vector>>::const_iterator ie = ellipseVectors.cbegin();
         ie != ellipseVectors.cend(); ie++) {
        float centerX = ie->first.x / unit;
        float centerY = ie->first.y / unit;
        float offsetCenterX = ie->second.x / unit;
        float offsetCenterY = ie->second.y / unit;
        float staangle = ellipseExVectors[i].first.x;
        float endangle = ellipseExVectors[i].first.y;
        float ratio = ellipseExVectors[i].second;
        GLfloat *ellipseArrBegin;
        /*椭圆长轴带扩展*/
        float majorAxis = offsetCenterX >= offsetCenterY ? offsetCenterX : offsetCenterY;
        /*由于时间原因以下算法只适用于 X>Y 的扁椭圆，如果 y>x 需要判断后处理，
         * 这种情况需要修改是因为椭圆的长轴会导致椭圆形状的变化*/
        if (staangle == 0 && endangle == 0) {
            ellipseArrBegin = new GLfloat[(maxEllipasePoint + 1) * 3];
            for (int z = 0; z < maxEllipasePoint + 1; z++) {
                float angleInRadians =
                        ((float) z / (float) maxEllipasePoint) * ((float) M_PI * 2.0f);
                if (z != 0) {
                    *(ellipseArrBegin + 0 + (z - 1) * 3) =
                            centerX + abs(offsetCenterX) * cos(angleInRadians);
                    *(ellipseArrBegin + 1 + (z - 1) * 3) =
                            centerY + ratio * abs(offsetCenterX) * sin(angleInRadians);
                    *(ellipseArrBegin + 2 + (z - 1) * 3) = 0.0f;
                }
            }
            glUniformMatrix4fv(mUMVPMatrixHandle, 1, GL_FALSE, mvp);
            glVertexAttribPointer(maPositionHandle, 3, GL_FLOAT, GL_FALSE, 0, ellipseArrBegin);
            glEnableVertexAttribArray(maPositionHandle);
            glDrawArrays(GL_LINE_LOOP, 0, maxEllipasePoint);
            delete[]ellipseArrBegin;
        } else if (endangle > M_PI * 2) {
            vector<float> ellipsePointVector;
            float angleLength = endangle - staangle;// - M_PI_2;

            /*由于时间原因只处理不超过2PI 的情况*/
            for (int z = 0; z < maxEllipasePoint + 1; z++) {
                float angleInRadians =
                        ((float) z / (float) maxEllipasePoint) * ((float) M_PI * 2.0f);
                if (angleInRadians >= angleLength && angleInRadians <= (angleLength + endangle -
                                                                        staangle)) {
                    ellipsePointVector.emplace_back(
                            centerX + abs(offsetCenterX) * cos(angleInRadians));
                    ellipsePointVector.emplace_back(
                            centerY + ratio * abs(offsetCenterX) * sin(angleInRadians));
                    ellipsePointVector.emplace_back(0.0f);
                }
            }
            int x = 0;
            ellipseArrBegin = new GLfloat[ellipsePointVector.size()];
            for (auto const &e:ellipsePointVector) {
                *(ellipseArrBegin + x) = e;
                x++;
            }
            glUniformMatrix4fv(mUMVPMatrixHandle, 1, GL_FALSE, mvp);
            glVertexAttribPointer(maPositionHandle, 3, GL_FLOAT, GL_FALSE, 0, ellipseArrBegin);
            glEnableVertexAttribArray(maPositionHandle);
            glDrawArrays(GL_LINE_STRIP, 0, x / 3);
            delete[]ellipseArrBegin;
        }
        i++;
    }

//****************************结束椭圆过程****************************
//    i = 0;

//****************************开始轻型线过程**************************

    int i1 = 0;
    for (auto const &ilw :lwlineVectors) {
        GLfloat *lwLineArr = new GLfloat[ilw.size() * 3];
        for (vector<std::pair<RS_Vector, double> >::const_iterator it = ilw.cbegin();
             it != ilw.cend(); it++) {
            *(lwLineArr + 0 + i1 * 3) = it->first.x / unit;
            *(lwLineArr + 1 + i1 * 3) = it->first.y / unit;
            *(lwLineArr + 2 + i1 * 3) = 0.0f;
            i1++;
        }
        glUniformMatrix4fv(mUMVPMatrixHandle, 1, GL_FALSE, mvp);
        glVertexAttribPointer(maPositionHandle, 3, GL_FLOAT, GL_FALSE, 0, lwLineArr);
        glEnableVertexAttribArray(maPositionHandle);
        glDrawArrays(GL_LINE_STRIP, 0, i1);
        delete[]lwLineArr;
        i1 = 0;
    }

//****************************结束轻型线过程**************************


//****************************开始画点过程****************************


//****************************结束画点过程****************************

//****************************开始画字过程****************************

    font->initFreeType();
    font->setMatrix(mUMVPMatrixHandle, maPositionHandle, mvp);
    font->draw();

//****************************结束画字过程****************************


    return JNI_TRUE;
}

JNIEXPORT jboolean JNICALL
Java_com_booway_dwgdemo_BoowayDwgJni_close(JNIEnv *env, jclass type) {

    // TODO
    return JNI_TRUE;
}

JNIEXPORT jboolean JNICALL
Java_com_booway_dwgdemo_BoowayDwgJni_destroyRenderer(JNIEnv *env, jclass type) {

    // TODO
    if (context == NULL) {
        return JNI_FALSE;
    } else {
        delete context;
        context = NULL;
    }
    delete fontfile;
    unit = 100;
    fontfile = NULL;
//    delete lineArrBegin;
//    lineArrBegin = NULL;
    delete font;
    font = NULL;
    scale = 1.0f;
    offsetX = 0.0f;
    offsetY = 0.0f;
    return JNI_TRUE;
}

JNIEXPORT jboolean JNICALL
Java_com_booway_dwgdemo_BoowayDwgJni_finit(JNIEnv *env, jclass type) {

    // TODO
    return JNI_TRUE;
}

}
extern "C"
JNIEXPORT jboolean JNICALL
Java_com_booway_dwgdemo_BoowayDwgJni_init(JNIEnv *env, jclass type, jstring fontFileName) {

    // TODO
    // TODO
    GLuint glProgram;
    GLuint vertexShader;
    GLuint fragmentShader;

    fontfile = env->GetStringUTFChars(fontFileName, 0);

/*shader code
    const char *shader_vertex =
            "uniform mediump mat4 MODELVIEWPROJECTIONMATRIX;\n"
                    "attribute vec4 POSITION;\n"
                    "void main(){\n"
                    "  gl_Position = POSITION;\n"
                    "}";
     This matrix member variable provides a hook to manipulate
     the coordinates of the objects that use this vertex shader
            "uniform mat4 uMVPMatrix;   \n"
            "attribute vec4 vPosition;  \n"
            "void main(){               \n"
            " gl_Position = uMVPMatrix * vPosition; \n"
            "}  \n";
    const char *shader_vertex = "uniform mat4 uMVPMatrix;   \n"
            "attribute vec4 vPosition;  \n"
            "void main(){               \n"
            " gl_Position = uMVPMatrix * vPosition; \n"
            "}  \n";
*/
    const char *shader_vertex = "attribute vec4 aPosition;\n"
            "attribute vec4 aColor;\n"
            "varying vec4 vColor;\n"
            "uniform mat4 uMVPMatrix;\n"
            "void main()\n"
            "{\n"
            "gl_Position = uMVPMatrix * aPosition;\n"
            "gl_PointSize = 1.0;  \n"
            "vColor = aColor;\n"
            "}";

    const char *shader_fragment = "precision mediump float;\n"
            "void main(){\n"
            "   gl_FragColor = vec4(1,1,1,1);\n"
            "}";

    glProgram = glCreateProgram();

    if (glProgram == 0) {
        ALOGE("init glProgram error!");
    }

    d_glprogram = glProgram;
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    //vertexShader
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &shader_vertex, NULL);

    //fragmentShader
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &shader_fragment, NULL);
    glCompileShader(vertexShader);
    glCompileShader(fragmentShader);

    glAttachShader(glProgram, vertexShader);
    glAttachShader(glProgram, fragmentShader);

    glLinkProgram(glProgram);

    mUMVPMatrixHandle = glGetUniformLocation(d_glprogram, "uMVPMatrix");
    maPositionHandle = glGetAttribLocation(d_glprogram, "aPosition");

    return JNI_TRUE;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_booway_dwgdemo_BoowayDwgJni_change(JNIEnv *env, jclass type, jint width, jint height) {

    // TODO
    screenWidth = width;
    screenHeight = height;

    return JNI_TRUE;
}extern "C"
JNIEXPORT jboolean JNICALL
Java_com_booway_dwgdemo_BoowayDwgJni_viewTranslate(JNIEnv *env, jclass type,
                                                   jfloat xAxis,
                                                   jfloat yAxis) {
//    if (xAxis == 0 && yAxis == 0)
//        return false;
    offsetX = xAxis / screenWidth * 2;
    offsetY = -yAxis / screenHeight * 2;

    return JNI_TRUE;

}extern "C"
JNIEXPORT jboolean JNICALL
Java_com_booway_dwgdemo_BoowayDwgJni_viewScale(JNIEnv *env, jclass type, jfloat sCoef) {

    // TODO
    scale = sCoef;
    return JNI_TRUE;
}