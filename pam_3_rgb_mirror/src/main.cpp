#include <fstream>
#include <ostream>
#include <print>
#include <vector>
#include <string_view>
#include <string>       // for getline
#include <expected>
#include <array>
#include <iostream>

using rgb = std::array<uint8_t, 3>;
using gray_scale = std::array<uint8_t, 1>;

template<typename T>
struct mat {
    size_t rows_, cols_;
    std::vector<T> data_;

    mat(size_t rows = 0, size_t cols = 0) : rows_(rows), cols_(cols), data_(rows*cols) {}

    auto rows() const {return rows_;}
    auto cols() const {return cols_;}
    auto size() const {return data_.size();}
    
    T& operator()(size_t r, size_t c){
        return data_[r*cols_+c];
    }
    const T& operator()(size_t r, size_t c) const {
        return data_[r*cols_+c];
    }

    /*
    template <typename self>
    auto&& opeator ()(this self&& self, size_t r, size_t c){
        return self.data_[r*self.cols_+c];
    }
    */

    /*char* */
    auto rawdata(){
        return reinterpret_cast<char*>(data_.data());
    }
    /*const char* */
    auto rawdata() const {
        return reinterpret_cast<const char*>(data_.data());
    }
    
    /*size_t*/
    auto rawsize() const {return size()*sizeof(T);}
};

template<typename T>
bool PAMwrite(std::string_view filename, const mat<T>& img){
    std::ofstream os(filename.data(), std::ios::binary);
    if (!os) {
        return false;
    }

    std::string tupltype = "GRAYSCALE";
    if(img.data_[0].size() == 3){tupltype = "RGB";}

    std::print(os, "P7\nWIDTH {}\nHEIGHT {}\nDEPTH {}\nMAXVAL 255\nTUPLTYPE {}\nENDHDR\n", img.cols(), img.rows(), img.data_[0].size(), tupltype);
    os.write(img.rawdata(), img.rawsize());
    return true;
}

template<typename T>
std::expected<mat<T>, std::string> PAMread(const std::string& filename){
    std::ifstream is(filename, std::ios::binary);
    if(!is){
        return std::unexpected("ERROR OPEN FILE");
    }
    std::string header;
    if(!std::getline(is, header) || header != "P7"){
        return std::unexpected("HEADER ERROR");
    }

    std::string token;
    size_t w = -1, h= -1;
    while(is >> token && token != "ENDHDR"){
        if(token == "WIDTH"){
            is >> w;
        }
        else if(token == "HEIGHT"){
            is >> h;
        }
        /*more cheks here...*/
    }

    if(w == -1 || h == -1){
        return std::unexpected("MISSING VALUES ERROR");
    }

    char skip;
    is.read(&skip, 1);

    mat<T> img(h, w);
    is.read(img.rawdata(), img.rawsize());
    return img;
}

int main(int argc, char* argv[]){
    
    /*EXERCISE 0-G*/
    /*
    if(argc != 2){
        return EXIT_FAILURE;
    }
    mat<uint8_t> img(256, 256);
    for(size_t r = 0; r < img.rows(); ++r){
        for(size_t c = 0; c < img.cols(); ++c){
            img(r, c) = static_cast<uint8_t>(r);
        }    
    }
    if(!PAMwrite(argv[1], img)){
        return EXIT_FAILURE;
    }
    */

    /*EXERCISE 0-R*/
    /*
    if(argc != 3){
        return EXIT_FAILURE;
    }
    */

    /*
    mat<uint8_t> img = PAMread(argv[1]);
    if(img.empty()){}   // zombie approach
    */// or...
    /*if(!PAMread(argv[1], img)){}*/

    /*
    auto res = PAMread<rgb>(argv[1]);
    if(res){
        auto& img = res.value();
        PAMwrite(argv[2], img);
    }else{
        std::print("{}", res.error());
    }
    */

    /*EXERCISE 1*/
    /*
    if(argc != 2){
        return EXIT_FAILURE;
    }
    mat<gray_scale> img(256, 256);

    //for(int i = 0; i < 256; i++){
    //    for(int j = 0; j < 256; j++){
    //        img.data_[i*256+j][0] = i;
    //    }
    //}

    for (int i = 0; i < 256 * 256; ++i) {
        int row = i / 256;
        img.data_[i][0] = row;
    }
    PAMwrite(argv[1], img);
    */
    

    /*EXERCISE 2*/
    
    if(argc != 3){
        return EXIT_FAILURE;
    }
    auto res = PAMread<gray_scale>(argv[1]);
    if(res){
        auto& img = res.value();
        mat<gray_scale> new_img(img.rows(), img.cols());
        
        int row = img.rows(); int new_row = 0;
        while(row > 0){
            --row;
            std::copy(
                img.data_.begin() + row * img.cols(),
                img.data_.begin() + (row + 1) * img.cols(),
                new_img.data_.begin() + new_row * img.cols()
            );
            ++new_row;
        }

        PAMwrite(argv[2], new_img);
    }else{
        std::print("{}", res.error());
    }
    


    /*EXERCISE 3*/
    /*
    if(argc != 3){
        return EXIT_FAILURE;
    }
    auto res = PAMread<rgb>(argv[1]);
    if(res){
        auto& img = res.value();
        mat<rgb> new_img(img.rows(), img.cols());
        
        for (int row = 0; row < img.rows(); ++row) {
            std::vector<rgb> temp_row(
                img.data_.begin() + row * img.cols(),
                img.data_.begin() + (row + 1) * img.cols()
            );
        
            std::reverse(temp_row.begin(), temp_row.end());
        
            std::copy(
                temp_row.begin(),
                temp_row.end(),
                new_img.data_.begin() + row * new_img.cols()
            );
        }

        PAMwrite(argv[2], new_img);
    }else{
        std::print("{}", res.error());
    }
    */

    return EXIT_SUCCESS;
}
