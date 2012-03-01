#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <ctime>
#include <cstdlib>

using namespace std;

typedef std::vector<string> vec_str; // Name a new type: vec_str, a vector of strings.

class Command
{
    vec_str param_list, tag_list, name_list, word_list;  // Some arrays

    public:
        void init() {
            param_list.clear();
            tag_list.clear();
            name_list.clear();
            word_list.clear();
        }

        /**
         * Add a parameter to the stack
         */
        void addParam(std::string input) {
            param_list.push_back(input);
        }

        /**
         * Add a tag to the stack
         */
        void addTag(std::string input) {
            tag_list.push_back(input);
        }

        /**
         * Add a name to the stack
         */
        void addName(std::string input) {
            name_list.push_back(input);
        }

        /**
         * Add a word to the stack
         */
        void addWord(std::string input) {
            word_list.push_back(input);
        }

        /**
         * Say hello.
         *
         * Usage: hi/hello/hey
         */
        void hi(){
            cout << "Hello!\n";
        };

        /**
         * Decide who should make the tea/coffee.
         *
         * Usage: tea[/coffee] alice bob carol dave [...]
         */
        void tea() {
            int array_size, rand; // size of names array, random int
            string name; // chosen name

            array_size = word_list.size();      // Get array size
            rand = randomInt(0, array_size-1);  // Get a random int
            name = word_list[rand];             // Get the name from the list
            name[0]=toupper(name[0]);           // Capitalise the first letter
            cout << name;                       // Output name...
            cout << " is on hot beverage duty.\n";  // ...and some other text.
        }

        /**
         * Return a random integer in a given range.
         */
        int randomInt(int low, int high) {
            return low + int(((high-low)+1)*rand()/(RAND_MAX + 1.0));
        }
};

/**
 * The Chimpcom Interpretter.
 *
 * Takes a command from the user, splits it up and runs the appropriate
 * function from a Command object.
 */
class Chimpcom
{
    string output;       // string to output to user
    string cmd_in;       // raw cmd input
    string cmd_name;     // the command name
    string input_text;   // remaining text after tags and params removed
    Command cmd;

    typedef void (Command::*MethodPtr)(); // pointer to a method
    std::map<std::string, MethodPtr> command_list; // method lookup table

    public:

        /**
         * Start the Chimpcom interpretter.
         *
         * Display a welcome message. Start the main loop.
         */
        int init()
        {
            cout << "Chimpcom.\n";

            // Pair up some command names with some method pointers.
            // This should be dynamic so that we can add command aliases
            // at runtime.
            command_list.insert(std::make_pair("hi", &Command::hi));
            command_list.insert(std::make_pair("hello", &Command::hi));
            command_list.insert(std::make_pair("hey", &Command::hi));
            command_list.insert(std::make_pair("tea", &Command::tea));
            command_list.insert(std::make_pair("coffee", &Command::tea));

            cout << "\n>>> ";       // Show a command Prompt
            getline(cin, cmd_in);   // Get input from user

            while (cmd_in != "quit") {
                parse_cmd(cmd_in);  // Split the given input up

                // Look up the given command name in the command list. If it's there then
                // run that command...
                if (command_list.find(cmd_name) != command_list.end()) {
                    (cmd.*command_list[cmd_name])(); // run the command
                } else { // ...otherwise show an error.
                    cout << "What?\n";
                }

                cout << "\n>>> ";       // Display a new command prompt
                getline(cin, cmd_in);   // Get input from the user
                cmd.init();             // Reset the command object.
            }

            return 0;
        }

        /**
         * Take a command from the user and split it into it's components
         */
        void parse_cmd(string input)
        {
            unsigned int i = 0;
            vec_str slices; // the command split by spaces

            // Slice up input
            slices = split(input, ' ');

            // Strip out command name
            cmd_name = slices[0];
            slices.erase(slices.begin());

            while (i < slices.size()) {         // for each word in command
                if (slices[i][0] == '-') {          // If first char is a "-" then we have a parameter
                    cmd.addParam(slices[i]);
                } else if (slices[i][0] == '#') {   // ...or a tag...
                    cmd.addTag(slices[i]);
                } else if (slices[i][0] == '@') {   // ...or a name...
                    cmd.addName(slices[i]);
                } else {                            // ... or a normal word
                    cmd.addWord(slices[i]);
                }
                i++;
            }
        }

        /**
         * Split a string by a character and return a vector of strings
         */
        vec_str &split(const string &s, char delim, vec_str &elems) {
            stringstream ss(s);
            string item;
            while(getline(ss, item, delim)) {
                elems.push_back(item);
            }
            return elems;
        }

        /**
         * Split a string by a character and return a vector of strings
         */
        vec_str split(const string &s, char delim) {
            vec_str elems;
            return split(s, delim, elems);
        }
};

/**
 * Main function. Gets the show on the road.
 */
int main(int argc, char* argv[])
{
    Chimpcom cmd_interface;             // Create a Chimpcom interface object
    cmd_interface.init();               // Start the interface
    cout << "Thank you and goodnight."; // Say goodbye
    return 0;                           // Leave
}
