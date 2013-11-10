#include <Global.h>
#include <Helpers.h>
#include <ExpressionParser.h>
#include <assert.h>

ExpressionVector* ExpressionParser::Parse(const char *input)
{
    if (!input || strlen(input) == 0)
        return NULL;

    ExpressionVector* ret = new ExpressionVector;

    Entity en = EN_UNDEFINED;

    uint32 i, j;
    for (i = 0; i < strlen(input); )
    {
        // try to determine, if it's an operator or parenthesis
        en = charEntity(input[i]);
        if (en != EN_UNDEFINED)
        {
            ret->push(en);
            i++;
            continue;
        }

        // it's a value
        for (j = i; j < strlen(input); j++)
        {
            en = charEntity(input[j]);
            if (en != EN_UNDEFINED || j == strlen(input)-1)
            {
                if (j == strlen(input)-1)
                {
                    char* val = new char[j-i+2];
                    strncpy(val, &(input[i]), j-i+1);
                    val[j-i+1] = '\0';
                    ret->push(EN_VALUE, val);

                    // in case of parenthesis at the end of expression
                    en = charEntity(input[j]);
                    if (en != EN_UNDEFINED)
                        ret->push(en);

                    i = j+1;
                }
                else
                {
                    char* val = new char[j-i+1];
                    strncpy(val, &(input[i]), j-i);
                    val[j-i] = '\0';
                    ret->push(EN_VALUE, val);
                    i = j;

                    goto continue_label;
                }
            }
        }
continue_label:;
    }

    return ret;
}

bool ExpressionParser::CheckSyntax(ExpressionVector* input)
{
    // empty or non-existant vectors are valid, and they are evaluated as zero in worker function, so we shall ignore it
    if (!input || input->empty())
        return true;

    // also input with only one element has to be containing only value element, otherwise it's evaluated as invalid
    if (input->size() == 1)
    {
        if ((*input)[0] == EN_VALUE)
            return true;
        else
            return false;
    }

    // There can't be more than one element type in a row (except parentheses)
    // We should say, that there are hypothetically three cathegories of element types:
    //   1. values (both numbers and strings, as references)
    //   2. operators
    //   3. parentheses
    int32 deepness = 0;
    for (uint32 i = 0; i < input->size()-1; i++)
    {
        if ((*input)[i] == EN_LEFT_PAR)
            deepness++;

        if ((*input)[i] == EN_RIGHT_PAR)
            deepness--;

        if (entityCathegory((*input)[i]) != EC_PARENTHESIS && entityCathegory((*input)[i]) == entityCathegory((*input)[i+1]))
            return false;
    }

    // parentheses aren't enclosed properly
    if (deepness != 0)
        return false;

    return true;
}

