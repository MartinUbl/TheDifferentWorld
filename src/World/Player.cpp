#include <Global.h>
#include <Player.h>
#include <Helpers.h>

Player::Player()
{
    m_positionX = 0;
    m_positionY = 0;
    m_lastMoveTick = getMSTime();

    memset(m_moveElements, 0, sizeof(m_moveElements));
    SetMoveElement(MOVE_ELEMENT_GRAVITY, true);
}

Player::~Player()
{
}

int32 Player::GetPositionX()
{
    return m_positionX;
}

int32 Player::GetPositionY()
{
    return m_positionY;
}

void Player::Draw()
{
    SF->Drawing->DrawRectangle(GetPositionX(), GetPositionY(), 10, 10, MAKE_COLOR_RGBA(0,0,0,225), 0);
}

void Player::UpdateMovement()
{
    if (getMSTime() < m_lastMoveTick + MOVE_UPDATE_INTERVAL)
        return;

    uint32 diff = getMSTimeDiff(m_lastMoveTick, getMSTime());

    m_lastMoveTick = getMSTime();

    if (IsMoveElementSet(MOVE_ELEMENT_LEFT))
        m_positionX -= 1*(diff/MOVE_UPDATE_INTERVAL);

    if (IsMoveElementSet(MOVE_ELEMENT_RIGHT))
        m_positionX += 1*(diff/MOVE_UPDATE_INTERVAL);

    if (IsMoveElementSet(MOVE_ELEMENT_GRAVITY))
    {
        float t = (float)getMSTimeDiff(m_moveElementTime[MOVE_ELEMENT_GRAVITY], getMSTime());

        // 2t is the derivative of gravity function s=1/2gt^2
        m_positionY += (int32)((2*t+200.0f)/200.0f);
    }
}

void Player::SetMoveElement(MoveElements me, bool set)
{
    m_moveElements[me] = set;
    // we save the time in order to determine i.e. movement speed in specified direction
    m_moveElementTime[me] = getMSTime();
}
