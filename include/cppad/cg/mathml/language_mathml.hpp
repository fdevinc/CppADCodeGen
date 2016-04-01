#ifndef CPPAD_CG_LANGUAGE_MATHML_INCLUDED
#define CPPAD_CG_LANGUAGE_MATHML_INCLUDED
/* --------------------------------------------------------------------------
 *  CppADCodeGen: C++ Algorithmic Differentiation with Source Code Generation:
 *    Copyright (C) 2015 Ciengis
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

namespace CppAD {
namespace cg {

/**
 * Generates presentation markup using the html  and MathML specification.
 * 
 * @author Joao Leal
 */
template<class Base>
class LanguageMathML : public Language<Base> {
protected:
    static const std::string _C_STATIC_INDEX_ARRAY;
    static const std::string _C_SPARSE_INDEX_ARRAY;
    static const std::string _ATOMIC_TX;
    static const std::string _ATOMIC_TY;
    static const std::string _ATOMIC_PX;
    static const std::string _ATOMIC_PY;
protected:
    // information from the code handler (not owned)
    LanguageGenerationData<Base>* _info;
    // current indentation
    size_t _indentationLevel;
    // css style
    std::string _style;
    // javascript source code
    std::string _javascript;
    // additional markup for the head
    std::string _headExtra;
    std::string _startEq;
    std::string _endEq;
    std::string _forStart;
    std::string _forEnd;
    std::string _forBodyStart;
    std::string _forBodyEnd;
    std::string _ifStart;
    std::string _ifEnd;
    std::string _elseIfStart;
    std::string _elseIfEnd;
    std::string _elseStart;
    std::string _elseEnd;
    std::string _condBodyStart;
    std::string _condBodyEnd;
    std::string _assignStr;
    std::string _assignAddStr;
    // new line characters
    std::string _endline;
    // output stream for the generated source code
    std::ostringstream _code;
    // creates the variable names
    VariableNameGenerator<Base>* _nameGen;
    // auxiliary string stream
    std::ostringstream _ss;
    //
    size_t _independentSize;
    //
    size_t _minTemporaryVarID;
    // maps the variable IDs to the their position in the dependent vector
    // (some IDs may be the same as the independent variables when dep = indep)
    std::map<size_t, size_t> _dependentIDs;
    // the dependent variable vector
    const CppAD::vector<CG<Base> >* _dependent;
    // the temporary variables that may require a declaration
    std::map<size_t, OperationNode<Base>*> _temporary;
    // whether or not to ignore assignment of constant zero values to dependent variables
    bool _ignoreZeroDepAssign;
    // the name of the file to be created without the extension
    std::string _filename;
    // the maximum number of assignment (~lines) per local file
    size_t _maxAssigmentsPerFile;
    // maps filenames to their content
    std::map<std::string, std::string>* _sources;
    // the values in the temporary array
    std::vector<const Argument<Base>*> _tmpArrayValues;
    // the values in the temporary sparse array
    std::vector<const Argument<Base>*> _tmpSparseArrayValues;
    // indexes defined as function arguments
    std::vector<const OperationNode<Base>*> _funcArgIndexes;
    std::vector<const LoopStartOperationNode<Base>*> _currentLoops;
    // the maximum precision used to print values
    size_t _parameterPrecision;
    // whether or not we are in an equation/align block
    bool _inEquationEnv;
private:
    std::string auxArrayName_;
    std::vector<int> varIds_;
public:

    /**
     * Creates a MathML language source code generator
     */
    LanguageMathML() :
        _info(nullptr),
        _indentationLevel(0),
        _style(".loop{}\n"
               ".loopBody{padding-left: 2em;}\n"
               ".condIf{}\n"
               ".condElseIf{}\n"
               ".condElse{}\n"
               ".condBody{padding-left: 2em;}\n"
               ".dep{color:#600;}\n"
               ".indep{color:#060;}\n"
               ".tmp{color:#006;}\n"),
        _startEq("<math display=\"block\">"),
        _endEq("</math>"),
        _forStart("<div class='loop'>"),
        _forEnd("</div>"),
        _forBodyStart("<div class='loopBody'>"),
        _forBodyEnd("</div>"),
        _ifStart("<div class='condIf'>"),
        _ifEnd("</div>"),
        _elseIfStart("<div class='condElseIf'>"),
        _elseIfEnd("</div>"),
        _elseStart("<div class='condElse'>"),
        _elseEnd("</div>"),
        _condBodyStart("<div class='condBody'>"),
        _condBodyEnd("</div>"),
        _assignStr("<mo>=</mo>"),
        _assignAddStr("<mo>+=</mo>"),
        _endline("\n"),
        _nameGen(nullptr),
        _independentSize(0),
        _dependent(nullptr),
        _ignoreZeroDepAssign(false),
        _filename("algorithm"),
        _maxAssigmentsPerFile(0),
        _sources(nullptr),
        _parameterPrecision(std::numeric_limits<Base>::digits10) {
    }

    inline const std::string& getAssignMarkup() const {
        return _assignStr;
    }

    inline void setAssignMarkup(const std::string& assign) {
        _assignStr = assign;
    }

    inline const std::string& getAddAssignMarkup() const {
        return _assignAddStr;
    }

    inline void setAddAssignMarkup(const std::string& assignAdd) {
        _assignAddStr = assignAdd;
    }

    inline bool isIgnoreZeroDepAssign() const {
        return _ignoreZeroDepAssign;
    }

    inline void setIgnoreZeroDepAssign(bool ignore) {
        _ignoreZeroDepAssign = ignore;
    }

    void setFilename(const std::string& name) {
        _filename = name;
    }

    /**
     * Defines the CSS style to be added to head section of the html document.
     * 
     * @param style the content of the CSS
     */
    void setStyle(const std::string& style) {
        _style = style;
    }

    const std::string& getStyle() const {
        return _style;
    }

    /**
     * Defines Javascript source code to be added to head section of the html document.
     * 
     * @param javascript the Javascript source code
     */
    void setJavascript(const std::string& javascript) {
        _javascript = javascript;
    }

    const std::string& getJavascript() const {
        return _javascript;
    }

    /**
     * Defines additional markup to be added to head section of the html document.
     * 
     * @param headExtra html markup to be added to the head section
     */
    void setHeadExtraMarkup(const std::string& headExtra) {
        _headExtra = headExtra;
    }

    const std::string& getHeadExtraMarkup() const {
        return _headExtra;
    }

    /**
     * Defines the surrounding markup for each equation.
     * 
     * @param begin the opening html markup
     * @param end the closing html markup
     */
    virtual void setEquationMarkup(const std::string& begin,
                                   const std::string& end) {
        _startEq = begin;
        _endEq = end;
    }

    /**
     * Provides the markup preceding each equation.
     */
    virtual const std::string& getEquationStartMarkup() const {
        return _startEq;
    }

    /**
     * Provides the markup after each equation.
     */
    virtual const std::string& getEquationEndMarkup() const {
        return _endEq;
    }

    /**
     * Defines the surrounging markup for each for loop.
     * 
     * @param begin the opening html markup
     * @param end the closing html markup
     */
    virtual void setForMarkup(const std::string& begin,
                              const std::string& end) {
        _forStart = begin;
        _forEnd = end;
    }

    /**
     * Provides the markup preceding each loop.
     */
    virtual const std::string& getForStartMarkup() const {
        return _forStart;
    }

    /**
     * Provides the markup after each loop.
     */
    virtual const std::string& getForEndMarkup() const {
        return _forEnd;
    }

    /**
     * Defines the surrounding markup for each If.
     * 
     * @param begin the opening html markup
     * @param end the closing html markup
     */
    virtual void setIfMarkup(const std::string& begin,
                             const std::string& end) {
        _ifStart = begin;
        _ifEnd = end;
    }

    /**
     * Provides the markup preceding each If.
     */
    virtual const std::string& getIfStartMarkup() const {
        return _ifStart;
    }

    /**
     * Provides the markup after each If.
     */
    virtual const std::string& getIfEndMarkup() const {
        return _ifEnd;
    }

    /**
     * Defines the surrounding markup for each else if.
     * 
     * @param begin the opening html markup
     * @param end the closing html markup
     */
    virtual void setElseIfMarkup(const std::string& begin,
                                 const std::string& end) {
        _elseIfStart = begin;
        _elseIfEnd = end;
    }

    /**
     * Provides the markup preceding each else if.
     */
    virtual const std::string& getElseIfStartMarkup() const {
        return _elseIfStart;
    }

    /**
     * Provides the markup after each else if.
     */
    virtual const std::string& getElseIfEndMarkup() const {
        return _elseIfEnd;
    }

    /**
     * Defines the surrounding markup for each else.
     * 
     * @param begin the opening html markup
     * @param end the closing html markup
     */
    virtual void setElseMarkup(const std::string& begin,
                               const std::string& end) {
        _elseStart = begin;
        _elseEnd = end;
    }

