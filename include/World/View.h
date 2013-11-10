#ifndef TDW_VIEW_H
#define TDW_VIEW_H

#include <Singleton.h>

enum ViewConstants
{
    VIEW_MOVEBOX_WIDTH_PCT = 10,
    VIEW_MOVEBOX_HEIGHT_PCT = 10,
};

class ViewMgr
{
    public:
        ViewMgr();
        ~ViewMgr();

        void Translate(bool reset = true);
        void GetViewPort(uint32 &beginX, uint32 &beginY, uint32 &endX, uint32 &endY);
        void MoveWithTarget(uint32 viewPosX, uint32 viewPosY);

        void LockViewPort();
        void UnlockViewPort();

    private:
        uint32 m_viewX;
        uint32 m_viewY;

        bool m_lock;
};

#define sView Singleton<ViewMgr>::instance()

#endif
