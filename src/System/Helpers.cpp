#include "Global.h"
#include "Helpers.h"

#include <sstream>

char* CharVectorToString(std::vector<char>* vect)
{
    if (!vect)
        return NULL;

    char* ret = new char[vect->size()+1];
    uint32 i;
    std::string a;
    for (i = 0; i < vect->size(); i++)
        a += (*vect)[i];

    strncpy(ret, a.c_str(), a.size());
    ret[vect->size()] = L'\0';

    return ret;
}

char* ExtractFolderFromPath(const char* input)
{
    for (int32 i = strlen(input); i >= 0; i--)
        if (input[i] == '\\')
        {
            char* tmp = new char[i+1];
            strncpy(tmp, input, i);
            tmp[i] = '\0';
            return tmp;
        }

    return NULL;
}

char* ExtractFilenameFromPath(const char* input)
{
    for (int32 i = strlen(input); i > 0; i--)
    {
        if (input[i-1] == '\\')
        {
            char* tmp = new char[strlen(input)-i+1];
            strncpy(tmp, (input+i), strlen(input)-i);
            tmp[strlen(input)-i] = '\0';
            return tmp;
        }
    }

    return NULL;
}

char* MakeFilePath(const char* dir, const char* filename)
{
    if (!dir)
        return (char*)filename;

    if (!filename)
        return NULL;

    char* tmp = NULL;

    if (dir[strlen(dir)-1] != '\\')
    {
        tmp = new char[strlen(dir)+1+strlen(filename)+1];
        sprintf(tmp, "%s\\%s", dir, filename);
    }
    else
    {
        tmp = new char[strlen(dir)+strlen(filename)+1];
        sprintf(tmp, "%s%s", dir, filename);
    }

    return tmp;
}

char* LeftSide(const char* input, const char delim)
{
    if (!input || strlen(input) < 1)
        return NULL;

    for (uint32 i = 0; i < strlen(input); i++)
    {
        if (input[i] == delim)
        {
            char* tmp = new char[i+1];
            strncpy(tmp, input, i);
            tmp[i] = '\0';
            return tmp;
        }
    }

    return (char*)input;
}

char* RightSide(const char* input, const char delim)
{
    if (!input || strlen(input) < 1)
        return NULL;

    for (uint32 i = 0; i < strlen(input); i++)
    {
        if (input[i] == delim)
        {
            char* tmp = new char[strlen(input)-i+1];
            strncpy(tmp, (input+i+1), strlen(input)-i-1);
            tmp[strlen(input)-i-1] = '\0';
            return tmp;
        }
    }

    return NULL;
}

bool EqualString(const char* first, const char* second, bool caseInsensitive)
{
    if (strlen(first) != strlen(second))
        return false;

    if (caseInsensitive)
    {
        for (uint32 i = 0; i < strlen(first); i++)
            if (UpperChar(first[i]) != UpperChar(second[i]))
                return false;
    }
    else
    {
        for (uint32 i = 0; i < strlen(first); i++)
            if (first[i] != second[i])
                return false;
    }

    return true;
}

bool EqualString(const char* first, const char* second)
{
    if (strlen(first) != strlen(second))
        return false;

    for (uint32 i = 0; i < strlen(first); i++)
        if (first[i] != second[i])
            return false;

    return true;
}

int ContainsString(const char* str, const char* substr)
{
    if (!str || !substr)
        return -1;

    if (strlen(substr) > strlen(str))
        return -1;

    bool wrong;

    for (uint32 i = 0; i < strlen(str)-strlen(substr); i++)
    {
        if (str[i] == substr[0])
        {
            wrong = false;
            for (uint32 j = 0; j < strlen(substr); j++)
            {
                if (str[i+j] != substr[j])
                {
                    wrong = true;
                    break;
                }
            }

            if (!wrong)
                return i;
        }
    }

    return -1;
}

bool IsNumeric(const char* inp)
{
    if (!inp || strlen(inp) < 1)
        return false;

    for (uint32 i = 0; i < strlen(inp); i++)
    {
        if (inp[i] == L'-')
        {
            if (i != 0)
                return false;
        }
        else if (inp[i] < L'0' || inp[i] > L'9')
            return false;
    }

    return true;
}

int ToInt(const char* inp)
{
    return atoi(inp);
}

char* RemoveBeginningSpaces(const char* input)
{
    if (!input)
        return NULL;

    if (input[0] != ' ')
        return (char*)input;

    for (uint32 i = 0; i < strlen(input); i++)
    {
        if (input[i] != ' ')
        {
            char* tmp = new char[strlen(input)-i];
            strncpy(tmp, (input+i), strlen(input)-i);
            tmp[strlen(input)-i] = '\0';
            return tmp;
        }
    }

    return NULL;
}

float* ParseVector2(char* input, char delim)
{
    float* vec = NULL;

    char* ls = LeftSide(input, delim);
    char* rs = RightSide(input, delim);

    if (ls && rs && IsNumeric(ls) && IsNumeric(rs))
    {
        vec = new float[2];
        vec[0] = (float)ToInt(ls);
        vec[1] = (float)ToInt(rs);
    }

    return vec;
}

static const char* patt_lowcase = {"abcdefghijklmnopqrstuvwxyz"};
static const char* patt_upcase  = {"ABCDEFGHIJKLMNOPQRSTUVWXYZ"};

char UpperChar(char inp)
{
    // check if it's already upcase char
    // only suitable for non-UTF-8 continual line of chars !!!!!!
    if (inp >= patt_upcase[0] && inp <= patt_upcase[strlen(patt_upcase)])
        return inp;

    for (uint32 i = 0; i < strlen(patt_lowcase); i++)
        if (patt_lowcase[i] == inp)
            return patt_upcase[i];

    return inp;
}

char LowerChar(char inp)
{
    // check if it's already lowcase char
    // only suitable for non-UTF-8 continual line of chars !!!!!!
    if (inp >= patt_lowcase[0] && inp <= patt_lowcase[strlen(patt_lowcase)])
        return inp;

    for (uint32 i = 0; i < strlen(patt_upcase); i++)
        if (patt_upcase[i] == inp)
            return patt_lowcase[i];

    return inp;
}

const char* ToUppercase(const char* input)
{
    char* ret = new char[strlen(input)+1];

    for (uint32 i = 0; i < strlen(input); i++)
        ret[i] = UpperChar(input[i]);

    ret[strlen(input)] = '\0';

    return ret;
}

const char* ToLowercase(const char* input)
{
    std::stringstream ss;

    for (uint32 i = 0; i < strlen(input); i++)
        ss << LowerChar(input[i]);

    ss << char(0);

    char* pp = new char[ss.str().size()];
    strncpy(pp, ss.str().c_str(), ss.str().size());

    return pp;
}

const wchar_t* ToWideString(const char* input)
{
    const size_t origsize = strlen(input)+1;
    wchar_t* wc = new wchar_t[origsize];
    mbstowcs(wc, input, origsize);

    return wc;
}

const char* ToMultiByteString(const wchar_t* input)
{
    const size_t origsize = wcslen(input)+1;
    char* mbc = new char[origsize];
    wcstombs(mbc, input, origsize);

    return mbc;
}
