// Name - Nishant Kumar
// Indian Institute of Technology,Patna
// RollNo.-2101CS52
// Project of CS209 : Computer Architecture
// Course Instructor: Dr Jimson Mathew

// Declaration of Authorship
// This txt file, claims.txt, is part of the assignment of CS321 - CS322 at the 
// department of Computer Science and Engg, IIT Patna . 
// *****************************************************************************/

#include <bits/stdc++.h>
using namespace std;

struct table
{
    string label;
    string operand;
    string mnemonic;
    int opType;
    bool labelcheck;
};

vector<pair<int, string>> machineCode; // will store the machine code
vector<pair<int, string>> errors;      // will store all the errors
vector<string> cleaned;                // code in clean form without spaces
vector<table> data;                    // to store info in data form for each instruction
map<string, pair<string, int>> Opcode; // for storing OPcodes
map<string, int> labels;               // for storing labels and their declaration line
vector<int> PC;                        // for pc values
string fName;                          // for opening .asm file
bool haltpresent = false;              // to check if halt is there or not

bool isNum(char x);
bool isChar(char x);
bool isOctal(string s);
bool isHexaDecimal(string s);
bool isDecimal(string s);
bool validName(string s);
int OctalToDec(string s);
int HexToDec(string s);
void initialization();
int operandtype(string s);
void errors_generated(int curr, string type);
bool ErrorsPrint();
string clean(string s, int curr);
void labelinstruction();
void SETinstructions(vector<string> &temp, string curr, string s, int j);
void SETimplementation();
void segData();
void inserttable();

string zeroextend(string s, int len = 6)
{
    // to append zero and make it of 24 bits
    // for example A changes to 00000A
    while ((int)s.size() < len)
    {
        s = '0' + s;
    }
    return s;
}

string DecToHex(int n, int shamt = 24)
{
    stringstream ss;
    if (shamt == -1)
    {
        unsigned int num = n;
        ss << hex << num;
        // converts value in num to hexadecimal and stores in ss
        return ss.str();
    }
    else if (n < 0)
    {
        n += (1 << shamt);
    }
    ss << hex << n;
    return ss.str();
}

int main()
{

    // first PASS

    // taking input the asm file
    ifstream infile;
    cout << "Enter the filename to assemble: " << endl;
    cin >> fName;

    infile.open(fName);

    string zz = fName;

    if (infile.fail())
    {
        cout << "File doesnt exists!! please make sure file is in the same directory! " << endl;
        exit(0);
    }

    // reading the full and  cleaning it
    string s;
    int counter = 0;
    while (getline(infile, s))
    {
        // getting all files
        string curr = clean(s, (int)cleaned.size());
        cleaned.push_back(curr);
        counter++;
    }

    initialization();

    // checking for labels /dealing with SET/data instructions
    labelinstruction();

    // changing size of data and PC
    // SET implementation
    if (errors.empty() == true)
    {
        SETimplementation();
    }

    PC.resize((int)cleaned.size());
    data.resize((int)cleaned.size());

    segData();
    inserttable();

    // First pass ends

    // if errorsprint is true, i.e., there are no errors
    if (ErrorsPrint() == true)
    {

        // SECOND PASS starts
        int i = 0;
        while (i < (int)data.size())
        {

            // cout << "cleaned[i] " << cleaned[i] << endl;
            //  if instruction is empty
            if (cleaned[i].empty())
            {
                i++;
                continue;
            }

            // if there is no mnemonic
            if (data[i].mnemonic == "")
            {
                string currMC = "        ";
                machineCode.push_back({i, currMC});
                i++;
                continue;
            }
            // for label
            if (data[i].opType == 1)
            {
                int n = 0;
                // offset is there (instruction is of type 2)
                if (Opcode[data[i].mnemonic].second == 2)
                {
                    int val_pc = PC[i];
                    // we get label number and subtract it from instruction number
                    int num = labels[data[i].operand];
                    n = num - (val_pc + 1);
                }
                else
                {

                    int val_pc = PC[i];
                    // else we simply put label number in it
                    n = labels[data[i].operand];
                }
                // for currMC , we zero extend the decForm and then add the op code of mnemonic
                string currMC = zeroextend(DecToHex(n)) + Opcode[data[i].mnemonic].first;
                machineCode.push_back({i, currMC});
            }
            else if (data[i].opType == 0)
            {

                // if op code is 0 (for like add etc) , we simply put add its op code with 000000
                string currMC = "000000" + Opcode[data[i].mnemonic].first;
                machineCode.push_back({i, currMC});
            }
            else
            {
                // in other cases
                // if its data , then we cant change it,else if its negative then we need to convert it to 2s complement

                int len, shamt;
                if (data[i].mnemonic == "data")
                {
                    len = 8;
                    shamt = -1;
                }
                else
                {
                    len = 6;
                    shamt = 24;
                }
                // changing the operand value to int
                int n = stoi(data[i].operand, 0, data[i].opType);
                int val_pc = PC[i];
                // now zero extending it and adding the opcode
                string currMC = zeroextend(DecToHex(n, shamt), len) + Opcode[data[i].mnemonic].first;
                machineCode.push_back({i, currMC});
            }
            i++;
        }
        // SECOND PASS ENDS

        // Writing to file
        ofstream outListCode("listCode.l");

        for (int it = 0; it < machineCode.size(); it++)
        {
            // writing the machine code in listCode.l
            outListCode << zeroextend(DecToHex(PC[machineCode[it].first])) << " " << machineCode[it].second << " " << cleaned[machineCode[it].first] << endl;
        }
        // closing the file
        outListCode.close();

        // To write object file
        ofstream outMachineCode;
        outMachineCode.open("machineCode.o", ios::binary | ios::out);

        for (int it = 0; it < machineCode.size(); it++)
        {
            unsigned int x;
            stringstream ss;
            // if there was no instruction
            if (machineCode[it].second.empty() || machineCode[it].second == "        ")
            {
                continue;
            }

            ss << hex << machineCode[it].second;
            ss >> x;
            static_cast<int>(x);
            outMachineCode.write((const char *)&x, sizeof(unsigned int));
        }

        // closing the file
        outMachineCode.close();

        cout << "LogCode is generated in: logFile.log" << endl;
        cout << "MachineCode is generated in: machineCode.o" << endl;
        cout << "ListingCode is generated in: listCode.l" << endl;
    }
    return 0;
}

