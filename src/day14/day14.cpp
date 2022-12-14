#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <numeric>
#include <functional>
#include <assert.h>

struct pos_t{
    int x, y;
};

pos_t operator-(const pos_t& a,const pos_t& b){
    return { a.x - b.x, a.y - b.y };
}

pos_t operator+(const pos_t& a,const pos_t& b){
    return { a.x + b.x, a.y + b.y };
}

pos_t minimum(const pos_t& a,const pos_t& b){
    return { std::min(a.x, b.x), std::min(a.y, b.y) };
}

pos_t maximum(const pos_t& a,const pos_t& b){
    return { std::max(a.x, b.x), std::max(a.y, b.y) };
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

struct grid_t{
    std::vector<char> data;
    int width;
    int height;

    char& operator()(int x,int y){
        return data[y*width+x];
    }
};

void print(const grid_t& grid){
    std::cout << std::endl;
    for(int y=0; y<grid.height; ++y){
        for(int x=0; x<grid.width; ++x){
            std::cout << grid.data[y*grid.width+x];
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

bool traversable(grid_t& grid, int x, int y){
    return grid(x,y) != '#' && grid(x,y) != 'o';
}

bool drop_sand(grid_t& grid, int x, int y)
{
    bool at_rest = false;
    while(!at_rest)
    {
        if(y+1 >= grid.height-1){
            break;
        }

        while(traversable(grid,x,y+1)){ 
            y++;
        }

        if(traversable(grid,x-1,y+1)){
            x--;
            y++;
            continue;
        }

        if(traversable(grid,x+1,y+1)){
            x++;
            y++;
            continue;
        }

        at_rest = true;
    }

    if(at_rest){
        grid(x,y) = 'o';
    }

    return at_rest;
}


auto part1(const paths_t& paths) 
{  
    pos_t start { 500, 0 };
    pos_t min_pos = { INT_MAX, INT_MAX };
    pos_t max_pos = { INT_MIN, INT_MIN };

    min_pos = minimum(min_pos, start);
    max_pos = maximum(max_pos, start);
    for(auto& path : paths){
        for(auto& pos : path){
            min_pos = minimum(min_pos, pos);
            max_pos = maximum(max_pos, pos);
        }
    }

    min_pos = min_pos - pos_t{ 1, 1 };
    max_pos = max_pos + pos_t{ 1, 1 };
    pos_t dims = max_pos - min_pos + pos_t{ 1, 1 };
    grid_t grid = { std::vector<char>(dims.x * dims.y, '.'), dims.x, dims.y };

    auto fill_line = [&](const pos_t& a, const pos_t& b){
        if(a.x == b.x){
            int ydir = a.y < b.y ? 1 : -1;
            for(int y=a.y; y!=b.y+ydir; y+=ydir){
                grid(a.x-min_pos.x, y-min_pos.y) = '#';
            }
        }else if(a.y == b.y){
            int xdir = a.x < b.x ? 1 : -1;
            for(int x=a.x; x!=b.x+xdir; x+=xdir){
                grid(x-min_pos.x, a.y-min_pos.y) = '#';
            }
        }
    };

    grid(start.x-min_pos.x, start.y-min_pos.y) = '+';
    for(auto& path : paths){
        for(int i=0; i<path.size()-1; ++i){
            fill_line(path[i],path[i+1]);
        }
    }

    while(drop_sand(grid, start.x-min_pos.x, 0)){

    }

    return std::count_if(grid.data.begin(), grid.data.end(), [](auto c){
        return c == 'o';  
    });
}

auto part2(const paths_t& paths) 
{  
    pos_t start { 500, 0 };
    pos_t min_pos = { INT_MAX, INT_MAX };
    pos_t max_pos = { INT_MIN, INT_MIN };

    min_pos = minimum(min_pos, start);
    max_pos = maximum(max_pos, start);
    for(auto& path : paths){
        for(auto& pos : path){
            min_pos = minimum(min_pos, pos);
            max_pos = maximum(max_pos, pos);
        }
    }

    max_pos = max_pos + pos_t{ 0, 2 };

    min_pos = min_pos - pos_t{ 200, 1 };
    max_pos = max_pos + pos_t{ 200, 1 };
    pos_t dims = max_pos - min_pos + pos_t{ 1, 1 };
    grid_t grid = { std::vector<char>(dims.x * dims.y, '.'), dims.x, dims.y };

    auto fill_line = [&](const pos_t& a, const pos_t& b){
        if(a.x == b.x){
            int ydir = a.y < b.y ? 1 : -1;
            for(int y=a.y; y!=b.y+ydir; y+=ydir){
                grid(a.x-min_pos.x, y-min_pos.y) = '#';
            }
        }else if(a.y == b.y){
            int xdir = a.x < b.x ? 1 : -1;
            for(int x=a.x; x!=b.x+xdir; x+=xdir){
                grid(x-min_pos.x, a.y-min_pos.y) = '#';
            }
        }
    };

    grid(start.x-min_pos.x, start.y-min_pos.y) = '+';
    for(auto& path : paths){
        for(int i=0; i<path.size()-1; ++i){
            fill_line(path[i],path[i+1]);
        }
    }

    fill_line({min_pos.x,max_pos.y-1},{max_pos.x,max_pos.y-1});

    while(grid(start.x-min_pos.x, start.y-min_pos.y) == '+'){
        drop_sand(grid, start.x-min_pos.x, 0);
    }

    return std::count_if(grid.data.begin(), grid.data.end(), [](auto c){
        return c == 'o';  
    });
}

void main()
{
    auto test_values = load_input("../src/day14/test_input.txt");
    auto actual_values = load_input("../src/day14/input.txt");

    /*std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;*/

    assert(part1(test_values) == 24);
    assert(part1(actual_values) == 763);

    assert(part2(test_values) == 93);
    assert(part2(actual_values) == 23921);

    std::cout << "PAASSASSASASSSSS!" << std::endl;
}