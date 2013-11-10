#include <Global.h>
#include <Textures.h>
#include <FileDatabase.h>
#include <Paths.h>
#include <Helpers.h>

TextureStorage::TextureStorage()
{
    m_textureMap.clear();
}

void TextureStorage::LoadTextureDatabase()
{
    Query* qry = DatabaseHandler::GetTableContents(DT_MEDIA, "textures");
    if (!qry)
    {
        m_textureMap.clear();
        return;
    }

    uint32 textureId;
    while (qry->fetch_row())
    {
        textureId = qry->getval();
        m_textureMap[textureId] = new TextureEntry;
        m_textureMap[textureId]->filename = qry->getstr();

        m_textureMap[textureId]->internalId = -1; // not loaded in memory
    }

    DatabaseHandler::CleanupQuery(qry);
}

int32 TextureStorage::GetTextureInternalId(uint32 id)
{
    std::map<uint32, TextureEntry*>::iterator itr = m_textureMap.find(id);
    if (itr == m_textureMap.end() || !(*itr).second)
        return -1;

    if ((*itr).second->internalId == -1)
    {
        (*itr).second->internalId = sSimplyFlat->TextureStorage->LoadTexture(MakeFilePath(DATA_PATH, (*itr).second->filename.c_str()), 0);

        // Still not loaded = missing / invalid
        if ((*itr).second->internalId == -1)
            (*itr).second->internalId = -2;
    }

    return (*itr).second->internalId;
}
