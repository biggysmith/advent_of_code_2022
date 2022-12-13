#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <numeric>
#include <functional>
#include <queue>

struct pos_t{
    int x, y;
};

pos_t operator+(const pos_t& a, const pos_t& b){
    return { a.x + b.x, a.y + b.y };
}

bool operator<(const pos_t& a, const pos_t& b){
    return std::make_tuple(a.x,a.y) < std::make_tuple(b.x,b.y);
}

struct grid_t {
    std::vector<int> heights;
    int width = 0;
    int height = 0;
    
    int get(const pos_t& p) const{
        return heights[p.y*width+p.x];
    }

    int get_height(const pos_t& a) const{
        int h = get(a);
        return h=='S' ? 'a' : h=='E' ? 'z' : h;
    };

    void set(const pos_t& p, int height){
        heights[p.y*width+p.x] = height;
    }

    bool in_grid(const pos_t& p) const{
        return p.x >= 0 && p.x < width && p.y >= 0 && p.y < height;
    }

    bool able_to_move(const pos_t& a,const pos_t& b,bool move_low_to_high) const{
        if(move_low_to_high){
            return get_height(b) - get_height(a) <= 1;
        }else{
            return get_height(a) - get_height(b) <= 1;
        }
    };
};

grid_t load_input(const std::string& file){
    grid_t ret;
    std::ifstream fs(file);

    std::string line; 
    while (std::getline(fs, line)) {
        for(auto c : line){
            ret.heights.push_back(c);
        }
        ret.width = std::max(ret.width, (int)line.size());
        ret.height++;
    }

    return ret;
}

pos_t find_height(const grid_t& grid, char h){
    for(int y=0; y<grid.height; ++y){
        for(int x=0; x<grid.width; ++x){
            if(grid.get({x,y}) == h){
                return {x, y};
            }
        }
    }
    return {-1, -1};
}

auto bfs(const grid_t& grid, const pos_t& src, char goal, bool move_low_to_high) 
{  
    std::queue<pos_t> q;

    grid_t visited { std::vector<int>(grid.width*grid.height, 0), grid.width, grid.height };
    grid_t steps { std::vector<int>(grid.width*grid.height, 0), grid.width, grid.height };

    steps.set(src, 0);
    q.push(src);

    while (!q.empty()) 
    {
        auto curr = q.front();
        q.pop();

        if(grid.get(curr) == goal){
            return steps.get(curr);
        }

        if(visited.get(curr)){
            continue;
        }
        
        visited.set(curr, 1);

        for(auto& d : std::vector<pos_t>{ {-1,0}, {1,0}, {0,-1}, {0,1} }){
            pos_t new_pos = curr + d;
            if(grid.in_grid(new_pos) && !visited.get(new_pos) && grid.able_to_move(curr,new_pos,move_low_to_high)){
                steps.set(new_pos, steps.get(curr) + 1);
                q.push(new_pos);
            }
        }
    }

    return -1;
}

auto part1(const grid_t& grid) 
{  
    auto src = find_height(grid, 'S');
    return bfs(grid, src, 'E', true);
}

auto part2(const grid_t& grid) 
{  
    auto src = find_height(grid, 'E');
    return bfs(grid, src, 'a', false);
}

void main()
{
    auto test_values = load_input("../src/day12/test_input.txt");
    auto actual_values = load_input("../src/day12/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}