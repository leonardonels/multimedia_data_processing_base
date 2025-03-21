#include <print>
#include <iostream>
#include <fstream>
#include <set>
#include <map>

// #include <iomanip>  // for setw()

/*Write a command-line C++ program that accepts the following syntax:
frequencies <input file> <output file>
The program takes a binary file as input and for each byte (interpreted as an unsigned 8-bit integer) it
counts its occurrences. The output is a text file consisting of one line for each different byte found in the
input file with the following format:
<byte><tab><occurrences><new line>
The byte is represented with its two-digit hexadecimal value, occurrences in base ten. The rows are sorted
by byte value, from the smallest to the largest.*/

#define DEBUG true

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::println(std::cerr, "Error: not enough params");
        return EXIT_FAILURE;
    }

    std::ifstream input(argv[1] , std::ios::binary);
    if (!input)
    {
        std::println(std::cerr, "Error opening {}", argv[1]);
        return EXIT_FAILURE;
    }

    std::ofstream output(argv[2]);
    if (!output) {
        std::cerr << "Error opening output file";
        return EXIT_FAILURE;
    }

    std::map<uint8_t, int> map;
    uint8_t byte;

    // while (true) {
    //     int c = input.get();
    //     if (c == EOF){ break; } 
    //     ++count[c] 
    // }

    // I prefer my map solution
    while (input >> byte) {
        map[byte]++;    // index easily follows hexadecimals, not only base 10 numbers
    }

    for (const auto& pair : map) {
        // To write each number of the byte as a single byte
        // output << std::hex << std::setw(2) << std::setfill('0') << i << dec << '/t' << count[i] << std::endl;

        // I still prefer my map solution
        output << std::hex << static_cast<int>(pair.first) << "    " << pair.second << std::endl;
    }

    // ------------------------------ DEBUG ---------------------------

    if(DEBUG){
        for (const auto& pair : map) {
            std::cout << std::hex << static_cast<int>(pair.first) << "    " << pair.second << std::endl;
        }
    }

    return 0;
}