#include <fstream>
#include <ostream>
#include <print>
#include <vector>
#include <string_view>
#include <string>
#include <expected>
#include <array>
#include <iostream>

#include <iomanip>
#include <cstdint>
#include <iterator>
#include <ranges>
#include <bit>
#include <unordered_map>
#include <algorithm>
#include <format>
#include <queue>
#include <memory>

using rgb = std::array<uint8_t, 3>;
using grayscale = std::array<uint8_t, 1>;
using diff = std::array<uint16_t, 1>;

template<typename T>
std::ostream& raw_write(std::ostream& os, const T& val, size_t size = sizeof(T))
{
	return os.write(reinterpret_cast<const char*>(&val), size);
}

template<typename T>
std::istream& raw_read(std::istream& is, T& val, size_t size = sizeof(T))
{
	return is.read(reinterpret_cast<char*>(&val), size);
}

class bitwriter {
	std::ostream& os_;
	uint8_t buffer_ = 0;
	size_t n_ = 0;

	void writebit(uint32_t bit) {
		buffer_ = (buffer_ << 1) | (bit & 1);
		++n_;
		if (n_ == 8) {
			raw_write(os_, buffer_);
			n_ = 0;
		}
	}

public:
	bitwriter(std::ostream& os) : os_(os) {}
	~bitwriter() {
		flush();
	}

	// Write the n least significant bits of u from MSB to LSB
	std::ostream& operator()(uint32_t u, size_t n) {
		while (n-- > 0) {
			writebit(u >> n);
		}
		return os_;
	}

	std::ostream& flush(uint32_t bit = 0) {
		while (n_ > 0) {
			writebit(bit);
		}
		return os_;
	}
};

class bitreader {
	std::istream& is_;
	uint8_t buffer_ = 0;
	size_t n_ = 0;

	uint32_t readbit() {
		if (n_ == 0) {
			raw_read(is_, buffer_);
			n_ = 8;
		}
		--n_;
		return (buffer_ >> n_) & 1;
	}

public:
	bitreader(std::istream& is) : is_(is) {}

	// Read n bits into u from MSB to LSB
	std::istream& operator()(uint32_t& u, size_t n) {
		u = 0;
		while (n-- > 0) {
			u = (u << 1) | readbit();
		}
		return is_;
	}

	operator bool() const {
		return is_.good();
	}
};

template<typename T>
struct frequency {
	std::unordered_map<T, uint32_t> counter_;

	void operator()(const T& val) {
		++counter_[val];
	}
};

template<typename T>
struct huffman {
	struct node {
		T sym_;
		uint32_t freq_;
		uint32_t code_, len_;
		node* left_ = nullptr;
		node* right_ = nullptr;

		node(T sym, uint32_t freq) : sym_(std::move(sym)), freq_(freq) {}
		node(node* left, node* right) :
			freq_(left->freq_ + right->freq_),
			left_(left), right_(right) {
		}
	};
	struct nodeptr {
		static inline int id = 0;
		int id_;
		node* p_;

		nodeptr(node* p) : p_(p), id_(id++) {}
		operator node* () { return p_; }

		//const node* operator->() const { return p_; }
		//node* operator->() { return p_; }

		// deducing this
		template<typename Self>
		auto&& operator->(this Self&& self) { return self.p_; }

		bool operator<(const nodeptr& other) const {
			if (p_->freq_ == other->freq_) {
				return id_ > other.id_;
			}
			return p_->freq_ > other->freq_;
		}
	};
	
	void make_codes(node* n, uint32_t code, uint32_t len) {
		if (n->left_ == nullptr) {
			n->code_ = code;
			n->len_ = len;
			map_[n->sym_] = n;
		}
		else {
			make_codes(n->left_, code << 1, len + 1);
			make_codes(n->right_, (code << 1) | 1, len + 1);
		}
	}

	std::unordered_map<uint8_t, node*> map_;
	std::vector<std::unique_ptr<node>> mem_;

	template<typename It>
	huffman(It first, It last) {
		auto f = std::for_each(first, last, frequency<uint8_t>{});

		std::priority_queue<nodeptr> nodes;

		for (const auto& [sym, freq] : f.counter_) {
			mem_.push_back(std::make_unique<node>(sym, freq));
			nodes.push(mem_.back().get());
		}
		while (nodes.size() > 1) {
			auto n1 = nodes.top();
			nodes.pop();
			auto n2 = nodes.top();
			nodes.pop();
			mem_.push_back(std::make_unique<node>(n1, n2));
			nodes.push(mem_.back().get());
		}
		auto root = nodes.top();
		nodes.pop();

		make_codes(root, 0, 0);
	}

	auto begin() { return map_.begin(); }
	auto end() { return map_.end(); }
	auto size() { return map_.size(); }
	auto operator[](const T& sym) { return map_[sym]; }
};

