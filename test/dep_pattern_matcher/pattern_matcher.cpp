/* --------------------------------------------------------------------------
 *  CppADCodeGen: C++ Algorithmic Differentiation with Source Code Generation:
 *    Copyright (C) 2013 Ciengis
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
#include "CppADCGTest.hpp"

typedef double Base;
typedef CppAD::CG<Base> CGD;
typedef CppAD::AD<CGD> ADCGD;

namespace CppAD {

    class CppADCGPatternTest : public CppADCGTest {
    public:

        inline CppADCGPatternTest(bool verbose = false, bool printValues = false) :
            CppADCGTest(verbose, printValues) {
        }

        void testPattern(std::vector<ADCGD> (*model)(std::vector<ADCGD>& x, size_t repeat),
                         size_t m,
                         size_t repeat,
                         size_t n_loops = 1,
                         bool createDynLib = false,
                         std::string name = "") {
            using namespace CppAD;

            size_t m2 = repeat * m;
            size_t n2 = m2;

            /**
             * Tape model
             */
            std::vector<ADCGD> x(n2);
            for (size_t j = 0; j < n2; j++)
                x[j] = 0.5;
            CppAD::Independent(x);

            std::vector<ADCGD> y = (*model)(x, repeat);

            ADFun<CGD> fun;
            fun.Dependent(y);

            if (createDynLib)
                testSourceCodeGen(fun, m, repeat, name);
            else
                testResults(fun, m, repeat, n_loops);
        }

        void testPatternWithAtomics(std::vector<ADCGD> (*model)(std::vector<ADCGD>& x, size_t repeat, const std::vector<CGAbstractAtomicFun<Base>*>& atoms),
                                    const std::vector<CGAbstractAtomicFun<Base>*>& atoms,
                                    size_t m,
                                    size_t repeat,
                                    size_t n_loops = 1,
                                    bool createDynLib = false,
                                    std::string name = "") {
            using namespace CppAD;

            size_t m2 = repeat * m;
            size_t n2 = m2;

            /**
             * Tape model
             */
            std::vector<ADCGD> x(n2);
            for (size_t j = 0; j < n2; j++)
                x[j] = 0.5;
            CppAD::Independent(x);

            std::vector<ADCGD> y = (*model)(x, repeat, atoms);

            ADFun<CGD> fun;
            fun.Dependent(y);

            if (createDynLib)
                testSourceCodeGen(fun, m, repeat, name);
            else
                testResults(fun, m, repeat, n_loops);
        }

    private:

        std::vector<std::set<size_t> > createRelatedDepCandidates(size_t m, size_t repeat) {
            std::vector<std::set<size_t> > relatedDepCandidates(m);
            for (size_t i = 0; i < repeat; i++) {
                for (size_t ii = 0; ii < m; ii++) {
                    relatedDepCandidates[ii].insert(i * m + ii);
                }
            }
            return relatedDepCandidates;
        }

        void testResults(ADFun<CGD>& fun, size_t m, size_t repeat, size_t n_loops) {
            /**
             * Generate operation graph
             */
            CodeHandler<double> h;
            size_t n2 = fun.Domain();

            std::vector<CGD> xx(n2);
            h.makeVariables(xx);
            for (size_t j = 0; j < n2; j++) {
                xx[j].setValue(j);
            }

            std::vector<CGD> yy = fun.Forward(0, xx);

            std::vector<std::set<size_t> > relatedDepCandidates = createRelatedDepCandidates(m, repeat);

            DependentPatternMatcher<double> matcher(relatedDepCandidates);

            std::vector<Loop<Base>*> loops = matcher.findLoops(yy, xx);
            std::cout << "loops: " << loops.size() << std::endl;
            ASSERT_EQ(loops.size(), n_loops);

            std::vector<EquationPattern<double>*> equations = matcher.getEquationPatterns();
            std::cout << "equation patterns: " << equations.size() << std::endl;
            ASSERT_EQ(equations.size(), m);

            std::auto_ptr<ADFun<CG<Base> > > newTape(matcher.createNewTape(yy, xx));

            // clean-up
            for (size_t l = 0; l < loops.size(); l++) {
                delete loops[l];
            }
        }

        void testSourceCodeGen(ADFun<CGD>& fun,
                               size_t m, size_t repeat,
                               const std::string& name) {

            std::vector<std::set<size_t> > relatedDepCandidates = createRelatedDepCandidates(m, repeat);
            /**
             * Create the dynamic library
             * (generate and compile source code)
             */
            CLangCompileModelHelper<double> compHelp(fun, name + "dynamic");
            compHelp.setRelatedDependents(relatedDepCandidates);

            compHelp.setCreateForwardZero(true);
            compHelp.setCreateJacobian(false);
            compHelp.setCreateHessian(false);
            compHelp.setCreateSparseJacobian(false);
            compHelp.setCreateSparseHessian(false);
            compHelp.setCreateForwardOne(false);
            compHelp.setCreateReverseOne(false);
            compHelp.setCreateReverseTwo(false);
            //compHelp.setMaxAssignmentsPerFunc(maxAssignPerFunc);

            GccCompiler<double> compiler;
            compiler.setSourcesFolder("sources_" + name + "_1");

            CLangCompileDynamicHelper<double> compDynHelp(compHelp);
            std::auto_ptr<DynamicLib<double> > dynamicLib(compDynHelp.createDynamicLibrary(compiler));

            /**
             * test the library
             */
            std::auto_ptr<DynamicLibModel<double> > model(dynamicLib->model(name + "dynamic"));
        }
    };
}

