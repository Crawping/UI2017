#ifndef INCLUDED_COMMAND_DEF
#define INCLUDED_COMMAND_DEF

enum { 
    TILE_SIZE = 256,  // ����ֿ��С
};


struct SOURCE_BITMAP_DATA
{
    HBITMAP hBitmap;
    byte* pBits;
    long  pitch;
    bool  hasAlphaChannel;
};


#endif // INCLUDED_COMMAND_DEF