//--------------------------------------------------------------------------------------------//

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

mat<std::array<uint8_t, 1>> u16t_to_u8t_grayscale (const mat<std::array<uint16_t, 1>>& img){
    mat<std::array<uint8_t, 1>> new_img(img.rows(), img.cols());
    for (int i = 0; i < new_img.rows()*new_img.cols(); i++){
        new_img.data_[i][0] = (img.data_[i][0] + 255)/2 + (img.data_[i][0] + 255)%2;
    }
    return new_img;
}

mat<std::array<uint16_t, 1>> PAMdiff (const mat<std::array<uint8_t, 1>>& img, const bool debug = false, const std::string debug_path = "debug.pam"){
    mat<std::array<uint16_t, 1>> new_img(img.rows(), img.cols());
        
	for (int row = 0; row < img.rows(); row++) {
    	for (int col = 0; col < img.cols(); col++){
			if (col == 0 && row == 0){
					new_img.data_[0][0] = img.data_[0][0];
			}else if (col == 0){
				new_img.data_[row * img.cols()][0] =  img.data_[row * img.cols()][0] - img.data_[(row-1) * img.cols()][0];
			}else{
				new_img.data_[row * img.cols() + col][0] = img.data_[row * img.cols() + col][0] - img.data_[row * img.cols() + col - 1][0];
			}
        }
    }

    if (debug){PAMwrite(debug_path, u16t_to_u8t_grayscale(new_img));}

    return new_img;
}

std::string filename(const std::string& filename, std::string channel, std::string ext_in, std::string ext_out) {
    size_t pos = filename.rfind(ext_in);
    std::string base = (pos != std::string::npos) ? filename.substr(0, pos) : filename;
    return base + "_" + channel + ext_out;
}

template<typename T>
std::string generate_pam_header(const mat<T>& img) {
    std::string tupltype = (img.data_[0].size() == 3) ? "RGB" : "GRAYSCALE";
    return std::format(
        "P7\nWIDTH {}\nHEIGHT {}\nDEPTH {}\nMAXVAL 255\nTUPLTYPE {}\nENDHDR\n",
        img.cols(), img.rows(), img.data_[0].size(), tupltype
    );
}

std::vector<uint8_t> pam_diff_to_bytes(const mat<grayscale>& img) {
    std::string header = generate_pam_header(img);
    std::vector<uint8_t> result(header.begin(), header.end());

    result.reserve(result.size() + img.rawsize());

    for (const auto& pixel : img.data_) {
        uint16_t value = pixel[0];
        result.push_back(static_cast<uint8_t>(value & 0xFF));         // byte basso
        result.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));  // byte alto
    }

    return result;
}

std::vector<uint8_t> pam_diff_codes_to_bytes(const mat<diff>& img) {
    std::vector<uint8_t> result;
    result.reserve(img.rawsize());

    for (const auto& pixel : img.data_) {
        uint16_t value = pixel[0];
        result.push_back(static_cast<uint8_t>(value & 0xFF));         // byte basso
        result.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));  // byte alto
    }

    return result;
}

std::vector<std::array<uint16_t, 1>> bytes_to_pam_diff_codes(const std::vector<uint8_t>& decoded_bytes) {
    if (decoded_bytes.size() % 2 != 0) {
        throw std::runtime_error("Numero dispari di byte: impossibile convertirli in uint16_t");
    }

    std::vector<std::array<uint16_t, 1>> result;
    result.reserve(decoded_bytes.size() / 2);

    for (size_t i = 0; i < decoded_bytes.size(); i += 2) {
        uint16_t low  = decoded_bytes[i];
        uint16_t high = decoded_bytes[i + 1];
        uint16_t value = static_cast<uint16_t>((high << 8) | low);
        result.push_back({ value });
    }

    return result;
}

mat<std::array<uint8_t, 1>> PAMrevdiff(const mat<std::array<uint16_t, 1>>& img){
	mat<std::array<uint8_t, 1>> new_img(img.rows(), img.cols());
        
    for (int row = 0; row < img.rows(); row++) {
    	for (int col = 0; col < img.cols(); col++){
			if (col == 0 && row == 0){
					new_img.data_[0][0] = img.data_[0][0];
			}else if (col == 0){
				new_img.data_[row * img.cols()][0] =  img.data_[row * img.cols()][0] + new_img.data_[(row-1) * img.cols()][0];
			}else{
				new_img.data_[row * img.cols() + col][0] = img.data_[row * img.cols() + col][0] + new_img.data_[row * img.cols() + col - 1][0];
			}
        }
    }

    return new_img;
}

//--------------------------------------------------------------------------------------------//

