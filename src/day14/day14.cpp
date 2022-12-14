#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <numeric>
#include <functional>
#include <unordered_map>

struct pos_t{
    int x, y;
};

struct pos_hash{
    std::size_t operator() (const pos_t& pos) const {
        return std::hash<int>()(pos.x) ^ std::hash<int>()(pos.y);
    }
};

bool operator==(const pos_t& a,const pos_t& b){
    return std::make_tuple(a.x,a.y) == std::make_tuple(b.x,b.y);
}

using path_t = std::vector<pos_t>;
using paths_t = std::vector<path_t>;

paths_t load_input(const std::string& file){
    paths_t ret;
    std::ifstream fs(file);

    std::string line; 
    while (std::getline(fs, line)) {
        ret.push_back(path_t());

        size_t pos = 0;
        std::string token;
        std::string delimiter = " -> ";

        auto parse_pos = [&]{
            token = line.substr(0, pos);
            auto comma = token.find(',');
            ret.back().push_back({ std::stoi(token.substr(0,comma)), std::stoi(token.substr(comma+1)) });
            line.erase(0, pos + delimiter.length());
        };

        while ((pos = line.find(delimiter)) != std::string::npos) {
            parse_pos();
        }
        parse_pos();
    }

    return ret;
}

struct cave_t{
    std::unordered_map<pos_t,char,pos_hash> points;
    int max_height;
    bool has_floor;

    char& operator()(int x,int y){
        return points[{x,y}];
    }

    char operator()(int x,int y) const{
        if(!points.count({x,y})){
            return '.';
        }
        return points.at({x,y});
    }
};

bool traversable(const cave_t& cave, int x, int y){
    if(cave.has_floor && y >= cave.max_height){
        return false;
    }
    return cave(x,y) != '#' && cave(x,y) != 'o';
}

bool drop_sand(cave_t& cave, int x, int y)
{
    bool at_rest = false;
    while(!at_rest)
    {
        if(!cave.has_floor && y+1 > cave.max_height){
            break; // in the void
        }

        if(cave.has_floor && y+1 >= cave.max_height){
            at_rest = true;
            break; // hit the floor
        }

        while(traversable(cave, x, y+1)){ 
            y++; // falling
        }

        if(traversable(cave, x-1, y+1)){
            x--; y++; // move diagonal left
            continue;
        }

        if(traversable(cave, x+1, y+1)){
            x++; y++; // move diagonal right
            continue;
        }

        at_rest = true;
    }

    if(at_rest){
        cave(x,y) = 'o';
    }

    return at_rest;
}

cave_t setup_cave(const paths_t& paths, const pos_t& start, bool has_floor)
{
    cave_t cave;
    cave.max_height = INT_MIN;
    cave.has_floor = has_floor;

    auto fill_line = [&](const pos_t& a, const pos_t& b){
        if(a.x == b.x){
            int ydir = a.y < b.y ? 1 : -1;
            for(int y=a.y; y!=b.y+ydir; y+=ydir){
                cave(a.x, y) = '#';
            }
        }else if(a.y == b.y){
            int xdir = a.x < b.x ? 1 : -1;
            for(int x=a.x; x!=b.x+xdir; x+=xdir){
                cave(x, a.y) = '#';
            }
        }
    };

    cave(start.x, start.y) = '+';
    for(auto& path : paths){
        for(int i=0; i<path.size()-1; ++i){
            fill_line(path[i], path[i+1]);
            cave.max_height = std::max(std::max(cave.max_height, path[i].y), path[i+1].y);
        }
    }

    if(has_floor){
        cave.max_height += 2;
    }

    return cave;
}

auto count_sand(const cave_t& cave){
   return std::count_if(cave.points.begin(), cave.points.end(), [](auto& pos_and_c){
        return pos_and_c.second == 'o';  
    }); 
}

auto part1(const paths_t& paths) 
{  
    pos_t start { 500, 0 };
    cave_t cave = setup_cave(paths, start, false);

    while(drop_sand(cave, start.x, start.y)){
    }

    return count_sand(cave);
}

auto part2(const paths_t& paths) 
{  
    pos_t start { 500, 0 };
    cave_t cave = setup_cave(paths, start, true);

    while(cave(start.x, start.y) == '+'){
        drop_sand(cave, start.x, start.y);
    }

    return count_sand(cave);
}

void main()
{
    auto test_values = load_input("../src/day14/test_input.txt");
    auto actual_values = load_input("../src/day14/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}