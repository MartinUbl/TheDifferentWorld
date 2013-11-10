#include <Global.h>
#include <Helpers.h>
#include <Paths.h>
#include <Fonts.h>
#include <Map.h>
#include <Ground.h>
#include <Textures.h>
#include <Stages.h>
#include <Config.h>
#include <Application.h>
#include <View.h>
#include <Player.h>

// UI positions
#define EDIT_BTN_X WIDTH-64-32
#define EDIT_BTN_Y HEIGHT-32-16
#define DELETE_BTN_X WIDTH-32-16
#define DELETE_BTN_Y HEIGHT-32-16

/*
 * Dialog stuff
 */

enum DialogTypes
{
    DIALOG_NONE          = 0,
    DIALOG_FUNCTION_EDIT = 1,
    DIALOG_INFO          = 2,
    MAX_DIALOG_TYPE
};

static struct DialogData
{
    std::string text;
    float ratio;
} dialogData;

/*
 * Game stage stuff
 */

GameStage::GameStage(): StageTemplate()
{
    m_editorMode = false;
    m_dialogOpened = 0;
}

void GameStage::Init()
{
    m_map = new Map;
    m_map->InitEmpty();

    dialogData.text = "";
    dialogData.ratio = 1.0f;
}

void GameStage::Run()
{
    uint32 mouseX = sApplication->GetMouseX();
    uint32 mouseY = sApplication->GetMouseY();

    SF->Drawing->ClearColor(127,127,127);
    SF->Drawing->DrawRectangle(0,0,640,480,0,TEXTURE(1));
    SF->Drawing->DrawRectangle(640,0,640,480,0,TEXTURE(1));
    SF->Drawing->DrawRectangle(0,480,640,480,0,TEXTURE(1));
    SF->Drawing->DrawRectangle(640,480,640,480,0,TEXTURE(1));
    glColor3ub(0,0,0);
    //SF->Drawing->PrintText(FONT(FT_MATH), 50, 50, 0, 0, L"f(x): y = x + 2");

    if (m_editorMode)
        SF->Drawing->PrintText(FONT(FT_MATH), 10, 10, 0, 0, L"Editor mode");

    sView->Translate();
    m_map->Draw();

    sPlayer->UpdateMovement();
    sPlayer->Draw();

    FunctionChainElement* fche = NULL;
    if ((fche = m_map->GetSelectedChainElement()) != NULL)
    {
        glColor3ub(0,0,0);
        SF->Drawing->PrintText(FONT(FT_MATH), 20, HEIGHT-SF->Drawing->GetFontHeight(FONT(FT_MATH))-20, 0, 0, L"y = %s", ToWideString(fche->ground->GetOriginalExpression()));
    }
    else if ((fche = m_map->GetClickedChainElement()) != NULL)
    {
        glColor3ub(0,0,0);
        SF->Drawing->PrintText(FONT(FT_MATH), 20, HEIGHT-SF->Drawing->GetFontHeight(FONT(FT_MATH))-20, 0, 0, L"y = %s", ToWideString(fche->ground->GetOriginalExpression()));
    }

    if (m_map->GetClickedChainElement() != NULL)
    {
        uint32 discol = MAKE_COLOR_RGBA(255,255,255,255);
        if (IN_SQUARE(mouseX, mouseY, EDIT_BTN_X, EDIT_BTN_Y, EDIT_BTN_X+32, EDIT_BTN_Y+32))
            discol = MAKE_COLOR_RGBA(255,0,0,127);
        SF->Drawing->DrawRectangle(EDIT_BTN_X, EDIT_BTN_Y, 32,32,discol, TEXTURE(2));

        discol = MAKE_COLOR_RGBA(255,255,255,255);
        if (IN_SQUARE(mouseX, mouseY, DELETE_BTN_X, DELETE_BTN_Y, DELETE_BTN_X+32, DELETE_BTN_Y+32))
            discol = MAKE_COLOR_RGBA(255,0,0,127);
        SF->Drawing->DrawRectangle(DELETE_BTN_X, DELETE_BTN_Y, 32,32,discol, TEXTURE(3));
    }

    if (m_dialogOpened > 0)
    {
        if (m_dialogOpened == DIALOG_FUNCTION_EDIT)
        {
            SF->Drawing->DrawRectangle(WIDTH/2 - 300, HEIGHT/2 - 80, 600, 160, MAKE_COLOR_RGBA(0,32,0,225), 0);
            glColor3ub(200,255,200);
            SF->Drawing->PrintText(FONT(FT_MAIN), WIDTH/2 - SF->Drawing->GetTextWidth(FONT(FT_MAIN), 0, L"Function edit")/2, HEIGHT/2 - 55, 0, 0, L"Function edit");

            if (IN_SQUARE(mouseX, mouseY, WIDTH/2 - SF->Drawing->GetTextWidth(FONT(FT_MAIN), 0, L"OK")/2 - 100, HEIGHT/2+35, WIDTH/2 + SF->Drawing->GetTextWidth(FONT(FT_MAIN), 0, L"OK")/2 - 100, HEIGHT/2+70))
                glColor3ub(255,200,200);
            else
                glColor3ub(200,255,200);
            SF->Drawing->PrintText(FONT(FT_MAIN), WIDTH/2 - SF->Drawing->GetTextWidth(FONT(FT_MAIN), 0, L"OK")/2 - 100, HEIGHT/2+35, 0, 0, L"OK");

            if (IN_SQUARE(mouseX, mouseY, WIDTH/2 - SF->Drawing->GetTextWidth(FONT(FT_MAIN), 0, L"Cancel")/2 + 60, HEIGHT/2+35, WIDTH/2 + SF->Drawing->GetTextWidth(FONT(FT_MAIN), 0, L"Cancel")/2 + 60, HEIGHT/2+70))
                glColor3ub(255,200,200);
            else
                glColor3ub(200,255,200);
            SF->Drawing->PrintText(FONT(FT_MAIN), WIDTH/2 - SF->Drawing->GetTextWidth(FONT(FT_MAIN), 0, L"Cancel")/2 + 60, HEIGHT/2+35, 0, 0, L"Cancel");

            glColor3ub(255,255,255);
            SF->Drawing->PrintText(FONT(FT_MATH), WIDTH/2 - 255, HEIGHT/2 - 20, 0, 0, L"y = %s |", ToWideString(dialogData.text.c_str()));

            // ratio mover
            SF->Drawing->DrawRectangle(WIDTH-50, HEIGHT/2 - 100, 50, 200, MAKE_COLOR_RGBA(0,32,0,225), 0);
            SF->Drawing->DrawRectangle(WIDTH-26, HEIGHT/2 - 80,  2,  150, MAKE_COLOR_RGBA(200,255,200,255), 0);

            int32 pos = HEIGHT/2-80+150 - (int32)(sqrt(2250*dialogData.ratio));
            SF->Drawing->DrawRectangle(WIDTH-45,pos,40,2,MAKE_COLOR_RGBA(200,255,200,255),0);

            SF->Drawing->PrintText(FONT(FT_MATH), WIDTH-42, HEIGHT/2 + 72, 0, -1, L"%2.2f", dialogData.ratio);
        }
        else if (m_dialogOpened == DIALOG_INFO)
        {
            SF->Drawing->DrawRectangle(WIDTH/2 - 300, HEIGHT/2 - 100, 600, 200, MAKE_COLOR_RGBA(0,32,0,225), 0);
            glColor3ub(200,255,200);
            SF->Drawing->PrintText(FONT(FT_MAIN), WIDTH/2 - SF->Drawing->GetTextWidth(FONT(FT_MAIN), 0, L"Info")/2, HEIGHT/2 - 75, 0, 0, L"Info");

            SF->Drawing->PrintText(FONT(FT_MATH), WIDTH/2 - SF->Drawing->GetTextWidth(FONT(FT_MATH), 0, L"%s", ToWideString(dialogData.text.c_str()))/2, HEIGHT/2 - 20, 0, 0, L"%s", ToWideString(dialogData.text.c_str()));

            if (IN_SQUARE(mouseX, mouseY, WIDTH/2 - SF->Drawing->GetTextWidth(FONT(FT_MAIN), 0, L"OK")/2, HEIGHT/2+55, WIDTH/2 + SF->Drawing->GetTextWidth(FONT(FT_MAIN), 0, L"OK")/2, HEIGHT/2+90))
                glColor3ub(255,200,200);
            else
                glColor3ub(200,255,200);
            SF->Drawing->PrintText(FONT(FT_MAIN), WIDTH/2 - SF->Drawing->GetTextWidth(FONT(FT_MAIN), 0, L"OK")/2, HEIGHT/2+55, 0, 0, L"OK");
        }
    }
}

