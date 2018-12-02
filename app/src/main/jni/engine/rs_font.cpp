//
// Created by booway on 2018/9/19.
//

#include "rs_font.h"

void RS_Font::initFreeType() {
    error = FT_Init_FreeType(&library);              /* initialize library */

    error = FT_New_Face(library, fontfile, 0, &face);/* create face object */

    error = FT_Select_Charmap(face, ft_encoding_unicode);

//    error = FT_Set_Char_Size(face, 6 * 64, 0, 300, 300);

    angle = -90 / 180. * 3.14159;  //
    target_height = HEIGHT;


    matrix.xx = (FT_Fixed) (cos(angle) * 0x10000L);
    matrix.xy = (FT_Fixed) (-sin(angle) * 0x10000L);
    matrix.yx = (FT_Fixed) (sin(angle) * 0x10000L);
    matrix.yy = (FT_Fixed) (cos(angle) * 0x10000L);

}

void RS_Font::draw() {

    if (pointCount == 0)
        mTextVectorToPointVertexs();

    int z = 0;
    float *pointArr = new float[pointCount * 3];
    for (auto const &a:allPoints) {
        for (auto const &v:a.first) {
            *(pointArr + z * 3) = ((v.first)) / 4000 + a.second.x / unit;
            if (a.second.z > 4) {
                *(pointArr + 1 + z * 3) = ((v.second)) / 4000 + (a.second.y - a.second.z) / unit;
            } else {
                *(pointArr + 1 + z * 3) = ((v.second)) / 4000 + a.second.y / unit;
            }

            *(pointArr + 2 + z * 3) = 0.0f;
            z++;
        }
    }

    glUniformMatrix4fv(mUMVPMatrixHandle, 1, GL_FALSE, mvp);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, pointArr);
    glEnableVertexAttribArray(maPositionHandle);
    glDrawArrays(GL_POINTS, 0, pointCount);
    delete[]pointArr;

    FT_Done_Face(face);
    FT_Done_FreeType(library);

}

void RS_Font::draw_bitmap(FT_Bitmap *bitmap,
                          FT_Int x,
                          FT_Int y) {
    FT_Int ii, jj, p, q;
    FT_Int x_max = x + bitmap->width;
    FT_Int y_max = y + bitmap->rows;


    /* for simplicity, we assume that `bitmap->pixel_mode' */
    /* is `FT_PIXEL_MODE_GRAY' (ii.e., not a bitmap font)   */

    for (ii = x, p = 0; ii < x_max; ii++, p++) {
        for (jj = y, q = 0; jj < y_max; jj++, q++) {
            if (ii < 0 || jj < 0 ||
                ii >= WIDTH || jj >= HEIGHT)
                continue;
            image[jj][ii] |= bitmap->buffer[q * bitmap->width + p];
        }
    }
}


RS_Font::RS_Font(const char *fontPath, float ut) {
    unit = ut;
    fontfile = fontPath;
}

void RS_Font::setMatrix(int handle, int pos, float *matrix) {
    maPositionHandle = pos;
    mUMVPMatrixHandle = handle;
    mvp = matrix;
}

void RS_Font::initVectexList(std::vector<std::pair<RS_Vector, std::string>> vectors) {
    mTextVectors = vectors;
}

void RS_Font::mTextVectorToPointVertexs() {

    double oldSize = 3.5;
    for (vector<std::pair<RS_Vector, string>>::const_iterator imt = mTextVectors.cbegin();
         imt != mTextVectors.cend(); imt++) {
        if (oldSize != imt->first.z) {
            oldSize = imt->first.z;
            error = FT_Set_Char_Size(face, 10 * 64, 0, (int) (imt->first.z * 100),
                                     (int) (imt->first.z * 100));
            slot = face->glyph;
        }
        pen.x = 0 * 64;
        pen.y = 1600 * 64;

        string tmp = imt->second;
        string::size_type idx = tmp.find(".shx;");

        if (idx != string::npos) {
            tmp = tmp.substr(15, 1);
        }
        const char *p = (&tmp)->c_str();
        size_t len = strlen(p) + 1;
        chinese_str = (wchar_t *) malloc(len * sizeof(wchar_t));
        mbstowcs(chinese_str, p, len);



//        wchar_t *const_enter_str = (wchar_t *) L"\n";
        for (n = 0; n < wcslen(chinese_str); n++) {
            FT_Set_Transform(face, &matrix, &pen);
            error = FT_Load_Char(face, chinese_str[n], FT_LOAD_RENDER);
            if (error)
                continue;

            draw_bitmap(&slot->bitmap, slot->bitmap_left, target_height - slot->bitmap_top);
            pen.x += slot->advance.x;
            pen.y += slot->advance.y;
        }
        delete chinese_str;
        chinese_str = NULL;

        int i, j;
        std::vector<pair<double, double>> vectorPoint;

        for (i = 0; i < HEIGHT; i++) {
            for (j = 0; j < WIDTH; j++) {
                if (image[i][j] != 0) {
                    vectorPoint.emplace_back(i, j);
                }
            }
        }
        allPoints.emplace_back(vectorPoint, imt->first);
        pointCount += vectorPoint.size();

        memset(image, 0, HEIGHT * WIDTH * sizeof(char));
    }
}
