#ifndef TDW_TEXTURES_H
#define TDW_TEXTURES_H

#include <Singleton.h>

struct TextureEntry
{
    std::string filename;
    int32 internalId;
};

class TextureStorage
{
    public:
        TextureStorage();

        void LoadTextureDatabase();
        int32 GetTextureInternalId(uint32 id);
    private:
        std::map<uint32, TextureEntry*> m_textureMap;
};

#define sTextures Singleton<TextureStorage>::instance()

#define TEXTURE(x) sTextures->GetTextureInternalId(x)

#endif
