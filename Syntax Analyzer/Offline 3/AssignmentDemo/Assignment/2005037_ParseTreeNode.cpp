#include<string>
#include<vector>
#include<limits.h>

using namespace std;

class ParseTreeNode{

    public: 

    string name ;  // name is for non-terminals
    bool isLeaf ;
    int startLine,endLine ;
    string token ;  //token,lexeme for terminals 
    string lexeme ;

    vector<ParseTreeNode*> children ;
    string nameList ;

    ParseTreeNode(const string &n){
        name = n ;
        isLeaf = false ;
        startLine = INT_MAX ;
        endLine = INT_MIN ;
        nameList= "" ;
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

        startLine = min(startLine, child->startLine);
        endLine = max(endLine, child->endLine);
        string childName ;

        if(child->isLeaf){
            childName = child->token ;
        }else{
            childName = child->name ;
        }
        nameList += " "+childName ;
        //concatening children's name

        children.push_back(child);
    }


};