ExpressionTreeElement* ExpressionParser::BuildTree(ExpressionVector *input, uint32 start, uint32 count)
{
    if (!input || input->empty() || count == 0)
        return NULL;

    if (count == 1)
    {
        if ((*input)[start] == EN_VALUE)
            return BuildValueElement((*input).getValue(start));
        else
            return NULL;
    }
    if (count == 2 && ((*input)[start] == EN_PLUS || (*input)[start] == EN_MINUS))
    {
        if ((*input)[start+1] == EN_VALUE)
        {
            ExpressionTreeElement* tmp = BuildValueElement((*input).getValue(start+1));

            // remain from expression preparsing, does it have to be there anyways?
            // polarity of single element is decided when parsing addition elements one by one
            /*if ((*input)[start] == EN_PLUS)
                tmp->polarity = true;
            else if ((*input)[start] == EN_MINUS)
                tmp->polarity = false;*/

            return tmp;
        }
    }

    int32 deepness = 0;
    uint32 i;

    ExpressionTreeElement* tmp = new ExpressionTreeElement;
    tmp->valueType = VT_EXPRESSION;
    tmp->items.clear();

    if (((*input)[start] == EN_MINUS || (*input)[start] == EN_PLUS) && (*input)[start+1] == EN_LEFT_PAR && (*input)[start+count-1] == EN_RIGHT_PAR)
    {
        start += 2;
        count -= 3;
    }
    else if ((*input)[start] == EN_LEFT_PAR && (*input)[start+count-1] == EN_RIGHT_PAR)
    {
        // we must verify, if the input is only one parenthesis block, or if its like (exp)/(exp2), where this condition is met, but will result in
        // wrong tree because of parsing the "inside" of the edge parenthesis, like "exp)/(exp2", which is, obviously, wrong

        bool singlePar = true;
        for (i = start+1; i < start+count-1; i++)
        {
            if ((*input)[i] == EN_LEFT_PAR)
                deepness++;

            if ((*input)[i] == EN_RIGHT_PAR)
            {
                if (deepness > 0)
                    deepness--;
                else
                {
                    singlePar = false;
                    break;
                }
            }
        }

        if (singlePar)
        {
            start += 1;
            count -= 2;
        }
    }

    uint32 lastBreak = start;

    // thanks to saving polarity of every element, it does not depend on the order of addition elements

    ExpressionTreeElement* child = NULL;

    // at first parse the simple addition elements (only values)
    for (i = start; i < input->size() && i < start+count; i++)
    {
        if ((*input)[i] == EN_LEFT_PAR)
            deepness++;

        if ((*input)[i] == EN_RIGHT_PAR)
            deepness--;

        // if the string doesn't begin with polarity unary operator and we are on the top scope
        if (i != start && deepness == 0 && ((*input)[i] == EN_PLUS || (*input)[i] == EN_MINUS || (lastBreak != start && (i == input->size()-1 || i == start+count-1)) ))
        {
            if (i == input->size()-1 || i == start+count-1)
            {
                child = BuildTree(input, lastBreak, i-lastBreak+1);
                if ((*input)[lastBreak] == EN_MINUS)
                    child->polarity = false;

                tmp->items.push_back(child);
            }
            else
            {
                child = BuildTree(input, lastBreak, i-lastBreak);
                if ((*input)[lastBreak] == EN_MINUS)
                    child->polarity = false;

                tmp->items.push_back(child);
            }

            lastBreak = i;
        }
    }

    deepness = 0;

    // If parser already found some dividable elements, we won't care about them anymore, call the operation "addition", and parse inner expressions
    if (lastBreak != start)
        tmp->operation = OP_ADD;
    else
    {
        for (i = start; i < input->size() && i < start+count; i++)
        {
            if ((*input)[i] == EN_LEFT_PAR)
                deepness++;

            if ((*input)[i] == EN_RIGHT_PAR)
                deepness--;

            // if the string doesn't begin with polarity unary operator and we are on the top scope
            if (i != start && deepness == 0 && ((*input)[i] == EN_MULT || (lastBreak != start && (i == input->size()-1 || i == start+count-1))))
            {
                // we are at the end
                if (i == input->size()-1 || i == start+count-1)
                    tmp->items.push_back(BuildTree(input, lastBreak+1, i-lastBreak));
                // we are at the very beginning
                else if (lastBreak == start)
                    tmp->items.push_back(BuildTree(input, lastBreak, i-lastBreak));
                // or we are in the middle.. somewhere..
                else
                    tmp->items.push_back(BuildTree(input, lastBreak+1, i-lastBreak-1));

                lastBreak = i;
            }
        }

        deepness = 0;

        if (lastBreak != start)
            tmp->operation = OP_MULTIPLY;
        else
        {
            for (i = start; i < input->size() && i < start+count; i++)
            {
                if ((*input)[i] == EN_LEFT_PAR)
                    deepness++;

                if ((*input)[i] == EN_RIGHT_PAR)
                    deepness--;

                // if the string doesn't begin with polarity unary operator and we are on the top scope
                if (i != start && deepness == 0 && ((*input)[i] == EN_MODULO || (lastBreak != start && (i == input->size()-1 || i == start+count-1))))
                {
                    if (i == input->size()-1 || i == start+count-1)
                        tmp->items.push_back(BuildTree(input, lastBreak+1, i-lastBreak));
                    else
                        tmp->items.push_back(BuildTree(input, lastBreak, i-lastBreak));

                    lastBreak = i;
                }
            }

            deepness = 0;

            if (lastBreak != start)
                tmp->operation = OP_MODULO;
            else
            {
                for (i = start; i < input->size() && i < start+count; i++)
                {
                    if ((*input)[i] == EN_LEFT_PAR)
                        deepness++;

                    if ((*input)[i] == EN_RIGHT_PAR)
                        deepness--;

                    // if the string doesn't begin with polarity unary operator and we are on the top scope
                    if (i != start && deepness == 0 && ((*input)[i] == EN_DIV || (lastBreak != start && (i == input->size()-1 || i == start+count-1))))
                    {
                        if (i == input->size()-1 || i == start+count-1)
                            tmp->items.push_back(BuildTree(input, lastBreak+1, i-lastBreak));
                        else
                            tmp->items.push_back(BuildTree(input, lastBreak, i-lastBreak));

                        lastBreak = i;
                    }
                }

                deepness = 0;

                if (lastBreak != start)
                    tmp->operation = OP_DIVIDE;
                else
                {
                    for (i = start; i < input->size() && i < start+count; i++)
                    {
                        if ((*input)[i] == EN_LEFT_PAR)
                            deepness++;

                        if ((*input)[i] == EN_RIGHT_PAR)
                            deepness--;

                        // if the string doesn't begin with polarity unary operator and we are on the top scope
                        if (i != start && deepness == 0 && ((*input)[i] == EN_POW || (lastBreak != start && (i == input->size()-1 || i == start+count-1))))
                        {
                            if (i == input->size()-1 || i == start+count-1)
                                tmp->items.push_back(BuildTree(input, lastBreak+1, i-lastBreak));
                            else
                                tmp->items.push_back(BuildTree(input, lastBreak, i-lastBreak));

                            lastBreak = i;
                        }
                    }

                    if (lastBreak != start)
                        tmp->operation = OP_POW;
                }
            }

        }
    }

    return tmp;
}

