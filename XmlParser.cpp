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
class preProcess;
class preProcess{
    public:
        void delblank(cstritera& sta){
            while(*sta==' '||*sta=='\n'){
                sta++;
            }
        }
        cstritera getPos(cstritera& sta,cstritera end,char c){
            auto it=find(sta,end,c);
            sta=it+1;
            return it;
        }
        cstritera getPos_(cstritera sta,cstritera end,char c){
            auto it=find(sta,end,c);
            return it;
        }
        string process(const string& str){
            string ret;

        }
};
class XML{
    public:
        XML(ostream& out):out(out){}
        virtual void writeMid()=0;
        virtual void writeMid(const string& blank)=0;
        virtual void writeBegin(){
            out<<"<"+tagName;
            if(!attributes.empty()){
                for(auto& it:attributes){
                    out<<" ";
                    out<<it.first+"="+"\""+it.second+"\"";
                }
            }
            out<<">"<<endl;
        }
        void writeBegin(const string& blank){
            out<<blank;
            writeBegin();
        }
        virtual void writeEnd(const string& blank){
            writeEnd();
        }
        void write(const string& blank){
            writeBegin(blank);
            writeMid(blank);
            writeEnd(blank);
        }
        void writeEnd(){
            out<<"</"+tagName+">"<<endl;
        }
        void write(){
            write("");
        };
        virtual void showMid()=0;
        string getTagName(){
            return tagName;
        };
        void setTagName(const string& name){
            tagName=name;
        };
        void showBegin(){
            cout<<tagName+":begin"<<endl;
            if(!attributes.empty()){
                cout<<"Attributes:";
                for(auto it:attributes){
                    cout<<it.first+": "+it.second<<"   ";
                }
                cout<<endl;
            }
        }
        void showEnd(){
            cout<<tagName+":end"<<endl;
        }
        void show(){
            showBegin();
            showMid();
            showEnd();
        }
        attrRef getAttr(){
            return attributes;
        }
        void addAttr(const string& attr,const string& val){
            attributes.insert(make_pair(attr,val));
        }
        void copy(shared_ptr<XML> xmlptr){
            tagName=xmlptr->tagName;
            attributes=xmlptr->attributes;
        }
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
        void writeMid(){
            out<<text;
        }
        void writeMid(const string& blank){
            out<<text;
        }
        void writeBegin(){
            out<<"<"+getTagName();
            if(!attributes.empty()){
                for(auto& it:attributes){
                    out<<" ";
                    out<<it.first+"="+"\""+it.second+"\"";
                }
            }
            out<<">";
        }
        void showMid(){
            cout<<text<<endl;
        }
        void setText(const string& text){
            this->text=text;
        }
    private:
        string text;
};
class XMLTree:public XML{
    public:
        XMLTree(ostream& out):XML(out){}
        void writeMid(){
            for(auto it:children){
                it->write();
            }
        }
        void writeMid(const string& blank){
            for(auto it:children){
                it->write(blank+"   ");
            }
        }
        using XML::writeEnd;
        void writeEnd(const string& blank){
            out<<blank;
            writeEnd();
        }
        void parseStr(const string& str);
        void showMid(){
            for(auto it:children){
                it->show();
            }
        }
        void addChild(shared_ptr<XML> xmlptr){
            children.push_back(xmlptr);
        }
        void showStr(){
            pdata->show();
        }
        void writeFile(){
            pdata->write();
        }
        ~XMLTree(){
        }
    private:
        vector<shared_ptr<XML>> children;
        XMLParse* xmlparse;
        shared_ptr<XML> pdata;
};
class XMLParse{
    public:
        void delblank(cstritera& sta){
            while(*sta==' '||*sta=='\n'){
                sta++;
            }
        }
        cstritera getPos(cstritera& sta,cstritera end,char c){
            auto it=find(sta,end,c);
            sta=it+1;
            return it;
        }
        cstritera getPos_(cstritera sta,cstritera end,char c){
            auto it=find(sta,end,c);
            return it;
        }
        shared_ptr<XML> parse(const string& xmlstr,ostream& out){
            //cout<<xmlstr<<endl;
            auto sta=xmlstr.begin();
            auto end=xmlstr.end();
            while(sta!=xmlstr.end()){
                delblank(sta);
                CHECK(sta);
                //cout<<*sta<<endl;
                if(*sta=='<'){
                    delblank(sta);
                    if(*(sta+1)!='/'){
                        string tagStr=getTag(sta,end);
                        delblank(sta);
                        CHECK(sta);
                        //cout<<string(sta,xmlstr.end())<<endl;
                        if(*sta=='<'){
                            shared_ptr<XMLTree> xmlptr(new XMLTree(out));
                            getTagAttr(tagStr,*xmlptr);
                            if(!stk.empty()){
                                if(typeid(*stk.top())!=typeid(XMLTree)){
                                    ERROR;
                                }
                                stk.top()->addChild(xmlptr);
                            }
                            stk.push(xmlptr);
                        }else{
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
                    }else{
                        if(stk.size()==1){
                            root=stk.top();
                            stk.pop();
                            break;
                        }
                        getTag(sta,end);
                        CHECK(sta);
                        stk.pop();
                    }                    
                }else{
                    if(stk.empty()){
                        ERROR;
                    }
                    auto top=stk.top();
                    string text=getText(sta,xmlstr.end());
                    top->setText(text);
                }
            }
            return root;
        }
        string getTag(cstritera& sta,cstritera end){
            auto gap1=getPos(sta,end,'<');
            CHECK(gap1);
            auto gap2=getPos(sta,end,'>');
            CHECK(gap2);
            return string(gap1,gap2+1);
        }
        string getText(cstritera& sta,cstritera end){
            delblank(sta);
            auto sta_=sta;
            auto end_=getPos(sta,end,'<');
            cout<<*end_<<endl;
            sta--;
            end_--;
            cout<<*end_<<endl;
            cout<<(*end_==' '||*end_=='\n')<<endl;
            while(*end_==' '||*end_=='\n'){
                end_--;
            }
            end_++;
            cout<<"'"+string(sta_,end_)+"'"<<endl;
            return string(sta_,end_);
        }
        void getTagAttr(const string& s,XML& xml){
            auto sta=s.begin()+1;
            auto end=s.end();
            auto it=find(sta,end,' ');
            string tag;
            if(it==end){
                tag=string(sta,end-1);
                xml.setTagName(tag);
                return;
            }
            tag=string(sta,it);
            xml.setTagName(tag);
            sta=++it;
            if(it==end){
                return;
            }else{
                while(true){
                    delblank(sta);
                    auto it=getPos_(sta,end,'=');
                    if(it==end){
                        break;
                    }
                    string attr(sta,it);
                    sta=++it;
                    delblank(sta);
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
                    sta=++it;
                    xml.addAttr(attr,val);
                }
            } 
        }
    private:
        stack<shared_ptr<XML>> stk;
        shared_ptr<XML> root;
        bool rootFlag=true;
};
void XMLTree::parseStr(const string& str){
    xmlparse=new XMLParse();
    pdata=xmlparse->parse(str,out);
    delete(xmlparse);
}

int main(){
    ifstream ifs("hello1.xml");
    if(ifs.fail()){
        ERROR;
    }
    string str;
    ifs.unsetf(ios::skipws);
    copy(istream_iterator<char>(ifs),istream_iterator<char>(),back_insert_iterator<string>(str));
    //cout<<str<<endl;
    ofstream ofs("helo1.xml");
    XMLTree xml(ofs);
    xml.parseStr(str);
    //xml.showStr();
    xml.writeFile();
    return 0;
}