    /**
     * Provides the markup preceding each else.
     */
    virtual const std::string& getElseStartMarkup() const {
        return _elseStart;
    }

    /**
     * Provides the markup after each else.
     */
    virtual const std::string& getElseEndMarkup() const {
        return _elseEnd;
    }

    virtual void setFunctionIndexArgument(const OperationNode<Base>& funcArgIndex) {
        _funcArgIndexes.resize(1);
        _funcArgIndexes[0] = &funcArgIndex;
    }

    virtual void setFunctionIndexArguments(const std::vector<const OperationNode<Base>*>& funcArgIndexes) {
        _funcArgIndexes = funcArgIndexes;
    }

    virtual const std::vector<const OperationNode<Base>*>& getFunctionIndexArguments() const {
        return _funcArgIndexes;
    }

    /**
     * Provides the maximum precision used to print constant values in the
     * generated source code
     * 
     * @return the maximum number of digits
     */
    virtual size_t getParameterPrecision() const {
        return _parameterPrecision;
    }

    /**
     * Defines the maximum precision used to print constant values in the
     * generated source code
     * 
     * @param p the maximum number of digits
     */
    virtual void setParameterPrecision(size_t p) {
        _parameterPrecision = p;
    }

    virtual void setMaxAssigmentsPerFunction(size_t maxAssigmentsPerFunction,
                                             std::map<std::string, std::string>* sources) {
        _maxAssigmentsPerFile = maxAssigmentsPerFunction;
        _sources = sources;
    }

    inline virtual ~LanguageMathML() {
    }

    /***************************************************************************
     *                               STATIC
     **************************************************************************/
    static inline void printIndexCondExpr(std::ostringstream& out,
                                          const std::vector<size_t>& info,
                                          const std::string& index) {
        CPPADCG_ASSERT_KNOWN(info.size() > 1 && info.size() % 2 == 0, "Invalid number of information elements for an index condition expression operation");

        size_t infoSize = info.size();
        for (size_t e = 0; e < infoSize; e += 2) {
            if (e > 0) {
                out << "<mo>&or;</mo>"; // or
            }
            size_t min = info[e];
            size_t max = info[e + 1];
            if (min == max) {
                out << "<mi>" << index << "</mi><mo>==</mo><mn>" << min << "</nm>";
            } else if (min == 0) {
                out << "<mi>" << index << "</mi><mo>&le;</mo><mn>" << max << "</mn>";
            } else if (max == std::numeric_limits<size_t>::max()) {
                out << "<mn>" << min << "</mn><mo>&le;</mo><mi>" << index << "</mi>";
            } else {
                if (infoSize != 2)
                    out << "<mfenced><mrow>";

                if (max - min == 1)
                    out << "<mn>" << min << "</mn><mo>==</mo><mi>" << index << "</mi><mo>&or;</mo><mi>" << index << "</mi><mo>==</mo><mn>" << max << "</mn>";
                else
                    out << "<mn>" << min << "</mn><mo>&le;</mo><mi>" << index << "</mi><mo>&and;</mo><mi>" << index << "</mi><mo>&le;</mo><mn>" << max << "</mn";

                if (infoSize != 2)
                    out << "</mrow></mfenced>";
            }
        }
    }

    /***************************************************************************
     * 
     **************************************************************************/

    inline void printStaticIndexArray(std::ostringstream& os,
                                      const std::string& name,
                                      const std::vector<size_t>& values);

    inline void printStaticIndexMatrix(std::ostringstream& os,
                                       const std::string& name,
                                       const std::map<size_t, std::map<size_t, size_t> >& values);

    /***************************************************************************
     * index patterns
     **************************************************************************/
    static inline void generateNames4RandomIndexPatterns(const std::set<RandomIndexPattern*>& randomPatterns);

    inline void printRandomIndexPatternDeclaration(std::ostringstream& os,
                                                   const std::string& identation,
                                                   const std::set<RandomIndexPattern*>& randomPatterns);

    static void indexPattern2String(std::ostream& os,
                                    const IndexPattern& ip,
                                    const OperationNode<Base>& index);

    static void indexPattern2String(std::ostream& os,
                                    const IndexPattern& ip,
                                    const std::vector<const OperationNode<Base>*>& indexes);

    static inline void linearIndexPattern2String(std::ostream& os,
                                                 const LinearIndexPattern& lip,
                                                 const OperationNode<Base>& index);

    /***************************************************************************
     *                              protected
     **************************************************************************/
protected:

