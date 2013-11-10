#ifndef TDW_CONFIG_H
#define TDW_CONFIG_H

#include <Singleton.h>

enum ConfigBoolValues
{
    CONFIG_FULLSCREEN = 0,
    CONFIG_BOOL_MAX
};

enum ConfigIntValues
{
    CONFIG_WINDOW_WIDTH  = 0,
    CONFIG_WINDOW_HEIGHT = 1,
    CONFIG_INT_MAX
};

enum ConfigStringValues
{
    CONFIG_STRING_MAX,
};

class ConfigMgr
{
    public:
        ConfigMgr();
        bool Load();

        bool GetBoolValue(ConfigBoolValues val);
        int64 GetIntValue(ConfigIntValues val);
        const char* GetStringValue(ConfigStringValues val);

    private:
        std::vector<bool> m_boolValues;
        std::vector<int64> m_intValues;
        std::vector<std::string> m_stringValues;
};

#define sConfig Singleton<ConfigMgr>::instance()

// macros for often used values
#define WIDTH sConfig->GetIntValue(CONFIG_WINDOW_WIDTH)
#define HEIGHT sConfig->GetIntValue(CONFIG_WINDOW_HEIGHT)

#endif