void compress(const std::string& infile, const std::string& outfile)
{
	using namespace std;
	using namespace std::literals;

	ifstream is(infile, std::ios::binary);
	if (!is) {
		exit(EXIT_FAILURE);
	}

    /*
	is.seekg(0, ios::end);
	auto filesize = is.tellg();
	is.seekg(0);
    
    vector<uint8_t> v(filesize);
    raw_read(is, v[0], filesize);
    */

    auto res = PAMread<grayscale>(infile);
    auto& img = res.value();
    auto new_img = PAMdiff(img);
    std::vector<uint8_t> v = pam_diff_codes_to_bytes(new_img);

	huffman<uint8_t> h(begin(v), end(v));

	vector<huffman<uint8_t>::node*> nodes;
	for (const auto& [sym, n] : h) {
		nodes.push_back(n);
	}
	sort(begin(nodes), end(nodes), 
		[](const huffman<uint8_t>::node* a, const huffman<uint8_t>::node* b) {
			return a->len_ < b->len_;
		});
	/*
	for (const auto& n: nodes) {
		if (isprint(n->sym_)) {
			println("{:c} {:0{}b}", n->sym_, n->code_, n->len_);
		}
		else {
			println("0x{:02x} {:0{}b}", n->sym_, n->code_, n->len_);
		}
	}
	*/

	ofstream os(outfile, std::ios::binary);
	if (!os) {
		exit(EXIT_FAILURE);
	}
	os << "HUFFDIFF";
    
    uint32_t width = static_cast<uint32_t>(new_img.cols());
    raw_write<uint32_t>(os, width);

    uint32_t height = static_cast<uint32_t>(new_img.rows());
    raw_write<uint32_t>(os, height);
	
    uint8_t table_size = static_cast<uint8_t>(h.size());
	os.put(table_size);
	
    bitwriter bw(os);
	for (const auto& [sym, n] : h) {
		bw(sym, 8);
		bw(n->len_, 5);
		bw(n->code_, n->len_);
	}
	bw(static_cast<uint32_t>(v.size()), 32);
	for (const auto& x : v) {
		auto n = h[x];
		bw(n->code_, n->len_);
	}
}

void decompress(const std::string& infile, const std::string& outfile)
{
	using namespace std;

	ifstream is(infile, std::ios::binary);
	if (!is) {
		exit(EXIT_FAILURE);
	}

	string header(8, ' ');
    uint32_t width = 0;
    uint32_t height = 0;
	// WRONG!!! -> is.read(reinterpret_cast<char*>(&header), 8);
	// WRONG!!! -> raw_read(is, header, 8);
	// is.read(&header[0], 8); // OK
	// is.read(header.data(), 8); // OK
	raw_read(is, header[0], 8); // OK
    raw_read<uint32_t>(is, width);
    raw_read<uint32_t>(is, height);
	size_t table_len = is.get();
	if (table_len == 0) {
		table_len = 256;
	}
	//typedef tuple<uint8_t, uint32_t, uint32_t> table_entry;
	using table_entry = tuple<uint8_t, uint32_t, uint32_t>; // sym, code, len
	vector<table_entry> table;
	bitreader br(is);
	for (size_t i = 0; i < table_len; ++i) {
		uint32_t sym, code, len;
		br(sym, 8);
		br(len, 5);
		br(code, len);
		table.emplace_back(sym, code, len);
	}
	uint32_t n;
	br(n, 32);

	sort(begin(table), end(table),
		[](const table_entry& a, const table_entry& b) {
			return get<2>(a) < get<2>(b);
		});
    
    /*
	ofstream os(outfile, std::ios::binary);
	if (!os) {
		exit(EXIT_FAILURE);
	}
    */

    vector<uint8_t> decoded_bytes;
	decoded_bytes.reserve(n);
	for (uint32_t i = 0; i < n; ++i) {
		uint32_t code = 0;
		uint32_t len = 0;
		bool found = false;
		size_t pos;
		for (pos = 0; pos < table.size(); ++pos) {
			while (len < get<2>(table[pos])) {
				uint32_t bit;
				br(bit, 1);
				code = (code << 1) | bit;
				++len;
			}
			if (code == get<1>(table[pos])) {
				found = true;
				break;
			}
		}
		if (!found) {
			exit(EXIT_FAILURE);
		}
        /*
		os.put(get<0>(table[pos]));
        */
        decoded_bytes.push_back(static_cast<uint8_t>(get<0>(table[pos])));
	}
    mat<diff> img(height, width);
    img.data_ = bytes_to_pam_diff_codes(decoded_bytes);
    PAMwrite(outfile, PAMrevdiff(img));
}

int main(int argc, char* argv[]){

	using namespace std;
	using namespace std::literals;

	if (argc != 4) {
		return EXIT_FAILURE;
	}
	if (argv[1] == "c"s) {
		compress(argv[2], argv[3]);
	}
	else if (argv[1] == "d"s) {
		decompress(argv[2], argv[3]);
	}
	else {
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}