#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <numeric>
#include <functional>
#include <sstream>
#include <map>
#include <set>

using jets_t = std::vector<char>;

jets_t load_input(const std::string& file){
    jets_t ret;
    std::ifstream fs(file);

    std::string line; 
    while (std::getline(fs, line)) {
        std::copy(line.begin(), line.end(), std::back_inserter(ret));
    }

    return ret;
}

struct pos_t{
    int x, y;
};

bool operator<(const pos_t& a,const pos_t& b){
    return std::tuple(a.x,a.y) < std::tuple(b.x,b.y);
}

struct rock_t{
    int width;
    int height;
    std::set<pos_t> shape;
};

using rocks_t = std::vector<rock_t>;

rocks_t get_rocks(){
    rock_t a { 4, 1, { {0,0}, {1,0}, {2,0}, {3,0} } };
    rock_t b { 3, 3, { {1,0}, {0,-1}, {1,-1}, {2,-1}, {1,-2} } };
    rock_t c { 3, 3, { {2,0}, {2,-1}, {0,-2}, {1,-2}, {2,-2} } };
    rock_t d { 1, 4, { {0,0}, {0,-1}, {0,-2}, {0,-3} } };
    rock_t e { 2, 2, { {0,0}, {1,0}, {0,-1}, {1,-1} } };
    return { a, b, c, d, e };
}

void print(const rock_t& rock){
    std::cout << std::endl;
    for(int y=0; y<rock.height; ++y){
        for(int x=0; x<rock.width; ++x){
            std::cout << (rock.shape.count({x,y}) ? '#' : '.');
        }
        std::cout << std::endl;
    }
}

struct chamber_t{
    std::vector<unsigned char> levels;

    bool get(int x,int y) const{
        //int py = levels.size()-y-1;
        return (levels[y] & (1 << x)) != 0;
    }

    void set(int x,int y,bool b){
        //int py = levels.size()-y-1;
        if(b){
            levels[y] |= (1 << x);
        }else{
            levels[y] &= ~(1 << x); 
        }
    }

    void print() const{
        std::cout << std::endl;
        for(int y=(int)levels.size()-1; y>=0; --y){
            std::cout << "|";
            for(int x=0; x<7; ++x){
                std::cout << (get(x,y) ? '#' : '.');
            }  
            std::cout << "|" << std::endl;
        }
        std::cout << "+-------+" << std::endl;
    }

    void remove_rock(int x, int y, const rock_t& rock){
        for(auto& pos : rock.shape){
            set(pos.x+x, pos.y+y, false);
        }
    }

    void place_rock(int x, int y, const rock_t& rock){
        for(auto& pos : rock.shape){
            set(pos.x+x, pos.y+y, true);
        }
    }

    bool can_move_rock_left(int x, int y, const rock_t& rock){
        for(auto& pos : rock.shape){
            int new_x = pos.x + x - 1;
            int new_y = pos.y + y;
            if(new_x < 0){
                return false;
            }
            if(!rock.shape.count({pos.x-1,pos.y}) && get(new_x, new_y)){
                return false;
            }
        }
        return true;
    }

    bool can_move_rock_right(int x, int y, const rock_t& rock){
        for(auto& pos : rock.shape){
            int new_x = pos.x + x + 1;
            int new_y = pos.y + y;
            if(new_x > 6){
                return false;
            }
            if(!rock.shape.count({pos.x+1,pos.y}) && get(new_x, new_y)){
                return false;
            }
        }
        return true;
    }

    bool can_move_rock_down(int x, int y, const rock_t& rock){
        for(auto& pos : rock.shape){
            int new_x = pos.x + x;
            int new_y = pos.y + y - 1;
            if(new_y == -1){
                return false;
            }
            if(!rock.shape.count({pos.x,pos.y-1}) && get(new_x, new_y)){
                return false;
            }
        }
        return true;
    }

    void move_rock_left(int x, int y, const rock_t& rock){
        remove_rock(x, y, rock);
        place_rock(x-1, y, rock);
    }

    void move_rock_right(int x, int y, const rock_t& rock){
        remove_rock(x, y, rock);
        place_rock(x+1, y, rock);
    }

    void move_rock_down(int x, int y, const rock_t& rock){
        remove_rock(x, y, rock);
        place_rock(x, y-1, rock);
    }
};

auto part1(const jets_t& jets) 
{
    auto rocks = get_rocks();

    chamber_t chamber;

    int j = 0;
    int x = 0;
    int y = 0;
    int max_y = -1;

    for(int i=0; i<2022; ++i){
        auto& rock = rocks[i % rocks.size()];

        x = 2;
        y = max_y + rock.height + 3;

        if(chamber.levels.size() < y+1){
            chamber.levels.resize(y+1);
        }

        chamber.place_rock(x, y, rock);

        bool falling = true;
        while(falling){
            char jet = jets[j++ % jets.size()];
            if(jet == '<'){
                if(chamber.can_move_rock_left(x, y, rock)){
                    chamber.move_rock_left(x, y, rock);
                    x--;
                }
            }else if(jet == '>'){
                if(chamber.can_move_rock_right(x, y, rock)){
                    chamber.move_rock_right(x, y, rock);
                    x++;
                }
            }

            if(chamber.can_move_rock_down(x, y, rock)){
                chamber.move_rock_down(x, y, rock);
                y--;
            }else{
                max_y = std::max(max_y, y);
                falling = false;
            }
        }

        int c = 0;
    }


    return max_y + 1;
}

auto part2(const jets_t& valves) 
{
    return 0;
}


void main()
{
    auto test_values = load_input("../src/day17/test_input.txt");
    auto actual_values = load_input("../src/day17/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    //std::cout << "part2: " << part2(test_values) << std::endl;
    //std::cout << "part2: " << part2(actual_values) << std::endl;
}