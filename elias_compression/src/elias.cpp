#include <iostream>
#include <fstream>
#include <bitset>
#include <string>

/*Write a command line program in C++ with this syntax:
    elias [c|d] <filein> <fileout>

The first parameter can be either “c” or “d”, for compression or decompression.
When the “c” option is specified, the program opens the specified input file as text (the name is provided
as the second command line parameter) and reads signed base 10 integers separated by whitespace. The
program must then map each value to the range [1, +∞] using the following correspondence (0, −1, 1, −2,
2, −3, 3, −4, 4, ...) to (1, 2, 3, 4, 5, 6, 7, 8, 9, ...), i.e. negative numbers are mapped to even values, while
non negative ones are mapped to odd numbers. The mapped numbers are then encoded with Elias γ code.

The output file is created in binary mode, with the name passed as the third parameter, and each number
is saved with the number of bits specified by the encoding. The last byte is padded with 0, so that it
cannot be read as a valid number.

When the “d” option is specified, the data is decoded from binary to text. The program must create a new
file, with the name passed as the third parameter, with the same numbers saved in decimal text format,
each followed by a new line.*/

#define DEBUG false

std::ostream& write_byte(std::ostream& os, std::bitset<8>& buffer)
{
    return os.write(reinterpret_cast<const char*>(&buffer), 1);
}

std::bitset<8> read_byte(std::ifstream& is)
{
    unsigned char byte;
    is.read(reinterpret_cast<char*>(&byte), 1);
    return std::bitset<8>(byte);
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::println(std::cerr, "Error: not enough params");
        return EXIT_FAILURE;
    }

    if (*argv[1] != 'c' && *argv[1] != 'd') {
        std::println(std::cerr, "Error with the first param: use 'c' or 'd' instead");
        return EXIT_FAILURE;
    }

    if (*argv[1]=='c'){ // Compression
        std::ifstream input(argv[2] /*, std::ios::binary*/);
        if (!input)
        {
            std::println(std::cerr, "Error opening {}", argv[2]);
            return EXIT_FAILURE;
        }

        std::ofstream output(argv[3], std::ios::binary);
        if (!output) {
            std::println(std::cerr, "Error opening {}", argv[3]);
            return EXIT_FAILURE;
        }

        int j = 0;
        int elias;
        int number;
        std::bitset<8> buffer;

        while (input >> number){
            if (number >= 0){    // Positive
                elias = number * 2 + 1;
            }else{   // Negative
                elias = - number * 2;
            }

            // Coding
            int bit = (number == 0) ? 1 : (static_cast<int>(std::floor(std::sqrt(elias))))*2 + 1;
            std::bitset<128> bits(elias);
            std::string binary = bits.to_string().substr(128 - bit);
            if (DEBUG) {std::cout << "elias representation of " << number << " is " << binary << std::endl;}

            // Write
            for (int i = 0; i < bit; i++){
                buffer[j] = (binary[i] == '1');
                if (j++ == 7){
                    write_byte(output, buffer);
                    j = 0;
                }
            }
        }
        while (j < 8){
            buffer[j] = 0;
            if (j++ == 7){
                write_byte(output, buffer);
            }
        }

    }else{  // Decompression
        std::ifstream input(argv[2], std::ios::binary);
        if (!input)
        {
            std::println(std::cerr, "Error opening {}", argv[2]);
            return EXIT_FAILURE;
        }

        std::ofstream output(argv[3] /*, std::ios::binary*/);
        if (!output) {
            std::println(std::cerr, "Error opening {}", argv[3]);
            return EXIT_FAILURE;
        }

        int num = -1;
        int i = 0;
        std::bitset<8> buffer;
        std::string bits = "";
        
        while ((buffer = read_byte(input))!=0){
            int j = 0;
            while (j != 8){
                bits += (buffer[j++] == 1) ? '1' : '0'; 
                if (bits[i] == '1' && num == -1){num = i * 2 + 1;}
                if (++i == num){
                    unsigned long longValue = std::stoul(bits, nullptr, 2);
                    int intValue = static_cast<int>(longValue);
                    int elias = (intValue%2 == 0) ? -intValue/2 : (intValue-1)/2;

                    if (DEBUG) {std::cout << "value -> " << elias << std::endl;}
                    
                    output << elias << std::endl;
                    
                    i = 0;
                    num = -1;
                    bits.clear();
                }
            }
        }
    }

    return 0;
}