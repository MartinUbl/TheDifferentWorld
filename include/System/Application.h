#ifndef TDW_APPLICATION_H
#define TDW_APPLICATION_H

#include <Singleton.h>

enum MouseButton
{
    BUTTON_LEFT,
    BUTTON_MIDDLE,
    BUTTON_RIGHT
};

enum AppStage
{
    STAGE_NONE = 0,
    STAGE_MENU = 1,
    STAGE_GAME = 2,
    MAX_STAGE
};

class Application
{
    public:
        Application();

        bool Init();
        int Run();

        void KeyPress(uint16 key, bool press);
        void MousePress(uint8 btn, bool press);
        void SetMouseXY(uint32 x, uint32 y);
        uint32 GetMouseX() { return m_mouseX; };
        uint32 GetMouseY() { return m_mouseY; };

        void RunGameStage();
        AppStage GetStage() { return m_stage; };
        void SetStage(AppStage stage) { m_stage = stage; };
        bool IsStartup() { return m_startup; };
        void SetStartup(bool state) { m_startup = state; };

    private:
        uint32 m_mouseX, m_mouseY;
        AppStage m_stage;
        bool m_startup;
};

#define sApplication Singleton<Application>::instance()

#endif
