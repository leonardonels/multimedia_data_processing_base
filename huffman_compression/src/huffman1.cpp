#include <fstream>
#include <vector>
#include <iomanip>
#include <cstdint>
#include <iterator>
#include <ranges>
#include <string>
#include <bit>
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <format>
#include <queue>
#include <memory>

#include <print>

/*Write a command line program in C++ with this syntax:
    huffman1 [c|d] <input file> <output file>

When the "c" option is specified, the program opens the specified file (the file must be treated as a binary
file, that is, it can contain any value from 0 to 255 in each byte), calculates the frequencies and generates the
corresponding Huffman codes. 

Then it produces a file with the following format:
    Field           Size                        Description
    MagicNumber     8 byte                      “HUFFMAN1”
    TableEntries    8-bit unsigned integer      Number of items in the following Huffman table (0 means 256 symbols).
    HuffmanTable    TableEntries                Triplet table with symbol, length and Huffman code: the length
                    triplets (sym = 8           and the Huffman code is specified for each symbol. The code is
                    bit, len = 5 bit,           written with as many bits as indicated in the triplet len field.
                    code = len bit)
    NumSymbols      32 bit unsigned             Number of symbols encoded in the file.
                    integer stored in
                    big endian
    Data            NumSymbols                  Values encoded with Huffman codes, according to the previous table.
                    Huffman codes

When the "d" option is specified, the program decompresses the contents of the input file (check that it’s
stored in the previous format) and saves it in the output file.*/

#define print(...) std::cout << std::format(__VA_ARGS__);
#define println(...) std::cout << std::format(__VA_ARGS__) << "\n";

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



void compress(const std::string& infile, const std::string& outfile)
{
	using namespace std;
	using namespace std::literals;

	ifstream is(infile, std::ios::binary);
	if (!is) {
		exit(EXIT_FAILURE);
	}

	is.seekg(0, ios::end);
	auto filesize = is.tellg();
	is.seekg(0);
	vector<uint8_t> v(filesize);
	raw_read(is, v[0], filesize);

	huffman<uint8_t> h(begin(v), end(v));

	vector<huffman<uint8_t>::node*> nodes;
	for (const auto& [sym, n] : h) {
		nodes.push_back(n);
	}
	sort(begin(nodes), end(nodes), 
		[](const huffman<uint8_t>::node* a, const huffman<uint8_t>::node* b) {
			return a->len_ < b->len_;
		});
	for (const auto& n: nodes) {
		if (isprint(n->sym_)) {
			println("{:c} {:0{}b}", n->sym_, n->code_, n->len_);
		}
		else {
			println("0x{:02x} {:0{}b}", n->sym_, n->code_, n->len_);
		}
	}

	ofstream os(outfile, std::ios::binary);
	if (!os) {
		exit(EXIT_FAILURE);
	}
	os << "HUFFMAN1";
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
	// WRONG!!! -> is.read(reinterpret_cast<char*>(&header), 8);
	// WRONG!!! -> raw_read(is, header, 8);
	// is.read(&header[0], 8); // OK
	// is.read(header.data(), 8); // OK
	raw_read(is, header[0], 8); // OK
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

	ofstream os(outfile, std::ios::binary);
	if (!os) {
		exit(EXIT_FAILURE);
	}

	for (uint32_t i = 0; i < n; ++i) {
		uint32_t code = 0;
		uint32_t len = 0;
		bool found = false;
		size_t pos;
		for (pos = 0; pos < table_len; ++pos) {
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
		os.put(get<0>(table[pos]));
	}
}

int main(int argc, char* argv[])
{
	{
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
	}
	_CrtDumpMemoryLeaks();
	return EXIT_SUCCESS;
}