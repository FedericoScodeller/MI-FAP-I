#include "Cost.hh"
#include <ios>
#include <iostream>
#include <ostream>

using namespace std;

int main()
{
   Cost c1;
   Cost c2(3);
   Cost c3(2,1.1);
   Cost c4(2,1.0);

   cout <<"c1: " << c1 << endl;
   cout <<"c2: " << c2 << endl;
   cout <<"c3: " << c3 << endl;
   cout <<"c4: " << c4 << endl;

   cout << boolalpha;
   cout << "c1 < c2 : " << (c1 < c2) << endl;
   cout << "c3 < c2 : " << (c3 < c2) << endl;
   cout << "c3 < c4 : " << (c3 < c4) << endl;
   cout << "c1 == c3 : " << (c1 == c3) << endl;
   cout << "c3 == c4 : " << (c3 == c4) << endl;

   c1=c3+=c2=c4;

   cout <<"c1: " << c1 << endl;
   cout <<"c2: " << c2 << endl;
   cout <<"c3: " << c3 << endl;
   cout <<"c4: " << c4 << endl;

   cout << boolalpha;
   cout << "c1 < c2 : " << (c1 < c2) << endl;
   cout << "c3 < c1 : " << (c3 < c1) << endl;
   cout << "c4 < c3 : " << (c4 < c3) << endl;
   cout << "c1 == c3 : " << (c1 == c3) << endl;
   cout << "c3 == c2 : " << (c3 == c2) << endl;
   cout << "c4 == c2 : " << (c4 == c2) << endl;

   return 0;
}