ExpressionTreeElement* ExpressionParser::BuildValueElement(const char* input)
{
    if (!input || strlen(input) == 0)
        return NULL;

    ExpressionTreeElement* tmp = new ExpressionTreeElement();

    // integer type
    if (IsNumeric(input))
    {
        tmp->valueType = VT_INTEGER;
        tmp->value.asLong = ToInt(input);
        return tmp;
    }

    // floating type
    char *left = NULL, *right = NULL;
    left = LeftSide(input, '.');
    right = RightSide(input, '.');

    if (left && right && IsNumeric(left) && IsNumeric(right))
    {
        tmp->valueType = VT_FLOAT;
        tmp->value.asDouble = atof(input);
        return tmp;
    }

    // if it doesn't fit our defined integer or float pattern, let's call it string
    tmp->valueType = VT_STRING;
    tmp->value.asString = (char*)input;
    return tmp;
}

ValueType ExprTreeElem::getEvaluableType()
{
    switch (valueType)
    {
        // integer, float and string are final types
        case VT_INTEGER:
        case VT_FLOAT:
        case VT_STRING:
            return valueType;
        // expression have to contain some of final types
        case VT_EXPRESSION:
        {
            // not evaluable when no items are present
            if (items.empty())
                return VT_STRING;

            ValueType et = items[0]->getEvaluableType();
            if (et != VT_INTEGER && et != VT_FLOAT)
                return VT_STRING;

            for (uint32 i = 1; i < items.size(); i++)
            {
                if (items[i]->getEvaluableType() != et && (items[i]->getEvaluableType() == VT_INTEGER || items[i]->getEvaluableType() == VT_FLOAT))
                    et = VT_FLOAT;
                else if (items[i]->getEvaluableType() != et)
                    return VT_STRING;
            }

            return et;
        }
    }

    return VT_STRING;
}