    virtual void generateSourceCode(std::ostream& out,
                                    const std::unique_ptr<LanguageGenerationData<Base> >& info) override {
        using CppAD::vector;

        const bool multiFile = _maxAssigmentsPerFile > 0 && _sources != nullptr;

        // clean up
        _code.str("");
        _ss.str("");
        _indentationLevel = 0;
        _temporary.clear();
        auxArrayName_ = "";
        _currentLoops.clear();


        // save some info
        _info = info.get();
        _independentSize = info->independent.size();
        _dependent = &info->dependent;
        _nameGen = &info->nameGen;
        _minTemporaryVarID = info->minTemporaryVarID;
        const vector<CG<Base> >& dependent = info->dependent;
        const std::vector<OperationNode<Base>*>& variableOrder = info->variableOrder;

        _tmpArrayValues.resize(_nameGen->getMaxTemporaryArrayVariableID());
        std::fill(_tmpArrayValues.begin(), _tmpArrayValues.end(), nullptr);
        _tmpSparseArrayValues.resize(_nameGen->getMaxTemporarySparseArrayVariableID());
        std::fill(_tmpSparseArrayValues.begin(), _tmpSparseArrayValues.end(), nullptr);

        varIds_.resize(_minTemporaryVarID + variableOrder.size());
        std::fill(varIds_.begin(), varIds_.end(), 0);

        /**
         * generate index array names (might be used for variable names)
         */
        generateNames4RandomIndexPatterns(info->indexRandomPatterns);

        /**
         * generate variable names
         */
        //generate names for the independent variables
        for (size_t j = 0; j < _independentSize; j++) {
            OperationNode<Base>& op = *info->independent[j];
            if (op.getName() == nullptr) {
                op.setName(_nameGen->generateIndependent(op, getVariableID(op)));
            }
        }

        // generate names for the dependent variables (must be after naming independents)
        for (size_t i = 0; i < dependent.size(); i++) {
            OperationNode<Base>* node = dependent[i].getOperationNode();
            if (node != nullptr && node->getOperationType() != CGOpCode::LoopEnd && node->getName() == nullptr) {
                if (node->getOperationType() == CGOpCode::LoopIndexedDep) {
                    size_t pos = node->getInfo()[0];
                    const IndexPattern* ip = info->loopDependentIndexPatterns[pos];
                    node->setName(_nameGen->generateIndexedDependent(*node, getVariableID(*node), *ip));

                } else {
                    node->setName(_nameGen->generateDependent(i));
                }
            }
        }

        /**
         * function variable declaration
         */
        const std::vector<FuncArgument>& indArg = _nameGen->getIndependent();
        const std::vector<FuncArgument>& depArg = _nameGen->getDependent();
        const std::vector<FuncArgument>& tmpArg = _nameGen->getTemporary();
        CPPADCG_ASSERT_KNOWN(indArg.size() > 0 && depArg.size() > 0,
                             "There must be at least one dependent and one independent argument");
        CPPADCG_ASSERT_KNOWN(tmpArg.size() == 3,
                             "There must be three temporary variables");

        auxArrayName_ = tmpArg[1].name + "p";

        /**
         * Determine the dependent variables that result from the same operations
         */
        // dependent variables indexes that are copies of other dependent variables
        std::set<size_t> dependentDuplicates;

        for (size_t i = 0; i < dependent.size(); i++) {
            OperationNode<Base>* node = dependent[i].getOperationNode();
            if (node != nullptr) {
                CGOpCode type = node->getOperationType();
                if (type != CGOpCode::Inv && type != CGOpCode::LoopEnd) {
                    size_t varID = getVariableID(*node);
                    if (varID > 0) {
                        std::map<size_t, size_t>::const_iterator it2 = _dependentIDs.find(varID);
                        if (it2 == _dependentIDs.end()) {
                            _dependentIDs[getVariableID(*node)] = i;
                        } else {
                            // there can be several dependent variables with the same ID
                            dependentDuplicates.insert(i);
                        }
                    }
                }
            }
        }

        // the names of local functions
        std::vector<std::string> mathMLFiles;
        if (multiFile) {
            mathMLFiles.reserve(variableOrder.size() / _maxAssigmentsPerFile);
        }

        /**
         * non-constant variables
         */
        if (variableOrder.size() > 0) {
            // generate names for temporary variables
            for (OperationNode<Base>* node : variableOrder) {
                CGOpCode op = node->getOperationType();
                if (!isDependent(*node) && op != CGOpCode::IndexDeclaration) {
                    // variable names for temporaries must always be created since they might have been used before with a different name/id
                    if (requiresVariableName(*node) && op != CGOpCode::ArrayCreation && op != CGOpCode::SparseArrayCreation) {
                        node->setName(_nameGen->generateTemporary(*node, getVariableID(*node)));
                    } else if (op == CGOpCode::ArrayCreation) {
                        node->setName(_nameGen->generateTemporaryArray(*node, getVariableID(*node)));
                    } else if (op == CGOpCode::SparseArrayCreation) {
                        node->setName(_nameGen->generateTemporarySparseArray(*node, getVariableID(*node)));
                    }
                }
            }

            /**
             * Source code generation magic!
             */
            if (info->zeroDependents) {
                // zero initial values
                const std::vector<FuncArgument>& depArg = _nameGen->getDependent();
                for (size_t i = 0; i < depArg.size(); i++) {
                    _code << _startEq;
                    const FuncArgument& a = depArg[i];
                    if (a.array) {
                        _code << a.name;
                    } else {
                        _code << "<mrow id='" << createID(*(*_dependent)[i].getOperationNode()) << "' class='dep'>" << _nameGen->generateDependent(i) << "</mrow>";
                    }
                    _code << _assignStr;
                    printParameter(Base(0.0));
                    _code << _endEq << _endline;
                }
            }

            size_t assignCount = 0;
            for (OperationNode<Base>* it : variableOrder) {
                // check if a new function should start
                if (assignCount >= _maxAssigmentsPerFile && multiFile && _currentLoops.empty()) {
                    assignCount = 0;
                    saveLocalFunction(mathMLFiles, mathMLFiles.empty() && info->zeroDependents);
                }

                OperationNode<Base>& node = *it;

                // a dependent variable assigned by a loop does require any source code (its done inside the loop)
                if (node.getOperationType() == CGOpCode::DependentRefRhs) {
                    continue; // nothing to do (this operation is right hand side only)
                } else if (node.getOperationType() == CGOpCode::TmpDcl) { // temporary variable declaration does not need any source code here
                    continue; // nothing to do (bogus operation)
                }

                assignCount += printAssigment(node);
            }

            if (mathMLFiles.size() > 0 && assignCount > 0) {
                assignCount = 0;
                saveLocalFunction(mathMLFiles, false);
            }
        }

        if (!mathMLFiles.empty()) {
            /**
             * Create the master html file which inputs the other files
             */
            CPPADCG_ASSERT_KNOWN(tmpArg[0].array,
                                 "The temporary variables must be saved in an array in order to generate multiple functions");
            printAlgorithmFileStart(_code);
            for (size_t i = 0; i < mathMLFiles.size(); i++) {
                _code << "<a href='" << mathMLFiles[i] << ".html'>part " << (i + 1) << "</a><br/>" << _endline;
            }
            printAlgorithmFileEnd(_code);
        }

        // dependent duplicates
        if (dependentDuplicates.size() > 0) {
            _code << "<!-- variable duplicates: " << dependentDuplicates.size() << " -->" << _endline;

            for (size_t index : dependentDuplicates) {
                const CG<Base>& dep = dependent[index];
                std::string varName = _nameGen->generateDependent(index);
                OperationNode<Base>* depNode = dep.getOperationNode();
                const std::string& origVarName = *depNode->getName();

                _code << _startEq
                        << "<mrow id='" << createID(depNode) << "' class='dep'>" << varName << "</mrow>"
                        << _assignStr
                        << "<mrow id='" << createID(depNode) << "' class='dep'>" << origVarName << "</mrow>";
                printAssigmentEnd();
            }
        }

        // constant dependent variables 
        bool commentWritten = false;
        for (size_t i = 0; i < dependent.size(); i++) {
            if (dependent[i].isParameter()) {
                if (!_ignoreZeroDepAssign || !dependent[i].isIdenticalZero()) {
                    if (!commentWritten) {
                        _code << "<!-- dependent variables without operations -->" << _endline;
                        commentWritten = true;
                    }

                    std::string varName = _nameGen->generateDependent(i);
                    _code << _startEq
                            << "<mrow class='dep'>" << varName << "</mrow>" << _assignStr; // id='" << createID(??)
                    printParameter(dependent[i].getValue());
                    printAssigmentEnd();
                }
            } else if (dependent[i].getOperationNode()->getOperationType() == CGOpCode::Inv) {
                if (!commentWritten) {
                    _code << "<!-- dependent variables without operations -->" << _endline;
                    commentWritten = true;
                }

                std::string varName = _nameGen->generateDependent(i);
                const std::string& indepName = *dependent[i].getOperationNode()->getName();
                _code << _startEq
                        << "<mrow id='" << createID(dependent[i].getOperationNode()) << "' class='dep'>" << varName << "</mrow>"
                        << _assignStr
                        << "<mrow id='" << createID(dependent[i].getOperationNode()) << "' class='indep'>" << indepName << "</mrow>";
                printAssigmentEnd(*dependent[i].getOperationNode());
            }
        }

        /**
         * encapsulate the code in a function
         */
        if (mathMLFiles.empty()) {
            // a single source file
            printAlgorithmFileStart(_ss);
            _ss << _code.str();
            printAlgorithmFileEnd(_ss);

            out << _ss.str();

            if (_sources != nullptr) {
                (*_sources)[_filename + ".html"] = _ss.str();
            }
        } else {
            // there are multiple source files (this last one is the master)
            (*_sources)[_filename + ".html"] = _code.str();
        }

    }

    inline size_t getVariableID(const OperationNode<Base>& node) const {
        return _info->varId[node];
    }

    inline virtual void printAlgorithmFileStart(std::ostream& out) {
        out << "<!DOCTYPE html>" << _endline <<
                "<html lang=\"en\">" << _endline <<
                "<head>" << _endline <<
                "<meta charset=\"utf-8\">" << _endline <<
                "<title>" << _filename << "</title>" << _endline;

        if (!_headExtra.empty()) {
            out << _headExtra;
        }

        if (!_style.empty()) {
            out << "<style>" << _endline <<
                    _style <<
                    "</style>" << _endline;
        }

        if (!_javascript.empty()) {
            out << "<script type=\"text/javascript\">\n" <<
                    _javascript << "\n"
                    "</script>\n";
        }

        out << "</head>" << _endline <<
                "" << _endline <<
                "<body>" << _endline <<
                "<!-- source file for '" << _filename << "' (automatically generated by CppADCodeGen) -->" << _endline <<
                "<div id='algorithm'>" << _endline;
    }

    inline virtual void printAlgorithmFileEnd(std::ostream& out) {
        out << "</div>" << _endline <<
                "</body>" << _endline <<
                "</html>";
    }

    inline unsigned printAssigment(OperationNode<Base>& node) {
        return printAssigment(node, node);
    }

    inline unsigned printAssigment(OperationNode<Base>& nodeName,
                                   const Argument<Base>& nodeRhs) {
        if (nodeRhs.getOperation() != nullptr) {
            return printAssigment(nodeName, *nodeRhs.getOperation());
        } else {
            printAssigmentStart(nodeName);
            printParameter(*nodeRhs.getParameter());
            printAssigmentEnd(nodeName);
            return 1;
        }
    }

    inline unsigned printAssigment(OperationNode<Base>& nodeName,
                                   OperationNode<Base>& nodeRhs) {
        bool createsVar = directlyAssignsVariable(nodeRhs); // do we need to do the assignment here?
        if (!createsVar) {
            printAssigmentStart(nodeName);
        }
        unsigned lines = printExpressionNoVarCheck(nodeRhs);
        if (!createsVar) {
            printAssigmentEnd(nodeRhs);
        }

        if (nodeRhs.getOperationType() == CGOpCode::ArrayElement) {
            OperationNode<Base>* array = nodeRhs.getArguments()[0].getOperation();
            size_t arrayId = getVariableID(*array);
            size_t pos = nodeRhs.getInfo()[0];
            if (array->getOperationType() == CGOpCode::ArrayCreation)
                _tmpArrayValues[arrayId - 1 + pos] = nullptr; // this could probably be removed!
            else
                _tmpSparseArrayValues[arrayId - 1 + pos] = nullptr; // this could probably be removed!
        }

        return lines;
    }

    inline virtual void printAssigmentStart(OperationNode<Base>& op) {
        printAssigmentStart(op, createVariableName(op), isDependent(op));
    }

