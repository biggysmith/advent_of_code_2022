#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <numeric>
#include <functional>
#include <regex>
#include <conio.h>

#define LEFT -1
#define RIGHT -2

struct pos_t{
    int x,y,z;
};

pos_t operator+(const pos_t&a, const pos_t& b){
    return pos_t{a.x+b.x, a.y+b.y, a.z+b.z};
}

struct map_t{
    std::vector<char> data;
    int width = 0;
    int height = 0;
    int depth = 0;

    char& get(int x,int y,int z){
        return data[z*width*height + y*width + x];
    }
};

using path_t = std::vector<int>;

struct note_t{
    map_t map;
    path_t path;
};

note_t load_input(const std::string& file){
    note_t ret;

    {
        std::ifstream fs(file);
        int w = 0;
        int h = 0;
        std::string line;
        while (std::getline(fs, line)) {
            if (line.empty()) {
                break;
            }
            w = std::max(w, (int)line.size());
            h++;
        }

        ret.map.width = w;
        ret.map.height = h;
        ret.map.data.resize(w*h*1, ' ');
    }

    std::ifstream fs(file);

    int h = 0;
    bool part1 = true;
    std::string line;
    while(std::getline(fs, line)){
        if(line.empty()){
            part1 = false;
            continue;
        }

        if(part1){
            for(int i=0; i<line.size(); ++i){
                ret.map.get(i, h, 0) = line[i];
            }
            h++;
        }else{
            std::regex word_regex("\\d+|\\D+");
            for(auto it=std::sregex_iterator(line.begin(), line.end(), word_regex); it!=std::sregex_iterator(); it++){
                if(it->str() == "L"){
                    ret.path.push_back(LEFT);
                }else if(it->str() == "R"){
                    ret.path.push_back(RIGHT);
                }else{
                    ret.path.push_back(std::stoi(it->str()));
                }
            }
        }

    }
    return ret;
}

std::ostream& operator<<(std::ostream& os, map_t& map){
    for(int y=0; y<map.height; ++y){
        for(int x=0; x<map.width; ++x){
            std::cout << map.get(x,y,0);
        }
        std::cout << std::endl;
    }
    return os;
}

int mod(int a, int b) { return (a % b + b) % b; }

auto part1(note_t note) 
{
    pos_t pos { 0, 0, 0 };
    std::vector<pos_t> dirs {
        { 1, 0, 0 },
        { 0, 1, 0 },
        { -1, 0, 0 },
        { 0, -1, 0 }
    };
    int dir_idx = 0;

    //std::cout << note.map << std::endl;

    while(note.map.get(pos.x, pos.y, pos.z) != '.'){
        pos.x++;
    }

    note.map.get(pos.x,pos.y,0) = '>';

    //std::cout << std::endl;
    //std::cout << note.map << std::endl;

    for(auto in : note.path){
        if(in == LEFT){
            dir_idx = mod(dir_idx-1, 4);
        }else if(in == RIGHT){
            dir_idx = mod(dir_idx+1, 4);
        }else{
            for(int i=0; i<in; ++i){
                pos_t new_pos = pos + dirs[dir_idx];

                if(new_pos.x >= note.map.width || (dir_idx==0 && note.map.get(new_pos.x,new_pos.y,0) == ' ')){
                    new_pos.x = 0;
                    while(note.map.get(new_pos.x, new_pos.y, new_pos.z) == ' '){
                        new_pos.x++;
                    }
                }else if(new_pos.x < 0 || (dir_idx==2 && note.map.get(new_pos.x,new_pos.y,0) == ' ')){
                    new_pos.x = note.map.width-1;
                    while(note.map.get(new_pos.x, new_pos.y, new_pos.z) == ' '){
                        new_pos.x--;
                    }
                }else if(new_pos.y >= note.map.height || (dir_idx==1 && note.map.get(new_pos.x,new_pos.y,0) == ' ')){
                    new_pos.y = 0;
                    while(note.map.get(new_pos.x, new_pos.y, new_pos.z) == ' '){
                        new_pos.y++;
                    }
                }else if(new_pos.y < 0 || (dir_idx==3 && note.map.get(new_pos.x,new_pos.y,0) == ' ')){
                    new_pos.y = note.map.height-1;
                    while(note.map.get(new_pos.x, new_pos.y, new_pos.z) == ' '){
                        new_pos.y--;
                    }
                }

                if(note.map.get(new_pos.x, new_pos.y, new_pos.z) != '#'){
                    pos = new_pos;
                    if(dir_idx==0){
                        note.map.get(pos.x,pos.y,0) = '>';
                    }else if(dir_idx==1){
                        note.map.get(pos.x,pos.y,0) = 'v';
                    }else if(dir_idx==2){
                        note.map.get(pos.x,pos.y,0) = '<';
                    }else if(dir_idx==3){
                        note.map.get(pos.x,pos.y,0) = '^';
                    }
                }
            }
        }
    }

    //std::cout << note.map << std::endl;
        
    return 1000 * (pos.y+1) + 4 * (pos.x+1) + dir_idx;
}

auto part2(note_t note)
{
    return 0;
}

void main()
{
    auto test_values = load_input("../src/day22/test_input.txt");
    auto actual_values = load_input("../src/day22/input.txt");

    //std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    //std::cout << "part2: " << part2(test_values) << std::endl;
    //std::cout << "part2: " << part2(actual_values) << std::endl;
}