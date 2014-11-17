#include <iostream>
#include <algorithm>
#include <map>
#include <vector>
#include "hds.h"
using namespace std;



int main()
{
     bool fRet = IntegerAnalyzer::IsEvenOrOdd(52);
     fRet = IntegerAnalyzer::IsDivisibleBy2To9(52);

     fRet = IntegerAnalyzer::IsEvenOrOdd(52);
     fRet = IntegerAnalyzer::IsEvenOrOdd(51);
     fRet = IntegerAnalyzer::IsEvenOrOdd(-52);
     fRet = IntegerAnalyzer::IsEvenOrOdd(-51);
     fRet = IntegerAnalyzer::IsEvenOrOdd(0);
     fRet = IntegerAnalyzer::IsEvenOrOdd(0000000);

     fRet = IntegerAnalyzer::IsDivisibleBy2To9(51);
     fRet = IntegerAnalyzer::IsDivisibleBy2To9(8);
     fRet = IntegerAnalyzer::IsDivisibleBy2To9(3);
     fRet = IntegerAnalyzer::IsDivisibleBy2To9(-1);

     fRet = IntegerAnalyzer::IsDivisibleBy2To9(INT_MIN);
     fRet = IntegerAnalyzer::IsDivisibleBy2To9(INT_MAX);

     eEvaluationType et = IntegerAnalyzer::GetEvaluationType(52);

     et = IntegerAnalyzer::GetEvaluationType(-52);
     et = IntegerAnalyzer::GetEvaluationType(51);
     et = IntegerAnalyzer::GetEvaluationType(52);
     et = IntegerAnalyzer::GetEvaluationType(48);


    return 0;
}