#ifndef XMLPARSER_H
#define XMLPAESER_H
#include<iostream>
#include<string>
#include<vector>
#include<map>
#include<fstream>
#include<stack>
#include<memory>
#include<algorithm>
#include<iterator>
#include<sstream>
using namespace std;
#define ERROR {cout<<"格式错误"<<endl;exit(0);}
#define CHECK(it) {if(it==end)ERROR;}

typedef map<string,string>& attrsRef;
typedef string::iterator stritera;
typedef string::const_iterator cstritera;

class XMLParse;
class XMLTree;
class XMLNode;
class XMLReader;
class PreProcess;
class XMLBuilder;
class XML;
typedef shared_ptr<XMLTree> TreePtr;
typedef shared_ptr<XMLNode> NodePtr;
typedef TreePtr Tree;
typedef NodePtr Node;
typedef shared_ptr<XML> XMLPtr;
typedef vector<XMLPtr> ChildrenRef;
class PreProcess{
    public:
        void delblank(cstritera& sta);
        void rdelblank(cstritera& end);
        cstritera getPos(cstritera& sta,cstritera end,char c);
        cstritera getPos_(cstritera sta,cstritera end,char c);
        void getTagAttr(cstritera& sta,cstritera end,string& tem);
        string process(const string& str);
        void processBeginTag(string& ret,cstritera& sta,cstritera end);
        void processEndTag(string& ret,cstritera& sta,cstritera end);
        void processTag(string& ret,cstritera& sta,cstritera end);
        void processText(string& ret,cstritera& sta,cstritera end);
};
class XML{
    friend class XMLNode;
    friend class XMLTree;
    public:
        XML(ostream& out):out(out){}
        virtual void writeMid(const string& blank,ostream&)=0;
        virtual void writeBegin(const string& blank,ostream&)=0;
        virtual void writeEnd(const string& blank,ostream&)=0;
        void write(const string& blank,ostream& out);
        void write();
        string getTagName();
        void setTagName(const string& name);
        attrsRef getAttr();
        string getAttrbyName(const string& name);
        bool removeAttrbyName(const string& name);
        XML& addAttr(const string& attr,const string& val);
        void copy(shared_ptr<XML> xmlptr);
        virtual void setText(const string& str){}
        virtual XML& addChild(shared_ptr<XML>){return *this;}
        virtual bool isTree(){return false;}
        virtual XMLPtr getChildbyTagName(const string& tagName){return XMLPtr();}
        virtual ChildrenRef getChildren(){return ChildrenRef();}
        virtual bool removeChildbyName(const string& name){return false;}
        virtual string getText(){return "";}
        string toString();
    private:
        string tagName;
    protected:
        map<string,string> attributes;
        ostream& out=cout;
};
class XMLNode:public XML{
    public:
        XMLNode(ostream& out):XML(out){}
        void writeMid(const string& blank,ostream&);
        void writeBegin(const string& blank,ostream&);
        void writeEnd(const string& blank,ostream&);
        void setText(const string& text);
        string getText(){return text;}
    private:
        string text;
};
class XMLTree:public XML{
    public:
        XMLTree(ostream& out):XML(out){}
        void writeBegin(const string& blank,ostream&);
        void writeMid(const string& blank,ostream&);
        using XML::writeEnd;
        void writeEnd(const string& blank,ostream&);
        void parseStr(const string& str);
        void parseFile(const string& fileName);
        XMLTree& addChild(shared_ptr<XML> xmlptr);
        XMLTree& addChild(const vector<XMLPtr>& childs);
        XMLPtr getChildbyTagName(const string& tagName);
        bool removeChildbyName(const string& name);
        bool isTree(){return true;}
        ChildrenRef getChildren();
        ~XMLTree(){
        }
    private:
        vector<shared_ptr<XML>> children;
};
class XMLParse{
    public:
        void delblank(cstritera& sta);
        cstritera getPos(cstritera& sta,cstritera end,char c);
        cstritera getPos_(cstritera sta,cstritera end,char c);
        shared_ptr<XML> parse(const string& xmlstr,ostream& out);
        void parseTreeTag(const string& tagStr,ostream& out);
        void parseTextTag(const string& tagStr,ostream& out);
        void parseTail();
        string getTag(cstritera& sta,cstritera end);
        string getText(cstritera& sta,cstritera end);
        void getTagAttr(const string& s,XML& xml);
        void parseSingleTag(string& tagStr,ostream& out);
        void parseText(cstritera& sta,const string& xmlstr);
        void popStk();
    private:
        stack<shared_ptr<XML>> stk;
        shared_ptr<XML> root;
        bool rootFlag=true;
};
class XMLReader:public XMLTree{
    public:
        XMLReader(ostream& out=cout):XMLTree(out){}
};
class XMLBuilder{
    public:
        XMLBuilder(ostream& out=cout):out(out){}
        TreePtr createTree();
        TreePtr createTree(const string& tagName);
        TreePtr createTree(const string& tagName,vector<vector<string>>& attrs);
        NodePtr createNode();
        NodePtr createNode(const string& tagName);
        NodePtr createNode(const string& tagName,vector<vector<string>>& attrs);
    private:
        ostream& out;
};

