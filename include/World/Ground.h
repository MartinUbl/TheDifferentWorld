#ifndef TDW_GROUND_H
#define TDW_GROUND_H

#include <ExpressionParser.h>

// one line per 5 pixels
#define GROUND_GRANULARITY 5

struct FuncVertex
{
    double x;
    double y;
};

// ground chunk is made of mathematical function in some interval
class GroundChunk
{
    public:
        GroundChunk(const char* expression, int64 interval_start, int64 interval_end, float ratio = 1.0f);
        ~GroundChunk();

        void Create(const char* expression, int64 interval_start, int64 interval_end, float ratio);
        void CleanUp();
        void ReCreate(const char* expression, int64 interval_start, int64 interval_end, float ratio);

        void Draw(double startX, double startY);

        bool IsPointInDistance(uint32 relx, uint32 rely, uint32 px, uint32 py, float distance);
        float GetMinimalDistance(uint32 relx, uint32 rely, uint32 px, uint32 py);
        double GetLastVertexX() { return m_lastVertex.x; };
        double GetLastVertexY() { return m_lastVertex.y; };
        float GetRatio() { return m_ratio; };
        const char* GetOriginalExpression() { return m_originalExpression.c_str(); };
    private:
        ExpressionTreeElement* m_parsedExpression;
        std::vector<FuncVertex*> m_vertices;
        std::string m_originalExpression;

        float m_ratio;
        FuncVertex m_lastVertex;
};

#endif
