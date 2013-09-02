#ifndef CPPAD_CG_LINEAR2_INDEX_PATTERN_INCLUDED
#define CPPAD_CG_LINEAR2_INDEX_PATTERN_INCLUDED
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

namespace CppAD {

    /**
     * Alternative linear pattern y = x / a + b
     * 
     * There might not be any equivalente LinearIndexPattern since it uses
     * integers and not floating points!
     */
    class Linear2IndexPattern : public IndexPattern {
    protected:
        // the index used in this index pattern;
        const Index* index_;
        // 1 / slope
        long a_;
        // constant term
        long b_;
    public:
        inline Linear2IndexPattern(const Index& index, long a, long b) :
            IndexPattern(index),
            a_(a),
            b_(b) {
        }

        inline long getInverseLinearSlope() const {
            return a_;
        }

        inline long getLinearConstantTerm() const {
            return b_;
        }

        inline virtual IndexPatternType getType() const {
            return LINEAR2;
        }

        inline virtual ~Linear2IndexPattern() {
        }
    };

}

#endif