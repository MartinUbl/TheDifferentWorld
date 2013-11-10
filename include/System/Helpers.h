#ifndef EXCDR_HELPERS_H
#define EXCDR_HELPERS_H

extern char* CharVectorToString(std::vector<char>* vect);
extern char* ExtractFolderFromPath(const char* input);
extern char* ExtractFilenameFromPath(const char* input);
extern char* MakeFilePath(const char* dir, const char* filename);

extern char* LeftSide(const char* input, const char delim);
extern char* RightSide(const char* input, const char delim);

extern float* ParseVector2(char* input, char delim);

extern char* RemoveBeginningSpaces(const char* input);

extern bool EqualString(const char* first, const char* second, bool caseInsensitive = false);
extern bool EqualString(const char* first, const char* second);
extern int ContainsString(const char* str, const char* substr);
extern bool IsNumeric(const char* inp);
extern int ToInt(const char* inp);
extern char UpperChar(char inp);
extern char LowerChar(char inp);
extern const char* ToUppercase(const char* input);
extern const char* ToLowercase(const char* input);

extern const wchar_t* ToWideString(const char* input);
extern const char*    ToMultiByteString(const wchar_t* input);

#define IN_SQUARE(x, y, a, b, c, d) (x >= a && x <= c && y >= b && y <= d)

#ifdef _WIN32
inline uint32 getMSTime() { return GetTickCount(); }
#else
inline uint32 getMSTime()
{
    struct timeval tv;
    struct timezone tz;
    gettimeofday( &tv, &tz );
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}
#endif

inline uint32 getMSTimeDiff(uint32 oldMSTime, uint32 newMSTime)
{
    // getMSTime() have limited data range and this is case when it overflow in this tick
    if (oldMSTime > newMSTime)
        return (0xFFFFFFFF - oldMSTime) + newMSTime;
    else
        return newMSTime - oldMSTime;
}

#endif