void ExprTreeElem::SimplifyChildren()
{
    uint32 i;

    if (items.empty())
        return;

    // At first simplify recursively all the children to make it the simpliest it can be
    for (i = 0; i < items.size(); i++)
        if (items[i]->valueType == VT_EXPRESSION)
            items[i]->SimplifyChildren();

    // if it's a non-comutative operation (division, modulo, power), and one of operands is string or non-simplificable expression, do not simplify
    // when item is listed as expression even after calling SimplifyChildren on it, then it means, that there are some non-simplificable elements
    // like string in it. This means, we are not able to simplify it more
    if (valueType == VT_EXPRESSION)
    {
        switch (operation)
        {
            case OP_DIVIDE:
            case OP_MODULO:
            case OP_POW:
                for (i = 0; i < items.size(); i++)
                    if (items[i]->valueType == VT_STRING || items[i]->valueType == VT_EXPRESSION)
                        return;
                break;
            default:
                break;
        }
    }

    ValueType et = items[0]->valueType;
    bool changed = false;

    // Fill initial simplifying base value
    valueUnion vun;
    if (et == VT_INTEGER)
    {
        changed = true;
        vun.asLong = items[0]->value.asLong*((items[0]->polarity)?1:(-1));
        items.erase(items.begin());
    }
    else if (et == VT_FLOAT)
    {
        changed = true;
        vun.asDouble = items[0]->value.asDouble*((items[0]->polarity)?1:(-1));
        items.erase(items.begin());
    }
    else
    {
        et = VT_INTEGER;
        if (operation == OP_ADD)
            vun.asLong = 0;
        else
            vun.asLong = 1;
    }

    // Now iterate through all items and attempt to simplify every items - they have to be numerical (integer / float)
    for (i = 0; i < items.size(); )
    {
        if (items[i]->valueType == VT_INTEGER || items[i]->valueType == VT_FLOAT)
        {
            changed = true;

            // We need to change to floating point value type if some of operands are float
            if (items[i]->valueType != et)
            {
                et = VT_FLOAT;
                vun.asDouble = (double)vun.asLong;
            }

            // Calculate
            // involve polarity as multiplying right operand with 1 or -1
            switch (operation)
            {
                case OP_ADD:
                {
                    if (et == VT_INTEGER)
                        vun.asLong += items[i]->value.asLong*((items[i]->polarity)?1:(-1));
                    else
                        vun.asDouble += items[i]->value.asDouble*((items[i]->polarity)?1:(-1));
                    break;
                }
                case OP_MULTIPLY:
                {
                    if (et == VT_INTEGER)
                        vun.asLong *= items[i]->value.asLong*((items[i]->polarity)?1:(-1));
                    else
                        vun.asDouble *= items[i]->value.asDouble*((items[i]->polarity)?1:(-1));
                    break;
                }
                case OP_DIVIDE:
                {
                    // If the division of division is not integer type, we need to change to floating value
                    // i.e. division 3 by 2 requires it, but 6 by 3 not - it's still fine to divide it as integers
                    if (et == VT_INTEGER && ((vun.asLong % items[i]->value.asLong) != 0))
                    {
                        et = VT_FLOAT;
                        vun.asDouble = (double)vun.asLong;
                    }

                    if (et == VT_INTEGER)
                    {
                        if (items[i]->value.asLong != 0)
                            vun.asLong /= items[i]->value.asLong*((items[i]->polarity)?1:(-1));
                    }
                    else
                    {
                        if (items[i]->value.asDouble != 0)
                            vun.asDouble /= items[i]->value.asDouble*((items[i]->polarity)?1:(-1));
                    }
                    break;
                }
                case OP_MODULO:
                {
                    // modulo is applicable only on integer types
                    // TODO for future: round floating values if modulo is requested, or throw an error
                    if (et == VT_INTEGER)
                    {
                        if (items[i]->value.asLong != 0)
                            vun.asLong %= items[i]->value.asLong*((items[i]->polarity)?1:(-1));
                    }
                    /*else
                    {
                        if (items[i]->value.asDouble != 0)
                            vun.asDouble %= items[i]->value.asDouble*((items[i]->polarity)?1:(-1));
                    }*/
                    break;
                }
                case OP_POW:
                {
                    if (et == VT_INTEGER)
                        vun.asLong = items[i]->value.asLong*((items[i]->polarity)?1:(-1));
                    else if (et == VT_FLOAT)
                        vun.asDouble = vun.asDouble, items[i]->value.asDouble*((items[i]->polarity)?1:(-1));
                    break;
                }
            }

            // And finally erase this element from map if it was properly evaluated with previous element(s)
            items.erase(items.begin()+i);
        }
        else
            i++;
    }

    // If we changed something (erased and element and evaluated it with result), we need to put the overall result to items back
    if (changed)
    {
        // decide polarity to keep uniformity
        bool pol = true;
        if (et == VT_FLOAT && vun.asDouble < 0.0)
        {
            vun.asDouble = -vun.asDouble;
            pol = false;
        }
        else if (et == VT_INTEGER && vun.asLong < 0)
        {
            vun.asLong = -vun.asLong;
            pol = false;
        }

        // and finally insert new element
        ExprTreeElem* tmp = new ExprTreeElem(pol, et, vun);

        //items.push_back(tmp);
        items.insert(items.begin(), tmp);
    }

    // If there is only one numerical item element, the whole expression is now not needed - change it to numerical type
    if (items.size() == 1 && (items[0]->valueType == VT_INTEGER || items[0]->valueType == VT_FLOAT))
    {
        valueType = items[0]->valueType;
        memcpy(&value, &(items[0]->value), sizeof(valueUnion));

        // equal polarities = positive value, otherwise it's negative value
        if (!polarity == items[0]->polarity)
            polarity = false;
        else
            polarity = true;

        items.clear();
    }

    // if there are only integer/float values left, we will simplify ourselves again to get only one-value
    for (i = 0; i < items.size(); i++)
        if (items[i]->valueType != VT_INTEGER && items[i]->valueType != VT_FLOAT)
            return;

    SimplifyChildren();
}

