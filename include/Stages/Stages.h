#ifndef TDW_MENU_STAGE_H
#define TDW_MENU_STAGE_H

#include <Singleton.h>

#include <Map.h>

class StageTemplate
{
    public:
        StageTemplate() {};

        virtual void Init() {};
        virtual void Run() {};

        virtual void MousePress(uint32 x, uint32 y, uint8 btn, bool press) {};
        virtual void KeyPress(uint16 key, bool press) {};
        virtual void PrintableKeyPress(uint16 key) {};
    private:
};

class MenuStage: public StageTemplate
{
    public:
        MenuStage();

        void Init();
        void Run();

        void MousePress(uint32 x, uint32 y, uint8 btn, bool press);
    private:
        //
};

class GameStage: public StageTemplate
{
    public:
        GameStage();

        void Init();
        void Run();

        void MousePress(uint32 x, uint32 y, uint8 btn, bool press);
        void KeyPress(uint16 key, bool press);
        void PrintableKeyPress(uint16 key);

        bool IsEditorMode() const { return m_editorMode; };

    private:
        Map* m_map;
        bool m_editorMode;
        uint8 m_dialogOpened;
};

#define sMenu PolySingleton<StageTemplate, MenuStage>::instance()
#define sGame PolySingleton<StageTemplate, GameStage>::instance()

#endif