bool isNum(char x)
{
    return (x >= '0' && x <= '9');
}

bool isChar(char x)
{
    return ((x >= 'A' && x <= 'Z') || (x >= 'a' && x <= 'z'));
}
bool isOctal(string s)
{
    if ((int)s.size() <= 1)
    {
        return false;
    }
    int i = 1;
    while (i < (int)s.size())
    {
        if ((s[i] >= '0') && (s[i] <= '7'))
        {
            i++;
        }
        else
        {
            return false;
        }
    }
    if (s[0] == '0')
    {
        return true;
    }
    return false;
}
bool isHexaDecimal(string s)
{
    bool ok = true;
    if ((int)s.size() <= 2)
    {
        return false;
    }
    if (s[0] != '0')
    {
        return false;
    }
    if (!(s[1] == 'x' || s[1] == 'X'))
    {
        return false;
    }
    int i = 2;
    while (i < (int)s.size())
    {
        bool now = (isNum(s[i]));
        now |= ((s[i] >= 'a' && s[i] <= 'f') || (s[i] >= 'A' && s[i] <= 'F'));
        ok &= now;
        i++;
    }
    return ok;
}
bool isDecimal(string s)
{

    int i = 0;
    while (i < (int)s.size())
    {
        if (isNum(s[i]))
        {
            i++;
        }
        else
        {
            return false;
        }
    }
    return true;
}
bool validName(string s)
{

    // 1xyz is false
    //_yz1,xyz1 is true
    int i = 1;
    while (i < (int)s.size())
    {
        if (isChar(s[i]) || isNum(s[i]))
        {
            i++;
        }
        else if (s[i] == '_')
        {
            i++;
        }
        else
        {
            return false;
        }
    }
    if (isChar(s[0]) || (s[0] == '_'))
    {
        return true;
    }
    return false;
}

int OctalToDec(string s)
{
    int res = 0;
    int x = 1;

    int i = s.size() - 1;
    while (i > 0)
    {
        res += (s[i] - '0') * x;
        x *= 8;
        i--;
    }
    return res;
}

int HexToDec(string s)
{
    int res = 0;
    int x = 1;

    int i = s.size() - 1;
    while (i > 1)
    {
        if (isNum(s[i]))
        {
            res += (s[i] - '0') * x;
        }
        else
        {
            res += ((s[i] - 'a') + 10) * x;
        }
        x *= 16;
        i++;
    }
    return res;
}