using namespace CppAD;

std::vector<ADCGD> model1(std::vector<ADCGD>& x, size_t repeat) {
    size_t m = 2;
    size_t m2 = repeat * m;

    // dependent variable vector 
    std::vector<ADCGD> y(m2);

    for (size_t i = 0; i < repeat; i++) {
        y[i * m] = cos(x[i * m]) + x[1] * x[2];
        y[i * m + 1] = x[i * m + 1] * x[i * m];
    }

    return y;
}

TEST_F(CppADCGPatternTest, DependentPatternMatcher) {
    using namespace CppAD;

    testPattern(model1, 2, 6);

    testPattern(model1, 2, 6, 1, true, "model1");
}

std::vector<ADCGD> model2(std::vector<ADCGD>& x, size_t repeat) {
    size_t m = 3;
    size_t m2 = repeat * m;

    assert(x.size() == m2);

    // dependent variable vector 
    std::vector<ADCGD> y(m2);

    for (size_t i = 0; i < repeat; i++) {
        y[i * m] = cos(x[i * m]) + x[1] * x[2];
        y[i * m + 1] = x[i * m + 1] * x[i * m];
        y[i * m + 2] = x[i * m + 1] * x[i * m + 2];
    }

    return y;
}

TEST_F(CppADCGPatternTest, DependentPatternMatcher3Eq) {
    using namespace CppAD;

    testPattern(model2, 3, 6);

    testPattern(model2, 3, 6, 1, true, "model2");
}

std::vector<ADCGD> model3(std::vector<ADCGD>& x, size_t repeat) {
    size_t m = 2;
    size_t m2 = repeat * m;

    // dependent variable vector 
    std::vector<ADCGD> y(m2);

    ADCGD tmp = x[1] * x[2];
    for (size_t i = 0; i < repeat; i++) {
        y[i * m] = cos(x[i * m]) + tmp;
        y[i * m + 1] = x[i * m + 1] * x[i * m] + tmp;
    }

    return y;
}

TEST_F(CppADCGPatternTest, DependentPatternMatcherCommonTmp) {
    using namespace CppAD;

    testPattern(model3, 2, 6);

    testPattern(model3, 2, 6, 1, true, "model3");
}

std::vector<ADCGD> model4(std::vector<ADCGD>& x, size_t repeat) {
    size_t m = 2;
    size_t m2 = repeat * m;

    // dependent variable vector 
    std::vector<ADCGD> y(m2);

    for (size_t i = 0; i < repeat; i++) {
        ADCGD tmp = x[1] * x[i];
        y[i * m] = cos(x[i * m]) + tmp;
        y[i * m + 1] = x[i * m + 1] * x[i * m] + tmp;
    }

    return y;
}

TEST_F(CppADCGPatternTest, DependentPatternMatcher4) {
    using namespace CppAD;

    testPattern(model4, 2, 6);

    testPattern(model4, 2, 6, 1, true, "model4");
}

std::vector<ADCGD> model5(std::vector<ADCGD>& x, size_t repeat) {
    size_t m = 2;
    size_t m2 = repeat * m;

    // dependent variable vector 
    std::vector<ADCGD> y(m2);

    for (size_t i = 0; i < repeat; i++) {
        ADCGD tmp = x[1] * x[i];
        y[i * m] = cos(x[i * m]) + tmp;

        if (i == 1) {
            for (size_t i2 = 0; i2 < repeat; i2++) {
                y[i2 * m + 1] = x[i2 * m + 1] * x[i2 * m] + tmp;
            }
        }
    }

    return y;
}

TEST_F(CppADCGPatternTest, DependentPatternMatcher5) {
    using namespace CppAD;

    testPattern(model5, 2, 6, 2);

    testPattern(model5, 2, 6, 2, true, "model5");
}

std::vector<ADCGD> model6(std::vector<ADCGD>& x, size_t repeat, const std::vector<CGAbstractAtomicFun<double>*>& atoms) {
    size_t m = 2;
    size_t m2 = repeat * m;

    CGAbstractAtomicFun<Base>& atomic0 = *atoms[0];

    // dependent variable vector 
    std::vector<ADCGD> y(m2);

    std::vector<ADCGD> ax(2), ay(1);

    for (size_t i = 0; i < repeat; i++) {
        y[i * m] = cos(x[i * m]);

        ax[0] = x[i * m];
        ax[1] = x[i * m + 1];
        atomic0(ax, ay);
        y[i * m + 1] = ay[0];
    }

    return y;
}

TEST_F(CppADCGPatternTest, DependentPatternMatcherAtomic) {
    using namespace CppAD;

    // create atomic function
    std::vector<ADCGD> y(1);
    std::vector<ADCGD> x(2);
    CppAD::Independent(x);
    y[0] = x[1] * x[0];
    CppAD::ADFun<CGD> funAtomic;
    funAtomic.Dependent(y);

    CGAtomicFunBridge<double> atomicfun("atomicFunc", funAtomic, true);
    std::vector<CGAbstractAtomicFun<double>*> atomics(1);
    atomics[0] = &atomicfun;

    testPatternWithAtomics(model6, atomics, 2, 6);

    testPatternWithAtomics(model6, atomics, 2, 6, 1, true, "model6");
}