double ExprTreeElem::GetAbsoluteValue()
{
    if (items.empty())
        return 0.0;

    // Recursivelly simplify myself to get the absolute value as one value in whole expression
    SimplifyChildren();

    for (uint32 i = 0; i < items.size(); i++)
    {
        if (items[i]->valueType == VT_INTEGER)
            return (double)items[i]->value.asLong*((items[i]->polarity) ? 1 : -1);
        else if (items[i]->valueType == VT_FLOAT)
            return items[i]->value.asDouble*((items[i]->polarity) ? 1.0 : -1.0);
    }

    // doesn't have absolute value
    return 0.0f;
}

double NumerateExpression(ExpressionTreeElement* expr, double (*refValueFunction)(const char*) )
{
    // This function MUSTN'T touch anything in expression tree element supplied as it's a pointer to shared memory

    if (!expr)
        return 0.0;

    double tmpval = 0.0;

    // if element value is string, we can only return real value as integer in the same tree element
    if (expr->valueType == VT_STRING)
    {
        tmpval = (*refValueFunction)(expr->value.asString);
        return (tmpval*((expr->polarity)?1:(-1)));
    }
    else if (expr->valueType == VT_INTEGER)
        return (expr->value.asLong*((expr->polarity)?1:(-1)));
    else if (expr->valueType == VT_FLOAT)
        return (expr->value.asDouble*((expr->polarity)?1:(-1)));

    if (expr->items.empty())
        return 0.0;

    tmpval = NumerateExpression(expr->items[0], refValueFunction);

    for (uint32 i = 1; i < expr->items.size(); i++)
    {
        switch (expr->operation)
        {
            case OP_ADD:
            {
                if (expr->items[i]->valueType == VT_INTEGER)
                    tmpval += expr->items[i]->value.asLong*(expr->items[i]->polarity ? 1 : -1);
                else if (expr->items[i]->valueType == VT_FLOAT)
                    tmpval += expr->items[i]->value.asDouble*(expr->items[i]->polarity ? 1 : -1);
                else
                    tmpval += NumerateExpression(expr->items[i], refValueFunction);
                break;
            }
            case OP_MULTIPLY:
            {
                if (expr->items[i]->valueType == VT_INTEGER)
                    tmpval *= expr->items[i]->value.asLong*(expr->items[i]->polarity ? 1 : -1);
                else if (expr->items[i]->valueType == VT_FLOAT)
                    tmpval *= expr->items[i]->value.asDouble*(expr->items[i]->polarity ? 1 : -1);
                else
                    tmpval *= NumerateExpression(expr->items[i], refValueFunction);
                break;
            }
            case OP_DIVIDE:
            {
                if (expr->items[i]->valueType == VT_INTEGER)
                {
                    if (expr->items[i]->value.asLong != 0)
                        tmpval /= expr->items[i]->value.asLong*(expr->items[i]->polarity ? 1 : -1);
                }
                else if (true)
                {
                    if (expr->items[i]->value.asDouble != 0)
                        tmpval /= expr->items[i]->value.asDouble*(expr->items[i]->polarity ? 1 : -1);
                }
                else
                {
                    double val = NumerateExpression(expr->items[i], refValueFunction);
                    if (val != 0)
                        tmpval /= val;
                }
                break;
            }
            case OP_MODULO:
            {
                // modulo is applicable only on integer types
                // TODO for future: round floating values if modulo is requested, or throw an error
                if (expr->items[i]->valueType == VT_INTEGER)
                {
                    if (expr->items[i]->value.asLong != 0)
                        tmpval = ((int64)tmpval) % expr->items[i]->value.asLong*(expr->items[i]->polarity ? 1 : -1);
                }
                break;
            }
            case OP_POW:
            {
                if (expr->items[i]->valueType == VT_INTEGER)
                    tmpval = pow(tmpval, expr->items[i]->value.asLong*(expr->items[i]->polarity ? 1 : -1));
                else if (expr->items[i]->valueType == VT_FLOAT)
                    tmpval = pow(tmpval, expr->items[i]->value.asDouble*(expr->items[i]->polarity ? 1 : -1));
                else
                    tmpval = pow(tmpval, NumerateExpression(expr->items[i], refValueFunction));
                break;
            }
        }
    }

    return tmpval*((expr->polarity)?1:(-1));
}

