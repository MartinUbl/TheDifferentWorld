#ifndef TDW_MAP_H
#define TDW_MAP_H

#include <Ground.h>

struct FunctionChain;
struct FunctionChainElement;

struct FunctionChainElement
{
    FunctionChain* parent;
    FunctionChainElement *prev, *next;
    GroundChunk* ground;
};

struct FunctionChain
{
    uint32 beginX, beginY;
    FunctionChainElement* chain;
};

class Map
{
    public:
        Map();
        void InitEmpty();
        void Load(const char* path);

        void Draw();

        // Editor mode stuff
        FunctionChainElement* GetSelectedChainElement() { return m_selectedChainElement; };
        FunctionChain* GetSelectedChain() { return m_selectedChain; };
        void SetClickedSelectedChainElement(bool set);
        FunctionChainElement* GetClickedChainElement() { return m_clickedChainElement; };

        void DeleteChainElement(FunctionChainElement* el);

    private:
        std::vector<FunctionChain*> m_chains;
        FunctionChainElement* m_selectedChainElement;
        FunctionChain* m_selectedChain;
        FunctionChainElement* m_clickedChainElement;
        FunctionChain* m_clickedChain;
};

#endif
