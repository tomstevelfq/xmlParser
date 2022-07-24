# xmlParser
xmlParser

//ParseXMLFile
    XMLReader reader1;
    reader1.parseFile("C:\\Users\\tomst\\Desktop\\xmlParser\\in.xml");  //read from a file
    cout<<reader1.toString()<<endl;   //converts to string.

//ParseXMLStr
    XMLReader reader1;
    string str="<mapper namespace=\"com.mdc\"><resultMap id=\"BaseMap\" type=\"Sch\"><result/><result column=\"Code\" property=\"Code\" /></resultMap></mapper>";
    reader1.parseStr(str);  //read from Str
    cout<<reader1.toString()<<endl;   //converts to string.
    reader1.write()  //defaut written by cout

//Write to File
    ofstream ofs("C:\\Users\\tomst\\Desktop\\xmlParser\\out2.xml");
    XMLReader reader2(ofs);  //config ofstream by constructor
    reader2.parseFile("C:\\Users\\tomst\\Desktop\\xmlParser\\in2.xml"); //parse in2.xml
    reader2.write();  //write to out2.xml

//Create some data
//ofstream ofs("out.xml");
//XMLBuilder builder(ofs);  <--!ostream must be configed when initialize a XMLBuilder object-->
    XMLBuilder builder;
    Tree t1=builder.createTree("Student");
    Tree t2=builder.createTree("Class");
    Tree t3=builder.createTree("Student");
    Tree t4=builder.createTree("Student");
    t2->addAttr("name","class 4").addAttr("teacher","liu").addAttr("number","48");
    t2->addAttr("ID","435325");
    Node n1=builder.createNode("name");
    n1->setText("cclt");
    Node n2=builder.createNode("age");
    n2->setText("34");
    Node n3=builder.createNode("Grade");
    n3->setText("78");
    t1->addChild({n1,n2}).addChild(n2);
    t1->addChild(n3);
    Node n5=builder.createNode("name");
    n5->setText("tom");
    Node n6=builder.createNode("age");
    n6->setText("47");
    t4->addChild(n5);
    t4->addChild(n6);
    Node n4=builder.createNode("age");
    n4->setText("72");
    t3->addChild(n4).addChild(t4);
    t2->addChild(t1);
    t2->addChild(t3);
    cout<<t2->toString()<<endl;
//The result is like the text below
    <Class ID="435325" name="class 4" number="48" teacher="liu">
       <Student>
          <name>cclt</name>
          <age>34</age>
          <age>34</age>
          <Grade>78</Grade>
       </Student>
       <Student>
          <age>72</age>
          <Student>
             <name>tom</name>
             <age>47</age>
          </Student>
       </Student>
    </Class>


//get tagName or attribute of element
    XMLBuilder builder;
    Tree t1=builder.createTree("school");
    Tree t2=builder.createTree("student");
    Node n1=builder.createNode("name");
    Node n2=builder.createNode("age");
    t1->addAttr("mianji","23").addAttr("name","high school").addAttr("people","455");
    t2->addAttr("class","4").addAttr("age","45");
    n1->setText("testtext");
    n2->setText("45");
    t2->addChild(n1).addChild(n2);
    t1->addChild(t2);
    string attr=t1->getAttrbyName("name");
    cout<<attr<<endl;
    string attr2=t2->getAttrbyName("age");
    cout<<attr2<<endl;
    XMLPtr c1=t2->getChildbyTagName("age");
    cout<<c1->getTagName()<<endl;
    cout<<c1->isTree()<<endl;  //is or not a tree
    if(!c1->isTree()){  //if c1 is not a tree
        cout<<c1->getText()<<endl;
        c1->setText("this is a new text");
        cout<<c1->getText()<<endl;
    }
    auto attrs=t1->getAttr();
    for(auto it:attrs){
        cout<<it.first<<"  "<<it.second<<endl;
    }
    auto childs=t2->getChildren();
    for(auto it:childs){
        cout<<it->getTagName()<<endl;
    }
    divid();
    t1->write();  //print t1
    divid();


//remove attribute or child of element
    t->removeChildbyName(name);
    t->removeAttrbyName(name);
