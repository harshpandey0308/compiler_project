#include<windows.h>
#include<string.h>

#include"Filedialog.h"

int OpenCFile(char *filepath , size_t pathsize){

    OPENFILENAMEA ofn;

    memset(&ofn , 0 , sizeof(ofn));

    filepath[0] = '\0';

    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFile = filepath;
    ofn.nMaxFile = (DWORD)pathsize;

    ofn.lpstrFilter = "C SOURCE FILE (*.c)\0*.c\0"
                        "ALL FILES (*.*)\0*.*\0";

    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;

    ofn.lpstrTitle = "Open C Source File";

    return GetOpenFileNameA(&ofn);
}