#ifndef CPPAD_CG_LANGUAGE_INCLUDED
#define CPPAD_CG_LANGUAGE_INCLUDED
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

namespace CppAD {

    /**
     * Information required for the generation of source code for a language
     * 
     * @author Joao Leal
     */
    template<class Base>
    class LanguageGenerationData {
    public:
        /**
         * The independent variables
         */
        const std::vector<OperationNode<Base> *>& independent;
        /**
         * The dependent variables
         */
        const vector<CG<Base> >& dependent;
        /**
         * the lowest ID used for temporary variables
         */
        size_t minTemporaryVarID;
        /**
         * The order of the assignment of the variables in the source code
         */
        const std::vector<OperationNode<Base>*>& variableOrder;
        /**
         * Provides the rules for variable name creation
         */
        VariableNameGenerator<Base>& nameGen;
        /**
         * maps atomic function IDs to their internal index
         */
        const std::map<size_t, size_t>& atomicFunctionId2Index;
        /**
         * maps atomic function IDs to their names
         */ 
        const std::map<size_t, std::string>& atomicFunctionId2Name;
        /**
         * a flag indicating whether or not temporary variable IDs have been recycled
         */
        const bool reuseIDs;
        //
        const std::set<const Index*>& indexes;
        //
        const std::vector<const IndexPattern*>& loopDependentIndexPatterns;
        //
        const std::vector<const IndexPattern*>& loopIndependentIndexPatterns;
        /**
         * whether or not the dependent variables should be zeroed before 
         * executing the operation graph
         */
        const bool zeroDependents;
    public:

        LanguageGenerationData(const std::vector<OperationNode<Base> *>& ind,
                               const vector<CG<Base> >& dep,
                               size_t minTempVID,
                               const std::vector<OperationNode<Base>*>& vo,
                               VariableNameGenerator<Base>& ng,
                               const std::map<size_t, size_t>& atomicId2Index,
                               const std::map<size_t, std::string>& atomicId2Name,
                               const bool ri,
                               const std::set<const Index*>& indexs,
                               const std::vector<const IndexPattern*>& dependentIndexPatterns,
                               const std::vector<const IndexPattern*>& independentIndexPatterns,
                               bool zero) :
            independent(ind),
            dependent(dep),
            minTemporaryVarID(minTempVID),
            variableOrder(vo),
            nameGen(ng),
            atomicFunctionId2Index(atomicId2Index),
            atomicFunctionId2Name(atomicId2Name),
            reuseIDs(ri),
            indexes(indexs),
            loopDependentIndexPatterns(dependentIndexPatterns),
            loopIndependentIndexPatterns(independentIndexPatterns),
            zeroDependents(zero) {
        }
    };

    /**
     * Creates the source code for a specific language
     * 
     * @author Joao Leal
     */
    template<class Base>
    class Language {
    protected:
        virtual void generateSourceCode(std::ostream& out, LanguageGenerationData<Base>& info) = 0;

        /**
         * Whether or not a new variable is created as a result of this operation
         * 
         * @param op Operation
         * @return true if a new variable is created
         */
        virtual bool createsNewVariable(const OperationNode<Base>& op) const = 0;

        virtual bool requiresVariableArgument(enum CGOpCode op, size_t argIndex) const = 0;

        friend class CodeHandler<Base>;
    };

}

#endif

