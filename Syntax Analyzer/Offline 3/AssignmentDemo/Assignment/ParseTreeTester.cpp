#include <iostream>
#include "2005037_ParseTreeNode.cpp"
using namespace std;

void printParseTree(ParseTreeNode *root, int space)
{

    for (int i = 0; i < space; i++)
    {
        cout << " ";
    }

    if (root->isLeaf)
    {
        cout << root->token << " : " << root->lexeme << "\t<Line : " << root->startLine << ">\n";
        return ;
    }

    cout<<root->name+" :"+root->nameList+"\t<Line : "<<root->startLine <<"-"<<root->endLine<< ">\n";

    for(ParseTreeNode* node:root->children){
        printParseTree(node,space+1);
    }


}

int main()
{

    ParseTreeNode *x = new ParseTreeNode("ID", "x", 2);
    ParseTreeNode *decl_list = new ParseTreeNode("decl_list");
    decl_list->addChild(x);
    ParseTreeNode *comma = new ParseTreeNode("COMMA", ",", 2);
    ParseTreeNode *y = new ParseTreeNode("ID", "y", 3);
    ParseTreeNode *decl_list2 = new ParseTreeNode("decl_list");

    decl_list2->addChild(decl_list);
    decl_list2->addChild(comma);
    decl_list2->addChild(y);

    ParseTreeNode *comma2 = new ParseTreeNode("COMMA", ",", 3);
    ParseTreeNode *z = new ParseTreeNode("ID", "z", 4);

    ParseTreeNode *decl_list3 = new ParseTreeNode("decl_list");

    decl_list3->addChild(decl_list2);
    decl_list3->addChild(comma2);
    decl_list3->addChild(z);

    ParseTreeNode *int_keyword = new ParseTreeNode("INT", "int", 1);
    ParseTreeNode *type_spec = new ParseTreeNode("type_spec");
    type_spec->addChild(int_keyword);

    ParseTreeNode *semicolon = new ParseTreeNode("semicolon", ";", 4);

    ParseTreeNode *var_decl = new ParseTreeNode("var_decl");
    var_decl->addChild(type_spec);
    var_decl->addChild(decl_list3);
    var_decl->addChild(semicolon);

    ParseTreeNode *unit = new ParseTreeNode("unit");
    unit->addChild(var_decl);

    printParseTree(unit,0);
}