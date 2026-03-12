#include<bits/stdc++.h>
using namespace std;
int main()
{
    string s;
    cin>>s;
    if((s.substr(0,7)=="http://" || s.substr(0,8)=="https://") && s.find('.')!=string::npos)
    {
        bool valid=true;
        for(int i=0;i<s.size();i++)
        {
            if(!(isalnum(s[i]) || s[i]=='-' || s[i]=='.' || (i<7 && s.substr(0,7)=="http://") || (i<8 && s.substr(0,8)=="https://")))
            {
                valid=false;
                break;
            }
        }
        if(valid)
            cout<<"valid";
        else
            cout<<"invalid";
    }
    else
        cout<<"invalid";
    return 0;
}
