DevProjectBuilder
=======

A Windows Win32 UI application to create and setup development environment for a new project.

Overview
--------
The application takes in a few details about the project; from those details creates the directories, development file stubs, startup.bat, build.bat and a CMD shortcut which runs the startup.bat file.

The user has the ability to create/set defaults for those commonly used environment details.

Installation
------------
Pull down the project and build. If building from the build.bat file some tweaks will need to be made to the startup.bat and build.bat to match your environment.  Run the startup.bat file first as it runs the subst command to associate the project root path to the V drive which the build.bat file uses.

Usage:
-------
Run the generated exe file and follow the prompts to create a project.

Notes
-----
This is a tool I currently use to setup my C/C++ projects and it favors this language.

Yes, I know about string.h but what is the fun in that?...

Would like TODOs
----------------
- Add back the ability to save and load defaults.
- The ability to save multiple defaults files.
- The ability to save and load project files.
- User specified file templates for generated project files.

License
-------
MIT
