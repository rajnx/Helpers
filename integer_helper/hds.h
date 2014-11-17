#pragma once 

/*++

Module Name:

hds.h

Abstract:

This contains a integer analyzer hash table, and the supporting structure and classe
needed to make it work.

You can use the analyzer by using the following syntax:

bool fRet = IntegerAnalyzer::IsEvenOrOdd(%some_magic_integer_number%)

* Public API:

- IsEvenOrOdd() Checks for the integer evenness or oddness. It will return true if passed
integer is even and vice-versa. 0, -0, 0000, -000 all returned as even.

- IsDivisibleBy2To9() returns an iterator, it may be empty.  Simple check its bool
value to determine if you are the end.  The hash table does not expose
an "End" property.

- GetEvaluationType() returns an evaluation type for particular integer in question.

Author:

Rajneesh (rajnx@hotmail.com) 8-Oct-2014

Revision History:

--*/

//
// Operation mapping types as returned by GetEvaluationType API.
//
enum eEvaluationType
{
    e_None              = 0x00000000,
    eEvenOrOdd          = 0x00000001,
    eDivisibleBy2To9    = 0x00000002,
    eEvaluatedForAll    = 0x00000003,
    eNotEvaluatedBefore = 0x00000004
};

struct AnalysisResults
/*++
    Abstract:

    Result object containing required information for further calls. This is cached unless Tree size runs out.

Author:

    Rajneesh 8-Oct-2014

--*/
{

    enum eOperationResult
    {
        eFalse = 0,    // Oh No!
        eTrue,         // Oh Yes!
        eNotEvaluated  // Any Oh not required - operation was never executed!
    };

    unsigned int fIsEvenOrOdd : 2;
    unsigned int fIsDivisibleBy2To9 : 2;
    int integer;

    AnalysisResults()
    {
        fIsEvenOrOdd = eNotEvaluated;
        fIsDivisibleBy2To9 = eNotEvaluated;
        integer = INT_MIN;
    }
};

class IntegerAnalysisTree
/*++

Abstract:

    This is the tree class which deals with creation of BST for evaluated integers and 
    retriving information using Big O(log(n)). Information currently contains all analysis
    results as declared in AnalysisResults structure.

Author:

    Rajneesh 8-Oct-2014

--*/
{
public:
    IntegerAnalysisTree() :m_root(NULL) {}
    virtual ~IntegerAnalysisTree() { Clear(); }

    //
    // Public interface for Actual Add data, refer below.
    //
    void AddData(AnalysisResults result)
    {
        AddData(result, &m_root);
    }

    //
    // Public interface for Actual Search, refer below..
    //
    AnalysisResults Search(int integer)
    {
        return Search(integer, m_root);
    }

    // Regular clear - Freeing all memory.
    void Clear()
    {
        Clear(m_root);
    }

private:
    struct TreeNode
    {
        AnalysisResults result;
        TreeNode *left;
        TreeNode *right;
    };

    TreeNode *m_root;

protected:
    TreeNode* CreateNode(AnalysisResults result)
    {
        TreeNode* node = new TreeNode();
        if (NULL != node)
        {
            node->result.fIsDivisibleBy2To9 = result.fIsDivisibleBy2To9;
            node->result.fIsEvenOrOdd = result.fIsEvenOrOdd;
            node->result.integer = result.integer;
            node->left = NULL;
            node->right = NULL;
        }

        return node;
    }

    //
    // Add result to the BST using usual algorithm. In case result key already exists, update
    // result with new information.
    //
    void AddData(AnalysisResults result, TreeNode **node)
    {
        if (NULL == *node)
        {
            *node = CreateNode(result);
            return;
        }

        if ((*node)->result.integer == result.integer)
        {
            (*node)->result = result;
            return;
        }

        if ((*node)->result.integer > result.integer)
            AddData(result, &(*node)->left);
        else
            AddData(result, &(*node)->right);
    }

    //
    // Using Binary search to retrieve analysis result of passed integer.
    //
    AnalysisResults Search(int integer, TreeNode *node)
    {
        if (!node)
            return AnalysisResults();

        if (node->result.integer == integer)
            return node->result;

        if (node->result.integer > integer)
            return Search(integer, node->left);
        else
            return Search(integer, node->right);
    }

    //
    // Deallocate all memory using inorder traversal.
    //
    void Clear(TreeNode *node)
    {
        if (node)
        {
            Clear(node->left);
            Clear(node->right);
            delete node;
            node = NULL;
        }
    }
};

class IntegerAnalyzer
/*++

Abstract:

    Main class for analyzing integers. Operation results are cached in Tree for subsequent access.
    Call for cache comes for the fact that Division (which in turn used by Modulo) is of order N*N.
    So for multiple calls it make sense to cache all operation results. This would tremedously improve
    performance in longer run.

Author:

    Rajneesh 8-Oct-2014

--*/
{
public:
    //
    // Using Modulo operator instead of using bitwise shift so as to be platform independent.
    //
    static bool IsEvenOrOdd(int integer)
    {
        AnalysisResults result = GetAnalysisResults(integer);

        if (AnalysisResults::eNotEvaluated == result.fIsEvenOrOdd)
        {
            result.integer = integer;
            result.fIsEvenOrOdd = (integer % 2) ? 0 : 1;

            if (result.integer != INT_MIN)
                m_EvaluationLog.AddData(result);
        }

        return result.fIsEvenOrOdd ? AnalysisResults::eTrue : AnalysisResults::eFalse;
    }

    //
    // Using Modulo operator instead of using bitwise shift so as to be platform independent.
    //
    static bool IsDivisibleBy2To9(int integer)
    {
        AnalysisResults result = GetAnalysisResults(integer);

        if (AnalysisResults::eNotEvaluated == result.fIsDivisibleBy2To9)
        {
            int i = 2;
            while (i <= 9)
            {
                if (integer%i == 0)
                    i++;
                else
                    break;
            }

            if (i<9)
                result.fIsDivisibleBy2To9 = AnalysisResults::eFalse;
            else
                result.fIsDivisibleBy2To9 = AnalysisResults::eTrue;

            if (result.integer != INT_MIN)
                m_EvaluationLog.AddData(result);
        }

        return result.fIsDivisibleBy2To9 ? AnalysisResults::eTrue : AnalysisResults::eFalse;
    }

    static eEvaluationType GetEvaluationType(int integer)
    {
        int evalType;
        AnalysisResults result = GetAnalysisResults(integer);

        if (result.integer != INT_MIN)
        {
            if (result.fIsEvenOrOdd != AnalysisResults::eNotEvaluated)
                evalType = eEvenOrOdd;

            if (result.fIsDivisibleBy2To9 != AnalysisResults::eNotEvaluated)
                evalType = evalType | eDivisibleBy2To9;
        }
        else
        {
            evalType = eNotEvaluatedBefore;
        }

        return (eEvaluationType)evalType;
    }

protected:
    IntegerAnalyzer();
    virtual ~IntegerAnalyzer()
    {
        m_EvaluationLog.Clear();
    }


    static inline AnalysisResults GetAnalysisResults(int integer)
    {
        return m_EvaluationLog.Search(integer);
    }

private:
    static IntegerAnalysisTree m_EvaluationLog;
};

IntegerAnalysisTree IntegerAnalyzer::m_EvaluationLog;