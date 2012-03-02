#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <map>
#include <ctime>
#include <cstdlib>
#include <iterator>

#ifdef __unix /* We're on Unix */
    #include <unistd.h>
    #define GetCurrentDir getcwd
#elif defined _WIN32 /* Windows */
    #include <direct.h>
    #include <windows.h>
    #define GetCurrentDir _getcwd
#endif

using namespace std;

typedef std::vector<string> vec_str; // Name a new type: vec_str, a vector of strings.


/**
 * Run a command using the system shell
 */
std::string exec(string cmd) {
    #ifdef __UNIX__
        FILE* pipe = popen(cmd, "r");
    #elif defined _WIN32
        FILE* pipe = _popen(cmd.c_str(), "r");
    #endif

    if (!pipe) return "ERROR";
    char buffer[128];
    std::string result = "";

    while(!feof(pipe)) {
        if(fgets(buffer, 128, pipe) != NULL)
                result += buffer;
    }

    #ifdef __UNIX__
        pclose(pipe);
    #elif defined _WIN32
        _pclose(pipe);
    #endif

    return result;
}

class Command
{
    vec_str param_list, tag_list, name_list, word_list;  // Some arrays
    string input;

    public:
        void init() {
            input = "";
            param_list.clear();
            tag_list.clear();
            name_list.clear();
            word_list.clear();
        }

