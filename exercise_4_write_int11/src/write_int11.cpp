#include <iostream>
#include <fstream>
#include <bitset>
#include <cmath>

/*Write a command line program in C++ with this syntax:
write_int11 <filein.txt> <fileout.bin>
The first parameter is the name of a text file that contains base 10 integers from -1000 to 1000 separated
by whitespace. The program must create a new file, with the name passed as the second parameter, with
the same numbers saved as 11-bit binary in 2’s complement. The bits are inserted in the file from the
most significant to the least significant. The last byte of the file, if incomplete, is filled with bits equal to
0. Since the incomplete byte will have at most 7 padding bits, there’s no risk of interpreting padding as
another value.*/

/*PS: write a 1byte writer whit an 8bit buffer*/

std::ostream& write_byte(std::ostream& os, std::bitset<8>& buffer)
{
    return os.write(reinterpret_cast<const char*>(&buffer), 1);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <filein.txt> <fileout.bin>" << std::endl;
        return EXIT_FAILURE;
    }

    std::ifstream inputFile(argv[1]);
    if (!inputFile) {
        std::cerr << "Error opening input file: " << argv[1] << std::endl;
        return EXIT_FAILURE;
    }

    std::ofstream outputFile(argv[2], std::ios::binary);
    if (!outputFile) {
        std::cerr << "Error opening output file: " << argv[2] << std::endl;
        return EXIT_FAILURE;
    }

    int number;
    std::bitset<11> bits;
    std::bitset<8> buffer;
    
// LITTLE endian
    int j = 0;

    while (inputFile >> number)
    {
        if (number < -1024 | number > 1023)
        {
            std::cout << "the number " << number << " is too big for 11 bits compression and will be skipped." << std::endl;
            continue;
        }
        
        bits = number;

        for (int i = 0; i < 11; i++)
        {
            buffer[j]=bits[i];
            if(j++==7){
                write_byte(outputFile, buffer);
                j=0;
            }
        }
    }
    while(j<8){
        buffer[j] = 0;
        if(j++==7){
            write_byte(outputFile, buffer);
        }
    }

// BIG endian
//    int j = 8;
//
//    while (inputFile >> number)
//    {
//        if (number < -1024 | number > 1023)
//        {
//            std::cout << "the number " << number << " is too big for 11 bits compression and will be skipped." << std::endl;
//            continue;
//        }
//        
//        bits = number;
//
//        for (int i = 0; i < 11; i++)
//        {
//            buffer[--j]=bits[i];
//            if(j==0){
//                write_byte(outputFile, buffer);
//                j=8;
//            }
//        }
//    }
//    while(j>0){
//        buffer[--j] = 0;
//        if(j==0){
//            write_byte(outputFile, buffer);
//        }
//    }

    return EXIT_SUCCESS;
}