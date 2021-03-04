# Command Injection Example

In my limited experience, command injections in C/C++ tend to occur with applications that either rely on system commands or external scripts (typically bash or python) to achieve certain tasks. Although this may sound odd at first, because technically in C/C++ you could build anything, my personal opinoin is that sometimes "dirty programming" is the necessary evil/solution in real world development for tight deadlines, internal politics, and stressful business expecations. In layman's terms, imagine you are always given ten seconds to do a ten-hour job.

For this vulnerable example, we are mimicking a scenario where a program needs to parse a custom file format and extracts a path to a PNG file. In order to verify the PNG, an economical way is to use the file command, which saves time and effort to develop. Unfortunately, the path is user-supplied and can be used to inject system commands.

The build directory contains the vulnerable app. You can use poc.rb to build the custom file format as a reference, and exploit from there.