    inline virtual void printAssigmentStart(OperationNode<Base>& node, const std::string& varName, bool isDep) {
        if (!isDep) {
            _temporary[getVariableID(node)] = &node;
        }

        _code << _startEq;
        _code << "<mrow id='" << createID(node) << "' class='" << (isDep ? "dep" : "tmp") << "'>" << varName << "</mrow>";

        CGOpCode op = node.getOperationType();
        if (op == CGOpCode::DependentMultiAssign || (op == CGOpCode::LoopIndexedDep && node.getInfo()[1] == 1)) {
            _code << _assignAddStr; // +=
        } else {
            _code << _assignStr; // =
        }
    }

    inline virtual void printAssigmentEnd() {
        _code << _endEq << _endline;
    }

    inline virtual void printAssigmentEnd(OperationNode<Base>& op) {
        printAssigmentEnd();
    }

    virtual void saveLocalFunction(std::vector<std::string>& localFuncNames,
                                   bool zeroDependentArray) {
        _ss << _filename << "__part_" << (localFuncNames.size() + 1);
        std::string funcName = _ss.str();
        _ss.str("");

        // loop indexes
        _nameGen->prepareCustomFunctionVariables(_ss);
        _ss << _code.str();
        _nameGen->finalizeCustomFunctionVariables(_ss);

        (*_sources)[funcName + ".html"] = _ss.str();
        localFuncNames.push_back(funcName);

        _code.str("");
        _ss.str("");
    }

    virtual bool createsNewVariable(const OperationNode<Base>& var,
                                    size_t totalUseCount) const override {
        CGOpCode op = var.getOperationType();
        if (totalUseCount > 1) {
            return op != CGOpCode::ArrayElement && op != CGOpCode::Index && op != CGOpCode::IndexDeclaration && op != CGOpCode::Tmp;
        } else {
            return ( op == CGOpCode::ArrayCreation ||
                    op == CGOpCode::SparseArrayCreation ||
                    op == CGOpCode::AtomicForward ||
                    op == CGOpCode::AtomicReverse ||
                    op == CGOpCode::ComLt ||
                    op == CGOpCode::ComLe ||
                    op == CGOpCode::ComEq ||
                    op == CGOpCode::ComGe ||
                    op == CGOpCode::ComGt ||
                    op == CGOpCode::ComNe ||
                    op == CGOpCode::LoopIndexedDep ||
                    op == CGOpCode::LoopIndexedTmp ||
                    op == CGOpCode::IndexAssign ||
                    op == CGOpCode::Assign) &&
                    op != CGOpCode::CondResult;
        }
    }

    virtual bool requiresVariableName(const OperationNode<Base>& var) const {
        CGOpCode op = var.getOperationType();
        return (_info->totalUseCount.get(var) > 1 &&
                op != CGOpCode::AtomicForward &&
                op != CGOpCode::AtomicReverse &&
                op != CGOpCode::LoopStart &&
                op != CGOpCode::LoopEnd &&
                op != CGOpCode::Index &&
                op != CGOpCode::IndexAssign &&
                op != CGOpCode::StartIf &&
                op != CGOpCode::ElseIf &&
                op != CGOpCode::Else &&
                op != CGOpCode::EndIf &&
                op != CGOpCode::CondResult &&
                op != CGOpCode::LoopIndexedTmp &&
                op != CGOpCode::Tmp);
    }

    /**
     * Whether or not this operation assign its expression to a variable by
     * itself.
     * 
     * @param var the operation node
     * @return 
     */
    virtual bool directlyAssignsVariable(const OperationNode<Base>& var) const {
        CGOpCode op = var.getOperationType();
        return isCondAssign(op) ||
                op == CGOpCode::ArrayCreation ||
                op == CGOpCode::SparseArrayCreation ||
                op == CGOpCode::AtomicForward ||
                op == CGOpCode::AtomicReverse ||
                op == CGOpCode::DependentMultiAssign ||
                op == CGOpCode::LoopStart ||
                op == CGOpCode::LoopEnd ||
                op == CGOpCode::IndexAssign ||
                op == CGOpCode::StartIf ||
                op == CGOpCode::ElseIf ||
                op == CGOpCode::Else ||
                op == CGOpCode::EndIf ||
                op == CGOpCode::CondResult ||
                op == CGOpCode::IndexDeclaration;
    }

    virtual bool requiresVariableArgument(enum CGOpCode op, size_t argIndex) const override {
        return op == CGOpCode::CondResult;
    }

    inline const std::string& createVariableName(OperationNode<Base>& var) {
        CGOpCode op = var.getOperationType();
        CPPADCG_ASSERT_UNKNOWN(getVariableID(var) > 0);
        CPPADCG_ASSERT_UNKNOWN(op != CGOpCode::AtomicForward);
        CPPADCG_ASSERT_UNKNOWN(op != CGOpCode::AtomicReverse);
        CPPADCG_ASSERT_UNKNOWN(op != CGOpCode::LoopStart);
        CPPADCG_ASSERT_UNKNOWN(op != CGOpCode::LoopEnd);
        CPPADCG_ASSERT_UNKNOWN(op != CGOpCode::Index);
        CPPADCG_ASSERT_UNKNOWN(op != CGOpCode::IndexAssign);
        CPPADCG_ASSERT_UNKNOWN(op != CGOpCode::IndexDeclaration);

        if (var.getName() == nullptr) {
            if (op == CGOpCode::ArrayCreation) {
                var.setName(_nameGen->generateTemporaryArray(var, getVariableID(var)));

            } else if (op == CGOpCode::SparseArrayCreation) {
                var.setName(_nameGen->generateTemporarySparseArray(var, getVariableID(var)));

            } else if (op == CGOpCode::LoopIndexedDep) {
                size_t pos = var.getInfo()[0];
                const IndexPattern* ip = _info->loopDependentIndexPatterns[pos];
                var.setName(_nameGen->generateIndexedDependent(var, getVariableID(var), *ip));

            } else if (op == CGOpCode::LoopIndexedIndep) {
                size_t pos = var.getInfo()[1];
                const IndexPattern* ip = _info->loopIndependentIndexPatterns[pos];
                var.setName(_nameGen->generateIndexedIndependent(var, getVariableID(var), *ip));

            } else if (getVariableID(var) <= _independentSize) {
                // independent variable
                var.setName(_nameGen->generateIndependent(var, getVariableID(var)));

            } else if (getVariableID(var) < _minTemporaryVarID) {
                // dependent variable
                std::map<size_t, size_t>::const_iterator it = _dependentIDs.find(getVariableID(var));
                CPPADCG_ASSERT_UNKNOWN(it != _dependentIDs.end());

                size_t index = it->second;
                var.setName(_nameGen->generateDependent(index));

            } else if (op == CGOpCode::LoopIndexedTmp || op == CGOpCode::Tmp) {
                CPPADCG_ASSERT_KNOWN(var.getArguments().size() >= 1, "Invalid number of arguments for loop indexed temporary operation");
                OperationNode<Base>* tmpVar = var.getArguments()[0].getOperation();
                CPPADCG_ASSERT_KNOWN(tmpVar != nullptr && tmpVar->getOperationType() == CGOpCode::TmpDcl, "Invalid arguments for loop indexed temporary operation");
                return createVariableName(*tmpVar);

            } else {
                // temporary variable
                var.setName(_nameGen->generateTemporary(var, getVariableID(var)));
            }
        }


        return *var.getName();
    }

    inline std::string createID(OperationNode<Base>* var) {
        return createID(*var);
    }

    virtual std::string createID(OperationNode<Base>& var) {
        size_t id = getVariableID(var);
        if (varIds_.size() <= id) {
            varIds_.resize(id + 1 + varIds_.size() * 3 / 2, 0);
        }

        int n = varIds_[id];
        varIds_[id]++;

        if (n == 0)
            return "v" + std::to_string(id);
        else
            return "v" + std::to_string(id) + "_" + std::to_string(n);
    }

    virtual void printIndependentVariableName(OperationNode<Base>& op) {
        CPPADCG_ASSERT_KNOWN(op.getArguments().size() == 0, "Invalid number of arguments for independent variable");
        //size_t id = getVariableID(op);
        _code << "<mrow id='" << createID(op) << "' class='indep'>" << _nameGen->generateIndependent(op, getVariableID(op)) << "</mrow>";
    }

    virtual unsigned print(const Argument<Base>& arg) {
        if (arg.getOperation() != nullptr) {
            // expression
            return printExpression(*arg.getOperation());
        } else {
            // parameter
            printParameter(*arg.getParameter());
            return 1;
        }
    }

