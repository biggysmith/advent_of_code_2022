#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <numeric>
#include <set>
#include <assert.h>

using rucksacks_t = std::vector<std::string>;

rucksacks_t load_input(const std::string& file){
    rucksacks_t ret;
    std::ifstream fs(file);
    std::string line;
    while (std::getline(fs, line)) {
        ret.push_back(line);
    }
    return ret;
}

int item_priority(char a){
    return a>=97 ? a-96 : a-65+27;
}

char find_common_item(const std::string& a,const std::string& b){
    std::set<char> set_a(a.begin(), a.end());
    std::set<char> set_b(b.begin(), b.end());

    std::vector<char> intersection;
    std::set_intersection(set_a.begin(), set_a.end(), set_b.begin(), set_b.end(), std::back_inserter(intersection));

    assert(intersection.size() == 1);
    return intersection.front();
}

int part1(const rucksacks_t& rucksacks) {
    return std::accumulate(rucksacks.begin(), rucksacks.end(), 0, [](auto& sum, auto& r){
          return sum + item_priority(find_common_item(r.substr(0,r.size()/2), r.substr(r.size()/2,r.size())));
    });
}


char find_common_item(const std::string& a,const std::string& b,const std::string& c){
    std::set<char> set_a(a.begin(), a.end());
    std::set<char> set_b(b.begin(), b.end());
    std::set<char> set_c(c.begin(), c.end());

    std::vector<char> intersection_0;
    std::vector<char> intersection_1;
    std::set_intersection(set_a.begin(), set_a.end(), set_b.begin(), set_b.end(), std::back_inserter(intersection_0));
    std::set_intersection(intersection_0.begin(), intersection_0.end(), set_c.begin(), set_c.end(), std::back_inserter(intersection_1));

    assert(intersection_1.size() == 1);
    return intersection_1.front();
}

int part2(const rucksacks_t& rucksacks) {  
    int sum = 0;
    for(int r=0; r<rucksacks.size(); r+=3){
        sum += item_priority(find_common_item(rucksacks[r+0], rucksacks[r+1], rucksacks[r+2]));
    }
    return sum;
}

void main()
{
    auto test_values = load_input("../src/day03/test_input.txt");
    auto actual_values = load_input("../src/day03/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}