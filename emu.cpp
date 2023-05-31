//Name - Nishant Kumar
//Indian Institute of Technology,Patna
//RollNo.-2101CS52
//Project of CS209 : Computer Architecture
//Course Instructor: Dr Jimson Mathew
// Declaration of Authorship
// This txt file, claims.txt, is part of the assignment of CS321 - CS322 at the 
// department of Computer Science and Engg, IIT Patna . 
// *****************************************************************************/

#include<bits/stdc++.h>
using namespace std;

int A,B,pc,sp,cont,exe;
vector<string> instructions;
int v[2];
map<int,string> mnemonic={{0,"ldc"},{1,"adc"},{2,"ldl"},{3,"stl"},{4,"ldnl"},{5,"stnl"},
{6,"add"},{7,"sub"},{8,"shl"},{9,"shr"},{10,"adj"},{11,"a2sp"},{12,"sp2a"},{13,"call"},
{14,"return"},{15,"brz"},{16,"brlz"},{17,"br"},{18,"halt"},{1000,"data"},{100,"set"}};
ofstream fout("output.txt");

const int N=1<<24;
int memory[N];

string DecToHex(unsigned int n){

    stringstream ss;

    ss<<hex<<n;

    string outs=ss.str();
    while((int)outs.size()!=8){
        outs='0'+outs;
    }
    return outs;
}

void ldc(int value);
void adc(int value);
void ldl(int offset);
void stl(int offset);
void ldnl(int offset);
void stnl(int offset);
void add(int num);
void sub(int num);
void shl(int num);
void shr(int num);
void adj(int value);
void a2sp(int num);
void sp2a(int num);
void call(int offset);
void ret(int offset);
void brz(int offset);
void brlz(int offset);
void br(int offset);


void (*call_func[])(int) = {ldc, adc, ldl, stl, ldnl, stnl, add, sub, shl, shr, adj, a2sp, sp2a, call, ret, brz, brlz, br}; 

void memdisplay(){
    int i=0;
    while(i<(int)instructions.size()){

        cout<<DecToHex(i)<<" ";
        int j=i;

        while(j<min(i+4,(int)instructions.size())){
            cout<<DecToHex(memory[j])<<" ";
            j++;
        }
        cout<<endl;
        i+=4;
    }
}

bool instr(int operation,int num=(1<<25)){
    int it=0;

    //number of instructions we are going to perform now  is stored in num
    //e.g. it is 1 for trace

    while(num-- && pc<instructions.size()){
        //number of operation executed is stores in cont
        cont++;
        //cant run more than 3e7 operations
        
        if(cont>(int)3e7){
            cout<<"Segmentation fault"<<endl;
            return false;
        }
        else if(pc>=instructions.size()){
            cout<<"Segmentation fault"<<endl;
            return false;
        }
            string dta=mnemonic[pc];
            int op_f=stoi(instructions[pc].substr(6,2),0,16);
            fout<<"Instruction: "<<" "<<instructions[pc]<<" "<<mnemonic[op_f]<<endl;;
            fout<<"A: "<<DecToHex(A)<<endl;
            fout<<"B: "<<DecToHex(B)<<endl;
            fout<<"SP: "<<DecToHex(sp)<<endl;
            fout<<"PC: "<<DecToHex(pc)<<endl;
        
        //getting op code
        int op=stoi(instructions[pc].substr(6,2),0,16);

        int counter=cont;

        if(op==18){
            cout<<"HALT found"<<endl;
            cout<<cont<<" number of statements were executed"<<endl;

            //returning when halt
            return true;
        }

        
        //machine code data without opcodes
        int val=stoi(instructions[pc].substr(0,6),0,16);

        if(val>=(1<<23)){
            val-=(1<<24);
        }

        //if instruction are of type -trace
        if(num==0){

            //display the instructions
            cout<<mnemonic[op]<<endl;
            cout<<"A: "<<DecToHex(A)<<endl;
            cout<<"B: "<<DecToHex(B)<<endl;
            cout<<"SP: "<<DecToHex(sp)<<endl;
            cout<<"PC: "<<DecToHex(pc)<<endl;

            
        }

        int extra=num;
        //calling the function
        (call_func[op])(val);
        
        //if the operations are of the type stl,stnl
        if(operation==2 && exe==2){
            cout<<"Writing memory["<<DecToHex(pc)<<"], from "<<DecToHex(v[0])<<" to "<<DecToHex(v[1])<<endl;
            exe=0;
        }

        //if the operations are of type ldl,ldnl
        else if(operation==1 && exe==1){
            cout<<"Reading memory["<<DecToHex(pc)<<"], has value: "<<DecToHex(v[0])<<endl;
            exe=0;
        }
        pc++;
    }
    return true;

}


