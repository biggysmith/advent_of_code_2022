#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <numeric>
#include <string_view>
#include <functional>

enum instructions_enum { e_noop=0, e_addx };

struct instruction_t {
    instructions_enum cmd;
    int arg;
};

using instructions_t = std::vector<instruction_t>;

instructions_t load_input(const std::string& file){
    instructions_t ret;
    std::ifstream fs(file);

    std::string line; 
    while (std::getline(fs, line)) {
        std::string in = line.substr(0,4);
        ret.push_back(in == "noop" ? instruction_t{e_noop, 0} : instruction_t{e_addx, std::stoi(line.substr(5))});
    }

    return ret;
}

void run(const instructions_t& instructions, const std::function<void(int,int)>& func)
{
    int X = 1;
    int cycle = 1;

    for(auto& in : instructions)
    {
        if(in.cmd == e_addx){
            func(cycle++, X);
            func(cycle++, X);
            X += in.arg;
        }else{
            func(cycle++, X);
        }
    }
}

auto part1(const instructions_t& instructions) 
{  
    int sum = 0;
    run(instructions, [&](int cycle, int X){
        if(((cycle+20) % 40) == 0){
            sum += cycle * X;
        }
    });

    return sum;
}

auto part2(const instructions_t& instructions) 
{  
    std::cout << '\n';
    run(instructions, [&](int cycle, int X){
        int xpos = cycle % 40;
        std::cout << (xpos >= X && xpos < (X+3) ? '#' : '.') << (xpos == 0 ? "\n" : "");
    });

    return "";
}

void main()
{
    auto test_values = load_input("../src/day10/test_input.txt");
    auto actual_values = load_input("../src/day10/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}