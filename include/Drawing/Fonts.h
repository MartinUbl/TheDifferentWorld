#ifndef TDW_FONTS_H
#define TDW_FONTS_H

#include <Singleton.h>

enum FontType
{
    FT_MAIN  = 0,
    FT_MATH  = 1,
    FT_TITLE = 2,
    MAX_FT
};

static const char* fontFilenames[] = {
    {"WishingOnAStar.ttf"}, // FT_MAIN
    {"SweetlyBroken.ttf"},  // FT_MATH
    {"SchoolFont.ttf"}      // FT_TITLE
};

static const uint32 fontSizes[] = {
    24, // FT_MAIN
    22, // FT_MATH
    42  // FT_TITLE
};

class FontMgr
{
    public:
        FontMgr();

        void LoadFonts();
        int32 GetFontId(FontType type);
    private:
        int32 m_fontIds[MAX_FT];
};

#define sFonts Singleton<FontMgr>::instance()

#define FONT(x) (sFonts->GetFontId(x))

#endif
