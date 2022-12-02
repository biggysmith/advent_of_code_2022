#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <numeric>

struct round_t{
    int a;
    int b;
};

using rounds_t = std::vector<round_t>;

rounds_t load_input(const std::string& file){
    rounds_t ret;
    std::ifstream fs(file);
    std::string line;
    while (std::getline(fs, line)) {
        ret.push_back(round_t{line[0]-65, line[2]-88});
    }
    return ret;
}

int modulo(int x,int y){
    return (x % y + y) % y;
}

int score(const round_t& r){
    int points[] = { 0, 6, 3 }; 
    return (r.b+1) + (points[r.a==r.b ? 2 : r.a==modulo(r.b-1,3)]);
}

int part1(const rounds_t& rounds) {
    return std::accumulate(rounds.begin(), rounds.end(), 0, [](auto& sum,auto& r){
        return sum + score(r);  
    });
}

int part2(const rounds_t& rounds) {  
    return std::accumulate(rounds.begin(), rounds.end(), 0, [](auto& sum,auto& r){
        int choice[] = { -1, 0, 1 }; 
        return sum + score({r.a, modulo(r.a + choice[r.b], 3)});  
    });
}

void main()
{
    auto test_values = load_input("../src/day02/test_input.txt");
    auto actual_values = load_input("../src/day02/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}