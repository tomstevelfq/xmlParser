#include<iostream>
#include<string>
#include<vector>
#include<stack>
#include<fstream>
#include<memory>
#include<sstream>
using namespace std;

class A{
    public:
        int a=0;
        A& operator=(const A& o){
            cout<<"ji"<<endl;
            a=o.a;
            return *this;
        }
        void fun(){
            A* aa=new A();
            aa->a=43535325;
            *this=*aa;
        }
};
class B:public A{
    public:
        B& operator=(const B& o){
            cout<<"pai"<<endl;
            a=o.a;
            return *this;
        }
        void fun(){
            cout<<"B"<<endl;
        }
};

int main(){
    if(shared_ptr<A>()){
        cout<<"bukong";
    }else{
        cout<<"kong"<<endl;
    }
    return 0;
}