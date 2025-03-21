#include <iostream>
#include <fstream>
#include <cstdio>   // for perror
#include <chrono>
#include <crtdbg.h>
#include <vector>
#include <algorithm>
#include <ranges>
#include <string>
#include <iterator>
#include <format>
#include <print>
#include <set>  // a set is a sorted container

template <typename T>
// Functor
struct comparator
{
    // Your state information
    bool operator()(const int a, const int b) const
    {
        return a < b;
    }
};
struct rev_comparator
{
    // Your state information
    bool operator()(const int a, const int b) const
    {
        return a > b;
    }
};
// Return value optimisation
std::vector<int> read(std::istream& is)
{
    std::vector<int> v;
    if (is.good())
    {
        int num;
        while (is >> num) {
            v.push_back(num);
        }
    }
    return v;
}

int main(int argc, char* argv[]) {
    {
        if (argc != 3) {
            //std::cerr << "Usage: sort_int <filein.txt> <fileout.txt>\n";
            // or better
            //std::format("Usage: {} <filein.txt> <fileout.txt>\n", argv[0]);  // not working on default c++17, fix needed!!!
            // or
            std::println("Usage: {} <filein.txt> <fileout.txt>", argv[0]);   // not working on default c++17, fix needed!!!
            return 1;
        }

        // std::ifstream input(argv[1], std::ios::binary);  // to explicitly specify the mode to binary, default is text mode

        std::ifstream filein(argv[1] /*, std::ios::binary*/);   // commenting the binary read is a good way yo specify the use of text mode instead
        if (filein.fail()) // or !filein or !filein.good()
        {
            std::cerr << "Error opening input file";
            // or better, if c++20 is present :(
            std::println(std::cerr, "Error opening {}", argv[1]);   // not working on default c++17, fix needed!!!
            return 1;
        }

        std::ofstream fileout(argv[2]);
        if (fileout.fail()) {
            std::cerr << "Error opening output file";
            // filein.close();  // No necessary anymore with the destructor in place
            return 1;
        }

        //std::vector<int> numbers = read(filein);

        // Let's change this to c++ without a read function
        //std::istream_iterator<int> start(filein);
        //std::istream_iterator<int> stop;
        //std::copy(start, stop, std::back_inserter(numbers));

        // or
        //std::copy(std::istream_iterator<int>(filein), std::istream_iterator<int>(), std::back_inserter(numbers));
        
        // or even better
        //std::istream_iterator<int> start(filein);
        //std::istream_iterator<int> stop;
        //std::vector<int> numbers(start, stop);

        // or way better
        std::vector<int> numbers{std::istream_iterator<int>(filein), std::istream_iterator<int>()};
        //std::sort(numbers.begin(), numbers.end(), rev_comparator());

        // both can be replaced with
        //std::set<int> numbers{std::istream_iterator<int>(filein), std::istream_iterator<int>()};
        //std::multiset<int> numbers{std::istream_iterator<int>(filein), std::istream_iterator<int>()}; // to keep multiple elements that are the same      

        using namespace std::ranges;  // not working on default c++17, fix needed!!!
        sort(numbers);                // not working on default c++17, fix needed!!!

        // print(numbers, fileout);
        // print(numbers, std::cout);
        // Now lets use strings
        //for(const auto& x : numbers){
        //    std::cout << x << " , ";
        //}
        // Becames
        std::copy(begin(numbers), end(numbers), std::ostream_iterator<int>(fileout, "\n")); // From the iterator library
        std::copy(begin(numbers), end(numbers), std::ostream_iterator<int>(std::cout, ", ")); // From the iterator library
        
    }
    _CrtDumpMemoryLeaks();  // Lists all memory leaks if there are any

    return 0;
}