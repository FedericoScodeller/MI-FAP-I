#include <iostream>
#include <vector>
using namespace std;

vector<vector<int>> MakeMatrix(vector<vector<string>> a);

struct FreqCost
{
    int hard_link_broken=0;
    double interference=0.0;
};


int main()
{
   FreqCost a,b;
    b.interference=64.9;
    a=b;
   cout << a.hard_link_broken << " & " << a.interference << endl;
    
    return EXIT_SUCCESS;
}

vector<vector<int>> MakeMatrix(vector<vector<string>> a)
{
    vector<vector<int>> v;
    v.push_back({1,2,4});
    v.push_back({64});
    for (int i = 0; i < a.size(); i++)
    {
        for (size_t j = 0; j < a.size(); j++)
        {
            cout << a[i][j];
        }
        
        cout << endl;
    }
    return v;
}

/*    vector<vector<int>> a;
    vector<int> v(3,0);

    a.push_back(v);
    a.push_back(v);
    a.push_back(v);
    
    for (int i = 0; i < a.size(); i++)
    {
        for (size_t j = i+1; j < a.size(); j++)
        {
            a[j][i]=1;
        }
    }

   for (int i = 0; i < a.size(); i++)
    {
        for (size_t j = i+1; j < a.size(); j++)
        {
            a[i][j]=2;
        }
    }
    for (int i = 0; i < a.size(); i++)
    {
        for (size_t j = i+1; j < a.size(); j++)
        {
            a[j][i]=a[i][j]=a[j][i]+a[i][j];
        }
    }
    
    for (int i = 0; i < a.size(); i++)
    {
        for (size_t j = 0; j < a.size(); j++)
        {
            cout << a[i][j];
        }
        
        cout << endl;
    }*/