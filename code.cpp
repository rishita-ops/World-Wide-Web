/*
World Wide Web 🌐
March 12, 2026
·
0
0

It's the World Wide Web's birthday! On March 12, 1989, British scientist Tim Berners-Lee submitted a proposal titled “Information Management: A Proposal” while working at CERN.

The proposal described a network of documents that could link to each other. His manager famously described it as 
“vague but exciting”, LOL. That idea became WWW!

Tim Berners-Lee

One part of the proposal is about URLs. URL stands for Uniform Resource Locator. It is the unique address used to identify and 
locate a webpage, image, or document on the internet. Frequently referred to as a web address".

Given a string representing a web address, determine if it's a valid URL or not.

For our validator, make sure it follows these rules:
It starts with http:// or https://
The domain must contain at least one dot .
The domain only contains letters, digits, hyphens -, or dots .
Return the string "valid" if it is, or "invalid" if it isn't.

Examples

Example 1

Input: https://codedex.io
Output: valid
Example 2

Input: https://netflixcom
Output: invalid
Example 3

Input: http://en.wikipedia.org
Output: valid
*/

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