void PreProcess::delblank(cstritera& sta){
    while(*sta==' '||*sta=='\n'){
        sta++;
    }
}
void PreProcess::rdelblank(cstritera& end){
    while(*end==' '||*end=='\n'){
        end--;
    }
}
cstritera PreProcess::getPos(cstritera& sta,cstritera end,char c){
    auto it=find(sta,end,c);
    sta=it+1;
    return it;
}
cstritera PreProcess::getPos_(cstritera sta,cstritera end,char c){
    auto it=find(sta,end,c);
    return it;
}
void PreProcess::getTagAttr(cstritera& sta,cstritera end,string& tem){
    while(true){
        delblank(sta);
        auto it=getPos_(sta,end,' ');
        if(string(sta,it)=="/"){
            tem+="/";
            sta=it;
            continue;
        }else if(it==end){
            if(string(sta,it)!=""){
                tem+=" "+string(sta,it);
            }
            sta=++it;
            break;
        }
        string str(sta,it);
        tem+=" "+str;
        sta=++it;
    } 
}
void PreProcess::processBeginTag(string& ret,cstritera& sta,cstritera end){
    auto tem1=getPos_(sta,end,'>');
    auto tem2=getPos_(sta,tem1,' ');
    ret+=string(sta,tem2);
    sta=tem2;
    delblank(sta);
    if(sta==tem1){
        sta++;
    }else{
        getTagAttr(sta,tem1,ret);
    }
    ret+=">";
}
void PreProcess::processEndTag(string& ret,cstritera& sta,cstritera end){
    sta++;
    delblank(sta);
    auto tem1=getPos_(sta,end,'>');
    auto tem2=getPos_(sta,tem1,' ');
    ret+="/"+string(sta,tem2)+">";
    sta=tem1+1;
}
void PreProcess::processText(string& ret,cstritera& sta,cstritera end){
    auto tem=sta;
    auto tem2=getPos_(sta,end,'<');
    sta=tem2;
    sta--;
    rdelblank(sta);
    sta++;
    ret+=string(tem,sta);
    sta=tem2;
}
void PreProcess::processTag(string& ret,cstritera& sta,cstritera end){
    sta++;
    delblank(sta);
    ret+="<";
    if(*(sta)!='/'){
        processBeginTag(ret,sta,end);
    }else{
        processEndTag(ret,sta,end);
    }
}
string PreProcess::process(const string& str){
    string ret;
    auto sta=str.begin();
    delblank(sta);
    auto end=str.end()-1;
    rdelblank(end);
    end++;
    while(sta!=end){
        delblank(sta);
        if(*sta=='<'){
            processTag(ret,sta,end);
        }else{
            processText(ret,sta,end);
        }
    }
    //cout<<ret<<endl;
    return ret;
}

