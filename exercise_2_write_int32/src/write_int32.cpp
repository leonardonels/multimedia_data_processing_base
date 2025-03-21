#include <print>
#include <iostream>
#include <fstream>
#include <cstdint>  // For int32_t

/*Write a command line program in C++ with this syntax:
write_int32 <filein.txt> <fileout.bin>
The first parameter is the name of a text file that contains signed base 10 integers from -1000 to 1000
separated by whitespace. The program must create a new file, with the name passed as the second
parameter, with the same numbers saved as 32-bit binary little endian numbers in 2's complement.*/

template<typename T>
std::ostream& raw_write(std::ostream& os, const T& val, size_t size = sizeof(T))
{
    return os.write(reinterpret_cast<const char*>(val), size);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::println("Usage: {} <filein.txt> <fileout.txt>", argv[0]);
        return EXIT_FAILURE;
    }

    std::ifstream inputFile(argv[1]);
    if (!inputFile) 
    {
        std::println("Usage: {} <filein.txt> <fileout.txt>", argv[0]);
        return EXIT_FAILURE;
    }

    std::ofstream outputFile(argv[2], std::ios::binary);
    if (!outputFile) {
        std::cerr << "Error opening output file";
        return EXIT_FAILURE;
    }

    // int number;
    // while (inputFile >> number) {
    //     int32_t num32 = static_cast<int32_t>(number);
    //     outputFile.write(reinterpret_cast<char*>(&num32), sizeof(int32_t));
    // }

     int number;
    // There is no need to cast to int32_t...
    while (inputFile >> number) {
        outputFile.write(reinterpret_cast<const char*>(&number), sizeof(int32_t));    //sizeof(int32_t) = 4
    }

    // --------------------------------- LESSON ---------------------------------

    // int number;
    //while (inputFile >> number) 
    //{
    //    raw_write<int32_t>(outputFile, number);
    //}

    // For vectors...
    // raw_write<int32_t>(outputFile, v[0], v.size()*size_of(int32_t))

    return EXIT_SUCCESS;
}
