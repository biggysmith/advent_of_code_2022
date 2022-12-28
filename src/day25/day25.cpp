#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <numeric>
#include <functional>
#include <map>

using fuel_list_t = std::vector<std::string>;

fuel_list_t load_input(const std::string& file)
{
    fuel_list_t ret;

    std::ifstream fs(file);
    std::string line;
    while (std::getline(fs, line)) {
        ret.push_back(line);
    }

    return ret;
}

std::map<char,int64_t> snafu_to_decimal_map = {
    { '=', -2 },
    { '-', -1 },
    { '0',  0 },
    { '1',  1 },
    { '2',  2 }
};

std::map<int64_t,char> decimal_to_snafu_map = {
    { -2, '=' },
    { -1, '-' },
    {  0, '0' },
    {  1, '1' },
    {  2, '2' }
};

int64_t snafu_to_decimal(const std::string& s){
    int64_t ret = 0;
    for(int64_t i=(int)s.size()-1,f=1; i>=0; i--,f*=5){
        ret += snafu_to_decimal_map[s[i]] * f;
    }
    return ret;
}

std::string decimal_to_snufu(int64_t d){
    std::string ret;

    int64_t i=0;
    double fd = (double)d; 
    for(int64_t f=1; fd>0; i++,f*=5){
        fd = round(fd/5);
    }

    for(int64_t j=i-1; j>=0; --j){
        int64_t k = (int64_t)pow(5,j);
        int64_t n = (int64_t)round(d/(double)k);  
        d -= (n*k);
        ret += decimal_to_snafu_map[n];
    }

    return ret;
}

auto part1(const fuel_list_t& fuel_list) 
{
    int64_t sum = 0;
    for(auto& fuel : fuel_list){
        sum += snafu_to_decimal(fuel);
    }
    return decimal_to_snufu(sum);
}

auto part2(const fuel_list_t& fuel_list) 
{
    return "Merry Christmas Everyone!!!";
}

void main()
{
    auto test_values = load_input("../src/day25/test_input.txt");
    auto actual_values = load_input("../src/day25/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}