bool XML::removeAttrbyName(const string& name){
    auto it=attributes.find(name);
    if(it!=attributes.end()){
        attributes.erase(it);
        return true;
    }
    return false;
}
string XML::getAttrbyName(const string& name){
    auto it=attributes.find(name);
    if(it!=attributes.end()){
        return it->second;
    }
    return "";
}
void XML::writeEnd(const string& blank,ostream& out){
    out<<"</"+tagName+">"<<endl;
}
void XML::write(const string& blank,ostream& out){
    writeBegin(blank,out);
    writeMid(blank,out);
    writeEnd(blank,out);
}
void XML::write(){
    write("",out);
};
string XML::toString(){
    stringstream ss;
    ss.unsetf(ios::skipws);
    write("",ss);
    return string(istream_iterator<char>(ss),istream_iterator<char>());
}
string XML::getTagName(){
    return tagName;
};
void XML::setTagName(const string& name){
    tagName=name;
}
attrsRef XML::getAttr(){
    return attributes;
}
XML& XML::addAttr(const string& attr,const string& val){
    attributes.insert(make_pair(attr,val));
    return *this;
}
void XML::copy(shared_ptr<XML> xmlptr){
    tagName=xmlptr->tagName;
    attributes=xmlptr->attributes;
}

void XMLNode::writeMid(const string& blank,ostream& out){
    out<<text;
}
void XMLNode::writeBegin(const string& blank,ostream& out){
    out<<blank;
    out<<"<"+getTagName();
    if(!attributes.empty()){
        for(auto& it:attributes){
            out<<" ";
            out<<it.first+"="+"\""+it.second+"\"";
        }
    }
    out<<">";
}
void XMLNode::writeEnd(const string& blank,ostream& out){
    out<<"</"+tagName+">"<<endl;
}
void XMLNode::setText(const string& text){
    this->text=text;
}

bool XMLTree::removeChildbyName(const string& name){
    for(auto it=children.begin();it!=children.end();it++){
        if((*it)->tagName==name){
            children.erase(it);
            return true;
        }
    }
    return false;
}
XMLPtr XMLTree::getChildbyTagName(const string& tagName){
    for(auto it:children){
        if(it->tagName==tagName){
            return it;
        }
    }
    return shared_ptr<XML>();
}
XMLTree& XMLTree::addChild(shared_ptr<XML> xmlptr){
    children.push_back(xmlptr);
    return *this;
}
XMLTree& XMLTree::addChild(const vector<XMLPtr>& childs){
    for(auto it:childs){
        children.push_back(it);
    }
    return *this;
}
ChildrenRef XMLTree::getChildren(){
    return children;
}
void XMLTree::writeBegin(const string& blank,ostream& out){
    out<<blank;
    out<<"<"+tagName;
    if(!attributes.empty()){
        for(auto& it:attributes){
            out<<" ";
            out<<it.first+"="+"\""+it.second+"\"";
        }
    }
    out<<">"<<endl;
}
void XMLTree::writeMid(const string& blank,ostream& out){
    for(auto it:children){
        it->write(blank+"   ",out);
    }
}
void XMLTree::writeEnd(const string& blank,ostream& out){
    out<<blank;
    out<<"</"+tagName+">"<<endl;
}
void XMLTree::parseStr(const string& str){
    PreProcess pre;
    auto xmlparse=new XMLParse();
    auto ptr=xmlparse->parse(pre.process(str),out);
    attributes=ptr->attributes;
    tagName=ptr->tagName;
    if(ptr->isTree()){
        children=dynamic_pointer_cast<XMLTree>(ptr)->children;
    }
    delete(xmlparse);
}
void XMLTree::parseFile(const string& fileName){
    ifstream ifs(fileName);
    if(ifs.fail()){
        ERROR;
    }
    string str;
    ifs.unsetf(ios::skipws);
    ::copy(istream_iterator<char>(ifs),istream_iterator<char>(),back_insert_iterator<string>(str));
    parseStr(str);
}

