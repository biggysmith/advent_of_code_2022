#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <numeric>
#include <assert.h>

std::string load_input(const std::string& file){
    std::ifstream fs(file);
    std::string line; 
    std::getline(fs, line);
    return line;
}

int find_marker(const std::string& data, int n)
{
    for(int pos=0; pos<data.size()-n; ++pos)
    {
        bool no_dups = true;
        int mx = std::min(pos+n,(int)data.size());

        for(int i=pos; i<mx && no_dups; ++i){
            for(int j=i+1; j<mx && no_dups; ++j){
                if(data[i] == data[j]){
                    no_dups = false;
                }
            }
        }

        if(no_dups){
            return pos+n;
        }
    }

    assert(false);
    return -1;
}


void main()
{
    auto actual_data = load_input("../src/day06/input.txt");

    std::cout << "part1: " << find_marker("bvwbjplbgvbhsrlpgdmjqwftvncz", 4) << std::endl;
    std::cout << "part1: " << find_marker("nppdvjthqldpwncqszvftbrmjlhg", 4) << std::endl;
    std::cout << "part1: " << find_marker("nznrnfrfntjfmvfwmzdfjlvtqnbhcprsg", 4) << std::endl;
    std::cout << "part1: " << find_marker("zcfzfwzzqfrljwzlrfnpqdbhtmscgvjw", 4) << std::endl;
    std::cout << "part1: " << find_marker(actual_data, 4) << std::endl;

    std::cout << "part2: " << find_marker("mjqjpqmgbljsphdztnvjfqwrcgsmlb", 14) << std::endl;
    std::cout << "part2: " << find_marker("bvwbjplbgvbhsrlpgdmjqwftvncz", 14) << std::endl;
    std::cout << "part2: " << find_marker("nppdvjthqldpwncqszvftbrmjlhg", 14) << std::endl;
    std::cout << "part2: " << find_marker("nznrnfrfntjfmvfwmzdfjlvtqnbhcprsg", 14) << std::endl;
    std::cout << "part2: " << find_marker("zcfzfwzzqfrljwzlrfnpqdbhtmscgvjw", 14) << std::endl;
    std::cout << "part2: " << find_marker(actual_data, 14) << std::endl;
}