#ifndef TDW_PLAYER_H
#define TDW_PLAYER_H

#include <Singleton.h>

enum MoveElements
{
    MOVE_ELEMENT_RIGHT   = 0,
    MOVE_ELEMENT_LEFT    = 1,
    MOVE_ELEMENT_GRAVITY = 2,
    MAX_MOVE_ELEMENT
};

#define MOVE_UPDATE_INTERVAL 5

class Player
{
    public:
        Player();
        ~Player();

        int32 GetPositionX();
        int32 GetPositionY();

        void Draw();

        void UpdateMovement();
        void SetMoveElement(MoveElements me, bool set);
        bool IsMoveElementSet(MoveElements me) { return m_moveElements[me]; };

    private:
        int32 m_positionX, m_positionY;
        bool m_moveElements[MAX_MOVE_ELEMENT];
        uint32 m_moveElementTime[MAX_MOVE_ELEMENT];
        uint32 m_lastMoveTick;
};

#define sPlayer Singleton<Player>::instance()

#endif
