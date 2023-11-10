#define BIORBD_API_EXPORTS
#include "Utils/Equation.h"

#ifdef _WIN32
    #include <string>
#endif

#include <math.h>
#include "Utils/Error.h"

using namespace BIORBD_NAMESPACE;

std::vector<utils::Equation>
utils::Equation::prepareMathSymbols()
{
    // Classify in the order of priority of the operations
    std::vector<utils::Equation> symbols;
    symbols.push_back("(");
    symbols.push_back(")");
    symbols.push_back("e");
    symbols.push_back("/");
    symbols.push_back("*");
    symbols.push_back("+");
    symbols.push_back("-");

    return symbols;
}

utils::Equation::Equation() :
    utils::String("")
{

}

utils::Equation::Equation(const char *c) :
    utils::String(c)
{

}

utils::Equation::Equation(const utils::String &s) :
    utils::String(s)
{

}

utils::Equation::Equation(const std::basic_string<char> &c) :
    utils::String(c)
{

}

std::vector<utils::Equation> utils::Equation::splitIntoEquation(
    utils::Equation wholeEq,
    const std::map<utils::Equation, double>& variables)
{
    // output variable
    std::vector<utils::Equation> eq;

    // Replace the variables by a number
    replaceVar(wholeEq, variables);

    // Declaration of the arithmetic markers
    const std::vector<utils::Equation>& symbols(prepareMathSymbols());

    // All along the string
    while (1) {
        int firstIdx(static_cast<int>(wholeEq.size())+1);// Assign an index too big
        size_t toStop(0); // Set stop at 0
        // Go through each symbol to see if it's in the equation
        for (size_t i=0; i<symbols.size(); ++i) {
            int idx = static_cast<int>(wholeEq.find(symbols[i]));
            if (idx < 0) { // If not, write it down
                ++toStop;
                continue;
            }
            if (idx < firstIdx && idx != -1) { // If yes, look if it's before another symbol
                firstIdx = idx;
            }
        }
        if (toStop == symbols.size()) { // If no symbols left, exit the while loop
            eq.push_back(wholeEq);
            break;
        }
        if (wholeEq.size() == 1) {
            eq.push_back(wholeEq(0));
            break;
        }
        if (firstIdx+1 == static_cast<int>(wholeEq.size())) {
            eq.push_back(wholeEq.substr(0,static_cast<size_t>(firstIdx)));
            eq.push_back(wholeEq(static_cast<size_t>(firstIdx)));
            break;
        } else if (firstIdx == 0) {
            if (!wholeEq(0).compare("-")) {
                // If the equation starts with "-"
                std::vector<utils::Equation> tp(splitIntoEquation(wholeEq.substr(1),
                                                        variables));
                if (eq.size() != 0 && !eq[eq.size()-1].compare("e") ) {
                    // special case of scientific notation (1e-x)
                    eq.push_back(wholeEq(0));
                    wholeEq = wholeEq.substr(1);
                } else if (!tp[0].compare("(")) {
                    // Resolve now the parenthese to apply the minus after
                    size_t idx = wholeEq.find_first_of(")");
                    utils::Equation newWholeEq(wholeEq.substr(2, idx-2));
                    double res(-1*evaluateEquation(splitIntoEquation(newWholeEq, variables)));
                    wholeEq = to_string(res) + wholeEq.substr(idx+1);
                } else {
                    tp[0] = "-" + tp[0];
                    eq.insert(eq.end(), tp.begin(), tp.end());
                    return eq;
                }
            } else if (!wholeEq(0).compare("+")) {
                // If the equation starts with +
                std::vector<utils::Equation> tp(splitIntoEquation(wholeEq.substr(1),
                                                        variables));
                if (eq.size() != 0) { // If there's nothing before, it's false in all cases 1ex
                    eq.push_back("+");
                }
                eq.insert(eq.end(), tp.begin(), tp.end());
                return eq;
            } else {
                eq.push_back(wholeEq(0));
                wholeEq = wholeEq.substr(1);
            }
        } else {
            eq.push_back(wholeEq.substr(0, static_cast<size_t>(firstIdx))); // Take everything before symbol
            eq.push_back(wholeEq(static_cast<size_t>(firstIdx))); // Get the symbol
            wholeEq = wholeEq.substr(static_cast<size_t>(firstIdx) +1); // Keep everything after the symbol and restart
        }

    }

    // Replace constant with numbers
    replaceCste(eq);

    // Return the equation
    return eq;
}