double NumerateUnitTestHelper(const char* var)
{
    if (var[0] == 'x')
        return 3.0;
    else if (var[0] == 'y')
        return 11.0;
    else if (var[0] == 'z')
        return 2.25;
    else
        return 1.0;
}

void ExpressionParserUnitTest()
{
#define EV ExpressionVector
#define ETE ExpressionTreeElement
#define SIZETEST(x,y) assert(x->items.size() == y && "Size test failed")
#define POLTEST(x,y,b) assert(x->items[y]->polarity == b && "Polarity test failed")
#define VTTEST(x,y,b) assert(x->items[y]->valueType == b && "Value type test failed")
#define ENTEST(x,y,z) assert((*x)[y] == ExpressionParser::z && "Entity type test failed")
#define EQTEST(x,y) assert(NumerateExpression(x, NumerateUnitTestHelper) == y && "Numerate test failed")

    /* Variables for test:
     *
     *    x = 3
     *    y = 11
     *    z = 2.25
     *
     *    any other = 1
     */

    int32 i;

    /* test 1 - simple expression with string, integer and polarity, only addition/subtraction */

    EV* vec = ExpressionParser::Parse("1+x-y");
    ETE* par = ExpressionParser::BuildTree(vec, 0, vec->size());

    i = 0;
    ENTEST(vec, i++, EN_VALUE);
    ENTEST(vec, i++, EN_PLUS);
    ENTEST(vec, i++, EN_VALUE);
    ENTEST(vec, i++, EN_MINUS);
    ENTEST(vec, i++, EN_VALUE);

    SIZETEST(par, 3);

    i = 0;
    POLTEST(par, i++, true);
    POLTEST(par, i++, true);
    POLTEST(par, i++, false);

    i = 0;
    VTTEST(par, i++, VT_INTEGER);
    VTTEST(par, i++, VT_STRING);
    VTTEST(par, i++, VT_STRING);

    EQTEST(par, -7);

    delete vec;
    delete par;

    /* test 2 - simple expression with strings and integers, now with mutliplication */

    vec = ExpressionParser::Parse("2*x+y-4*z");
    par = ExpressionParser::BuildTree(vec, 0, vec->size());

    i = 0;
    ENTEST(vec, i++, EN_VALUE);
    ENTEST(vec, i++, EN_MULT);
    ENTEST(vec, i++, EN_VALUE);
    ENTEST(vec, i++, EN_PLUS);
    ENTEST(vec, i++, EN_VALUE);
    ENTEST(vec, i++, EN_MINUS);
    ENTEST(vec, i++, EN_VALUE);
    ENTEST(vec, i++, EN_MULT);
    ENTEST(vec, i++, EN_VALUE);

    SIZETEST(par, 3);

    EQTEST(par, 8);

    delete vec;
    delete par;

    /* test 3 - add division */

    vec = ExpressionParser::Parse("x*y/11+z*2");
    par = ExpressionParser::BuildTree(vec, 0, vec->size());

    i = 0;
    ENTEST(vec, i++, EN_VALUE);
    ENTEST(vec, i++, EN_MULT);
    ENTEST(vec, i++, EN_VALUE);
    ENTEST(vec, i++, EN_DIV);
    ENTEST(vec, i++, EN_VALUE);
    ENTEST(vec, i++, EN_PLUS);
    ENTEST(vec, i++, EN_VALUE);
    ENTEST(vec, i++, EN_MULT);
    ENTEST(vec, i++, EN_VALUE);

    EQTEST(par, 7.5);

    delete vec;
    delete par;

    /* test 4 - simplification test*/

    vec = ExpressionParser::Parse("1+2+x+12+y+22-10+15");
    par = ExpressionParser::BuildTree(vec, 0, vec->size());

    SIZETEST(par, 8);

    i = 0;
    ENTEST(vec, i++, EN_VALUE);
    ENTEST(vec, i++, EN_PLUS);
    ENTEST(vec, i++, EN_VALUE);
    ENTEST(vec, i++, EN_PLUS);
    ENTEST(vec, i++, EN_VALUE);
    ENTEST(vec, i++, EN_PLUS);
    ENTEST(vec, i++, EN_VALUE);
    ENTEST(vec, i++, EN_PLUS);
    ENTEST(vec, i++, EN_VALUE);
    ENTEST(vec, i++, EN_PLUS);
    ENTEST(vec, i++, EN_VALUE);
    ENTEST(vec, i++, EN_MINUS);
    ENTEST(vec, i++, EN_VALUE);
    ENTEST(vec, i++, EN_PLUS);
    ENTEST(vec, i++, EN_VALUE);

    EQTEST(par, 56);

    par->SimplifyChildren();

    SIZETEST(par, 3);
    EQTEST(par, 56);

    delete vec;
    delete par;

    /* test 5 - parenthesis logic, simpliest */

    vec = ExpressionParser::Parse("1-(x-y)");
    par = ExpressionParser::BuildTree(vec, 0, vec->size());

    EQTEST(par, 9);

#undef EQTEST
#undef ENTEST
#undef POLTEST
#undef SIZETEST
#undef ETE
#undef EV
}
