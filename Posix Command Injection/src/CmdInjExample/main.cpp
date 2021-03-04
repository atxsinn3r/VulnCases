/*
 * This example will load a custom file format and attempt to verify that a
 * user-supplied file path points a valid PNG. The verification is done by
 * calling the file command, which is unsafe.
 */

#include <iostream>
#include "FileParser.h"

int main() {
    const char* fileName = "test.bin";
    FileParser parser(fileName);
    if(parser.Validate()) {
        std::cout << "File looks good" << std::endl;
    } else {
        std::cout << "Invalid file" << std::endl;
    }
    return 0;
}
