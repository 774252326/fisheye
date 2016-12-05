// ****************************************************
// Libraries
// ****************************************************

#include <stdio.h>

void str2xyz(String msg, float &x, float &y, float &z)
{
    char info[32];
    msg.toCharArray(info,32);
    x = atof(strtok(info,","));
    y = atof(strtok(NULL,","));
    z = atof(strtok(NULL,","));
}