void GameStage::KeyPress(uint16 key, bool press)
{
    if (key == 'A' || key == 'D')
    {
        sPlayer->SetMoveElement(key == 'A' ? MOVE_ELEMENT_LEFT : MOVE_ELEMENT_RIGHT, press);
        return;
    }

    if (press)
    {
        if (key == 192) // ';'
        {
            m_editorMode = !m_editorMode;
        }
    }
    else
    {
    }
}

void GameStage::MousePress(uint32 x, uint32 y, uint8 btn, bool press)
{
    if (btn == BUTTON_LEFT && press)
    {
        if (m_dialogOpened > 0)
        {
            if (m_dialogOpened == DIALOG_FUNCTION_EDIT)
            {
                // OK
                if (IN_SQUARE(x, y, WIDTH/2 - SF->Drawing->GetTextWidth(FONT(FT_MAIN), 0, L"OK")/2 - 100, HEIGHT/2+35, WIDTH/2 + SF->Drawing->GetTextWidth(FONT(FT_MAIN), 0, L"OK")/2 - 100, HEIGHT/2+70))
                {
                    ExpressionVector* vec = ExpressionParser::Parse(dialogData.text.c_str());
                    if (ExpressionParser::CheckSyntax(vec))
                    {
                        if (m_map->GetClickedChainElement())
                            m_map->GetClickedChainElement()->ground->ReCreate(dialogData.text.c_str(), 0, 100, dialogData.ratio);
                        m_map->SetClickedSelectedChainElement(false);

                        m_dialogOpened = 0;
                    }
                    else
                    {
                        m_dialogOpened = DIALOG_INFO;
                        dialogData.text = "Wrong expression!";
                    }
                }
                // CANCEL
                else if (IN_SQUARE(x, y, WIDTH/2 - SF->Drawing->GetTextWidth(FONT(FT_MAIN), 0, L"Cancel")/2 + 60, HEIGHT/2+35, WIDTH/2 + SF->Drawing->GetTextWidth(FONT(FT_MAIN), 0, L"Cancel")/2 + 60, HEIGHT/2+70))
                {
                    m_map->SetClickedSelectedChainElement(false);
                    m_dialogOpened = 0;
                }
                // Scale mover
                else if (IN_SQUARE(x, y, WIDTH-35, HEIGHT/2-80, WIDTH-18, HEIGHT/2-80+150))
                {
                    float ay = y - (((float)HEIGHT/2.0f)-80.0f);
                    dialogData.ratio = (pow(150.0f-ay,2)/2250);
                    if (dialogData.ratio < 0.01f)
                        dialogData.ratio = 0.01f;
                    if (dialogData.ratio > 9.99f)
                        dialogData.ratio = 9.99f;
                }
            }
            else if (m_dialogOpened == DIALOG_INFO)
            {
                // OK
                if (IN_SQUARE(x, y, WIDTH/2 - SF->Drawing->GetTextWidth(FONT(FT_MAIN), 0, L"OK")/2, HEIGHT/2+55, WIDTH/2 + SF->Drawing->GetTextWidth(FONT(FT_MAIN), 0, L"OK")/2, HEIGHT/2+90))
                    m_dialogOpened = 0;
            }
        }
        else if (m_map->GetClickedChainElement())
        {
            if (IN_SQUARE(x, y, EDIT_BTN_X, EDIT_BTN_Y, EDIT_BTN_X+32, EDIT_BTN_Y+32))
            {
                m_dialogOpened = DIALOG_FUNCTION_EDIT;
                dialogData.text = m_map->GetClickedChainElement()->ground->GetOriginalExpression();
                dialogData.ratio = m_map->GetClickedChainElement()->ground->GetRatio();
            }
            else if (IN_SQUARE(x, y, DELETE_BTN_X, DELETE_BTN_Y, DELETE_BTN_X+32, DELETE_BTN_Y+32))
            {
                m_map->DeleteChainElement(m_map->GetClickedChainElement());
            }
            else
            {
                if (m_map->GetSelectedChainElement())
                    m_map->SetClickedSelectedChainElement(true);
                else
                    m_map->SetClickedSelectedChainElement(false);
            }
        }
        else
        {
            if (m_map->GetSelectedChainElement())
                m_map->SetClickedSelectedChainElement(true);
            else
                m_map->SetClickedSelectedChainElement(false);
        }
    }
}

void GameStage::PrintableKeyPress(uint16 key)
{
    if (m_dialogOpened > 0)
    {
        if (m_dialogOpened == DIALOG_FUNCTION_EDIT)
        {
            // backspace
            if (key == 8)
            {
                if (dialogData.text.size() > 0)
                    dialogData.text.erase(--dialogData.text.end());
            }
            // enter
            else if (key == 13)
            {
                // make it hit OK
            }
            // allowed keys ()+-*/^, and alphanumeric chars
            else if (key == 40 || key == 41 || key == 42 || key == 43 || key == 45 || key == 47 || key == 94 || (key >= 48 && key <= 57) || (key >= 65 && key <= 90) || (key >= 97 && key <= 122))
            {
                dialogData.text.push_back((char)key);
            }
        }
    }
}
