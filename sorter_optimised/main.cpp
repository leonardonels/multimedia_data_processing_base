#include <iostream>
#include <fstream>
#include <cstdlib>  // for std::malloc and std::realloc
#include <cstdio>   // for perror
#include <cassert>  // for assert
#include <crtdbg.h>
#include <utility>

template<typename T>
class vector {
public:
    size_t size_, capacity_;
    T* data_;

    // Default constructor
    vector() : size_(0), capacity_(1), data_(nullptr)
    {
        std::cout << "Creating vector" << std::endl;    // For debug purpose

        data_ = (T*)malloc(capacity_ * sizeof(T));
        if (data_ == nullptr) {
            perror("Constructor memory allocation error!");
            exit(EXIT_FAILURE); // EXIT_FAILURE == 1
        }
    }
    // Copy constructor
    vector(const vector& other)
    {
        std::cout << "Copy constructor" << std::endl;   // For debug purpose

        size_ = other.size_;
        capacity_ = other.capacity_;
        data_ = (T*)malloc(capacity_ * sizeof(T));
        if (data_ == nullptr) {
            perror("Copy memory allocation error!");
            exit(EXIT_FAILURE); // EXIT_FAILURE == 1
        }
        for (size_t i = 0; i < size_; ++i) {
            data_[i] = other.data_[i];
        }
    }
    // Move constructor
    vector(vector&& other)  //rvalue reference
    {
        std::cout << "Move constructor" << std::endl;

        size_ = other.size_;
        capacity_ = other.capacity_;
        data_ = other.data_;
        other.data_ = nullptr;
        other.capacity_ = 0;
    }

    // void as return would not allow  something like other = original = numbers    -   called copy assignment
    vector& operator=(const vector& other)  // Now I want to covert it to the = operator
    {
        std::cout << "Copy assignment" << std::endl;    // For debug purpose

        if (this == &other){return *this;}
        if (other.size_ > capacity_)
        {
            free(data_);    // pay attention to self assaignment numbers = numbers
            capacity_ = other.capacity_;
            data_ = (T*)malloc(capacity_ * sizeof(T));
            if (data_ == nullptr)
            {
                perror("Operator= memory allocation error!");
                exit(EXIT_FAILURE); // EXIT_FAILURE == 1
            }
        }
        size_ = other.size_;
        for (size_t i = 0; i < size_; ++i)
        {
            data_[i] = other.data_[i];
        }
        return *this;
    }
    // move assignment
    vector& operator=(vector&& other)  // Now I want to covert it to the = operator
    {
        std::cout << "Move assignment" << std::endl;    // For debug purpose

        if (this != &other)
        {
            //free(data_);  // Instead of freeing our data, we can save it and put to other to be destroyed anyway
            capacity_ = other.capacity_;
            size_ = other.size_;
            //int* tmp_data = other.data_;
            //data_ = other.data_;
            //other.data_ = tmp_data;
            std::swap(data_, other.data_);  // Even better!
        }
        return *this;
    }
    // Destructor
    ~vector() {
        std::cout << "Destroying vector" << std::endl;  // For debug purpose

        free(data_);
    }

    void push_back(const T& x) {
        if (size_ == capacity_) {
            capacity_ *= 2;
            T* new_data = (T*)realloc(data_, capacity_ * sizeof(T));
            if (new_data == nullptr) {
                perror("PushBack memory allocation error!");
                exit(EXIT_FAILURE);
            }
            data_ = new_data;
        }
        data_[size_] = x;
        size_++;
    }

    size_t size() const {
        return size_;
    }

    const T& at(size_t index) const {
        assert(index < size_); // Works only in debug mode, in release mode will be ignored
        return data_[index];
    }
};

int compare_ints(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

void print(const vector<int>& v, std::ostream& os) {
    for (size_t i = 0; i < v.size(); ++i) {
        os << v.at(i) << "\n";
    }
}
// Returning void and working on a reference is unnatural coding
void read(vector<int>& v, std::istream& is)
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
vector<int> read(std::istream& is)
{
    vector<int> v;
    if (is.good())
    {
        int i;
        while (is >> i) {
            v.push_back(i);
        }
    }
    return v;
}

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

        vector<int> numbers = read(filein);

        std::qsort(numbers.data_, numbers.size(), sizeof(int), compare_ints);

        print(numbers, fileout);

        filein.close();
        fileout.close();

    }
    _CrtDumpMemoryLeaks();  // Lists all memory leaks if there are any

    return 0;
}