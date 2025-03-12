#include <iostream>
#include <fstream>
#include <cstdlib>  // for std::malloc and std::realloc
#include <cstdio>   // for perror
#include <chrono>
#include <crtdbg.h>
#include <vector>

int compare_ints(const void *a, const void *b)
{
    return (*(int *)a - *(int *)b);
}

void print(const std::vector<int>& v, std::ostream& os)
{
    for (const auto& x : v)
    {
        os << x << "\n";
    }

}
// Returning void and working on a reference is unnatural coding
void read(std::vector<int>& v, std::istream& is)
{
    if (is.good())
    {
        int i;
        while (is >> i) {
            v.push_back(i);
        }
    }
}
// Return value optimisation
std::vector<int> read(std::istream& is)
{
    std::vector<int> v;
    if (is.good())
    {
        int i;
        while (is >> i) {
            v.push_back(i);
        }
    }
    return v;
}


int global_id = 0;
struct widget
{
    int id_;
    int x_;

    widget()
    {
        id_ = global_id++;
        x_ = 5;
    }
    widget(int x)
    {
        id_ = global_id++;
        x_ = x;
    }
    widget(const widget& other)
    {
        id_ = global_id++;
        x_ = other.x_;
    }
    widget& operator=(const widget& other)
    {
        x_ = other.x_;
        return *this;
    }
    ~widget()
    {
        //global_id--;
        std::cout << "Destroying widget" << std::endl;
    }
};

int main(int argc, char* argv[]) {
    {
        if (argc != 3) {
            std::cerr << "Usage: sort_int <filein.txt> <fileout.txt>\n";
            return 1;
        }

        std::ifstream filein(argv[1]);
        if (!filein.is_open()) {
            perror("Error opening input file");
            return 1;
        }

        std::ofstream fileout(argv[2]);
        if (!fileout.is_open()) {
            perror("Error opening output file");
            filein.close();
            return 1;
        }

        std::vector<int> numbers = read(filein);
        
        std::qsort(numbers.data(), numbers.size(), sizeof(int), compare_ints);

        print(numbers, fileout);

        // Lesson purpose
        int x = numbers[0];
        numbers[0]=7;

        std::vector<double> v;
        v.push_back(3.);
        v.push_back(4.56);
        v.push_back(0.1);
        v.push_back(3.14);

        std::vector<widget> vec;
        vec.push_back(widget(7));
        vec.push_back(widget(25));
        vec.push_back(widget(123));
        vec.emplace_back(widget(125));

        filein.close();
        fileout.close();

    }
    _CrtDumpMemoryLeaks();  // Lists all memory leaks if there are any

    return 0;
}