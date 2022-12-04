#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <numeric>
#include <set>
#include <assert.h>

struct interval_t {
    int begin;
    int end;
};

struct assignment_t {
    interval_t elf1;
    interval_t elf2;
};

bool is_subset(const interval_t& a, const interval_t& b) {
    return a.begin >= b.begin && a.end <= b.end;
}

bool is_subset(const assignment_t& a) {
    return is_subset(a.elf1,a.elf2) || is_subset(a.elf2,a.elf1);
}

bool is_overlapping(const interval_t& a, const interval_t& b) {
    return !(b.begin > a.end || a.begin > b.end);
}

bool is_overlapping(const assignment_t& a) {
    return is_overlapping(a.elf1,a.elf2);
}

using assignments_t = std::vector<assignment_t>;

assignments_t load_input(const std::string& file){
    assignments_t ret;
    std::ifstream fs(file);
    std::string line;
    while (std::getline(fs, line)) {
        assignment_t ass;
        sscanf_s(line.c_str(), "%d-%d,%d-%d\n", &ass.elf1.begin, &ass.elf1.end, &ass.elf2.begin, &ass.elf2.end); 
        ret.push_back(ass);
    }
    return ret;
}

int part1(const assignments_t& assignments) {
    return std::count_if(assignments.begin(), assignments.end(), [](auto& a){
        return is_subset(a);
    });
}

int64_t part2(const assignments_t& assignments) {  
    return std::count_if(assignments.begin(), assignments.end(), [](auto& a){
        return is_overlapping(a);
    });
}

void main()
{
    auto test_values = load_input("../src/day04/test_input.txt");
    auto actual_values = load_input("../src/day04/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}