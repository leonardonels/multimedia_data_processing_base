#include <print>
#include <iostream>
#include <fstream>
#include <cstdint>  // For int32_t

/*Write a command line program in C++ with this syntax:
read_int32 <filein.bin> <fileout.txt>
The first parameter is the name of a binary file containing 32-bit numbers 2’s complement, in little
endian. The program must create a new file, with the name passed as the second parameter, with the same
numbers saved in decimal text format separated by a new line.*/

/*PS: reading the file dimension to read the entire file in a single pass is possible*/

/*PPS: use vecotrs*/

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::println("Usage: {} <filein.txt> <fileout.txt>", argv[0]);
        return EXIT_FAILURE;
    }

    std::ifstream inputFile(argv[1], std::ios::binary);
    if (!inputFile) 
    {
        std::println("Usage: {} <filein.txt> <fileout.txt>", argv[0]);
        return EXIT_FAILURE;
    }

    std::ofstream outputFile(argv[2]/*, std::ios::binary*/);
    if (!outputFile) {
        std::cerr << "Error opening output file";
        return EXIT_FAILURE;
    }

    int32_t number32;
    while (inputFile.read(reinterpret_cast<char*>(&number32), sizeof(int32_t))) {
        //int number = static_cast<int>(number32);      // int cast isn't required from int32_t
        outputFile << number32 << " ";
    }

    // --------------------------------- LESSON ---------------------------------
    //std::ranges::copy(v, outputFile);


    return EXIT_SUCCESS;
}
