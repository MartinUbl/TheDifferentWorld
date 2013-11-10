#include <Global.h>
#include <Fonts.h>
#include <Paths.h>
#include <Helpers.h>

FontMgr::FontMgr()
{
    memset(m_fontIds, 0, sizeof(uint32)*MAX_FT);
}

void FontMgr::LoadFonts()
{
    uint32 i;

    for (i = 0; i < MAX_FT; i++)
        m_fontIds[i] = sSimplyFlat->BuildFont(MakeFilePath(FONTS_PATH,fontFilenames[i]), fontSizes[i]);
}

int32 FontMgr::GetFontId(FontType type)
{
    if (type != MAX_FT)
        return m_fontIds[type];

    return -1;
}
