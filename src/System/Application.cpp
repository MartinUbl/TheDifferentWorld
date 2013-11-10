#include <Global.h>
#include <Application.h>
#include <Config.h>
#include <Stages.h>
#include <Fonts.h>
#include <Textures.h>
#include <ExpressionParser.h>

Application::Application()
{
    m_mouseX = 0;
    m_mouseY = 0;
    m_stage = STAGE_NONE;
    m_startup = true;
}

void Application::KeyPress(uint16 key, bool press)
{
    if (instanceExists<StageTemplate, MenuStage>())
        sMenu->KeyPress(key, press);

    if (instanceExists<StageTemplate, GameStage>())
        sGame->KeyPress(key, press);
}

void KeyPressed(uint16 key, bool press)
{
    sApplication->KeyPress(key, press);
}

void Application::MousePress(uint8 btn, bool press)
{
    if (instanceExists<StageTemplate, MenuStage>())
        sMenu->MousePress(GetMouseX(), GetMouseY(), btn, press);

    if (instanceExists<StageTemplate, GameStage>())
        sGame->MousePress(GetMouseX(), GetMouseY(), btn, press);
}

void Application::SetMouseXY(uint32 x, uint32 y)
{
    m_mouseX = x;
    m_mouseY = y;
}

void MouseButtonPress(bool left, bool press)
{
    sApplication->MousePress(left ? BUTTON_LEFT : BUTTON_RIGHT, press);
}

LRESULT CALLBACK MyWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_MOUSEMOVE)
    {
        sApplication->SetMouseXY(LOWORD(lParam), HIWORD(lParam));
        return 0;
    }
    else if (msg == WM_CHAR)
    {
        if (PolySingleton<StageTemplate, GameStage>::exists())
            sGame->PrintableKeyPress(wParam);
    }

    return sSimplyFlat->SFWndProc(hWnd, msg, wParam, lParam);
}

bool Application::Init()
{
    if (!sConfig->Load())
        return false;

    // functionality unit tests
    ExpressionParserUnitTest();

    if (!sSimplyFlat->CreateMainWindow("The Different World", sConfig->GetIntValue(CONFIG_WINDOW_WIDTH), sConfig->GetIntValue(CONFIG_WINDOW_HEIGHT), 32, sConfig->GetBoolValue(CONFIG_FULLSCREEN), 60, &MyWndProc))
        return false;

    sSimplyFlat->Interface->HookEvent(0, KeyPressed);
    sSimplyFlat->Interface->HookMouseEvent(MouseButtonPress);

    sFonts->LoadFonts();

    sTextures->LoadTextureDatabase();

    return true;
}

int Application::Run()
{
    MSG msg;

    while (true)
    {
        if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
        {
            if (msg.message != WM_QUIT)
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            else
                break;

            continue;
        }

        SF->BeforeDraw();

        RunGameStage();

        SF->AfterDraw();
    }

    return 0;
}

void Application::RunGameStage()
{
    if (m_stage == STAGE_NONE)
    {
        if (instanceExists<StageTemplate, MenuStage>())
            cleanupSingleton<StageTemplate, MenuStage>();

        if (instanceExists<StageTemplate, GameStage>())
            cleanupSingleton<StageTemplate, GameStage>();

        m_stage = STAGE_MENU;
    }
    else if (m_stage == STAGE_MENU)
    {
        if (!instanceExists<StageTemplate, MenuStage>())
        {
            if (instanceExists<StageTemplate, GameStage>())
                cleanupSingleton<StageTemplate, GameStage>();

            sMenu->Init();
        }

        sMenu->Run();
    }
    else if (m_stage == STAGE_GAME)
    {
        if (!instanceExists<StageTemplate, GameStage>())
        {
            if (instanceExists<StageTemplate, MenuStage>())
                cleanupSingleton<StageTemplate, MenuStage>();

            sGame->Init();
        }

        sGame->Run();
    }
}