void initialization()
{

    // 0 means no operand

    Opcode["sub"] = {"07", 0};
    Opcode["shl"] = {"08", 0};
    Opcode["add"] = {"06", 0};
    Opcode["shr"] = {"09", 0};
    Opcode["a2sp"] = {"0B", 0};
    Opcode["sp2a"] = {"0C", 0};
    Opcode["return"] = {"0E", 0};
    Opcode["HALT"] = {"12", 0};

    // 1 means one argument is given
    Opcode["data"] = {"", 1};
    Opcode["ldc"] = {"00", 1};
    Opcode["adc"] = {"01", 1};
    Opcode["adj"] = {"0A", 1};
    Opcode["SET"] = {"", 1};

    // 2 means offset is to be given
    Opcode["ldl"] = {"02", 2};
    Opcode["stl"] = {"03", 2};
    Opcode["ldnl"] = {"04", 2};
    Opcode["stnl"] = {"05", 2};
    Opcode["call"] = {"0D", 2};
    Opcode["brz"] = {"0F", 2};
    Opcode["brlz"] = {"10", 2};
    Opcode["br"] = {"11", 2};
}

int operandtype(string s)
{
    // returns whether operand if label/hex/value/decimal/octal

    if (s.empty())
    {
        return 0;
    }
    if (s[0] == '+' || s[0] == '-')
    {
        // removing the sign from the string
        reverse(s.begin(), s.end());
        s.pop_back();
        reverse(s.begin(), s.end());
    }
    if (s.empty() == true)
    {
        return -1;
    }
    else if (isHexaDecimal(s) == true)
    {
        return 16;
    }
    else if (isDecimal(s) == true)
    {
        return 10;
    }
    else if (isOctal(s) == true)
    {
        return 8;
    }
    else if (validName(s) == true)
    {
        return 1;
    }
    return -1;
}

void errors_generated(int curr, string type)
{
    errors.push_back({curr + 1, "Error ar line: " + to_string(curr) + " of type: " + type});
}

bool ErrorsPrint()
{

    // creates a file named logFile.log
    ofstream plog("logFile.log");
    plog << "Check logFile.log" << endl;

    // it there is no error , we return true
    if (errors.empty() == true)
    {

        cout << "No errors found" << endl;
        plog << "Machine code generated in MachineCode.o" << endl;
        plog << "Listing code generated in listCode.l" << endl;

        // checking for HALT
        if (haltpresent == false)
        {
            plog << "Warning!! HALT not present!" << endl;
        }
        // closing the file
        plog.close();

        return true;
    }
    else
    {

        // sorting the file
        sort(errors.begin(), errors.end());

        // printing the number of errors
        cout << (int)errors.size() << " number of errors generated! CHECK logFile.log" << endl;

        int i = 0;

        // printing the errors and returning false
        while (i < (int)errors.size())
        {
            plog << errors[i].second << endl;
            i++;
        }
        plog.close();
    }
    return false;
}
string clean(string s, int curr)
{

    int ct = 0;
    // this while loop removes the space before beginning and after ending
    while (ct < 2)
    {
        reverse(s.begin(), s.end());
        while (s.back() == ' ' || s.back() == '\t')
        {
            s.pop_back();
        }
        ct++;
    }
    string now = "";
    int i = 0;
    while (i < (int)s.size())
    {
        if (s[i] == ';')
        {
            // if comment then directly break
            break;
        }
        else if (s[i] == ':')
        {
            // if lable colon is there
            now += ':';
            // adding a space after colon
            if (i == (int)s.size() - 1 || s[i + 1] != ' ')
            {
                now += " ";
            }
            i++;
            continue;
        }
        // if its a normal letter
        else if ((s[i] != ' ') && (s[i] != '\t'))
        {
            now += s[i];
            i++;
            continue;
        }
        now += " ";
        int j = i;
        // till there is space in between texts then, we are increasing the iterator
        while (s[i] == s[j] && j < (int)s.size())
        {
            j++;
        }
        i = j;
    }

    // removing extra spaces towards the back of text
    int num = 0;
    for (int i = now.size() - 1; i >= 0; i--)
    {
        if (now[i] == ' ' || now[i] == '\t')
        {
            num++;
        }
        else
        {
            break;
        }
    }
    while (num)
    {
        now.pop_back();
        num--;
    }
    int spaces = 0;
    // counting spaces now
    int j = 0;
    while (j < (int)now.size())
    {
        if (now[j] == ' ')
        {
            spaces++;
        }
        j++;
    }
    // if more than 2 spaces are there
    // eg... here max number of spaces is there var1: data 0
    if (spaces >= 3)
    {
        errors_generated(curr + 1, "Syntax Error");
    }
    return now;
}

