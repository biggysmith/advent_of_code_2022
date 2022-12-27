#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <numeric>
#include <functional>
#include <unordered_set>
#include <unordered_map>

struct pos_t{
    int x, y;
};

struct pos_hash{
    std::size_t operator()(const pos_t& pos) const {
        return std::hash<int>()(pos.x) ^ std::hash<int>()(pos.y);
    }
};

bool operator==(const pos_t& a,const pos_t& b){
    return std::tuple(a.x,a.y) == std::tuple(b.x,b.y);
}

pos_t operator+(const pos_t& a,const pos_t& b){
    return { a.x + b.x, a.y + b.y };
}

using elves_t = std::unordered_set<pos_t,pos_hash>;

elves_t load_input(const std::string& file){
    elves_t ret;
    std::ifstream fs(file);

    int y = 0;
    std::string line; 
    while(std::getline(fs, line)){
        for(int x=0; x<line.size(); ++x){
            if(line[x] == '#'){
                ret.insert({x,y});
            }
        }
        y++;
    }
    return ret;
}

int number_empty_tiles(const elves_t& elves)
{
    pos_t min_pos = { INT_MAX, INT_MAX };
    pos_t max_pos = { INT_MIN, INT_MIN };

    for(auto& pos : elves){
        min_pos.x = std::min(pos.x, min_pos.x);
        min_pos.y = std::min(pos.y, min_pos.y);
        max_pos.x = std::max(pos.x, max_pos.x);
        max_pos.y = std::max(pos.y, max_pos.y);
    }

    int width = max_pos.x - min_pos.x + 1;
    int height = max_pos.y - min_pos.y + 1;

    return (width * height) - (int)elves.size();
}

enum dir_enum { e_north=0, e_north_east, e_east, e_south_east, e_south, e_south_west, e_west, e_north_west, e_none };
pos_t dir_move[8] { { 0, -1 }, { 1, -1 }, { 1, 0 }, { 1, 1 }, { 0, 1 }, { -1, 1 }, { -1, 0 }, { -1, -1 } };

dir_enum check_positions(const elves_t& elves, const pos_t& elf, int round)
{
    bool adjacent[8];
    for(int d=0; d<8; ++d){
        adjacent[d] = elves.count(elf + dir_move[d]);
    }

    if(std::none_of(adjacent, adjacent+8, [](auto b){ return b; })){
        return e_none;
    }

    std::function<dir_enum()> checks[] = {
        [&]{ return (!adjacent[e_north] && !adjacent[e_north_east] && !adjacent[e_north_west]) ? e_north : e_none; },
        [&]{ return (!adjacent[e_south] && !adjacent[e_south_east] && !adjacent[e_south_west]) ? e_south : e_none; },
        [&]{ return (!adjacent[e_west]  && !adjacent[e_north_west] && !adjacent[e_south_west]) ? e_west  : e_none; },
        [&]{ return (!adjacent[e_east]  && !adjacent[e_north_east] && !adjacent[e_south_east]) ? e_east  : e_none; }
    };

    for(int i=0; i<4; ++i){
        dir_enum result = checks[(round+i) % 4]();
        if(result != e_none){
            return result;
        }
    }

    return e_none;
}

struct result_t{
    int empty_tiles;
    int round;
};

auto process(elves_t elves, bool part1) 
{
    int round = 0;

    while(true)
    {
        std::unordered_map<pos_t,std::vector<pos_t>,pos_hash> proposed_moves;
        for(auto& elf : elves)
        {
            dir_enum proposed_dir = check_positions(elves, elf, round);
            if(proposed_dir != e_none){
                proposed_moves[elf+dir_move[proposed_dir]].push_back(elf);
            }
        }

        if(proposed_moves.empty()){
            break;
        }

        for(auto& [dst, srcs] : proposed_moves){
            if(srcs.size() == 1){
                elves.erase(srcs.front());
                elves.insert(dst);
            }
        }

        round++;
        if(part1 && round == 10){
            break;
        }
    }

    return result_t { number_empty_tiles(elves), round+1 };
}


void main()
{
    auto test_values = load_input("../src/day23/test_input_1.txt");
    auto actual_values = load_input("../src/day23/input.txt");

    std::cout << "part1: " << process(test_values, true).empty_tiles << std::endl;
    std::cout << "part1: " << process(actual_values, true).empty_tiles  << std::endl;

    std::cout << "part2: " << process(test_values, false).round << std::endl;
    std::cout << "part2: " << process(actual_values, false).round << std::endl;
}