    virtual unsigned printExpression(OperationNode<Base>& node) {
        if (getVariableID(node) > 0) {
            const std::string& name = createVariableName(node); // use variable name
            //size_t id = getVariableID(node);

            CGOpCode op = node.getOperationType();
            if (getVariableID(node) >= _minTemporaryVarID || op == CGOpCode::ArrayCreation || op == CGOpCode::SparseArrayCreation || op == CGOpCode::LoopIndexedDep || op == CGOpCode::LoopIndexedIndep) {

                _code << "<mrow id='" << createID(node) << "' class='tmp'>" << name << "</mrow>"; // TODO!!!!!!!!!!!!!!!!!!!!!!!

            } else if (getVariableID(node) <= _independentSize) {
                // independent variable
                _code << "<mrow id='" << createID(node) << "' class='indep'>" << name << "</mrow>";

            } else {
                // dependent variable
                _code << "<mrow id='" << createID(node) << "' class='dep'>" << name << "</mrow>";

            }

            return 1;
        } else {
            // print expression code
            return printExpressionNoVarCheck(node);
        }
    }

    virtual unsigned printExpressionNoVarCheck(OperationNode<Base>& node) {
        CGOpCode op = node.getOperationType();
        switch (op) {
            case CGOpCode::ArrayCreation:
                printArrayCreationOp(node);
                break;
            case CGOpCode::SparseArrayCreation:
                printSparseArrayCreationOp(node);
                break;
            case CGOpCode::ArrayElement:
                printArrayElementOp(node);
                break;
            case CGOpCode::Assign:
                return printAssignOp(node);

            case CGOpCode::Abs:
            case CGOpCode::Acos:
            case CGOpCode::Asin:
            case CGOpCode::Atan:
            case CGOpCode::Cosh:
            case CGOpCode::Cos:
            case CGOpCode::Exp:
            case CGOpCode::Log:
            case CGOpCode::Sign:
            case CGOpCode::Sinh:
            case CGOpCode::Sin:
            case CGOpCode::Sqrt:
            case CGOpCode::Tanh:
            case CGOpCode::Tan:
                printUnaryFunction(node);
                break;
            case CGOpCode::AtomicForward: // atomicFunction.forward(q, p, vx, vy, tx, ty)
                printAtomicForwardOp(node);
                break;
            case CGOpCode::AtomicReverse: // atomicFunction.reverse(p, tx, ty, px, py)
                printAtomicReverseOp(node);
                break;
            case CGOpCode::Add:
                printOperationAdd(node);
                break;
            case CGOpCode::Alias:
                return printOperationAlias(node);

            case CGOpCode::ComLt:
            case CGOpCode::ComLe:
            case CGOpCode::ComEq:
            case CGOpCode::ComGe:
            case CGOpCode::ComGt:
            case CGOpCode::ComNe:
                printConditionalAssignment(node);
                break;
            case CGOpCode::Div:
                printOperationDiv(node);
                break;
            case CGOpCode::Inv:
                printIndependentVariableName(node);
                break;
            case CGOpCode::Mul:
                printOperationMul(node);
                break;
            case CGOpCode::Pow:
                printPowFunction(node);
                break;
            case CGOpCode::Pri:
                // do nothing
                break;
            case CGOpCode::Sub:
                printOperationMinus(node);
                break;

            case CGOpCode::UnMinus:
                printOperationUnaryMinus(node);
                break;

            case CGOpCode::DependentMultiAssign:
                return printDependentMultiAssign(node);

            case CGOpCode::Index:
                return 0; // nothing to do
            case CGOpCode::IndexAssign:
                printIndexAssign(node);
                break;
            case CGOpCode::IndexDeclaration:
                return 0; // already done

            case CGOpCode::LoopStart:
                printLoopStart(node);
                break;
            case CGOpCode::LoopIndexedIndep:
                printLoopIndexedIndep(node);
                break;
            case CGOpCode::LoopIndexedDep:
                printLoopIndexedDep(node);
                break;
            case CGOpCode::LoopIndexedTmp:
                printLoopIndexedTmp(node);
                break;
            case CGOpCode::TmpDcl:
                // nothing to do
                return 0;
            case CGOpCode::Tmp:
                printTmpVar(node);
                break;
            case CGOpCode::LoopEnd:
                printLoopEnd(node);
                break;
            case CGOpCode::IndexCondExpr:
                printIndexCondExprOp(node);
                break;
            case CGOpCode::StartIf:
                printStartIf(node);
                break;
            case CGOpCode::ElseIf:
                printElseIf(node);
                break;
            case CGOpCode::Else:
                printElse(node);
                break;
            case CGOpCode::EndIf:
                printEndIf(node);
                break;
            case CGOpCode::CondResult:
                printCondResult(node);
                break;
            default:
                throw CGException("Unknown operation code '", op, "'.");
        }
        return 1;
    }

    virtual unsigned printAssignOp(OperationNode<Base>& node) {
        CPPADCG_ASSERT_KNOWN(node.getArguments().size() == 1, "Invalid number of arguments for assign operation");

        return print(node.getArguments()[0]);
    }

    virtual void printUnaryFunction(OperationNode<Base>& op) {
        CPPADCG_ASSERT_KNOWN(op.getArguments().size() == 1, "Invalid number of arguments for unary function");

        switch (op.getOperationType()) {
            case CGOpCode::Abs:
                _code << "<mi>abs</mi>";
                break;
            case CGOpCode::Acos:
                _code << "<mi>arccos</mi>";
                break;
            case CGOpCode::Asin:
                _code << "<mi>arcsin</mi>";
                break;
            case CGOpCode::Atan:
                _code << "<mi>arctan</mi>";
                break;
            case CGOpCode::Cosh:
                _code << "<mi>cosh</mi>";
                break;
            case CGOpCode::Cos:
                _code << "<mi>cos</mi>";
                break;
            case CGOpCode::Exp:
                _code << "<mi>exp</mi>"; ///////////////////////////////////////// consider using superscript
                break;
            case CGOpCode::Log:
                _code << "<mi>ln</mi>";
                break;
            case CGOpCode::Sinh:
                _code << "<mi>sinh</mi>";
                break;
            case CGOpCode::Sign:
                _code << "<mi>sgn</mi>";
                break;
            case CGOpCode::Sin:
                _code << "<mi>sin</mi>";
                break;
            case CGOpCode::Sqrt:
                _code << "<msqrt><mrow>";
                print(op.getArguments()[0]);
                _code << "</mrow></msqrt>";
                return;
            case CGOpCode::Tanh:
                _code << "<mi>tanh</mi>";
                break;
            case CGOpCode::Tan:
                _code << "<mi>tan</mi>";
                break;
            default:
                throw CGException("Unknown function name for operation code '", op.getOperationType(), "'.");
        }

        _code << "<mo>&ApplyFunction;</mo>"
                "<mfenced><mrow>";
        print(op.getArguments()[0]);
        _code << "</mrow></mfenced>";
    }

    virtual void printPowFunction(OperationNode<Base>& op) {
        CPPADCG_ASSERT_KNOWN(op.getArguments().size() == 2, "Invalid number of arguments for pow() function");

        auto encloseInParentheses = [this](const OperationNode<Base>* node) {
            while (node != nullptr) {
                if (getVariableID(*node) != 0)
                    return false;
                if (node->getOperationType() == CGOpCode::Alias)
                    node = node->getArguments()[0].getOperation();
                else
                    break;
            }
            return node != nullptr &&
                    getVariableID(*node) == 0 &&
                    !isFunction(node->getOperationType());
        };


        bool encloseBase = encloseInParentheses(op.getArguments()[0].getOperation());
        bool encloseExpo = encloseInParentheses(op.getArguments()[1].getOperation());

        _code << "<msup>";
        if (encloseBase)
            _code << "<mfenced>";
        _code << "<mrow>";
        print(op.getArguments()[0]);
        _code << "</mrow>";
        if (encloseBase)
            _code << "</mfenced>";
        if (encloseExpo)
            _code << "<mfenced>";
        _code << "<mrow>";
        print(op.getArguments()[1]);
        _code << "</mrow>";
        if (encloseExpo)
            _code << "</mfenced>";
        _code << "</msup>";
    }

    virtual unsigned printOperationAlias(OperationNode<Base>& op) {
        CPPADCG_ASSERT_KNOWN(op.getArguments().size() == 1, "Invalid number of arguments for alias");
        return print(op.getArguments()[0]);
    }

    virtual void printOperationAdd(OperationNode<Base>& op) {
        CPPADCG_ASSERT_KNOWN(op.getArguments().size() == 2, "Invalid number of arguments for addition");

        print(op.getArguments()[0]);
        _code << "<mo>+</mo>";
        print(op.getArguments()[1]);
    }

    virtual void printOperationMinus(OperationNode<Base>& op) {
        CPPADCG_ASSERT_KNOWN(op.getArguments().size() == 2, "Invalid number of arguments for subtraction");

        const Argument<Base>& left = op.getArguments()[0];
        const Argument<Base>& right = op.getArguments()[1];

        bool encloseRight = encloseInParenthesesMul(right);

        print(left);
        _code << "<mo>-</mo>";
        if (encloseRight) {
            _code << "<mfenced><mrow>";
        }
        print(right);
        if (encloseRight) {
            _code << "</mrow></mfenced>";
        }
    }

