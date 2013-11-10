#include <Global.h>
#include <Stages.h>
#include <Fonts.h>
#include <Config.h>
#include <Application.h>
#include <Helpers.h>

static const wchar_t* title = L"THE DIFFERENT WORLD";

#define COL_WHITE glColor3ub(255,255,255)
#define COL_YELLOW glColor3ub(255,255,0)

MenuStage::MenuStage(): StageTemplate()
{
}

void MenuStage::Init()
{
}

void MenuStage::Run()
{
    SF->Drawing->DrawRectangleGradient(0, 0, sConfig->GetIntValue(CONFIG_WINDOW_WIDTH), sConfig->GetIntValue(CONFIG_WINDOW_HEIGHT), MAKE_COLOR_RGBA(63,63,63,255), MAKE_COLOR_RGBA(127,127,127,255), VERT_RL);

    uint32 mx = sApplication->GetMouseX();
    uint32 my = sApplication->GetMouseY();

    if (IN_SQUARE(mx, my, 50, 50, 250, 80))
        COL_YELLOW;
    else
        COL_WHITE;
    SF->Drawing->PrintText(FONT(FT_MAIN), 50,50,0,0,L"*  New game");

    if (IN_SQUARE(mx, my, 50, 110, 250, 140))
        COL_YELLOW;
    else
        COL_WHITE;
    SF->Drawing->PrintText(FONT(FT_MAIN), 50,110,0,0,L"*  Load game");

    if (IN_SQUARE(mx, my, 50, 170, 250, 200))
        COL_YELLOW;
    else
        COL_WHITE;
    SF->Drawing->PrintText(FONT(FT_MAIN), 50,170,0,0,L"*  Options");

    if (IN_SQUARE(mx, my, 50, 230, 250, 260))
        COL_YELLOW;
    else
        COL_WHITE;
    SF->Drawing->PrintText(FONT(FT_MAIN), 50,230,0,0,L"*  Exit");

    COL_WHITE;
    SF->Drawing->PrintText(FONT(FT_TITLE), WIDTH-SF->Drawing->GetTextWidth(FONT(FT_TITLE), 0, title)-30, HEIGHT-SF->Drawing->GetFontHeight(FONT(FT_TITLE))-80, 0, 0, title);
    SF->Drawing->PrintText(FONT(FT_MAIN),  WIDTH-SF->Drawing->GetTextWidth(FONT(FT_TITLE), 0, title)+15, HEIGHT-SF->Drawing->GetFontHeight(FONT(FT_MAIN))-30, 0, 0, L"Can you see the difference differential?");

    glLineWidth(5.0);
    glColor3ub(255,0,0);
    glBegin(GL_LINES);
      glVertex2f((GLfloat)WIDTH-SF->Drawing->GetTextWidth(FONT(FT_TITLE), 0, title)+15+SF->Drawing->GetTextWidth(FONT(FT_MAIN), 0, L"Can you see the "), (GLfloat)HEIGHT-SF->Drawing->GetFontHeight(FONT(FT_MAIN))-10);
      glVertex2f((GLfloat)WIDTH-SF->Drawing->GetTextWidth(FONT(FT_TITLE), 0, title)+15+SF->Drawing->GetTextWidth(FONT(FT_MAIN), 0, L"Can you see the difference"), (GLfloat)HEIGHT-SF->Drawing->GetFontHeight(FONT(FT_MAIN))-28);
    glEnd();
}

void MenuStage::MousePress(uint32 x, uint32 y, uint8 btn, bool press)
{
    if (press && IN_SQUARE(x, y, 50, 50, 250, 80))
        sApplication->SetStage(STAGE_GAME);
}
