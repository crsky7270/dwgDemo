//
// Created by booway on 2018/9/19.
//
#include <ft2build.h>
#include FT_FREETYPE_H
#include "rs_vector.h"
#include <wchar.h>
#include <cmath>
#include <sstream>
#include <vector>
#include <GLES2/gl2.h>

#define WIDTH 800
#define HEIGHT 1600

#ifndef DWGDEMO_RS_FONT_H
#define DWGDEMO_RS_FONT_H

using namespace std;

class RS_Font {
private:
    FT_Library library;
    FT_Face face;
    FT_GlyphSlot slot;
    FT_Matrix matrix;
    FT_Vector pen;
    FT_Error error;

    float unit = 100;

    //    char *text;
    double angle;
    int target_height;
    int n;//, num_chars;
    int mUMVPMatrixHandle = 0;
    int maPositionHandle = 0;
    float *mvp;
    wchar_t *chinese_str = (wchar_t *) L"博微booway123123abc";
    const char *fontfile;
    std::vector<std::pair<RS_Vector, std::string>> mTextVectors;

    unsigned char image[HEIGHT][WIDTH];

    void draw_bitmap(FT_Bitmap *bitmap,
                     FT_Int x,
                     FT_Int y);

    void mTextVectorToPointVertexs();

public:
    RS_Font() = default;

    RS_Font(const char *fontPath, float unit);

    ~RS_Font() = default;

    void initFreeType();

    void draw();

    void initVectexList(std::vector<std::pair<RS_Vector, std::string>> vectors);

    void setMatrix(int handle, int pos, float *matrix);

    vector<pair<vector<pair<double, double>>, RS_Vector>> allPoints;
    int pointCount;


};


#endif //DWGDEMO_RS_FONT_H
