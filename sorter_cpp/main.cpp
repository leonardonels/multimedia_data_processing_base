#include <iostream>
#include <fstream>
#include <cstdlib>  // for std::malloc and std::realloc
#include <cstdio>   // for perror
#include <cassert>  // for assert
#include <crtdbg.h>

class vector {
public:
    size_t size_, capacity_;
    int* data_;

    // Default constructor
    vector() : size_(0), capacity_(0), data_(nullptr) {
        data_ = (int*)malloc(capacity_ * sizeof(int));
        if (data_ == nullptr) {
            perror("Memory allocation error!");
            exit(EXIT_FAILURE); // EXIT_FAILURE == 1
        }
    }

    // Copy constructor
    vector(const vector& other)
        : size_(other.size_), capacity_(other.capacity_) {
        data_ = (int*)malloc(capacity_ * sizeof(int));
        if (data_ == nullptr) {
            perror("Memory allocation error!");
            exit(EXIT_FAILURE); // EXIT_FAILURE == 1
        }
        for (size_t i = 0; i < size_; ++i) {
            data_[i] = other.data_[i];
        }
    }

    // deprecated
    /*
    void copy(const vector& other)  // Now I want to covert it to the = operator
    {
        free(data_);    // pay attention to self assaignement numbers = numbers
        size_ = other.size_;
        capacity_ = other.capacity_;
        data_ = (int*)malloc(capacity_ * sizeof(int));
        if (data_ == nullptr)
        {
            perror("Memory allocation error!");
            exit(EXIT_FAILURE); // EXIT_FAILURE == 1
        }
        for (size_t i = 0; i < size_; ++i)
        {
            data_[i] = other.data_[i];
        }
    }
    */

    // void as return would not allow  something like other = original = numbers
    vector& operator=(const vector& other)  // Now I want to covert it to the = operator
    {
        if (this == &other){return *this;}
        if (other.size_ > capacity_)
        {
            free(data_);    // pay attention to self assaignement numbers = numbers
            capacity_ = other.capacity_;
            data_ = (int*)malloc(capacity_ * sizeof(int));
            if (data_ == nullptr)
            {
                perror("Memory allocation error!");
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

    // Destructor
    ~vector() {
        free(data_);
    }

    void push_back(int x) {
        if (size_ == capacity_) {
            capacity_ *= 2;
            int* new_data = (int*)realloc(data_, capacity_ * sizeof(int));
            if (new_data == nullptr) {
                perror("Memory allocation error!");
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

    int at(size_t index) const {
        assert(index < size_); // Works only in debug mode, in release mode will be ignored
        return data_[index];
    }
};

int compare_ints(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

void print(const vector& v, std::ostream& os) {
    for (size_t i = 0; i < v.size(); ++i) {
        os << v.at(i) << "\n";
    }
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

        vector numbers;
        int num;
        while (filein >> num) {
            numbers.push_back(num);
        }

        // Instantiated only for the purpose of the lesson
        vector original(numbers);
        // Instantiated only for the purpose of the lesson
        vector copy;
        copy = original = numbers;

        std::qsort(numbers.data_, numbers.size(), sizeof(int), compare_ints);

        print(numbers, fileout);

        filein.close();
        fileout.close();

    }
    _CrtDumpMemoryLeaks();  // Lists all memory leaks if there are any

    return 0;
}
