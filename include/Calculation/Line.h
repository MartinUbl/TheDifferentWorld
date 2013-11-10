#ifndef TDW_LINE_H
#define TDW_LINE_H

#include <Vector.h>

// 2D line
struct Line
{
    // define line using two points
    Line(float ax, float ay, float bx, float by): dirVector(bx-ax, by-ay)
    {
        m_limited = false;
        dirVector.unitMultiply(1.0f);

        memberPoint[0] = ax;
        memberPoint[1] = ay;

        linearMembers[0] = dirVector.y;
        linearMembers[1] = -dirVector.x;
        linearMembers[2] = -linearMembers[0]*ax-linearMembers[1]*ay;
    };
    // define line using point and direction vector
    Line(float ax, float ay, CVector2 dirvect): dirVector(dirvect.x, dirvect.y)
    {
        m_limited = false;
        dirVector.unitMultiply(1.0f);

        memberPoint[0] = ax;
        memberPoint[1] = ay;

        linearMembers[0] = dirVector.y;
        linearMembers[1] = -dirVector.x;
        linearMembers[2] = -linearMembers[0]*ax-linearMembers[1]*ay;
    }

    float GetPointDistance(float px, float py, bool ignoreLimit = false)
    {
        CVector2 normalvect(linearMembers[0], linearMembers[1]);
        if (normalvect.mySize() == 0)
            return 0.0f;

        if (ignoreLimit || !m_limited)
            return fabs(linearMembers[0]*px+linearMembers[1]*py+linearMembers[2]) / normalvect.mySize();
        else
        {
            float cx, cy;
            GetContactPoint(px, py, cx, cy);
            if (IsPointOnLine(cx, cy))
                return fabs(linearMembers[0]*px+linearMembers[1]*py+linearMembers[2]) / normalvect.mySize();
            else
                return min(sqrt(pow(px-m_limitations[0],2)+pow(py-m_limitations[1],2)), sqrt(pow(px-m_limitations[2],2)+pow(py-m_limitations[3],2)));
        }
    }

    void GetContactPoint(float px, float py, float &x, float &y)
    {
        Line normal(px, py, CVector2(linearMembers[0], linearMembers[1]));
        LineIntersection(&normal, x, y);
    }

    bool LineIntersection(Line* sec, float &x, float &y)
    {
        if (dirVector.IsParallel(sec->dirVector))
        {
            x = 0;
            y = 0;
            return false;
        }

        // second: by + c = 0 ; y = -c/b
        if (sec->linearMembers[0] == 0)
        {
            y = -sec->linearMembers[2]/sec->linearMembers[1];
            x = (-linearMembers[1]*y-linearMembers[2])/linearMembers[0];
            return true;
        }
        // first:  ax + c = 0 ; x = -c/a
        if (linearMembers[1] == 0)
        {
            x = -linearMembers[2]/linearMembers[0];
            y = (-sec->linearMembers[0]*x-sec->linearMembers[2])/sec->linearMembers[1];
            return true;
        }
        // first:  by + c = 0 ; y = -c/b
        if (linearMembers[0] == 0)
        {
            y = -linearMembers[2]/linearMembers[1];
            x = (-sec->linearMembers[1]*y-sec->linearMembers[2])/sec->linearMembers[0];
            return true;
        }

        y = (sec->linearMembers[2]*linearMembers[0]-linearMembers[2]*sec->linearMembers[0])/(linearMembers[1]*sec->linearMembers[0]-sec->linearMembers[1]*linearMembers[0]);
        x = (-linearMembers[1]*y-linearMembers[2])/linearMembers[0];
        return true;
    }

    void SetLimit(float minx, float miny, float maxx, float maxy)
    {
        m_limited = true;
        m_limitations[0] = minx;
        m_limitations[1] = miny;
        m_limitations[2] = maxx;
        m_limitations[3] = maxy;
    }

    bool IsPointOnLine(float px, float py)
    {
        if (m_limited)
        {
            Line a(m_limitations[0], m_limitations[1], px, py);
            Line b(m_limitations[2], m_limitations[3], px, py);

            if (fabs(a.dirVector.x + b.dirVector.x) < 0.1f && fabs(a.dirVector.y + b.dirVector.y) < 0.1f)
                return true;
            else
                return false;
        }
        else
            return GetPointDistance(px, py, true) < 1.0f;
    }

    // parametric
    float memberPoint[2];
    CVector2 dirVector;

    // linear
    float linearMembers[3]; // ax + by + c = 0, we need to store a,b and c

    // limitations
    bool m_limited;
    float m_limitations[4]; // 2 x,y coordinates (endpoints)
};

#endif
