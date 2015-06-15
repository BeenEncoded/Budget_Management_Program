###Budget Program:

**About this program:**
This software was designed and created so that I could more easily create and manage my own budgets.  There are many features that I won't go into detail here, but I tried to make it as flexible and easy to use as I could -- without a GUI -- so that creating budgets is quick, and reviewing them is easy.
  
**Project build dependencies:**
To build this program requires the following software (note the versions are indicative of those I am currently using, please let me know if earlier versions will also work):
-  Make (4.1)
-  CMake (3.2.3)
-  Any C++ compiler that supports C++11 (C++11 is required); Clang (3.6.1) or GCC (5.1.0) is what I've used to successfully compile with

The source code relies on the following third-party libraries:
-  UnitTest++ (1.4-7)
-  Boost (1.58.0-2)
-  NCurses (5.9-7)

**This program currently can only be compiled on Linux**, but I may port it to Windows in the future, depending on my situation.  

**Project outline:**
This project is essentially designed with two parts: the data and the menus.  It's really quite simple: all the menus do is provide an interface to allow the user to manage and manipulate data structures, and you will see that reflected in the program's design.  All todos are in the codebase, and there's even a "todo.txt" if you want to check that out and mabey even contribute by knocking a few of those out for me.  The todos can be grepped with the following regex:
```
/[/\*]+ *todo
```
You can replace "todo" with "note", "attn", "test", or "bug" to find anything else of importance in the project that needs doing.  Adding a todo -- or any other type of note -- is as simple as:
```
//todo need to do this
```
Please keep those updated if you change anything that they refer to.  

**Communication:**
If you find a bug, please feel free to submit a pull request with the resolution, or create an issue.  If you have decided to contribute and take on one of the many todos, please open an issue and assign it to yourself so that I know you're working on it.
