#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <numeric>
#include <string_view>
#include <functional>
#include <queue>
#include <set>
#include <map>

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

    int& get(const pos_t& p){
        return heights[p.y*width+p.x];
    }

    bool in_grid(const pos_t& p) const{
        return p.x >= 0 && p.x < width && p.y >= 0 && p.y < height;
    }

    bool able_to_move(const pos_t& a,const pos_t& b) const{
        int curr_height = get(a)=='S' ? 'a' : get(a);
        int next_height = get(b)=='E' ? 'z'+1 : get(b);
        return next_height - curr_height <= 1;
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

std::vector<pos_t> find_heights(const grid_t& grid, char h){
    std::vector<pos_t> heights;
    for(int y=0; y<grid.height; ++y){
        for(int x=0; x<grid.width; ++x){
            if(grid.get({x,y}) == h){
                heights.push_back({x,y});
            }
        }
    }
    return heights;
}

auto dijkstra(const grid_t& grid, const pos_t& src, const pos_t& dst) 
{  
    std::queue<pos_t> q;

    grid_t visited { std::vector<int>(grid.width*grid.height, 0), grid.width, grid.height };
    grid_t cost { std::vector<int>(grid.width*grid.height, 0), grid.width, grid.height };

    cost.get(src) = 0;
    q.push(src);

    while (!q.empty()) {
        auto curr = q.front();
        q.pop();

        if(visited.get(curr)){
            continue;
        }
        
        visited.get(curr) = 1;

        for(auto& d : std::vector<pos_t>{ {-1,0}, {1,0}, {0,-1}, {0,1} }){
            pos_t new_pos = curr + d;
            if(grid.in_grid(new_pos) && grid.able_to_move(curr,new_pos) && !visited.get(new_pos)){
                cost.get(new_pos) = cost.get(curr) + 1;
                q.push(new_pos);
            }
        }

    }

    return cost.get(dst)==0 ? INT_MAX : cost.get(dst);
}

auto part1(const grid_t& grid) 
{  
    auto src = find_heights(grid, 'S');
    auto dst = find_heights(grid, 'E');
    return dijkstra(grid, src[0], dst[0]);
}

auto part2(const grid_t& grid) 
{  
    auto srcs = find_heights(grid, 'a');
    auto dst = find_heights(grid, 'E');

    int min_step = INT_MAX;
    for(auto& src : srcs){
        min_step = std::min(min_step, dijkstra(grid, src, dst[0]));
    }
    return min_step;
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