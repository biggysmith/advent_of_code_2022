#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <numeric>
#include <functional>
#include <set>

using jets_t = std::string;

jets_t load_input(const std::string& file){
    jets_t ret;
    std::ifstream fs(file);

    std::string line; 
    std::getline(fs, line);
    return line;
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

struct chamber_t{
    std::vector<char> levels;

    bool get(int x,int y) const{
        return (levels[y] & (1 << x)) != 0;
    }

    void set(int x,int y,bool b){
        b ? levels[y] |= (1 << x) : levels[y] &= ~(1 << x);
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
            if(pos.x + x - 1 < 0){
                return false;
            }else if(!rock.shape.count({pos.x-1,pos.y}) && get(pos.x + x - 1, pos.y + y)){
                return false;
            }
        }
        return true;
    }

    bool can_move_rock_right(int x, int y, const rock_t& rock){
        for(auto& pos : rock.shape){
            if(pos.x + x + 1 > 6){
                return false;
            }else if(!rock.shape.count({pos.x+1,pos.y}) && get(pos.x + x + 1, pos.y + y)){
                return false;
            }
        }
        return true;
    }

    bool can_move_rock_down(int x, int y, const rock_t& rock){
        for(auto& pos : rock.shape){
            if(pos.y + y - 1 == -1){
                return false;
            }else if(!rock.shape.count({pos.x,pos.y-1}) && get(pos.x + x, pos.y + y - 1)){
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

template <typename T>
void hash_combine(std::size_t& seed, const T& v){
    seed ^= std::hash<T>()(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}

auto process(const jets_t& jets, bool part2) 
{
    chamber_t chamber;
    auto rocks = get_rocks();

    int max_y = -1;
    std::vector<size_t> hashes;
    std::vector<int> max_heights;

    for(int i=0, j=0; i<2022; ++i)
    {
        auto& rock = rocks[i % rocks.size()];

        int x = 2;
        int y = max_y + rock.height + 3;

        if(chamber.levels.size() < y+1){
            chamber.levels.resize(y+1);
        }

        chamber.place_rock(x, y, rock);

        for(bool falling=true; falling; ++j)
        {
            if(jets[j % jets.size()] == '<'){
                if(chamber.can_move_rock_left(x, y, rock)){
                    chamber.move_rock_left(x--, y, rock);
                }
            }else{
                if(chamber.can_move_rock_right(x, y, rock)){
                    chamber.move_rock_right(x++, y, rock);
                }
            }

            if(chamber.can_move_rock_down(x, y, rock)){
                chamber.move_rock_down(x, y--, rock);
            }else{
                falling = false;

                max_y = std::max(max_y, y);
                max_heights.push_back(max_y);

                if(part2)
                {
                    size_t hash = 0;
                    hash_combine(hash, j % jets.size());
                    hash_combine(hash, i % rocks.size());
                    for(int h=max_y-4; h!=max_y; h++){ // hash surface
                        hash_combine(hash, chamber.levels[std::max(0,h)]);
                    }

                    auto it = std::find(hashes.begin(), hashes.end(), hash);
                    if(it != hashes.end()){      
                        int start = (int)std::distance(hashes.begin(), it);
                        int end = i;
                        int cycle_width = end - start;

                        size_t pre_cycle_height = max_heights[start];

                        size_t repeats = (1'000'000'000'000 - start) / cycle_width;
                        size_t repeated_cycles_height = repeats * (max_heights[end] - max_heights[start]);

                        size_t remaining_cycles = (1'000'000'000'000 - start) % cycle_width;
                        size_t post_cycle_height = max_heights[start + remaining_cycles] - max_heights[start];

                        return pre_cycle_height + repeated_cycles_height + post_cycle_height; 
                    }else{
                        hashes.push_back(hash);
                    }
                }
            }
        }

    }

    return max_y + 1ull;
}

void main()
{
    auto test_values = load_input("../src/day17/test_input.txt");
    auto actual_values = load_input("../src/day17/input.txt");

    std::cout << "part1: " << process(test_values, false) << std::endl;
    std::cout << "part1: " << process(actual_values, false) << std::endl;

    std::cout << "part2: " << process(test_values, true) << std::endl;
    std::cout << "part2: " << process(actual_values, true) << std::endl;
}
