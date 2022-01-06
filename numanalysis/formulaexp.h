#ifndef FORMULAEXP_H
#define FORMULAEXP_H
#include <QString>
#include <QMap>
#include <QDebug>

class FormulaExp{
public:
    FormulaExp(const QString& formula);
public:
    const double calculate();
    QMap<QString, double> variableList = { {"pi", 3.141592653589793238} };
    static QMap<QString, int> operatorList;
private:
    const double applyLOperator(const QString& ope, const double& val1, const double& val2);
    const double applyROperator(const QString& ope, const double& val);
private:
    QList<QString> tokenList;
};






inline const double FormulaExp::applyLOperator(const QString &ope, const double& val1, const double& val2)
{
    if(ope == "+") return val1 + val2;
    else if(ope == "-") return val1 - val2;
    else if(ope == "*") return val1 * val2;
    else if(ope == "/") return val1 / val2;
    else if(ope == "^") return pow(val1, val2);
    else return NAN;
}

inline const double FormulaExp::applyROperator(const QString &ope, const double& val)
{
    if(ope == "sqrt") return sqrt(val);
    else if(ope == "sin") return sin(val);
    else if(ope == "cos") return cos(val);
    else if(ope == "tan") return tan(val);
    else if(ope == "abs") return (val > 0) ? val : -val;
    else if(ope == "log") return log10(val);
    else if(ope == "ln") return log(val);
    else return NAN;
}


#endif // FORMULAEXP_H
