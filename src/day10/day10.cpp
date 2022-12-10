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

    for(int i=0; i<instructions.size(); ++i)
    {
        int steps = instructions[i].cmd == e_addx ? 2 : 1;
        for(int j=0; j<steps; ++j, ++cycle)
        {
            func(cycle, X);

            if(j==1){
                X += instructions[i].arg;
            }
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
    int w = 40;
    int h = 6;
    std::vector<char> screen(w*h, '.');

    run(instructions, [&](int cycle, int X){
        int pos = cycle % w;
        if(pos >= X && pos < (X+3)){
            screen[cycle-1] = '#';
        }
    });

    std::cout << std::endl;
    for(int y=0; y<h; ++y){
        std::cout << std::string_view(screen.data()+y*w, w) << std::endl;
    }

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