void labelinstruction()
{
    // stores labels with the position they are declared
    int i = 0;
    while (i < (int)cleaned.size())
    {
        // cout<<"current instruction is "<<cleaned[i]<<endl;
        string temp = "";
        int j = 0;
        while (j < (int)cleaned[i].size())
        {
            if (cleaned[i][j] == ':')
            {
                // if label found
                if (!validName(temp))
                {
                    errors_generated(i + 1, "Invalid label name");
                    break;
                }
                // if label is already there
                if (labels.count(temp))
                {

                    // form is label: data and that label is already SET to a specified value.then we simply update it
                    if (cleaned[i].size() > j + 5 && cleaned[i].substr(j + 2, 4) == "data" && labels[temp] < 0)
                    {
                        labels[temp] = i;
                        j++;
                        continue;
                    }
                    // form is label: SET
                    else if (cleaned[i].size() > j + 4 && cleaned[i].substr(j + 2, 3) == "SET")
                    {
                        j++;
                        continue;
                    }
                    // if there is something other than set and data, then error
                    errors_generated(i + 1, "Multiple declaration of label found: " + temp);
                }
                if (cleaned[i].size() > j + 4 && cleaned[i].substr(j + 2, 3) == "SET")
                {
                    // giving set instructions label as -i
                    labels[temp] = -i;
                    j++;
                    continue;
                }
                // if its not SET or data instruction
                //-i in label[] for SET,else i
                j++;
                labels[temp] = i;
                break;
            }
            temp += cleaned[i][j];
            j++;
            // cout<<"curr is "<<curr<<endl;
        }
        i++;
    }
}

// in temp we are storing all instructions
// s is the current instruction
// j currently points to ':' in
void SETinstructions(vector<string> &temp, string curr, string s, int j)
{

    // this means SET instruction is not there
    if (s.size() <= j + 5)
    {
        return;
    }

    // since SET is pseudo instruction, we do it various other methods
    temp.push_back("adj 10000");                                  // stack pointer at any random memory (sp=sp+10000)
    temp.push_back("stl -1");                                     // here to store A        memory[sp-1]=A ,A=B
    temp.push_back("stl 0");                                      // here to store B        memory[sp]=A ,A=B
    temp.push_back("ldc " + s.substr(j + 6, s.size() - (j + 6))); // this translates to ldc value
    // above means B=A , A=value
    temp.push_back("ldc " + curr.substr(0, j)); // curr.substr(0,j) gives the label name
    // B=A , A=label
    temp.push_back("stnl 0");     // memory[A+0]=B
    temp.push_back("ldl 0");      // B=A, A=memory[sp+0]
    temp.push_back("ldl -1");     // B=A , A=memory[sp-1];
    temp.push_back("adj -10000"); // Adjust stack ptr
}

void SETimplementation()
{

    vector<string> temp;
    int i = 0;
    while (i < (int)cleaned.size())
    {

        string curr = "";
        // int zz = PC[i];
        bool chk_set = false;
        int j = 0;
        while (j < (int)cleaned[i].size())
        {

            curr += cleaned[i][j];

            if (cleaned[i][j] == ':')
            {

                curr.pop_back();
                // checking if its a SET instruction

                if (cleaned[i].size() > j + 5 && cleaned[i].substr(j + 2, 3) == "SET")
                {
                    chk_set = true;
                    // as label[curr] was assigned value -i
                    if (abs(labels[curr]) != i)
                    {
                        // if SET instruction is repeated with same label
                        // first time it is set to -i, but we need to change
                        SETinstructions(temp, curr, cleaned[i], j);
                    }
                    else
                    {
                        // temp will contain values like label: data value
                        labels[curr] = (int)temp.size() - 1;
                        int gg = labels[curr];
                        // for reserving a memory location to the current label
                        // basically SET using data
                        temp.push_back(cleaned[i].substr(0, j + 1) + " data " + cleaned[i].substr(j + 6, (int)cleaned[i].size() - (j + 6)));
                    }
                    break;
                }
            }
            j++;
        }
        // if there was no SET instruction and in the currrent instruction, then pushing it in temp for further actions
        if (!cleaned[i].empty() && !chk_set)
        {
            temp.push_back(cleaned[i]);
        }
        i++;
    }
    // after processing everything , we make cleaned=temp
    // temp was made just to deal with SET instructions
    cleaned = temp;
}