    virtual void printOperationDiv(OperationNode<Base>& op) {
        CPPADCG_ASSERT_KNOWN(op.getArguments().size() == 2, "Invalid number of arguments for division");

        const Argument<Base>& left = op.getArguments()[0];
        const Argument<Base>& right = op.getArguments()[1];

        _code << "<mfrac>";
        _code << "<mrow>";
        print(left);
        _code << "</mrow>";
        _code << "<mrow>";
        print(right);
        _code << "</mrow>";
        _code << "</mfrac>";
    }

    inline bool encloseInParenthesesMul(const Argument<Base>& arg) const {
        if (arg.getParameter() != nullptr) {
            return ((*arg.getParameter()) < 0);
        } else {
            return encloseInParenthesesMul(arg.getOperation());
        }
    }

    inline bool encloseInParenthesesMul(const OperationNode<Base>* node) const {
        while (node != nullptr) {
            if (getVariableID(*node) != 0) {
                return false;
            } else if (node->getOperationType() == CGOpCode::Alias) {
                node = node->getArguments()[0].getOperation();
            } else {
                break;
            }
        }
        return node != nullptr &&
                getVariableID(*node) == 0 &&
                node->getOperationType() != CGOpCode::Div &&
                node->getOperationType() != CGOpCode::Mul &&
                !isFunction(node->getOperationType());
    }

    virtual void printOperationMul(OperationNode<Base>& op) {
        CPPADCG_ASSERT_KNOWN(op.getArguments().size() == 2, "Invalid number of arguments for multiplication");

        const Argument<Base>& left = op.getArguments()[0];
        const Argument<Base>& right = op.getArguments()[1];

        bool encloseLeft = encloseInParenthesesMul(left);
        bool encloseRight = encloseInParenthesesMul(right);

        auto isNumber = [](const OperationNode<Base>* node, int pos) -> bool {
            while (node != nullptr) {
                if (node->getOperationType() == CGOpCode::Alias) {
                    node = node->getArguments()[0].getOperation();
                    continue;
                }
                CGOpCode op = node->getOperationType();
                if (op == CGOpCode::Mul) {
                    node = node->getArguments()[pos].getOperation();
                } else if (pos == 0 && op == CGOpCode::Pow) {
                    node = node->getArguments()[0].getOperation();
                } else {
                    return false;
                }
            }
            return true; // a constant number
        };

        if (encloseLeft) {
            _code << "<mfenced><mrow>";
        }
        print(left);
        if (encloseLeft) {
            _code << "</mrow></mfenced>";
        }

        if (isNumber(left.getOperation(), 1) && isNumber(right.getOperation(), 0))
            _code << "<mo>&times;</mo>"; // numbers too close together are difficult to distinguish
        else
            _code << "<mo>&it;</mo>"; // invisible times

        if (encloseRight) {
            _code << "<mfenced><mrow>";
        }
        print(right);
        if (encloseRight) {
            _code << "</mrow></mfenced>";
        }
    }

    virtual void printOperationUnaryMinus(OperationNode<Base>& op) {
        CPPADCG_ASSERT_KNOWN(op.getArguments().size() == 1, "Invalid number of arguments for unary minus");

        const Argument<Base>& arg = op.getArguments()[0];

        bool enclose = encloseInParenthesesMul(arg);

        _code << "<mo>-</mo>";
        if (enclose) {
            _code << "<mfenced><mrow>";
        }
        print(arg);
        if (enclose) {
            _code << "</mrow></mfenced>";
        }
    }

    virtual void printConditionalAssignment(OperationNode<Base>& node) {
        CPPADCG_ASSERT_UNKNOWN(getVariableID(node) > 0);

        const std::vector<Argument<Base> >& args = node.getArguments();
        const Argument<Base> &left = args[0];
        const Argument<Base> &right = args[1];
        const Argument<Base> &trueCase = args[2];
        const Argument<Base> &falseCase = args[3];

        bool isDep = isDependent(node);
        const std::string& varName = createVariableName(node);

        if ((trueCase.getParameter() != nullptr && falseCase.getParameter() != nullptr && *trueCase.getParameter() == *falseCase.getParameter()) ||
                (trueCase.getOperation() != nullptr && falseCase.getOperation() != nullptr && trueCase.getOperation() == falseCase.getOperation())) {
            // true and false cases are the same
            printAssigmentStart(node, varName, isDep);
            print(trueCase);
            printAssigmentEnd(node);
        } else {

            _code << _ifStart << _startEq << "<mi>if</mi>"
                    "<mfenced><mrow>";
            print(left);
            _code << "<mo>";
            getComparison(_code, node.getOperationType());
            _code << "</mo>";
            print(right);
            _code << "</mrow></mfenced>" << _endEq << _endline
                    << _condBodyStart << _endline;

            //checkEquationEnvStart(); // no need
            printAssigmentStart(node, varName, isDep);
            print(trueCase);
            printAssigmentEnd(node);
            _code << _condBodyEnd << _endline << _ifEnd << _endline;

            // else
            _code << _elseStart << _startEq << "<mi>else</mi>" << _endEq << _endline
                    << _condBodyStart << _endline;
            //checkEquationEnvStart(); // no need
            printAssigmentStart(node, varName, isDep);
            print(falseCase);
            printAssigmentEnd(node);
            _code << _condBodyEnd << _endline << _elseEnd << _endline; // end if
        }
    }

    inline bool isSameArgument(const Argument<Base>& newArg,
                               const Argument<Base>* oldArg) {
        if (oldArg != nullptr) {
            if (oldArg->getParameter() != nullptr) {
                if (newArg.getParameter() != nullptr) {
                    return (*newArg.getParameter() == *oldArg->getParameter());
                }
            } else {
                return (newArg.getOperation() == oldArg->getOperation());
            }
        }
        return false;
    }

    virtual void printArrayCreationOp(OperationNode<Base>& op);

    virtual void printSparseArrayCreationOp(OperationNode<Base>& op);

    inline void printArrayStructInit(const std::string& dataArrayName,
                                     size_t pos,
                                     const std::vector<OperationNode<Base>*>& arrays,
                                     size_t k);

    inline void printArrayStructInit(const std::string& dataArrayName,
                                     OperationNode<Base>& array);

    inline void markArrayChanged(OperationNode<Base>& ty);

    inline size_t printArrayCreationUsingLoop(size_t startPos,
                                              OperationNode<Base>& array,
                                              size_t startj,
                                              std::vector<const Argument<Base>*>& tmpArrayValues);

    inline std::string getTempArrayName(const OperationNode<Base>& op);

    virtual void printArrayElementOp(OperationNode<Base>& op);

    virtual void printAtomicForwardOp(OperationNode<Base>& atomicFor) {
        CPPADCG_ASSERT_KNOWN(atomicFor.getInfo().size() == 3, "Invalid number of information elements for atomic forward operation");
        int q = atomicFor.getInfo()[1];
        int p = atomicFor.getInfo()[2];
        size_t p1 = p + 1;
        const std::vector<Argument<Base> >& opArgs = atomicFor.getArguments();
        CPPADCG_ASSERT_KNOWN(opArgs.size() == p1 * 2, "Invalid number of arguments for atomic forward operation");

        size_t id = atomicFor.getInfo()[0];
        std::vector<OperationNode<Base>*> tx(p1), ty(p1);
        for (size_t k = 0; k < p1; k++) {
            tx[k] = opArgs[0 * p1 + k].getOperation();
            ty[k] = opArgs[1 * p1 + k].getOperation();
        }

        CPPADCG_ASSERT_KNOWN(tx[0]->getOperationType() == CGOpCode::ArrayCreation, "Invalid array type");
        CPPADCG_ASSERT_KNOWN(p == 0 || tx[1]->getOperationType() == CGOpCode::SparseArrayCreation, "Invalid array type");
        CPPADCG_ASSERT_KNOWN(ty[p]->getOperationType() == CGOpCode::ArrayCreation, "Invalid array type");

        // tx
        for (size_t k = 0; k < p1; k++) {
            printArrayStructInit(_ATOMIC_TX, k, tx, k);
        }
        // ty
        printArrayStructInit(_ATOMIC_TY, *ty[p]);
        _ss.str("");

        _code << _startEq
                << _info->atomicFunctionId2Name.at(id) << "<mo>.</mo><mo>forward</mo>"
                "<mfenced separators=','>"
                "<mn>" << q << "</mn>"
                "<mn>" << p << "</mn>"
                "<mrow class='tmp'>" << _ATOMIC_TX << "</mrow>"
                "<mrow><mo>&amp;</mo><mrow class='tmp'>" << _ATOMIC_TY << "</mrow></mrow>"
                "</mfenced>"
                << _endEq << _endline;

        /**
         * the values of ty are now changed
         */
        markArrayChanged(*ty[p]);
    }