void XMLParse::delblank(cstritera& sta){
    while(*sta==' '||*sta=='\n'){
        sta++;
    }
}
cstritera XMLParse::getPos(cstritera& sta,cstritera end,char c){
    auto it=find(sta,end,c);
    sta=it+1;
    return it;
}
cstritera XMLParse::getPos_(cstritera sta,cstritera end,char c){
    auto it=find(sta,end,c);
    return it;
}
void XMLParse::parseTreeTag(const string& tagStr,ostream& out){
    shared_ptr<XMLTree> xmlptr(new XMLTree(out));
    getTagAttr(tagStr,*xmlptr);
    if(!stk.empty()){
        if(typeid(*stk.top())!=typeid(XMLTree)){
            ERROR;
        }
        stk.top()->addChild(xmlptr);
    }
    stk.push(xmlptr);
}
void XMLParse::parseTextTag(const string& tagStr,ostream& out){
    shared_ptr<XMLNode> xmlnode(new XMLNode(out));
    getTagAttr(tagStr,*xmlnode);
    if(!stk.empty()){
        if(typeid(*stk.top())!=typeid(XMLTree)){
            ERROR;
        }
        stk.top()->addChild(xmlnode);
    }
    stk.push(xmlnode);
}
void XMLParse::popStk(){
    if(stk.size()==1){
        root=stk.top();
    }
    stk.pop();
}
void XMLParse::parseSingleTag(string& tagStr,ostream& out){
    tagStr.erase(tagStr.end()-2,tagStr.end());
    tagStr+=">";
    parseTextTag(tagStr,out);
    popStk();
}
void XMLParse::parseText(cstritera& sta,const string& xmlstr){
    if(stk.empty()){
        ERROR;
    }
    auto top=stk.top();
    string text=getText(sta,xmlstr.end());
    top->setText(text);
}
shared_ptr<XML> XMLParse::parse(const string& xmlstr,ostream& out){
    auto sta=xmlstr.begin();
    auto end=xmlstr.end();
    while(sta!=xmlstr.end()){
        if(*sta=='<'){
            if(*(sta+1)!='/'){
                string tagStr=getTag(sta,end);
                CHECK(sta);
                auto it=sta+1;
                if(*(tagStr.end()-2)=='/'){
                    parseSingleTag(tagStr,out);
                }else{
                    if(*sta=='<'&&*it!='/'){
                        parseTreeTag(tagStr,out);
                    }else{
                        parseTextTag(tagStr,out);
                    }
                }
                
            }else{
                popStk();
                getTag(sta,end);
            }                    
        }else{
            parseText(sta,xmlstr);
        }
    }
    return root;
}
string XMLParse::getTag(cstritera& sta,cstritera end){
    auto gap1=getPos(sta,end,'<');
    CHECK(gap1);
    auto gap2=getPos(sta,end,'>');
    CHECK(gap2);
    return string(gap1,gap2+1);
}
string XMLParse::getText(cstritera& sta,cstritera end){
    delblank(sta);
    auto sta_=sta;
    auto end_=getPos(sta,end,'<');
    sta--;
    end_--;
    while(*end_==' '||*end_=='\n'){
        end_--;
    }
    end_++;
    return string(sta_,end_);
}
void XMLParse::getTagAttr(const string& s,XML& xml){
    auto sta=s.begin()+1;
    auto end=s.end();
    auto it=find(sta,end,' ');
    if(it==end){
        xml.setTagName(string(sta,end-1));
        return;
    }
    xml.setTagName(string(sta,it));
    sta=++it;
    if(it==end){
        return;
    }else{
        while(true){
            auto it=getPos_(sta,end,'=');
            if(it==end){
                break;
            }
            string attr(sta,it);
            sta=++it;
            it=getPos_(sta,end,'"');
            if(it==end){
                ERROR;
            }
            sta=++it;
            it=getPos_(sta,end,'"');
            if(it==end){
                ERROR;
            }
            string val(sta,it);
            sta=it+2;
            xml.addAttr(attr,val);
        }
    } 
}

TreePtr XMLBuilder::createTree(){
    return TreePtr(new XMLTree(out));
}
TreePtr XMLBuilder::createTree(const string& tagName){
    TreePtr ret(new XMLTree(out));
    ret->setTagName(tagName);
    return ret;
}
TreePtr XMLBuilder::createTree(const string& tagName,vector<vector<string>>& attrs){
    TreePtr ret=createTree(tagName);
    for(auto& it:attrs){
        ret->addAttr(it[0],it[1]);
    }
    return ret;
}
NodePtr XMLBuilder::createNode(){
    return NodePtr(new XMLNode(out));
}
NodePtr XMLBuilder::createNode(const string& tagName){
    NodePtr ret(new XMLNode(out));
    ret->setTagName(tagName);
    return ret;
}
NodePtr XMLBuilder::createNode(const string& tagName,vector<vector<string>>& attrs){
    NodePtr ret=createNode(tagName);
    for(auto& it:attrs){
        ret->addAttr(it[0],it[1]);
    }
    return ret;
}
#endif