        void setInput(string in) {
            input = in;
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
        void do_hi(){
            cout << "Hello!\n";
        };


        /**
         * Show some help
         */
        void do_help() {
            string output = "\nYou are now using Chimpcom. Welcome.\n\n"
                            ""
                            "Available commands\n"
                            "==================\n"
                            "hi       Say hello\n"
                            "help     Show this message\n"
                            "tea      Decides who should make the tea\n"
                            "coffee   Does the same as tea\n"
                            "go       Open a URL in your default browser\n"
                            "shell    Send a command to the system shell\n"
                            "up       Go up a directory\n"
                            "count    Count number of lines/files. \n"
                            "\n\n"
                            "If you type an unknown command Chimpcom will send it to the system shell.  "
                            "You can therefore use cd, dir/ls etc.  "
                            "For more information just make it up and pretend that you're right.\n";

            cout << output;
        }


        /**
         * Open URL in default browser.
         */
        void do_go() {
            cout << "Opening URL in default browser...\n";
            openURL(word_list[0]);
        }


        /**
         * Decide who should make the tea/coffee.
         *
         * Usage: tea[/coffee] alice bob carol dave [...]
         */
        void do_tea() {
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
         * Change current working directory
         */
        void do_cd(){
            chdir(word_list[0].c_str());
        }


        /**
         * Move the current working directory to the parent directory.
         * If x is specified then go up x levels.
         */
        void do_up() {
            int times;

            if (word_list.size()) {
                stringstream(word_list[0]) >> times;
            } else {
                times = 1;
            }

            for (int x = 0; x < times; x++) {
                chdir("..");
            }
        }

        /**
         * Count number of files and/or number of lines in files.
         */
        void do_count(){
            int num_lines, num_files, x, list_size = 0;
            const char * filename;

            list_size = word_list.size();

            while (x < list_size) {
                filename = word_list[0].c_str();
                std::ifstream in(filename);
                num_lines += std::count(std::istreambuf_iterator<char>(in),
                           std::istreambuf_iterator<char>(), '\n');
                num_files++;
                x++;
            }

            cout << num_lines;
        }

        /**
         * Send a command to the shell. Used to bypass Chimpcom commands.
         * e.g. "shell help" would show windows help whereas "help" would
         * show Chimpcom help.
         */
        void do_shell() {
            // #############################
            // This is implemented wrong. shouldn't base it on word_list as that
            // contains no parameters. Should use input instead and remove first word.
            const char* const delim = " ";
            ostringstream imploded;
            string cmd_str;
            copy(word_list.begin(), word_list.end(), ostream_iterator<string>(imploded, delim));
            cout << exec(imploded.str());
        }


        /**
         * Return a random integer in a given range.
         */
        int randomInt(int low, int high) {
            return low + int(((high-low)+1)*rand()/(RAND_MAX + 1.0));
        }


        /**
         * Opens the given URL in the default browser
         */
        void openURL(string theURL) {
            // Prepend protocol if there isn't one already
            if (theURL.compare(0,3,"http") != 0) {
                theURL = "http://" + theURL;
            }

            #ifdef __unix__
                cout << "This feature is currently unavailable on Unix. \n";
            #elif defined _WIN32
                ShellExecute(NULL, "open", theURL.c_str(), NULL, NULL, SW_SHOWNORMAL);
            #endif
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
    Command cmd;         // The command object

    typedef void (Command::*MethodPtr)(); // pointer to a method
    std::map<std::string, MethodPtr> command_list; // method lookup table


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
        std::transform(cmd_name.begin(), cmd_name.end(), cmd_name.begin(), ::tolower);
        slices.erase(slices.begin());

        while (i < slices.size()) {         // for each word in command
            if (slices[i][0] == '-') {          // If first char is a "-" then we have a parameter
                cmd.addParam(slices[i]);
            } else if (slices[i][0] == '#') {   // ...or a tag...
                cmd.addTag(slices[i]);
            } else if (slices[i][0] == '@') {   // ...or a name...
                cmd.addName(slices[i]);
            } else {                            // ...or a normal word
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


    /**
     * Get current working directory
     */
    string getCurrentDir() {
        char cCurrentPath[FILENAME_MAX];

        if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath) / sizeof(TCHAR))) {
            return "I don't know where I am.\n";
        }
        return cCurrentPath;
    }

    /**
     * Output a command prompt
     */
    void prompt() {
        cout << "\n";
        cout << getCurrentDir();
        cout << ">";       // Display a new command prompt
    }


    public:


        /**
         * Start the Chimpcom interpretter.
         *
         * Display a welcome message. Start the main loop.
         */
        int init()
        {
            cout << "Chimpcom Command Line Interface v0.1.\n";

            // Pair up some command names with some method pointers.
            // This _should_ be dynamic so that we can add command aliases
            // at runtime.....
            command_list.insert(std::make_pair("hi", &Command::do_hi));
            command_list.insert(std::make_pair("hello", &Command::do_hi));
            command_list.insert(std::make_pair("hey", &Command::do_hi));
            command_list.insert(std::make_pair("help", &Command::do_help));
            command_list.insert(std::make_pair("?", &Command::do_help));
            command_list.insert(std::make_pair("tea", &Command::do_tea));
            command_list.insert(std::make_pair("coffee", &Command::do_tea));
            command_list.insert(std::make_pair("go", &Command::do_go));
            command_list.insert(std::make_pair("cd", &Command::do_cd));
            command_list.insert(std::make_pair("up", &Command::do_up));
            command_list.insert(std::make_pair("count", &Command::do_count));
            command_list.insert(std::make_pair("shell", &Command::do_shell));

            // Go to home directory
            char home[80];
            #ifdef __unix__
                home = getenv("HOME")
            #elif defined _WIN32
                strcpy (home, getenv("HOMEDRIVE"));
                strcat (home, getenv("HOMEPATH"));
            #endif
            chdir(home);

            prompt();               // Show prompt
            getline(cin, cmd_in);   // Get input from user

            while (cmd_in != "quit") {
                cmd.setInput(cmd_in);
                parse_cmd(cmd_in);  // Split the given input up

                // Look up the given command name in the command list.
                // If it's there then run that command...
                if (command_list.find(cmd_name) != command_list.end()) {
                    (cmd.*command_list[cmd_name])(); // run the command
                } else { // ...otherwise run as a shell command
                    cout << exec(cmd_in);
                }


                prompt();               // Show prompt
                getline(cin, cmd_in);   // Get input from the user
                cmd.init();             // Reset the command object.
            }

            return 0;
        }
};

/**
 * Main function. Gets the show on the road.
 */
int main(int argc, char* argv[])
{
    Chimpcom cmd_interface;             // Create a Chimpcom interface object
    cmd_interface.init();               // Start the interface
    cout << "\nThank you and goodnight."; // Say goodbye
    return 0;                           // Leave
}
