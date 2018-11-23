//
// Created by booway on 2018/8/30.
//

#ifndef DWGDEMO_RS_DATABASE_H
#define DWGDEMO_RS_DATABASE_H


#include "rs_vector.h"

class rs_database {
    std::vector<std::pair<RS_Vector, double> > verListLwLine;
    std::vector<int> verListLwLineNum;

};


#endif //DWGDEMO_RS_DATABASE_H
