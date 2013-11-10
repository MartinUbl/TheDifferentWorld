#include <Global.h>
#include <View.h>
#include <Config.h>

ViewMgr::ViewMgr()
{
    m_viewX = 0;
    m_viewY = 0;
}

ViewMgr::~ViewMgr()
{
}

/*
 * Translates the OpenGL matrix to our viewport
 */
void ViewMgr::Translate(bool reset)
{
    if (reset)
        glLoadIdentity();

    glTranslatef(-(GLfloat)m_viewX, -(GLfloat)m_viewY, 0);
}

/*
 * Returns the absolute viewport
 */
void ViewMgr::GetViewPort(uint32 &beginX, uint32 &beginY, uint32 &endX, uint32 &endY)
{
    beginX = m_viewX;
    beginY = m_viewY;

    endX = beginX + WIDTH;
    endY = beginY + HEIGHT;
}

/*
 * Moves the view to target position (coordinates supplied are the visible coords on screen, the relative ones)
 */
void ViewMgr::MoveWithTarget(uint32 viewPosX, uint32 viewPosY)
{
    // avoid moving camera when locked
    if (m_lock)
        return;

    int32 deltaX = (int32)viewPosX - WIDTH/2;
    int32 deltaY = (int32)viewPosY - HEIGHT/2;

    if (abs(deltaX) > (VIEW_MOVEBOX_WIDTH_PCT*WIDTH)/100)
        m_viewX -= ((deltaX > 0)?1:(-1))*(abs(deltaX) - (VIEW_MOVEBOX_WIDTH_PCT*WIDTH)/100);
    if (abs(deltaY) > (VIEW_MOVEBOX_HEIGHT_PCT*HEIGHT)/100)
        m_viewY -= ((deltaY > 0)?1:(-1))*(abs(deltaY) - (VIEW_MOVEBOX_HEIGHT_PCT*HEIGHT)/100);
}

/*
 * Locks the viewport on current coordinates - avoiding any camera move
 */
void ViewMgr::LockViewPort()
{
    m_lock = true;
}

/*
 * Unlocks viewport to allow camera moving with target
 */
void ViewMgr::UnlockViewPort()
{
    m_lock = false;

    // TODO: move viewport immediatelly? delayed?
}
