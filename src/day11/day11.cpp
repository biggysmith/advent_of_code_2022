#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <numeric>
#include <sstream>
#include <list>

enum operation_enum { e_add, e_mul, e_squ }; 

struct monkey_t{
    std::list<uint64_t> items;
    operation_enum op;
    int op_arg;
    int test_arg;
    int true_num;
    int false_num;
    int inspection_count = 0;
};

using monkeys_t = std::vector<monkey_t>;

std::vector<std::string> split(const std::string& s){
    std::string token;
    std::vector<std::string> tokens;
    std::stringstream ss(s);
    while(ss >> token){
        tokens.push_back(token);
    }
    return tokens;
}

monkeys_t load_input(const std::string& file){
    monkeys_t ret;
    std::ifstream fs(file);

    std::string line; 
    while (std::getline(fs, line)) {
        auto tokens = split(line);
        if(!tokens.empty()){
            if(tokens[0] == "Monkey"){
                ret.push_back(monkey_t());
            }else if(tokens[0] == "Starting"){
                for(int i=2; i<tokens.size(); ++i){
                    ret.back().items.push_back(std::stoi(tokens[i]));
                }
            }else if(tokens[0] == "Operation:"){
                if(tokens[5] == "old"){
                    ret.back().op = e_squ;
                    ret.back().op_arg = 0;
                }else if(tokens[4] == "*"){
                    ret.back().op = e_mul;
                    ret.back().op_arg = std::stoi(tokens[5]);
                }else if(tokens[4] == "+"){
                    ret.back().op = e_add;
                    ret.back().op_arg = std::stoi(tokens[5]);
                }
            }else if(tokens[0] == "Test:"){
                ret.back().test_arg = std::stoi(tokens[3]);
            }else if(tokens[1] == "true:"){
                ret.back().true_num = std::stoi(tokens[5]);
            }else if(tokens[1] == "false:"){
                ret.back().false_num = std::stoi(tokens[5]);
            }
        }
    }

    return ret;
}

auto process(monkeys_t monkeys, bool part1, int rounds) 
{  
    int mod_product = std::accumulate(monkeys.begin(), monkeys.end(), 1, [](auto& product, auto& monkey){
        return product * monkey.test_arg;
    });

    for(int r=0; r<rounds; ++r)
    {
        for(auto& monkey : monkeys)
        {
            for(auto it=monkey.items.begin(); it != monkey.items.end(); it=monkey.items.erase(it))
            {
                monkey.inspection_count++;

                auto& item = *it;
                if(monkey.op == e_add){
                    item += monkey.op_arg;
                }else if(monkey.op == e_mul){
                    item *= monkey.op_arg;
                }else if(monkey.op == e_squ){
                    item *= item;
                }

                if(part1){
                    item /= 3;
                }

                int next_monkey = (item % monkey.test_arg) == 0 ? monkey.true_num : monkey.false_num;
                monkeys[next_monkey].items.push_back(item % mod_product);
            }
        }        
    }
    

    std::vector<uint64_t> counts;
    for(auto& monkey : monkeys){
        counts.push_back(monkey.inspection_count);
    }
    std::sort(counts.begin(), counts.end(), std::greater<uint64_t>());

    return counts[0] * counts[1];
}

void main()
{
    auto test_values = load_input("../src/day11/test_input.txt");
    auto actual_values = load_input("../src/day11/input.txt");

    std::cout << "part1: " << process(test_values, true, 20) << std::endl;
    std::cout << "part1: " << process(actual_values, true, 20) << std::endl;

    std::cout << "part2: " << process(test_values, false, 10'000) << std::endl;
    std::cout << "part2: " << process(actual_values, false, 10'000) << std::endl;
}