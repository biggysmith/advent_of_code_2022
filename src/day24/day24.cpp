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

bool operator<(const pos_t& a,const pos_t& b){
    return std::tuple(a.x,a.y,a.z) < std::tuple(b.x,b.y,b.z);
}

pos_t operator+(const pos_t& a,const pos_t& b){
    return { a.x + b.x, a.y + b.y, a.z + b.z };
}

struct blizzard_t{
    char dir;
    int x, y;
};

bool operator<(const blizzard_t& a,const blizzard_t& b){
    return std::tuple(a.x,a.y) < std::tuple(b.x,b.y);
}

struct valley_t{
    std::multiset<blizzard_t> blizzards;
    int width = 0;
    int height = 0;

    bool in_bounds(const pos_t& pos){
        if((pos.x==src().x && pos.y==src().y) || (pos.x==dst().x && pos.y==dst().y)){
            return true;
        }
        return pos.x>0 && pos.x<width-1 && pos.y>0 && pos.y<height-1;
    }

    bool valid(const pos_t& pos){
        return in_bounds(pos) && !blizzards.count({' ', pos.x, pos.y});
    }

    pos_t src() const{ return {1,0,0}; }
    pos_t dst() const{ return {width-2, height-1,0}; }
};

bool is_blizzard(char c){
    return c == '>' || c == 'v' || c == '<' || c == '^';
}

valley_t load_input(const std::string& file)
{
    valley_t ret;

    std::ifstream fs(file);
    std::string line;
    while (std::getline(fs, line)) {
        for(int x=0; x<line.size(); ++x){
            if(is_blizzard(line[x])){
                ret.blizzards.insert({line[x], x, ret.height});
            }
        }
        ret.width = std::max(ret.width, (int)line.size());
        ret.height++;
    }

    return ret;
}

int next(int i, int end){
    if(i >= end-1){
        return 1;
    }else if(i <= 0){
        return end-2;
    }else{
        return i;
    }
}

valley_t update(const valley_t& valley)
{
    valley_t ret;
    ret.width = valley.width;
    ret.height = valley.height;
    for(auto& blizzard : valley.blizzards){
        switch(blizzard.dir){
            case '<':   ret.blizzards.insert({'<', next(blizzard.x-1, ret.width), blizzard.y});     break;
            case '>':   ret.blizzards.insert({'>', next(blizzard.x+1, ret.width), blizzard.y});     break;
            case '^':   ret.blizzards.insert({'^', blizzard.x, next(blizzard.y-1, ret.height)});    break;
            case 'v':   ret.blizzards.insert({'v', blizzard.x, next(blizzard.y+1, ret.height)});    break;
        };
    }
    return ret;
}

auto bfs(std::vector<valley_t>& valleys, const pos_t& src, const pos_t& dst)
{
    std::set<pos_t> visited;
    std::queue<pos_t> q;
    q.push(src);

    while (!q.empty()) 
    {
        auto curr = q.front();
        q.pop();

        if(curr.x == dst.x && curr.y == dst.y){
            return curr.z;
        }

        if(visited.count(curr)){
            continue;
        }

        visited.insert(curr);
        
        if(curr.z+1 >= valleys.size()){
            valleys.push_back(update(valleys.back()));
        }

        for(auto& d : std::vector<pos_t>{ {0,0,1}, {-1,0,1}, {1,0,1}, {0,-1,1}, {0,1,1} }){
            pos_t new_pos = curr + d;
            if(valleys[new_pos.z].valid(new_pos) && !visited.count(new_pos)){
                q.push(new_pos);
            }
        }
    }

    return -1;
}

auto part1(const valley_t& valley) 
{
    std::vector<valley_t> valleys;
    valleys.push_back(valley);
    valleys.push_back(update(valley));

    return bfs(valleys, valley.src(), valley.dst());
}

auto part2(const valley_t& valley) 
{
    std::vector<valley_t> valleys;
    valleys.push_back(valley);
    valleys.push_back(update(valley));

    pos_t src = valley.src();
    pos_t dst = valley.dst();

    auto first = bfs(valleys, src, dst);
    auto second = bfs(valleys, {dst.x,dst.y,first}, src);
    auto third = bfs(valleys, {src.x,src.y,second}, dst);

    return third;
}

void main()
{
    auto test_values = load_input("../src/day24/test_input.txt");
    auto actual_values = load_input("../src/day24/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}