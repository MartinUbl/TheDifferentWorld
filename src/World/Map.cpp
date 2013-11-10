#include <Global.h>
#include <Map.h>
#include <Helpers.h>
#include <Config.h>
#include <Application.h>
#include <Stages.h>

Map::Map()
{
    m_selectedChainElement = NULL;
    m_selectedChain = NULL;
    m_clickedChainElement = NULL;
}

void Map::InitEmpty()
{
    m_chains.clear();

    FunctionChain* mainch = new FunctionChain();
    mainch->beginX = 0;
    mainch->beginY = 230;

    FunctionChainElement* fc = new FunctionChainElement();
    fc->prev = NULL;
    fc->next = NULL;
    fc->parent = mainch;
    fc->ground = new GroundChunk("0",0,150);

    // and link the first chain element (it will contain next chain elements in "next" pointer)
    mainch->chain = fc;

    fc->next = new FunctionChainElement();
    fc->next->prev = fc;
    fc->next->next = NULL;
    fc->next->parent = mainch;
    //fc->next->ground = new GroundChunk("-x^3+5*x^2+x",-60,100,0.05f);
    fc->next->ground = new GroundChunk("x",-50,50, 0.5f);

    m_chains.push_back(mainch);
}

void Map::Load(const char *path)
{
    /*FILE* f = fopen(path, "r");

    m_chunks.clear();
    m_chunkStarts.clear();

    if (!f)
        return;

    char *buf = new char[256];
    char *left, *right;
    while (fgets(buf, 255, f))
    {
        left = LeftSide(buf, ';');
        right = RightSide(buf, ';');
    }*/
}

void Map::Draw()
{
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
    uint32 mouseX = sApplication->GetMouseX(), mouseY = sApplication->GetMouseY();

    bool editor = false;
    if (sGame->IsEditorMode())
        editor = true;
    else
        glColor3ub(0,0,0);

    bool selected = false;
    FunctionChain* tmpch;
    FunctionChainElement* fche;
    for (uint32 i = 0; i < m_chains.size(); i++)
    {
        tmpch = m_chains[i];
        fche = tmpch->chain;
        while (fche)
        {
            if (fche->prev)
            {
                if (editor)
                {
                    if (fche == m_clickedChainElement)
                        glColor3ub(63,195,63);
                    else if (!selected && fche->ground->IsPointInDistance((uint32)fche->prev->ground->GetLastVertexX(), (uint32)fche->prev->ground->GetLastVertexY(), mouseX, mouseY, 20.0f))
                    {
                        selected = true;
                        glColor3ub(255,0,0);
                        m_selectedChainElement = fche;
                        m_selectedChain = tmpch;
                    }
                    else
                        glColor3ub(0,0,0);
                }

                fche->ground->Draw(fche->prev->ground->GetLastVertexX(), fche->prev->ground->GetLastVertexY());
            }
            else
            {
                if (editor)
                {
                    if (fche == m_clickedChainElement)
                        glColor3ub(0,255,0);
                    else if (!selected && fche->ground->IsPointInDistance((uint32)tmpch->beginX, (uint32)HEIGHT-tmpch->beginY, mouseX, mouseY, 20.0f))
                    {
                        selected = true;
                        glColor3ub(255,0,0);
                        m_selectedChainElement = fche;
                        m_selectedChain = tmpch;
                    }
                    else
                        glColor3ub(0,0,0);
                }

                fche->ground->Draw(tmpch->beginX, HEIGHT-tmpch->beginY);
            }

            fche = fche->next;
        }
    }

    if (!selected)
    {
        m_selectedChainElement = NULL;
        m_selectedChain = NULL;
    }
}

void Map::SetClickedSelectedChainElement(bool set)
{
    if (set)
    {
        m_clickedChainElement = m_selectedChainElement;
        m_clickedChain = m_selectedChain;
    }
    else
    {
        m_clickedChainElement = NULL;
        m_clickedChain = NULL;
    }
}

void Map::DeleteChainElement(FunctionChainElement *el)
{
    // cannot delete the only one chain element in chain
    if (!el->prev && !el->next)
        return;

    if (el->prev)
        el->prev->next = el->next;
    if (el->next)
        el->next->prev = el->prev;

    if (el->parent->chain == el)
    {
        if (el->next)
            el->parent->chain = el->next;
        else
            el->parent->chain = el->prev;
    }

    if (m_selectedChainElement == el)
        m_selectedChainElement = NULL;
    if (m_clickedChainElement == el)
        m_clickedChainElement = NULL;

    delete el->ground;
    delete el;
}
