#ifndef TDW_EXPRESSION_PARSER_H
#define TDW_EXPRESSION_PARSER_H

enum MathOperation
{
    OP_ADD      = 0,    // addition (includes subtracting as addition of negative value)
    OP_MULTIPLY = 1,    // multiplication
    OP_DIVIDE   = 2,    // division
    OP_MODULO   = 3,    // modulo
    OP_POW      = 4,    // power (exponent following)
    OP_MAX
};

enum ValueType
{
    VT_EXPRESSION = 0,  // another expression inside
    VT_INTEGER    = 1,  // integer type (cast as long)
    VT_FLOAT      = 2,  // floating point value (cast as double)
    VT_STRING     = 3,  // string value (cast as char* )
    VT_MAX
};

class ExprVector;
typedef ExprVector ExpressionVector;

struct ExprTreeElem;
typedef ExprTreeElem ExpressionTreeElement;

struct ExprTreeElem
{
    union valueUnion
    {
        double    asDouble;
        long      asLong;
        char*     asString;
    };

    ExprTreeElem()
    {
        polarity = true;
        valueType = VT_EXPRESSION;
    }
    ExprTreeElem(bool p_polarity, ValueType p_valueType, valueUnion p_val)
    {
        polarity = p_polarity;
        valueType = p_valueType;
        switch (valueType)
        {
            case VT_INTEGER:
                value.asLong = p_val.asLong;
                break;
            case VT_FLOAT:
                value.asDouble = p_val.asDouble;
                break;
            case VT_STRING:
                value.asString = p_val.asString;
                break;
            case VT_EXPRESSION:
            default:
                break;
        }
    }
    ValueType getEvaluableType();
    void SimplifyChildren();
    double GetAbsoluteValue();

    bool polarity; // positive (true) / negative (false)

    ValueType valueType;
    union valueUnion value;
    std::vector<ExpressionTreeElement*> items;

    // in case of expression type, there has to be an mathematic operation defined
    MathOperation operation;
};

class ExpressionParser
{
    public:
        enum Entity
        {
            EN_INVALID   = 0, // start/end of expression
            EN_VALUE     = 1,
            EN_PLUS      = 2,
            EN_MINUS     = 3,
            EN_MULT      = 4,
            EN_DIV       = 5,
            EN_MODULO    = 6,
            EN_LEFT_PAR  = 7,
            EN_RIGHT_PAR = 8,
            EN_POW       = 9,
            EN_UNDEFINED
        };
        enum EntityCathegory
        {
            EC_VALUE       = 0,
            EC_OPERATOR    = 1,
            EC_PARENTHESIS = 2,
            EC_MAX
        };

        static Entity charEntity(char input)
        {
            switch (input)
            {
                case '+': return EN_PLUS;
                case '-': return EN_MINUS;
                case '*': return EN_MULT;
                case '/': return EN_DIV;
                case '%': return EN_MODULO;
                case '(': return EN_LEFT_PAR;
                case ')': return EN_RIGHT_PAR;
                case '^': return EN_POW;
                default:
                    return EN_UNDEFINED;
            }
        }
        static char entityChar(Entity input)
        {
            switch (input)
            {
                case EN_PLUS:      return '+';
                case EN_MINUS:     return '-';
                case EN_MULT:      return '*';
                case EN_DIV:       return '/';
                case EN_MODULO:    return '%';
                case EN_LEFT_PAR:  return '(';
                case EN_RIGHT_PAR: return ')';
                case EN_POW:       return '^';
                default:
                    return 0;
            }
        }
        static EntityCathegory entityCathegory(Entity input)
        {
            switch (input)
            {
                case EN_VALUE:
                    return EC_VALUE;
                case EN_PLUS:
                case EN_MINUS:
                case EN_MULT:
                case EN_DIV:
                case EN_MODULO:
                case EN_POW:
                    return EC_OPERATOR;
                case EN_LEFT_PAR:
                case EN_RIGHT_PAR:
                    return EC_PARENTHESIS;
            }
            return EC_VALUE;
        }

        static ExpressionVector* Parse(const char* input);
        static bool CheckSyntax(ExpressionVector* input);
        static ExpressionTreeElement* BuildTree(ExpressionVector* input, uint32 start, uint32 count);

        static ExpressionTreeElement* BuildValueElement(const char* input);
};

class ExprVector: public std::vector<ExpressionParser::Entity>
{
    public:
        ExprVector(): std::vector<ExpressionParser::Entity>()
        {
            pos = 0;
        }

        ExpressionParser::Entity prev()
        {
            if (pos == 0)
                return ExpressionParser::EN_INVALID;

            return (*this)[--pos];
        }

        ExpressionParser::Entity actual()
        {
            if (size() == 0)
                return ExpressionParser::EN_INVALID;

            return (*this)[pos];
        }

        ExpressionParser::Entity next()
        {
            if (pos >= size())
                return ExpressionParser::EN_INVALID;

            return (*this)[++pos];
        }

        void push(ExpressionParser::Entity ent)
        {
            push_back(ent);
            m_valueVector.push_back(NULL);
        }

        void push(ExpressionParser::Entity ent, const char* val)
        {
            push_back(ent);
            m_valueVector.push_back(new std::string(val));
        }

        void rewind()
        {
            pos = 0;
        }

        const char* getValue()
        {
            if (size() == 0 || pos >= m_valueVector.size() || (*this)[pos] != ExpressionParser::EN_VALUE)
                return NULL;

            return m_valueVector[pos]->c_str();
        }
        const char* getPrevValue()
        {
            if (pos == 0 || size() == 0 || pos >= m_valueVector.size() || (*this)[pos-1] != ExpressionParser::EN_VALUE)
                return NULL;

            return m_valueVector[pos-1]->c_str();
        }
        const char* getNextValue()
        {
            if (size() == 0 || pos+1 >= size() || pos+1 >= m_valueVector.size() || (*this)[pos+1] != ExpressionParser::EN_VALUE)
                return NULL;

            return m_valueVector[pos+1]->c_str();
        }

        const char* getValue(uint32 offset)
        {
            if (offset >= size() || offset >= m_valueVector.size() || (*this)[offset] != ExpressionParser::EN_VALUE)
                return NULL;

            return m_valueVector[offset]->c_str();
        }

    private:
        uint32 pos;
        std::vector<std::string*> m_valueVector;
};

double NumerateExpression(ExpressionTreeElement* expr, double (*refValueFunction)(const char*));

void ExpressionParserUnitTest();

#endif
