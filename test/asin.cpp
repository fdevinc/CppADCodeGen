/* --------------------------------------------------------------------------
 *  CppADCodeGen: C++ Algorithmic Differentiation with Source Code Generation:
 *    Copyright (C) 2012 Ciengis
 *
 *  CppADCodeGen is distributed under multiple licenses:
 *
 *   - Common Public License Version 1.0 (CPL1), and
 *   - GNU General Public License Version 2 (GPL2).
 *
 * CPL1 terms and conditions can be found in the file "epl-v10.txt", while
 * terms and conditions for the GPL2 can be found in the file "gpl2.txt".
 * ----------------------------------------------------------------------------
 * Author: Joao Leal
 */
#include "CppADCGOperationTest.hpp"
#include "asin.hpp"

using namespace CppAD;

TEST_F(CppADCGOperationTest, asin) {
    // independent variable vector
    std::vector<double> u(1);
    u[0] = 0.5;

    // create f: U -> Z and vectors used for derivative calculations   
    test0nJac("asin", &AsinFunc<double >, &AsinFunc<CG<double> >, u);
}
