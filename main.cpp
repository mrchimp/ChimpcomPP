#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>

using namespace std;

typedef std::vector<string> vec_str;

class Chimpcom
{
    public:
        void hi(const std::string &arg){
            cout << "Hello!\n";
        };
};

class Interface
{
    string output;       // string to output to user
    string cmd_in;       // raw cmd input
    string cmd_name;     // the command name
    vec_str param_list;  // list of params
    vec_str tag_list;    // list of tags
    vec_str name_list;   // list of names
    string input_text;   // remaining text after tags and params removed
    Chimpcom chimpcom;

    typedef void (Chimpcom::*MethodPtr)(const std::string &);
    std::map<std::string, MethodPtr> command_list; // method lookup table

    public:
        int init()
        {
            cout << "Chimpcom.\n";

            command_list.insert(std::make_pair("hi", &Chimpcom::hi));


            cout << "\n>>> ";
            getline(cin, cmd_in);

            while (cmd_in != "quit") {
                param_list.clear();
                tag_list.clear();
                name_list.clear();
                parse_cmd(cmd_in);

                if (command_list.find(cmd_name) == command_list.end()) {
                    cout << "What?\n";
                } else {
                    (chimpcom.*command_list[cmd_name])("blah");
                }

                cout << "\n>>> ";
                getline(cin, cmd_in);
            }

            return 0;
        }

        void parse_cmd(string cmd)
        {
            unsigned int i = 0;
            vec_str slices;
            string slice;

            // Slice up input
            slices = split(cmd, ' ');

            // Strip out command name
            cmd_name = slices[0];
            slices.erase(slices.begin());

            while (i < slices.size()) {
                if (slices[i][0] == '-') {
                    cout << "Parameter found!\n";
                    param_list.push_back(slices[i]);
                    slices.erase(slices.begin() + i);
                } else if (slices[i][0] == '#') {
                    cout << "Tag found!\n";
                    tag_list.push_back(slices[i]);
                    slices.erase(slices.begin() + i);
                } else if (slices[i][0] == '@') {
                    cout << "Name found!\n";
                    name_list.push_back(slices[i]);
                    slices.erase(slices.begin() + i);
                } else {
                    i++;
                }
            }
        }

        vec_str &split(const string &s, char delim, vec_str &elems) {
            stringstream ss(s);
            string item;
            while(getline(ss, item, delim)) {
                elems.push_back(item);
            }
            return elems;
        }

        vec_str split(const string &s, char delim) {
            vec_str elems;
            return split(s, delim, elems);
        }
};

int main(int argc, char* argv[])
{
    Interface cmd_interface;
    cmd_interface.init();
    cout << "Thank you and goodnight.";
    return 0;
}
