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
#include "tan.hpp"

using namespace CppAD;

TEST_F(CppADCGOperationTest, tan) {
    double eps = 100. * std::numeric_limits<double>::epsilon();

    // independent variable vector, indices, values, and declaration
    std::vector<double> u(1);
    u[0] = .7;

    test0nJac("tan_first", &tanFirstFunc<double >, &tanFirstFunc<CG<double> >, u, eps, eps);

    test0nJac("tan_last", &tanLastFunc<double >, &tanLastFunc<CG<double> >, u, eps, eps);
}

TEST_F(CppADCGOperationTest, tanh) {
    double eps = 100. * std::numeric_limits<double>::epsilon();

    // independent variable vector, indices, values, and declaration
    std::vector<double> u(1);
    u[0] = .5;

    test0nJac("tanh_first", &tanhFirstFunc<double >, &tanhFirstFunc<CG<double> >, u, eps, eps);

    test0nJac("tanh_last", &tanhLastFunc<double >, &tanhLastFunc<CG<double> >, u, eps, eps);
}