void segData()
{
    // separates the code in DATA segment
    vector<string> instr, dataseg;
    int i = 0;
    while (i < (int)cleaned.size())
    {
        bool chk_data = false;
        int j = 0;
        while (j < (int)cleaned[i].size())
        {
            // if there is data , then push it in dataseg
            if (cleaned[i].substr(j, 4) == "data" && (j + 4 < cleaned[i].size()))
            {
                chk_data = true;
                dataseg.push_back(cleaned[i]);
                break;
            }
            // if data like
            // label:
            // data value
            else if (cleaned[i].back() == ':' && i + 1 < (int)cleaned.size() && cleaned[i + 1].substr(0, 4) == "data")
            {
                chk_data = true;
                dataseg.push_back(cleaned[i]);
                break;
            }
            j++;
        }
        // if not data, then push in instr
        if (chk_data == false)
        {
            instr.push_back(cleaned[i]);
        }
        i++;
    }
    // basically we insert dataseg at the end and make cleaned=instr
    //(we just rearranged)
    instr.insert(instr.end(), dataseg.begin(), dataseg.end());
    cleaned = instr;
}

void inserttable()
{
    // ----------------------------------------------------------
    //  Label | Mnemonic | Operand | Operand Type (Hex, Oct etc)
    // ----------------------------------------------------------

    int pc = 0;
    // we go to each cleaned statement
    for (int i = 0; i < (int)cleaned.size(); i++)
    {

        // current instruction
        string temp = "";

        // to fill data we create an array
        string c_data[10] = {"", "", "", ""};

        int it = 1;
        int j = 0;
        while (j < (int)cleaned[i].size())
        {

            //"we have got 1 segment of data (mneumonic)"
            if (cleaned[i][j] == ' ')
            {
                c_data[it] = temp;
                temp = "";
                it++;
                j++;
                continue;
            }
            // if colon(curr should be label) put it in 1st column
            else if (cleaned[i][j] == ':')
            {
                c_data[0] = temp;
                temp = "";
                j++;
                j++;
                continue;
            }
            temp += cleaned[i][j];

            int z = PC[i];
            // pushing last segment of data
            if (j == (int)cleaned[i].size() - 1)
            {
                c_data[it] = temp;
                it++;
            }
            j++;
        }

        // if mnemonic is HALT
        if (c_data[1] == "HALT")
        {
            haltpresent = true;
        }

        // if label is there
        if (!c_data[0].empty())
        {
            labels[c_data[0]] = pc;
        }

        // if label is there or not
        if (!c_data[0].empty())
        {
            data[i].labelcheck = true;
        }
        else
        {
            data[i].labelcheck = false;
        }

        PC[i] = pc;

        if (it == 1)
        {
            // only label is there
            data[i].label = c_data[0];
            data[i].mnemonic = "";
            data[i].operand = "";
            data[i].opType = 0;
            continue;
        }
        pc++;

        // if Mnemonic is not present
        if (!Opcode.count(c_data[1]))
        {
            errors_generated(i + 1, "Invalid Mnemonic");
            continue;
        }

        int xyz = PC[i];
        // matches if instruction is of right type or not
        if (min(Opcode[c_data[1]].second, 1) != it - 2)
        {
            errors_generated(i + 1, "Invalid OpCode Syntax");
            continue;
        }

        data[i].label = c_data[0];
        data[i].mnemonic = c_data[1];
        data[i].operand = c_data[2];
        data[i].opType = operandtype(c_data[2]);
        // if no lable/data varibale is there
        if (!labels.count(data[i].operand) && data[i].opType == 1)
        {
            errors_generated(i + 1, "No such lable/data variable");
        }

        // if NaN etc
        else if (data[i].opType == -1)
        {
            errors_generated(i + 1, "Invalid Number");
        }
    }
}
