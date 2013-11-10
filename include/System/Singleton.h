#ifndef CUB_SINGLETON_H
#define CUB_SINGLETON_H

#include <assert.h>

template <class T>
class Singleton
{
    public:
        static T* instance()
        {
            if (!m_instance)
                m_instance = new T;

            assert(m_instance != NULL);

            return m_instance;
        }
        static void cleanup()
        {
            if (m_instance)
            {
                delete m_instance;
                m_instance = NULL;
            }
        }
        static bool exists()
        {
            return (m_instance != NULL);
        }

    private:
        static T* m_instance;
};

template <class T> T* Singleton<T>::m_instance = NULL;

template <class T>
void cleanupSingleton()
{
    Singleton<T>::cleanup();
}

template <class T>
bool instanceExists()
{
    return Singleton<T>::exists();
}

// polymorphic singleton - store derivated class and return base
template <class T, class V>
class PolySingleton
{
    public:
        static V* instance()
        {
            if (!m_instance)
                m_instance = new V;

            assert(m_instance != NULL);

            return (V*)m_instance;
        }
        static void cleanup()
        {
            if (m_instance)
            {
                delete m_instance;
                m_instance = NULL;
            }
        }
        static bool exists()
        {
            return (m_instance != NULL);
        }

    private:
        static T* m_instance;
};

template <class T, class V> T* PolySingleton<T, V>::m_instance = NULL;

template <class T, class V>
void cleanupSingleton()
{
    PolySingleton<T, V>::cleanup();
}

template <class T, class V>
bool instanceExists()
{
    return PolySingleton<T, V>::exists();
}

#endif
