#include <Global.h>
#include <Ground.h>
#include <Helpers.h>
#include <Vector.h>
#include <Line.h>
#include <Stages.h>
#include <Application.h>

static GroundChunk* actualParsed = NULL;
static double parseStep = 0;

double ReferenceFunction(const char* input)
{
    if (!actualParsed)
        return parseStep;

    return parseStep*actualParsed->GetRatio();
}

GroundChunk::GroundChunk(const char *expression, int64 interval_start, int64 interval_end, float ratio)
{
    Create(expression, interval_start, interval_end, ratio);
}

GroundChunk::~GroundChunk()
{
    CleanUp();
}

void GroundChunk::Create(const char *expression, int64 interval_start, int64 interval_end, float ratio)
{
    m_originalExpression = expression;

    ExpressionVector* vec = ExpressionParser::Parse(expression);
    m_parsedExpression = ExpressionParser::BuildTree(vec, 0, vec->size());
    m_parsedExpression->SimplifyChildren();

    m_ratio = ratio;
    actualParsed = this;
    parseStep = interval_start;

    FuncVertex* v = NULL;
    for (int32 i = interval_start; i <= interval_end; i += GROUND_GRANULARITY)
    {
        parseStep = i;

        v = new FuncVertex;
        v->x = i;
        // switch polarity due to switched axis polarity in OpenGL 2D versus math
        v->y = -NumerateExpression(m_parsedExpression, ReferenceFunction);
        m_vertices.push_back(v);
    }

    actualParsed = NULL;
    parseStep = 0;

    m_lastVertex.x = 0;
    m_lastVertex.y = 0;
}

void GroundChunk::CleanUp()
{
    for (uint32 i = 0; i < m_vertices.size(); i++)
        delete m_vertices[i];

    m_vertices.clear();
}

void GroundChunk::ReCreate(const char* expression, int64 interval_start, int64 interval_end, float ratio)
{
    CleanUp();
    Create(expression, interval_start, interval_end, ratio);
}

void GroundChunk::Draw(double startX, double startY)
{
    if (m_vertices.size() < 2)
        return;

    double gabs = m_parsedExpression->GetAbsoluteValue();

    int32 beginX = (int32)m_vertices[0]->x;
    int32 beginY = (int32)(m_vertices[0]->y+gabs);

    glLineWidth(1.2f);
    uint32 i;
    for (i = 0; i < m_vertices.size()-1; i++)
    {
        glBegin(GL_LINES);
          glVertex2d((GLdouble)startX+m_vertices[i]->x-beginX, (GLdouble)startY+m_vertices[i]->y-beginY);
          glVertex2d((GLdouble)startX+m_vertices[i+1]->x-beginX, (GLdouble)startY+m_vertices[i+1]->y-beginY);
        glEnd();

        if (sGame->IsEditorMode() && sApplication->GetMouseX() > startX+m_vertices[0]->x-beginX && sApplication->GetMouseX() < startX+m_vertices[m_vertices.size()-1]->x-beginX)
        {
            Line ln((float)(startX+m_vertices[i]->x-beginX), (float)(startY+m_vertices[i]->y-beginY), (float)(startX+m_vertices[i+1]->x-beginX), (float)(startY+m_vertices[i+1]->y-beginY));
            ln.SetLimit((float)(startX+m_vertices[i]->x-beginX), (float)(startY+m_vertices[i]->y-beginY), (float)(startX+m_vertices[i+1]->x-beginX), (float)(startY+m_vertices[i+1]->y-beginY));
            float cpx, cpy;
            Line vert((float)sApplication->GetMouseX(), (float)sApplication->GetMouseY(), CVector2(0,1));
            ln.LineIntersection(&vert, cpx, cpy);

            //if (ln.IsPointOnLine(cpx, cpy))
            {
                glBegin(GL_QUADS);
                  glVertex2f(cpx-2.0f, cpy-2.0f);
                  glVertex2f(cpx+2.0f, cpy-2.0f);
                  glVertex2f(cpx+2.0f, cpy+2.0f);
                  glVertex2f(cpx-2.0f, cpy+2.0f);
                glEnd();
            }
        }
    }

    m_lastVertex.x = startX+m_vertices[i]->x-beginX;
    m_lastVertex.y = startY+m_vertices[i]->y-beginY;
}

bool GroundChunk::IsPointInDistance(uint32 relx, uint32 rely, uint32 px, uint32 py, float dist)
{
    if (m_vertices.size() < 2)
        return false;

    double gabs = m_parsedExpression->GetAbsoluteValue();

    int32 beginX = (int32)m_vertices[0]->x;
    int32 beginY = (int32)(m_vertices[0]->y+gabs);

    uint32 i;
    Line* ln = NULL;
    for (i = 0; i < m_vertices.size()-1; i++)
    {
        ln = new Line((float)(relx+m_vertices[i]->x-beginX), (float)(rely+m_vertices[i]->y-beginY), (float)(relx+m_vertices[i+1]->x-beginX), (float)(rely+m_vertices[i+1]->y-beginY));
        ln->SetLimit((float)(relx+m_vertices[i]->x-beginX), (float)(rely+m_vertices[i]->y-beginY), (float)(relx+m_vertices[i+1]->x-beginX), (float)(rely+m_vertices[i+1]->y-beginY));
        if (ln->GetPointDistance((float)px, (float)py) <= dist)
        {
            delete ln;
            return true;
        }
        delete ln;
    }

    return false;
}

float GroundChunk::GetMinimalDistance(uint32 relx, uint32 rely, uint32 px, uint32 py)
{
    if (m_vertices.size() < 2)
        return -1.0f;

    float tmpdist = -1.0f;

    double gabs = m_parsedExpression->GetAbsoluteValue();

    int32 beginX = (int32)m_vertices[0]->x;
    int32 beginY = (int32)(m_vertices[0]->y+gabs);

    uint32 i;
    Line* ln = NULL;
    float tmp;
    for (i = 0; i < m_vertices.size()-1; i++)
    {
        ln = new Line((float)(relx+m_vertices[i]->x-beginX), (float)(rely+m_vertices[i]->y-beginY), (float)(relx+m_vertices[i+1]->x-beginX), (float)(rely+m_vertices[i+1]->y-beginY));
        ln->SetLimit((float)(relx+m_vertices[i]->x-beginX), (float)(rely+m_vertices[i]->y-beginY), (float)(relx+m_vertices[i+1]->x-beginX), (float)(rely+m_vertices[i+1]->y-beginY));

        tmp = ln->GetPointDistance((float)px, (float)py);

        if (tmpdist < 0.0f || tmp < tmpdist)
            tmpdist = tmp;

        delete ln;
    }

    return tmpdist;
}