    virtual void printAtomicReverseOp(OperationNode<Base>& atomicRev) {
        CPPADCG_ASSERT_KNOWN(atomicRev.getInfo().size() == 2, "Invalid number of information elements for atomic reverse operation");
        int p = atomicRev.getInfo()[1];
        size_t p1 = p + 1;
        const std::vector<Argument<Base> >& opArgs = atomicRev.getArguments();
        CPPADCG_ASSERT_KNOWN(opArgs.size() == p1 * 4, "Invalid number of arguments for atomic reverse operation");

        size_t id = atomicRev.getInfo()[0];
        std::vector<OperationNode<Base>*> tx(p1), px(p1), py(p1);
        for (size_t k = 0; k < p1; k++) {
            tx[k] = opArgs[0 * p1 + k].getOperation();
            px[k] = opArgs[2 * p1 + k].getOperation();
            py[k] = opArgs[3 * p1 + k].getOperation();
        }

        CPPADCG_ASSERT_KNOWN(tx[0]->getOperationType() == CGOpCode::ArrayCreation, "Invalid array type");
        CPPADCG_ASSERT_KNOWN(p == 0 || tx[1]->getOperationType() == CGOpCode::SparseArrayCreation, "Invalid array type");

        CPPADCG_ASSERT_KNOWN(px[0]->getOperationType() == CGOpCode::ArrayCreation, "Invalid array type");

        CPPADCG_ASSERT_KNOWN(py[0]->getOperationType() == CGOpCode::SparseArrayCreation, "Invalid array type");
        CPPADCG_ASSERT_KNOWN(p == 0 || py[1]->getOperationType() == CGOpCode::ArrayCreation, "Invalid array type");

        // tx
        for (size_t k = 0; k < p1; k++) {
            printArrayStructInit(_ATOMIC_TX, k, tx, k);
        }
        // py
        for (size_t k = 0; k < p1; k++) {
            printArrayStructInit(_ATOMIC_PY, k, py, k);
        }
        // px
        printArrayStructInit(_ATOMIC_PX, *px[0]);
        _ss.str("");

        _code << _startEq
                << _info->atomicFunctionId2Name.at(id) << "<mo>.</mo><mo>reverse</mo>"
                "<mfenced separators=','>"
                "<mn>" << p << "</mn>"
                "<mrow class='tmp'>" << _ATOMIC_TX << "</mrow>"
                "<mrow><mo>&amp;</mo><mrow class='tmp'>" << _ATOMIC_PX << "</mrow></mrow>"
                "<mrow class='tmp'>" << _ATOMIC_PY << "</mrow>"
                "</mfenced>"
                << _endEq << _endline;

        /**
         * the values of px are now changed
         */
        markArrayChanged(*px[0]);
    }

    virtual unsigned printDependentMultiAssign(OperationNode<Base>& node) {
        CPPADCG_ASSERT_KNOWN(node.getOperationType() == CGOpCode::DependentMultiAssign, "Invalid node type");
        CPPADCG_ASSERT_KNOWN(node.getArguments().size() > 0, "Invalid number of arguments");

        const std::vector<Argument<Base> >& args = node.getArguments();
        for (size_t a = 0; a < args.size(); a++) {
            bool useArg = false;
            const Argument<Base>& arg = args[a];
            if (arg.getParameter() != nullptr) {
                useArg = true;
            } else {
                CGOpCode op = arg.getOperation()->getOperationType();
                useArg = op != CGOpCode::DependentRefRhs && op != CGOpCode::LoopEnd && op != CGOpCode::EndIf;
            }

            if (useArg) {
                printAssigment(node, arg); // ignore other arguments!
                return 1;
            }
        }
        return 0;
    }

    virtual void printLoopStart(OperationNode<Base>& node) {
        CPPADCG_ASSERT_KNOWN(node.getOperationType() == CGOpCode::LoopStart, "Invalid node type");

        LoopStartOperationNode<Base>& lnode = static_cast<LoopStartOperationNode<Base>&> (node);
        _currentLoops.push_back(&lnode);

        const std::string& jj = *lnode.getIndex().getName();
        std::string lastIt;
        if (lnode.getIterationCountNode() != nullptr) {
            lastIt = *lnode.getIterationCountNode()->getIndex().getName() + " <mo>-</mo> <mn>1</mn>";
        } else {
            lastIt = "<mn>" + std::to_string(lnode.getIterationCount() - 1) + "</mn>";
        }

        _code << _forStart << _startEq << "<mi>for</mi>"
                "<mfenced><mrow>"
                << jj << "<mo>&isin;</mo>"
                "<mfenced open='[' close='[' separators=';'>"
                "<mn>0</mn>" << lastIt <<
                "</mfenced>"
                "</mrow></mfenced>" << _endEq << _endline
                << _forBodyStart;
        _indentationLevel++;
    }

    virtual void printLoopEnd(OperationNode<Base>& node) {
        CPPADCG_ASSERT_KNOWN(node.getOperationType() == CGOpCode::LoopEnd, "Invalid node type");

        _indentationLevel--;

        _code << _forBodyEnd << _forEnd << _endline;

        _currentLoops.pop_back();
    }

    virtual void printLoopIndexedDep(OperationNode<Base>& node) {
        CPPADCG_ASSERT_KNOWN(node.getArguments().size() >= 1, "Invalid number of arguments for loop indexed dependent operation");

        // LoopIndexedDep
        print(node.getArguments()[0]);
    }

    virtual void printLoopIndexedIndep(OperationNode<Base>& node) {
        CPPADCG_ASSERT_KNOWN(node.getOperationType() == CGOpCode::LoopIndexedIndep, "Invalid node type");
        CPPADCG_ASSERT_KNOWN(node.getInfo().size() == 1, "Invalid number of information elements for loop indexed independent operation");

        // CGLoopIndexedIndepOp
        size_t pos = node.getInfo()[1];
        const IndexPattern* ip = _info->loopIndependentIndexPatterns[pos];
        _code << _nameGen->generateIndexedIndependent(node, getVariableID(node), *ip);
    }

    virtual void printLoopIndexedTmp(OperationNode<Base>& node) {
        CPPADCG_ASSERT_KNOWN(node.getOperationType() == CGOpCode::LoopIndexedTmp, "Invalid node type");
        CPPADCG_ASSERT_KNOWN(node.getArguments().size() == 2, "Invalid number of arguments for loop indexed temporary operation");
        OperationNode<Base>* tmpVar = node.getArguments()[0].getOperation();
        CPPADCG_ASSERT_KNOWN(tmpVar != nullptr && tmpVar->getOperationType() == CGOpCode::TmpDcl, "Invalid arguments for loop indexed temporary operation");

        print(node.getArguments()[1]);
    }

    virtual void printTmpVar(OperationNode<Base>& node) {
        CPPADCG_ASSERT_KNOWN(node.getOperationType() == CGOpCode::Tmp, "Invalid node type");
        CPPADCG_ASSERT_KNOWN(node.getArguments().size() > 0, "Invalid number of arguments for temporary variable usage operation");
        OperationNode<Base>* tmpVar = node.getArguments()[0].getOperation();
        CPPADCG_ASSERT_KNOWN(tmpVar != nullptr && tmpVar->getOperationType() == CGOpCode::TmpDcl, "Invalid arguments for loop indexed temporary operation");

        _code << "<mrow id='" << createID(tmpVar) << "' class='tmp'>" << *tmpVar->getName() << "</mrow>";
    }

    virtual void printIndexAssign(OperationNode<Base>& node) {
        CPPADCG_ASSERT_KNOWN(node.getOperationType() == CGOpCode::IndexAssign, "Invalid node type");
        CPPADCG_ASSERT_KNOWN(node.getArguments().size() > 0, "Invalid number of arguments for an index assignment operation");

        IndexAssignOperationNode<Base>& inode = static_cast<IndexAssignOperationNode<Base>&> (node);

        const IndexPattern& ip = inode.getIndexPattern();
        _code << _startEq
                << (*inode.getIndex().getName())
                << _assignStr;
        indexPattern2String(_code, ip, inode.getIndexPatternIndexes());
        _code << _endEq << _endline;
    }

    virtual void printIndexCondExprOp(OperationNode<Base>& node) {
        CPPADCG_ASSERT_KNOWN(node.getOperationType() == CGOpCode::IndexCondExpr, "Invalid node type");
        CPPADCG_ASSERT_KNOWN(node.getArguments().size() == 1, "Invalid number of arguments for an index condition expression operation");
        CPPADCG_ASSERT_KNOWN(node.getArguments()[0].getOperation() != nullptr, "Invalid argument for an index condition expression operation");
        CPPADCG_ASSERT_KNOWN(node.getArguments()[0].getOperation()->getOperationType() == CGOpCode::Index, "Invalid argument for an index condition expression operation");

        const std::vector<size_t>& info = node.getInfo();

        IndexOperationNode<Base>& iterationIndexOp = static_cast<IndexOperationNode<Base>&> (*node.getArguments()[0].getOperation());
        const std::string& index = *iterationIndexOp.getIndex().getName();

        printIndexCondExpr(_code, info, index);
    }

