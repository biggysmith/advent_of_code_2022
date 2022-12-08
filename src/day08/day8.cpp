#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <numeric>

struct trees_t {
    std::vector<int> grid;
    int width;
    int height;

    int operator()(int x,int y) const{
        return grid[y*width+x];
    }
};

trees_t load_input(const std::string& file){
    trees_t ret;
    std::ifstream fs(file);

    int h = 0;
    std::string line; 
    while (std::getline(fs, line)) {
        ret.width = (int)line.size();
        for(auto c : line){
            ret.grid.push_back(c-'0');
        }
        h++;
    }
    ret.height = h;

    return ret;
}

bool is_visible(const trees_t& trees, int x, int y)
{
    if(x==0 || y==0 || x==trees.width-1 || y==trees.height-1){
        return true;
    }

    int height = trees(x,y);

    auto check_dir = [&](int begin,int end,int step,bool xdir){
        for(int i=begin; i!=end; i+=step){
            if(height <= trees(xdir ? i : x, xdir ? y : i)){
                return false;
            }
        }
        return true;
    };

    return check_dir(x+1, trees.width, 1, true) ||      // right
           check_dir(x-1, -1, -1, true) ||              // left
           check_dir(y+1, trees.height, 1, false) ||    // down
           check_dir(y-1, -1, -1, false);               // up
}

auto part1(const trees_t& trees) 
{
    int visible = 0;
    for(int y=0; y<trees.height; ++y){
        for(int x=0; x<trees.width; ++x){
            visible += is_visible(trees,x,y);
        }
    }

    return visible;
} 


int count_visible(const trees_t& trees, int x, int y)
{
    if(x==0 || y==0 || x==trees.width-1 || y==trees.height-1){
        return 0;
    }

    int height = trees(x,y);

    auto count_trees = [&](int begin,int end,int step,bool xdir){
        int count = 0;
        for(int i=begin; i!=end; i+=step, ++count){
            if(height <= trees(xdir ? i : x, xdir ? y : i)){
                ++count;
                break;
            }
        }
        return count;
    };

    return count_trees(x+1, trees.width, 1, true) *      // right
           count_trees(x-1, -1, -1, true) *              // left
           count_trees(y+1, trees.height, 1, false) *    // down
           count_trees(y-1, -1, -1, false);              // up
}

auto part2(const trees_t& trees) 
{  
    int test = count_visible(trees,2,3);

    int best = 0;
    for(int y=0; y<trees.height; ++y){
        for(int x=0; x<trees.width; ++x){
            best = std::max(best, count_visible(trees,x,y));
        }
    }

    return best;
}

void main()
{
    auto test_values = load_input("../src/day08/test_input.txt");
    auto actual_values = load_input("../src/day08/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}