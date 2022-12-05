#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <numeric>
#include <set>
#include <stack>
#include <assert.h>

using stacks_t = std::vector<std::stack<char>>;

struct move_t{
    int num;
    int from;
    int to;
};

struct instructions_t{
    stacks_t stacks;
    std::vector<move_t> moves;
};

instructions_t load_input(const std::string& file){
    instructions_t ret;
    std::ifstream fs(file);

    int stage = 0;
    int num_stacks = 0;
    std::vector<std::vector<char>> create_lists;

    std::string line; 
    while (std::getline(fs, line)) {
        if(num_stacks == 0){
            num_stacks = (int)ceil(line.size() / 4.0f);
            create_lists.resize(num_stacks);
        }

        if(line == "" || line[1]=='1'){
            stage = 1;
            continue;
        }

        if(stage == 0)
        {
            for(int i=0; i<num_stacks; ++i){
                char crate = line[1+(i*4)];
                if(crate != ' '){
                    create_lists[i].push_back(crate);
                }
            }
        }
        else
        {
            move_t move;
            sscanf_s(line.c_str(), "move %d from %d to %d\n", &move.num, &move.from, &move.to);
            ret.moves.push_back(move);
        }
    }

    ret.stacks.resize(num_stacks);
    for(int i=0; i<num_stacks; ++i){
        for(int j=(int)create_lists[i].size()-1; j>=0; --j){
            ret.stacks[i].push(create_lists[i][j]);
        }
    }

    return ret;
}

auto part1(instructions_t instructions) 
{
    for(auto& move : instructions.moves){
        for(int i=0; i<move.num; ++i){
            instructions.stacks[move.to-1].push(instructions.stacks[move.from-1].top());
            instructions.stacks[move.from-1].pop();
        }
    }

    std::string ret(instructions.stacks.size(), ' ');
    for(int i=0; i<ret.size(); ++i){
        ret[i] = instructions.stacks[i].top();
    }

    return ret;
} 

auto part2(instructions_t instructions) 
{  
    for(auto& move : instructions.moves){
        std::vector<char> to_move;
        for(int i=0; i<move.num; ++i){
            to_move.push_back(instructions.stacks[move.from-1].top());
            instructions.stacks[move.from-1].pop();
        }

        for(int i=(int)to_move.size()-1; i>=0; --i){
            instructions.stacks[move.to-1].push(to_move[i]);
        }
    }

    std::string ret(instructions.stacks.size(), ' ');
    for(int i=0; i<ret.size(); ++i){
        ret[i] = instructions.stacks[i].top();
    }

    return ret;
}

void main()
{
    auto test_values = load_input("../src/day05/test_input.txt");
    auto actual_values = load_input("../src/day05/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}