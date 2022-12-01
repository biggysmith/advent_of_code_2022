#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <numeric>

using elf_list_t = std::vector<std::vector<int>>;

elf_list_t load_input(const std::string& file){
    elf_list_t ret(1);
    std::ifstream fs(file);
    std::string line;
    while (std::getline(fs, line)) {
        if(line == ""){
            ret.push_back(std::vector<int>());
        }else{
            ret.back().push_back(std::stoi(line));
        }
    }
    return ret;
}

int part1(const elf_list_t& elf_list){
    int max_sum = 0;
    for(auto& elf_calories : elf_list){
        max_sum = std::max(max_sum, std::accumulate(elf_calories.begin(),elf_calories.end(),0));
    }
    return max_sum;
}

int part2(const elf_list_t& elf_list){
    std::vector<int> sums;
    for(auto& elf_calories : elf_list){
        sums.push_back(std::accumulate(elf_calories.begin(),elf_calories.end(),0));
    }
    std::partial_sort(sums.begin(), sums.begin()+3, sums.end(), std::greater<int>());
    return std::accumulate(sums.begin(),sums.begin()+3,0);
}


void main()
{
    auto test_values = load_input("../src/day01/test_input.txt");
    auto actual_values = load_input("../src/day01/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}