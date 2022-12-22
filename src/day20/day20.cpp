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
    int64_t index;
    int64_t val;
};

bool operator==(const number_t& a,const number_t& b){
    return std::tuple(a.index,a.val) == std::tuple(b.index,b.val);
}

using numbers_t = std::vector<number_t>;

numbers_t load_input(const std::string& file){
    numbers_t ret;
    std::ifstream fs(file);

    int c = 0;
    std::string line; 
    while(std::getline(fs, line)){
        ret.push_back({ c++, std::stoi(line) });
    }
    return ret;
}

int64_t mod(int64_t a, int64_t b) { return (a % b + b) % b; }

void move(numbers_t& vec, const number_t& num) {
    auto it = std::find(vec.begin(), vec.end(), num);
    auto pos = mod(std::distance(vec.begin(),it) + num.val, vec.size()-1ULL);

    vec.erase(it);
    vec.insert(vec.begin() + pos, num);
}

auto coordinate(const numbers_t& numbers,int64_t zero_pos,int64_t v){
    return numbers[(v+zero_pos) % numbers.size()].val;
};

auto coordinate_sum(const numbers_t& numbers) {
    auto zero_pos = std::distance(numbers.begin(), std::find_if(numbers.begin(), numbers.end(), [](auto& num){
        return num.val == 0;
    }));

    return coordinate(numbers, zero_pos, 1000) + coordinate(numbers, zero_pos, 2000) + coordinate(numbers, zero_pos, 3000);
}

numbers_t mix(const numbers_t& coded, const numbers_t& orig) {
    numbers_t decoded(coded.begin(), coded.end());

    for(int i=0; i<coded.size(); ++i){
        move(decoded, orig[i]);
    }

    return decoded;
}

auto part1(const numbers_t& coded) 
{
    return coordinate_sum(mix(coded, coded));
}

auto part2(const numbers_t& coded)
{
    numbers_t key_coded(coded.begin(), coded.end());

    for(auto& num : key_coded){
        num.val *= 811589153;
    }

    numbers_t decoded = key_coded;
    for(int i=0; i<10; ++i){
        decoded = mix(decoded, key_coded);
    }

    return coordinate_sum(decoded);
}

void main()
{
    auto test_values = load_input("../src/day20/test_input.txt");
    auto actual_values = load_input("../src/day20/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}
