//
//  Cast.cpp
//  MuPDF
//
//  Created by Ｙuancheng SU on 14-7-18.
//  Copyright (c) 2014年 Artifex Software, Inc. All rights reserved.
//

#include <iostream>
#include "CFuntion.h"
char *CONST_CAST(const char* str )
{
    if(str)
        return const_cast<char*>(str);
    return NULL;
}