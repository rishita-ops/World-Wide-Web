#include <bits/stdc++.h>
using namespace std;

int main() 
{
    string s;
    cout<<"Enter the URL : ";
    cin >> s;

    string prefix;
    int start;

    if (s.substr(0, 7) == "http://") 
    {
        prefix = "http://";
        start = 7;
    } 
    else if (s.substr(0, 8) == "https://") 
    {
        prefix = "https://";
        start = 8;
    } 
    else 
    {
        cout << "invalid";
        return 0;
    }

    string rest = s.substr(start); 

    if (rest.find('.') == string::npos || rest.empty()) 
    {
        cout << "invalid";
        return 0;
    }

    bool valid = true;
    for (char c : rest) 
    {
        if (!(isalnum(c) || c == '-' || c == '.' || c == '/' ||
              c == '_' || c == '?' || c == '=' || c == '&' ||
              c == '#' || c == '%' || c == '+' || c == ':' || c == '@')) {
            valid = false;
            break;
        }
    }

    cout << (valid ? "valid" : "invalid");
    return 0;
}
