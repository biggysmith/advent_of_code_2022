#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <numeric>
#include <set>

struct motion_t {
    char dir;
    int steps;
};

using motions_t = std::vector<motion_t>;

motions_t load_input(const std::string& file){
    motions_t ret;
    std::ifstream fs(file);

    std::string line; 
    while (std::getline(fs, line)) {
        motion_t motion;
        sscanf_s(line.c_str(), "%c %d\n", &motion.dir, 1, &motion.steps);
        ret.push_back(motion);
    }

    return ret;
}

struct pos_t{
    int x;
    int y;
};

pos_t& operator+=(pos_t& a,const pos_t& b){
    a.x += b.x; a.y += b.y; return a;
}

bool operator<(const pos_t& a,const pos_t& b){
    return std::tuple(a.x,a.y) < std::tuple(b.x,b.y);
}

bool operator==(const pos_t& a,const pos_t& b){
    return std::tuple(a.x,a.y) == std::tuple(b.x,b.y);
}

auto process(const motions_t& motions, int num_knots) 
{  
    std::set<pos_t> visited;

    std::vector<pos_t> knots(num_knots, {0,0});

    pos_t& H = knots.front();
    pos_t& T = knots.back();

    for(auto& motion : motions){
        for(int s=0; s<motion.steps; ++s)
        {
            if(motion.dir == 'R'){
                H += {1, 0};
            }else if(motion.dir == 'D'){
                H += {0, 1};
            }else if(motion.dir == 'L'){
                H += {-1, 0};
            }else if(motion.dir == 'U'){
                H += {0, -1};
            }

            for(int k=1; k<knots.size(); ++k)
            {
                pos_t& K = knots[k];   // current knot
                pos_t& L = knots[k-1]; // last knot

                if(pos_t{ K.x+2, K.y } == L){ 
                    K += {1, 0};
                }else if(pos_t{ K.x-2, K.y } == L){
                    K += {-1, 0};
                }else if(pos_t{ K.x, K.y+2 } == L){
                    K += {0, 1};
                }else if(pos_t{ K.x, K.y-2 } == L){
                    K += {0, -1};
                }else if(pos_t{ K.x+2, K.y-1 } == L || pos_t{ K.x+1, K.y-2 } == L || pos_t{ K.x+2, K.y-2 } == L){
                    K += {1, -1};
                }else if(pos_t{ K.x-2, K.y-1 } == L || pos_t{ K.x-1, K.y-2 } == L || pos_t{ K.x-2, K.y-2 } == L){
                    K += {-1, -1};
                }else if(pos_t{ K.x+2, K.y+1 } == L || pos_t{ K.x+1, K.y+2 } == L || pos_t{ K.x+2, K.y+2 } == L){
                    K += {1, 1};
                }else if(pos_t{ K.x-2, K.y+1 } == L || pos_t{ K.x-1, K.y+2 } == L || pos_t{ K.x-2, K.y+2 } == L){
                    K += {-1, 1};
                }
            }

            visited.insert(T);
        }
    }

    return visited.size();
}

void main()
{
    auto test_values = load_input("../src/day09/test_input.txt");
    auto test_values2 = load_input("../src/day09/test_input2.txt");
    auto actual_values = load_input("../src/day09/input.txt");

    std::cout << "part1: " << process(test_values, 2) << std::endl;
    std::cout << "part1: " << process(actual_values, 2) << std::endl;

    std::cout << "part2: " << process(test_values, 10) << std::endl;
    std::cout << "part2: " << process(test_values2, 10) << std::endl;
    std::cout << "part2: " << process(actual_values, 10) << std::endl;
}