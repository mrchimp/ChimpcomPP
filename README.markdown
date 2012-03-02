Chimpcom Command Interface
==========================

A multi-platform command line interface. Provides various commands. If a command is not known then the command is passed to the system shell (this bit doesn't work properly yet).

Available Commands
------------------

 * Hi - says hello
 * Help - shows a help screen
 * Tea - chooses who makes the tea e.g. "tea alice bob carol"
 * Go - opens a given url in the default browser e.g. "go deviouschimp.co.uk"
 * Shell - runs a command in the system shell e.g. "dir" or "ls"
 * Up - alias for "cd ..". Can be issued multiple times, e.g. "up 2"
 * Count - count the number of files in a folder or the number lines in file(s).
 * Any other shell commands
 
To do list
----------
 
  * Make Chimpcom into a separate dll that can be used in other programs and respond to single commands. The wrap the looping interface around that.
  * Implement other functions from deviouschimp.co.uk/cmd
  * Ccommand-specific help.
  * Add SQLite into the mix.
  * User accounts and sync them through deviouschimp.co.uk/cmd
  * Dynamic url and command shortcuts.
  