#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <numeric>
#include <functional>
#include <set>
#include <unordered_set>
#include <queue>
#include "timer.hpp"

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

auto part1(const droplet_t& droplet)
{
    int area = 0;
    for(auto& pos : droplet){
        for(auto& d : std::vector<pos_t>{ {-1,0,0}, {1,0,0}, {0,-1,0}, {0,1,0}, {0,0,-1}, {0,0,1} }){
            area += !droplet.count(pos + d);
        }
    }
    return area;
}

bool inside_box(const pos_t& pos, const pos_t& min_pos, const pos_t& max_pos){
    return pos.x >= min_pos.x && pos.x < max_pos.x && pos.y >= min_pos.y && pos.y < max_pos.y && pos.z >= min_pos.z && pos.z < max_pos.z;
}

auto trapped_air(const droplet_t& droplet, const pos_t& src, const pos_t& min_pos, const pos_t& max_pos) 
{  
    if(droplet.count(src)){
        return false;
    }

    std::queue<pos_t> q;

    std::set<pos_t> visited;
    q.push(src);

    pos_t ouside_min_pos = min_pos + pos_t{ -1, -1, -1 };
    pos_t ouside_max_pos = max_pos + pos_t{ 1, 1, 1 };

    while (!q.empty()) 
    {
        auto curr = q.front();
        q.pop();

        if(!inside_box(curr, min_pos, max_pos)){
            return false;
        }

        if(visited.count(curr)){
            continue;
        }
        
        visited.insert(curr);

        for(auto& d : std::vector<pos_t>{ {-1,0,0}, {1,0,0}, {0,-1,0}, {0,1,0}, {0,0,-1}, {0,0,1} }){
            pos_t new_pos = curr + d;
            if(!droplet.count(new_pos) && !visited.count(new_pos) && inside_box(new_pos, ouside_min_pos, ouside_max_pos)){
                q.push(new_pos);
            }
        }
    }

    return true;
}

auto part2(const droplet_t& droplet)
{
    scoped_timer timer;

    pos_t min_pos = { INT_MAX, INT_MAX, INT_MAX };
    pos_t max_pos = { INT_MIN, INT_MIN, INT_MIN };

    for(auto& pos : droplet){
        min_pos = minimum(min_pos, pos);
        max_pos = maximum(max_pos, pos);
    }

    min_pos = min_pos + pos_t{ -1, -1, -1 };
    max_pos = max_pos + pos_t{ 1, 1, 1 };

    std::set<pos_t> air;

    for(int z=min_pos.z; z<max_pos.z; ++z){
        for(int y=min_pos.y; y<max_pos.y; ++y){
            for(int x=min_pos.x; x<max_pos.x; ++x){
    
                pos_t pos { x, y, z };
                if(trapped_air(droplet, pos, min_pos, max_pos)){
                    air.insert(pos);
                }

            }
        }
    }

    auto add_side = [&](const pos_t& pos){
        return !droplet.count({pos.x, pos.y, pos.z}) && !air.count({pos.x, pos.y, pos.z});
    };

    int area = 0;
    for(auto& pos : droplet){
        for(auto& d : std::vector<pos_t>{ {-1,0,0}, {1,0,0}, {0,-1,0}, {0,1,0}, {0,0,-1}, {0,0,1} }){
            area += add_side(pos + d);
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
