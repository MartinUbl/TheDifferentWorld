#include <Global.h>
#include <Config.h>

ConfigMgr::ConfigMgr()
{
    m_boolValues.resize(CONFIG_BOOL_MAX);
    m_intValues.resize(CONFIG_INT_MAX);
    m_stringValues.resize(CONFIG_STRING_MAX);

    uint32 i;
    for (i = 0; i < CONFIG_BOOL_MAX; i++)
        m_boolValues[i] = false;

    for (i = 0; i < CONFIG_INT_MAX; i++)
        m_intValues[i] = 0;

    for (i = 0; i < CONFIG_STRING_MAX; i++)
        m_stringValues[i] = "";
}

bool ConfigMgr::Load()
{
    // for now, just load default values
    // TODO: DO

    m_boolValues[CONFIG_FULLSCREEN] = false;
    m_intValues[CONFIG_WINDOW_WIDTH] = 800;
    m_intValues[CONFIG_WINDOW_HEIGHT] = 600;

    return true;
}

bool ConfigMgr::GetBoolValue(ConfigBoolValues val)
{
    if (val != CONFIG_BOOL_MAX)
        return m_boolValues[val];

    return false;
}

int64 ConfigMgr::GetIntValue(ConfigIntValues val)
{
    if (val != CONFIG_INT_MAX)
        return m_intValues[val];

    return 0;
}

const char* ConfigMgr::GetStringValue(ConfigStringValues val)
{
    if (val != CONFIG_STRING_MAX)
        return m_stringValues[val].c_str();

    return "";
}