void utils::Equation::replaceCste(
    std::vector<utils::Equation> &eq)
{
    for (size_t i=0; i<eq.size(); ++i)
        if (!eq[i].tolower().compare("pi")) {
            eq[i] = to_string(M_PI);
        }
}

void utils::Equation::replaceVar(
    utils::Equation &eq,
    const std::map<utils::Equation, double>& variables)
{
    for (auto var : variables)
        while (eq.find(var.first) != std::string::npos) {
            size_t pos(eq.find(var.first));
            size_t length(var.first.length());
            eq = eq.substr(0, pos) + "(" +
                 to_string(var.second) + ")" +
                 eq.substr(pos + length);
        }
}


double utils::Equation::evaluateEquation(
    std::vector<utils::Equation> wholeEq)
{
    return evaluateEquation(wholeEq,0);
}

double utils::Equation::evaluateEquation(
    std::vector<utils::Equation> eq,
    size_t math)
{
    // If everything was done
    if (eq.size() == 1) {
        return stod(eq[0]);
    }

    // Declaration of the arithmetic markers
    const std::vector<utils::Equation>& symbols(prepareMathSymbols());
    std::vector<utils::Equation> eq2;
    bool continuer(true);

    for (size_t j=0; j<eq.size(); ++j) {
        if (!eq[j].compare(symbols[math]) && continuer) {
            if (j==0 && (!symbols[math].compare("+") || !symbols[math].compare("-"))) {
                // Crush the previous value
                if (!symbols[math].compare("+")) {
                    eq2[j-1] = to_string(0.0 + stod(eq[j+1]));
                } else if (!symbols[math].compare("-")) {
                    eq2[j-1] = to_string(0.0 - stod(eq[j+1]));
                }
            } else {
                // Crush the previous value
                if (!symbols[math].compare("(")) {
                    std::vector<utils::Equation> eq_tp;
                    bool foundIdx(false);
                    int cmpValues(0);
                    size_t cmpOpen(0);
                    for (size_t k=j+1; k<eq.size(); ++k) {
                        if (!eq[k].compare("(")) {
                            cmpOpen++;
                        } else if (!eq[k].compare(")")) {
                            if (cmpOpen == 0) {
                                foundIdx = true;
                                break;
                            } else {
                                cmpOpen--;
                            }
                        }

                        eq_tp.push_back(eq[k]);
                        ++cmpValues;
                    }
                    utils::Error::check(foundIdx, "You must close brackets!");

                    eq2.push_back(to_string(evaluateEquation(eq_tp)));
                    j+=static_cast<size_t>(cmpValues);
                } else if (!symbols[math].compare("/")) {
                    eq2[j-1] = to_string(stod(eq[j-1]) / stod(eq[j+1]));
                } else if (!symbols[math].compare("*")) {
                    eq2[j-1] = to_string(stod(eq[j-1]) * stod(eq[j+1]));
                } else if (!symbols[math].compare("+")) {
                    eq2[j-1] = to_string(stod(eq[j-1]) + stod(eq[j+1]));
                } else if (!symbols[math].compare("-")) {
                    eq2[j-1] = to_string(stod(eq[j-1]) - stod(eq[j+1]));
                } else if (!symbols[math].compare("e")) {
                    if (!eq[j+1].compare("-")) {
                        eq2[j-1] = to_string(stod(eq[j-1]) * pow(10,-1*stod(eq[j+2])));
                        ++j;
                    } else if (!eq[j+1].compare("+")) {
                        eq2[j-1] = to_string(stod(eq[j-1]) * pow(10,stod(eq[j+2])));
                        ++j;
                    } else {
                        eq2[j-1] = to_string(stod(eq[j-1]) * pow(10,stod(eq[j+1])));
                    }

                }
            }

            j+=2;
            continuer = false;
        }
        if (j<eq.size()) {
            eq2.push_back(eq[j]);
        }
    }

    if (continuer) {
        return evaluateEquation(eq2, ++math);
    } else {
        return evaluateEquation(eq2, math);
    }
}

double utils::Equation::evaluateEquation(
    utils::Equation wholeEq,
    const std::map<utils::Equation, double>& variables)
{
    return evaluateEquation(splitIntoEquation(wholeEq, variables));
}
double utils::Equation::evaluateEquation(
    utils::Equation wholeEq)
{
    std::map<utils::Equation, double> dumb;
    return evaluateEquation(splitIntoEquation(wholeEq, dumb));
}