    virtual void printStartIf(OperationNode<Base>& node) {
        /**
         * the first argument is the condition, following arguments are
         * just extra dependencies that must be defined outside the if
         */
        CPPADCG_ASSERT_KNOWN(node.getOperationType() == CGOpCode::StartIf, "Invalid node type");
        CPPADCG_ASSERT_KNOWN(node.getArguments().size() >= 1, "Invalid number of arguments for an 'if start' operation");
        CPPADCG_ASSERT_KNOWN(node.getArguments()[0].getOperation() != nullptr, "Invalid argument for an 'if start' operation");

        _code << _ifStart << _startEq << "<mi>if</mi>"
                "<mfenced><mrow>";
        printIndexCondExprOp(*node.getArguments()[0].getOperation());
        _code << "</mrow></mfenced>" << _endEq << _endline
                << _condBodyStart << _endline;

        _indentationLevel++;
    }

    virtual void printElseIf(OperationNode<Base>& node) {
        /**
         * the first argument is the condition, the second argument is the 
         * if start node, the following arguments are assignments in the
         * previous if branch
         */
        CPPADCG_ASSERT_KNOWN(node.getOperationType() == CGOpCode::ElseIf, "Invalid node type");
        CPPADCG_ASSERT_KNOWN(node.getArguments().size() >= 2, "Invalid number of arguments for an 'else if' operation");
        CPPADCG_ASSERT_KNOWN(node.getArguments()[0].getOperation() != nullptr, "Invalid argument for an 'else if' operation");
        CPPADCG_ASSERT_KNOWN(node.getArguments()[1].getOperation() != nullptr, "Invalid argument for an 'else if' operation");

        _indentationLevel--;

        // close previous environment
        _code << _condBodyEnd << _endline;
        CGOpCode nType = node.getArguments()[0].getOperation()->getOperationType();
        if (nType == CGOpCode::StartIf) {
            _code << _ifEnd << _endline;
        } else if (nType == CGOpCode::ElseIf) {
            _code << _elseIfEnd << _endline;
        }

        // start new else if
        _code << _elseIfStart << _startEq << "<mi>else if</mi>"
                "<mfenced><mrow>";
        printIndexCondExprOp(*node.getArguments()[1].getOperation());
        _code << "</mrow></mfenced>" << _endEq << _endline
                << _condBodyStart << _endline;

        _indentationLevel++;
    }

    virtual void printElse(OperationNode<Base>& node) {
        /**
         * the first argument is the  if start node, the following arguments
         * are assignments in the previous if branch
         */
        CPPADCG_ASSERT_KNOWN(node.getOperationType() == CGOpCode::Else, "Invalid node type");
        CPPADCG_ASSERT_KNOWN(node.getArguments().size() >= 1, "Invalid number of arguments for an 'else' operation");

        _indentationLevel--;

        // close previous environment
        _code << _condBodyEnd << _endline;
        CGOpCode nType = node.getArguments()[0].getOperation()->getOperationType();
        if (nType == CGOpCode::StartIf) {
            _code << _ifEnd << _endline;
        } else if (nType == CGOpCode::ElseIf) {
            _code << _elseIfEnd << _endline;
        }

        // start else
        _code << _elseStart << _startEq << "<mi>else</mi>" << _endEq << _endline
                << _condBodyStart << _endline;
        _code << _elseStart;

        _indentationLevel++;
    }

    virtual void printEndIf(OperationNode<Base>& node) {
        CPPADCG_ASSERT_KNOWN(node.getOperationType() == CGOpCode::EndIf, "Invalid node type for an 'end if' operation");

        _indentationLevel--;

        // close previous environment
        _code << _condBodyEnd << _endline;
        CGOpCode nType = node.getArguments()[0].getOperation()->getOperationType();
        if (nType == CGOpCode::StartIf) {
            _code << _ifEnd << _endline;
        } else if (nType == CGOpCode::ElseIf) {
            _code << _elseIfEnd << _endline;
        } else {
            assert(nType == CGOpCode::Else);
            _code << _elseEnd << _endline;
        }
    }

    virtual void printCondResult(OperationNode<Base>& node) {
        CPPADCG_ASSERT_KNOWN(node.getOperationType() == CGOpCode::CondResult, "Invalid node type");
        CPPADCG_ASSERT_KNOWN(node.getArguments().size() == 2, "Invalid number of arguments for an assignment inside an if/else operation");
        CPPADCG_ASSERT_KNOWN(node.getArguments()[0].getOperation() != nullptr, "Invalid argument for an an assignment inside an if/else operation");
        CPPADCG_ASSERT_KNOWN(node.getArguments()[1].getOperation() != nullptr, "Invalid argument for an an assignment inside an if/else operation");

        // just follow the argument
        OperationNode<Base>& nodeArg = *node.getArguments()[1].getOperation();
        printAssigment(nodeArg);
    }

    inline bool isDependent(const OperationNode<Base>& arg) const {
        if (arg.getOperationType() == CGOpCode::LoopIndexedDep) {
            return true;
        }
        size_t id = getVariableID(arg);
        return id > _independentSize && id < _minTemporaryVarID;
    }

    virtual void printParameter(const Base& value) {
        // make sure all digits of floating point values are printed
        std::ostringstream os;
        os << std::setprecision(_parameterPrecision) << value;

        std::string number = os.str();
        size_t pos = number.find('e');
        if (pos != std::string::npos) {
            _code << "<mn>" << number.substr(0, pos) << "</mn><mo>&times;</mo>";
            _code << "<msup><mn>10</mn><mn>";
            pos++;
            if (number[pos] == '-') {
                _code << "-";
                pos++;
            } else if (number[pos] == '+') {
                pos++;
            }
            while (pos < number.size() - 1 && number[pos] == '0')
                pos++; // remove zeros

            _code << number.substr(pos) << "</mn></msup>";

        } else {
            _code << "<mn>" << number << "</mn>";
        }


    }

    virtual void getComparison(std::ostream& os, enum CGOpCode op) const {
        switch (op) {
            case CGOpCode::ComLt:
                os << "&lt;";
                return;

            case CGOpCode::ComLe:
                os << "&le;";
                return;

            case CGOpCode::ComEq:
                os << "==";
                return;

            case CGOpCode::ComGe:
                os << "&ge;";
                return;

            case CGOpCode::ComGt:
                os << "&gt;";
                return;

            case CGOpCode::ComNe:
                os << "&ne;";
                return;

            default:
                CPPAD_ASSERT_UNKNOWN(0);
        }
        throw CGException("Invalid comparison operator code"); // should never get here
    }

    static bool isFunction(enum CGOpCode op) {
        return isUnaryFunction(op) || op == CGOpCode::Pow;
    }

    static bool isUnaryFunction(enum CGOpCode op) {
        switch (op) {
            case CGOpCode::Abs:
            case CGOpCode::Acos:
            case CGOpCode::Asin:
            case CGOpCode::Atan:
            case CGOpCode::Cosh:
            case CGOpCode::Cos:
            case CGOpCode::Exp:
            case CGOpCode::Log:
            case CGOpCode::Sign:
            case CGOpCode::Sinh:
            case CGOpCode::Sin:
            case CGOpCode::Sqrt:
            case CGOpCode::Tanh:
            case CGOpCode::Tan:
                return true;
            default:
                return false;
        }
    }

    static bool isCondAssign(enum CGOpCode op) {
        switch (op) {
            case CGOpCode::ComLt:
            case CGOpCode::ComLe:
            case CGOpCode::ComEq:
            case CGOpCode::ComGe:
            case CGOpCode::ComGt:
            case CGOpCode::ComNe:
                return true;
            default:
                return false;
        }
    }
};

template<class Base>
const std::string LanguageMathML<Base>::_C_STATIC_INDEX_ARRAY = "index";

template<class Base>
const std::string LanguageMathML<Base>::_C_SPARSE_INDEX_ARRAY = "idx";

template<class Base>
const std::string LanguageMathML<Base>::_ATOMIC_TX = "atx";

template<class Base>
const std::string LanguageMathML<Base>::_ATOMIC_TY = "aty";

template<class Base>
const std::string LanguageMathML<Base>::_ATOMIC_PX = "apx";

template<class Base>
const std::string LanguageMathML<Base>::_ATOMIC_PY = "apy";

} // END cg namespace
} // END CppAD namespace

#endif