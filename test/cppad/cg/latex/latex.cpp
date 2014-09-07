/* --------------------------------------------------------------------------
 *  CppADCodeGen: C++ Algorithmic Differentiation with Source Code Generation:
 *    Copyright (C) 2014 Ciengis
 *
 *  CppADCodeGen is distributed under multiple licenses:
 *
 *   - Eclipse Public License Version 1.0 (EPL1), and
 *   - GNU General Public License Version 3 (GPL3).
 *
 *  EPL1 terms and conditions can be found in the file "epl-v10.txt", while
 *  terms and conditions for the GPL3 can be found in the file "gpl3.txt".
 * ----------------------------------------------------------------------------
 * Author: Joao Leal
 */

#include <cppad/cg/cppadcg.hpp>
#include <cppad/cg/latex/latex.hpp>
#include <gtest/gtest.h>

using namespace CppAD;
using namespace CppAD::cg;

TEST(CppADCGLatexTest, latex) {
    // use a special object for source code generation
    typedef CG<double> CGD;
    typedef AD<CGD> ADCG;

    // independent variable vector
    CppAD::vector<ADCG> U(2);
    U[0] = 2.;
    U[1] = 3.;
    Independent(U);

    // dependent variable vector 
    CppAD::vector<ADCG> Z(1);

    // the model
    ADCG a = U[0] / 1. + U[1] * U[1];
    ADCG b = a / 2e-6;
    Z[0] = b + 1 / (sign(b)*5 * a);

    ADFun<CGD> fun(U, Z); // the model tape

    /**
     * start the special steps for source code generation
     * for a Jacobian
     */
    CodeHandler<double> handler;

    CppAD::vector<CGD> indVars(2);
    handler.makeVariables(indVars);

    CppAD::vector<CGD> jac = fun.SparseJacobian(indVars);

    LanguageLatex<double> langLatex;
    LangLatexDefaultVariableNameGenerator<double> nameGen;

    std::ostringstream code;
    handler.generateCode(code, langLatex, jac, nameGen);
    std::cout << code.str();
}