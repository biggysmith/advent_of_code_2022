#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <numeric>
#include <functional>
#include <queue>
#include "timer.hpp"

struct number_t{
    int index;
    int val;
};

bool operator==(const number_t& a,const number_t& b){
    return std::tuple(a.val,a.index) == std::tuple(b.val,b.index);
}

using encrypted_data_t = std::vector<number_t>;

encrypted_data_t load_input(const std::string& file){
    encrypted_data_t ret;
    std::ifstream fs(file);

    int c = 0;
    std::string line; 
    while(std::getline(fs, line)){
        ret.push_back({ c++, std::stoi(line) });
    }
    return ret;
}

int64_t mod(int64_t a, int64_t b) { return (a % b + b) % b; }

std::ostream& operator<<(std::ostream& os, const std::vector<int>& q){
    for(auto i : q){
        os << i << ", ";
    }
    return os;
}

template<typename T>
void move(T& vec, const number_t& num) 
{
    auto it = std::find(vec.begin(), vec.end(), num);
    auto pos = mod(std::distance(vec.begin(),it) + num.val, vec.size()-1ull);

    vec.erase(it);
    vec.insert(vec.begin() + pos, num);
}

auto part1(const encrypted_data_t& coded)
{
    //scoped_timer timer;

    std::vector<number_t> decoded(coded.begin(), coded.end());

    for(auto& num : coded){
        move(decoded, num);
    }

    auto zero_pos = std::distance(decoded.begin(), std::find_if(decoded.begin(), decoded.end(), [](auto& num){
        return num.val == 0;
    }));

    auto get = [&](int v){
        return decoded[(v+zero_pos) % decoded.size()].val;
    };

    return get(1000) + get(2000) + get(3000);
}

auto part2( encrypted_data_t data)
{

    return 0;
}

void main()
{
    auto test_values = load_input("../src/day20/test_input.txt");
    auto actual_values = load_input("../src/day20/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    //std::cout << "part2: " << part2(test_values) << std::endl;
    //std::cout << "part2: " << part2(actual_values) << std::endl;
}
