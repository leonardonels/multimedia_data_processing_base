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

std::string filename(const std::string& filename, const std::string channel) {
    size_t pos = filename.rfind(".pam");
    std::string base = (pos != std::string::npos) ? filename.substr(0, pos) : filename;
    return base + "_" + channel + ".pam";
}

int main(int argc, char* argv[]){
    
    if(argc != 2){
        return EXIT_FAILURE;
    }
    auto res_r = PAMread<gray_scale>(filename(argv[1], "red"));
    auto res_g = PAMread<gray_scale>(filename(argv[1], "green"));
    auto res_b = PAMread<gray_scale>(filename(argv[1], "blue"));
    if(res_r){
        if(res_g){
            if(res_b){
                
                auto& red_img = res_r.value();
                auto& green_img = res_g.value();
                auto& blue_img = res_b.value();
                mat<rgb> new_img(red_img.rows(), red_img.cols());

                for (int i = 0; i < red_img.rawsize()/(sizeof(uint8_t)); i++) {
                
                    new_img.data_[i][0] = red_img.data_[i][0];
                    new_img.data_[i][1] = green_img.data_[i][0];
                    new_img.data_[i][2] = blue_img.data_[i][0];
                
                }
            
                PAMwrite(filename(argv[1], "rgb"), new_img);

            }else{
                std::print("{}", res_b.error());
            }
        }else{
            std::print("{}", res_g.error());
        }
    }else{
        std::print("{}", res_r.error());
    }

    return EXIT_SUCCESS;
}
