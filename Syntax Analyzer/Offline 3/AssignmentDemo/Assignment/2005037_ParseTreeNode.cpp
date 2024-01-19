#include<string>
#include<vector>
#include<limits.h>
#include "2005037_SymbolTable.cpp"

using namespace std;

class ParseTreeNode{

    public: 

    string name ;  // name is for non-terminals
    bool isLeaf ;
    int startLine,endLine ;
    string token ;  //token,lexeme for terminals 
    string lexeme ;
    string dataType ; // INT, FLOAT OR VOID 

    vector<ParseTreeNode*> children ;
    string nameList ;
    string lastFoundLexeme ;
    string lastFoundToken ;

    ParseTreeNode(const string &n){
        name = n ;
        isLeaf = false ;
        startLine = INT_MAX ;
        endLine = INT_MIN ;
        nameList= "" ;
        dataType = "" ;
        //initially nameList is empty
    }

    ParseTreeNode(const string&l,const string&t,int sL){
        token = t;
        lexeme = l;
        startLine = sL;
        endLine = sL;
        isLeaf = true ;

        //nameList not applicable for leaf nodes
    }


    void addChild(ParseTreeNode* child){
        //cout<<"adding child "<<endl;
        startLine = min(startLine, child->startLine);
        endLine = max(endLine, child->endLine);
        string childName ;

        if(child->isLeaf){
            //cout<<"child is a leaf node"<<endl;
            //cout<<"lexeme "<<child->lexeme<<" token "<<child->token<<endl;

            childName = child->token ;
            lastFoundLexeme = child->lexeme ;
            lastFoundToken = child->token ;
        }else{
            childName = child->name ;
            //let's update my lastFoundLexeme
            lastFoundLexeme = child->lastFoundLexeme ;
            lastFoundToken = child->lastFoundToken;
        }
        nameList += " "+childName ;
        //concatening children's name
        //cout<<"Current nameList "<<nameList<<endl;
        children.push_back(child);
        //cout<<"children added successfully"<<endl;
    }


};