void showisa()
{
	cout<<"Opcode Mnemonic Operand"<<'\n';
	cout<<"0      ldc      value "<<"\n";
	cout<<"1      adc      value "<<"\n";
	cout<<"2      ldl      value "<<"\n";
	cout<<"3      stl      value "<<"\n";
	cout<<"4      ldnl     value "<<"\n";
	cout<<"5      stnl     value "<<"\n";
	cout<<"6      add            "<<"\n";
	cout<<"7      sub            "<<"\n";
	cout<<"9      shr            "<<"\n";
	cout<<"10     adj      value "<<"\n";
	cout<<"11     a2sp           "<<"\n";
	cout<<"12     sp2a           "<<"\n";
	cout<<"13     call     offset"<<"\n";
	cout<<"14     return         "<<"\n";
	cout<<"15     brz      offset"<<"\n";
	cout<<"16     brlz     offset"<<"\n";
	cout<<"17     br       offset"<<"\n";
	cout<<"18     HALT           "<<"\n";
	cout<<"       SET      value "<<"\n";
}

void help(){
    cout<<"You can perform the following instructions using this emulator"<<endl;
    cout<<"To get help -help"<<endl;
    cout<<"Trace: Emulate one line at a time using -trace"<<endl;
    cout<<"run: Emulate till the end of the code using -run"<<endl;
    cout<<"Show register values using -reg"<<endl;
    cout<<"Show memory dump using -dump"<<endl;
    cout<<"Read operation using -read"<<endl;
    cout<<"Write operation using -write"<<endl;
    cout<<"Show isa using -isa"<<endl;
    
}
int main(){

    

    //taking input machine file
    string s;
    cout<<"Enter your .o file"<<endl;

    cin>>s;
    ifstream file(s,ios::binary|ios::in);

    //defining instructions
    help();

    unsigned int x;
    int it=0;
    while(file.read((char*)&x,sizeof(int))){
        memory[it]=x;
        it++;
        instructions.push_back(DecToHex(x));
    }
    

    while(true){

        cout<<"Enter the command to run or press 0 to exit"<<endl;

        string a;
        cin>>a;

        string ano = a;

        if(a=="-help"){
            help();
        }
        else if(a=="0"){
            exit(0);
        }
        else if(a=="-read"){
            //to read
            bool k=instr(1);
            if(k==0){
                exit(0);
            }
        }
        else if(a=="-write"){
            //to write
            bool k=instr(2);
            if(k==0){
                exit(0);
            }
        }
        else if(a=="-isa"){
            showisa();
        }
        else if(a=="-trace"){
            bool k=instr(0,1);
            if(k==0){
                exit(0);
            }
        }
        else if(a=="-run"){
            bool k=instr(0);
            if(k==0){
                exit(0);
            }
        }
        else if(a=="-dump"){
            memdisplay();
        }
        else if(a=="-reg"){
            cout<<"A: "<<DecToHex(A)<<endl;
            cout<<"B: "<<DecToHex(B)<<endl;
            cout<<"SP: "<<DecToHex(sp)<<endl;
            cout<<"PC: "<<DecToHex(pc)<<endl;
        }
        else{
            cout<<"Incorrect command"<<endl;
        }
    }
    fout.close();
}


void ldc(int value){
    B=A;
    A=value;
}

void adc(int value){
    A+=value;
}
void ldl(int offset){
    B=A;
    if(sp+offset>=N||sp+offset<0){
        cout<<"Segmentation fault:out of range memory access"<<endl;
        exit(0);
    }
    A=memory[sp+offset];
    v[0]=sp+offset;
    v[1]=0;
    exe=1;
}

void stl(int offset){
    if(sp+offset>=N||sp+offset<0){
        cout<<"Segmentation fault:out of range memory access"<<endl;
        exit(0);
    }
    v[0]=memory[sp+offset];
    v[1]=A;
    exe=2;
    memory[sp+offset]=A;
    A=B;
}

void ldnl(int offset){
    if(sp+offset>=N||sp+offset<0){
        cout<<"Segmentation fault:out of range memory access"<<endl;
        exit(0);
    }
    A=memory[A+offset];
    v[0]=sp+offset;
    v[1]=0;
    exe=1;
}

void stnl(int offset){
    if(sp+offset>=N||sp+offset<0){
        cout<<"Segmentation fault:out of range memory access"<<endl;
        exit(0);
    }
    v[0]=memory[A+offset];
    v[1]=B;
    exe=2;
    memory[A+offset]=B;
}
void add(int num){
    A=A+B;
}
void sub(int num){
    A=B-A;
}
void shl(int num){
    A=B<<A;
}
void shr(int num){
    A=B>>A;
}
void adj(int value){
    sp=sp+value;
}
void a2sp(int num){
    sp=A;
    A=B;
}
void sp2a(int num){
    B=A;
    A=sp;
}
void call(int offset){
    B=A;
    A=pc;
    pc+=offset;
}
void ret(int offset){
    pc=A;
    A=B;
}
void brz(int offset){
    if(A==0){
        pc=pc+offset;
    }
}
void brlz(int offset){
    if(A<0){
        pc=pc+offset;
    }
}
void br(int offset){
    pc=pc+offset;
}
