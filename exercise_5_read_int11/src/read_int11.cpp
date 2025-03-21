#include <iostream>
#include <fstream>
#include <bitset>
#include <vector>
#include <cmath>

/*Write a command line program in C ++ with this syntax:
read_int11 <filein.bin> <fileout.txt>
The first parameter is the name of a binary file that contains 11-bit numbers in 2’s complement, with the
bits sorted from most significant to least significant. The program must create a new file, with the name
passed as the second parameter, with the same numbers saved in decimal text format separated by a new
line. Ignore any excess bits in the last byte.*/

std::bitset<8> read_byte(std::ifstream& is)
{
    unsigned char byte;
    is.read(reinterpret_cast<char*>(&byte), 1);
    return std::bitset<8>(byte);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <filein.txt> <fileout.bin>" << std::endl;
        return EXIT_FAILURE;
    }

    std::ifstream inputFile(argv[1], std::ios::binary);
    if (!inputFile) {
        std::cerr << "Error opening input file: " << argv[1] << std::endl;
        return EXIT_FAILURE;
    }

    std::ofstream outputFile(argv[2]/*, std::ios::binary*/);
    if (!outputFile) {
        std::cerr << "Error opening output file: " << argv[2] << std::endl;
        return EXIT_FAILURE;
    }

    std::bitset<8> buffer;
    std::bitset<11> bits;
    
// LITTLE endian
    int j = 0;

    while ((buffer = read_byte(inputFile))!=0)
    {
       for(int i = 0; i<8; i++)
       {
            bits[j] = buffer[i];
            if(++j==11)
            {
                unsigned long longValue = bits.to_ulong();
                int intValue = static_cast<int>(longValue);
                outputFile << intValue << " ";
                j = 0;
            }
       }
    }
    while(j<11)
    {
        bits[j]=0;
        if(++j==11)
        {
            unsigned long longValue = bits.to_ulong();
            int intValue = static_cast<int>(longValue);
            outputFile << intValue;
        }

    }

    return EXIT_SUCCESS;
}