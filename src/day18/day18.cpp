#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <numeric>
#include <functional>
#include <set>
#include <queue>

struct pos_t{
    int x, y, z;
};

pos_t operator+(const pos_t& a,const pos_t& b){
    return { a.x+b.x, a.y+b.y, a.z+b.z };
}

bool operator<(const pos_t& a,const pos_t& b){
    return std::tuple(a.x,a.y,a.z) < std::tuple(b.x,b.y,b.z);
}

pos_t minimum(const pos_t& a,const pos_t& b){
    return { std::min(a.x,b.x), std::min(a.y,b.y), std::min(a.z,b.z) };
}

pos_t maximum(const pos_t& a,const pos_t& b){
    return { std::max(a.x,b.x), std::max(a.y,b.y), std::max(a.z,b.z) };
}

using droplet_t = std::set<pos_t>;

droplet_t load_input(const std::string& file){
    droplet_t ret;
    std::ifstream fs(file);

    std::string line; 
    while(std::getline(fs, line)){
        pos_t pos;
        sscanf_s(line.c_str(), "%d,%d,%d", &pos.x, &pos.y, &pos.z);
        ret.insert(pos);
    }
    return ret;
}

bool inside_box(const pos_t& pos, const pos_t& min_pos, const pos_t& max_pos){
    return pos.x >= min_pos.x && pos.x < max_pos.x && pos.y >= min_pos.y && pos.y < max_pos.y && pos.z >= min_pos.z && pos.z < max_pos.z;
}

std::set<pos_t> flood_fill(const std::set<pos_t>& lava, const pos_t& pos, const pos_t& min_pos, const pos_t& max_pos) 
{ 
    std::set<pos_t> water;

    std::queue<pos_t> q;
    q.push(pos);

    pos_t outside_min_pos = min_pos + pos_t{ -1, -1, -1 };
    pos_t outside_max_pos = max_pos + pos_t{ 1, 1, 1 };

    while (!q.empty()) 
    {
        auto curr = q.front();
        q.pop();

        if(water.count(curr)){
            continue;
        }
        
        water.insert(curr);

        for(auto& d : std::vector<pos_t>{ {-1,0,0}, {1,0,0}, {0,-1,0}, {0,1,0}, {0,0,-1}, {0,0,1} }){
            pos_t new_pos = curr + d;
            if(!lava.count(new_pos) && !water.count(new_pos) && inside_box(new_pos, outside_min_pos, outside_max_pos)){
                q.push(new_pos);
            }
        }
    }

    return water;
} 


auto part1(const droplet_t& lava)
{
    int area = 0;
    for(auto& pos : lava){
        for(auto& d : std::vector<pos_t>{ {-1,0,0}, {1,0,0}, {0,-1,0}, {0,1,0}, {0,0,-1}, {0,0,1} }){
            area += !lava.count(pos + d);
        }
    }
    return area;
}

auto part2(const droplet_t& lava)
{
    pos_t min_pos = { INT_MAX, INT_MAX, INT_MAX };
    pos_t max_pos = { INT_MIN, INT_MIN, INT_MIN };

    for(auto& pos : lava){
        min_pos = minimum(min_pos, pos);
        max_pos = maximum(max_pos, pos);
    }

    min_pos = min_pos + pos_t{ -1, -1, -1 };
    max_pos = max_pos + pos_t{ 1, 1, 1 };

    std::set<pos_t> water = flood_fill(lava, min_pos, min_pos, max_pos);

    int area = 0;
    for(auto& pos : lava){
        for(auto& d : std::vector<pos_t>{ {-1,0,0}, {1,0,0}, {0,-1,0}, {0,1,0}, {0,0,-1}, {0,0,1} }){
            auto new_pos = pos + d;
            area += !lava.count(new_pos) && water.count(new_pos);
        }
    }

    return area;
}

void main()
{
    auto test_values = load_input("../src/day18/test_input.txt");
    auto actual_values = load_input("../src/day18/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}
