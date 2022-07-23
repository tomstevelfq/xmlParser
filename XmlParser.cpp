#include<iostream>
#include<string>
#include<vector>
#include<map>
#include<fstream>
#include<stack>
#include<memory>
#include<algorithm>
#include<iterator>
using namespace std;
#define ERROR {cout<<"格式错误"<<endl;exit(0);}
#define CHECK(it) {if(it==end)ERROR;}

typedef map<string,string>& attrRef;
typedef string::iterator stritera;
typedef string::const_iterator cstritera;

class XMLParse;
class XMLTree;
class PreProcess;
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
    public:
        XML(ostream& out):out(out){}
        virtual void writeMid()=0;
        virtual void writeMid(const string& blank)=0;
        virtual void writeBegin();
        void writeBegin(const string& blank);
        virtual void writeEnd(const string& blank);
        void write(const string& blank);
        void writeEnd();
        void write();
        virtual void showMid()=0;
        string getTagName();
        void setTagName(const string& name);
        void showBegin();
        void showEnd();
        void show();
        attrRef getAttr();
        void addAttr(const string& attr,const string& val);
        void copy(shared_ptr<XML> xmlptr);
        virtual void setText(const string& str){}
        virtual void addChild(shared_ptr<XML>){}
    private:
        string tagName;
    protected:
        map<string,string> attributes;
        ostream& out;
};
class XMLNode:public XML{
    public:
        XMLNode(ostream& out):XML(out){}
        void writeMid();
        void writeMid(const string& blank);
        void writeBegin();
        void showMid();
        void setText(const string& text);
    private:
        string text;
};
class XMLTree:public XML{
    public:
        XMLTree(ostream& out):XML(out){}
        void writeMid();
        void writeMid(const string& blank);
        using XML::writeEnd;
        void writeEnd(const string& blank);
        void parseStr(const string& str);
        void parseFile(const string& fileName);
        void showMid();
        void addChild(shared_ptr<XML> xmlptr);
        void showStr();
        void writeFile();
        ~XMLTree(){
        }
        vector<shared_ptr<XML>> children;
    private:
        XMLParse* xmlparse;
        shared_ptr<XML> pdata;
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

void XML::writeBegin(){
    out<<"<"+tagName;
    if(!attributes.empty()){
        for(auto& it:attributes){
            out<<" ";
            out<<it.first+"="+"\""+it.second+"\"";
        }
    }
    out<<">"<<endl;
}
void XML::writeBegin(const string& blank){
    out<<blank;
    writeBegin();
}
void XML::writeEnd(const string& blank){
    writeEnd();
}
void XML::write(const string& blank){
    writeBegin(blank);
    writeMid(blank);
    writeEnd(blank);
}
void XML::writeEnd(){
    out<<"</"+tagName+">"<<endl;
}
void XML::write(){
    write("");
};
string XML::getTagName(){
    return tagName;
};
void XML::setTagName(const string& name){
    tagName=name;
};
void XML::showBegin(){
    cout<<tagName+":begin"<<endl;
    if(!attributes.empty()){
        cout<<"Attributes:";
        for(auto it:attributes){
            cout<<it.first+": "+it.second<<"   ";
        }
        cout<<endl;
    }
}
void XML::showEnd(){
    cout<<tagName+":end"<<endl;
}
void XML::show(){
    showBegin();
    showMid();
    showEnd();
}
attrRef XML::getAttr(){
    return attributes;
}
void XML::addAttr(const string& attr,const string& val){
    attributes.insert(make_pair(attr,val));
}
void XML::copy(shared_ptr<XML> xmlptr){
    tagName=xmlptr->tagName;
    attributes=xmlptr->attributes;
}

void XMLNode::writeMid(){
    out<<text;
}
void XMLNode::writeMid(const string& blank){
    out<<text;
}
void XMLNode::writeBegin(){
    out<<"<"+getTagName();
    if(!attributes.empty()){
        for(auto& it:attributes){
            out<<" ";
            out<<it.first+"="+"\""+it.second+"\"";
        }
    }
    out<<">";
}
void XMLNode::showMid(){
    cout<<text<<endl;
}
void XMLNode::setText(const string& text){
    this->text=text;
}

void XMLTree::writeMid(){
    for(auto it:children){
        it->write();
    }
}
void XMLTree::writeMid(const string& blank){
    for(auto it:children){
        it->write(blank+"   ");
    }
}
void XMLTree::writeEnd(const string& blank){
    out<<blank;
    writeEnd();
}
void XMLTree::parseStr(const string& str){
    PreProcess pre;
    xmlparse=new XMLParse();
    pdata=xmlparse->parse(pre.process(str),out);
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
void XMLTree::showMid(){
    for(auto it:children){
        it->show();
    }
}
void XMLTree::addChild(shared_ptr<XML> xmlptr){
    children.push_back(xmlptr);
}
void XMLTree::showStr(){
    pdata->show();
}
void XMLTree::writeFile(){
    pdata->write();
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

int main(){
    ofstream ofs("C:\\Users\\tomst\\Desktop\\xmlParser\\helo1.xml");
    XMLTree xml(ofs);
    xml.parseFile("C:\\Users\\tomst\\Desktop\\xmlParser\\hello1.xml");
    xml.writeFile();
    return 0;
}