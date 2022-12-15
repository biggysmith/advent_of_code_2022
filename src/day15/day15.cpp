#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <numeric>
#include <functional>
#include <optional>
#include <unordered_set>
#include <map>
#include <assert.h>

struct pos_t{
    int x, y;
};

struct sensor_t{
    pos_t sensor;
    pos_t beacon;
};

using sensors_t = std::vector<sensor_t>;

sensors_t load_input(const std::string& file){
    sensors_t ret;
    std::ifstream fs(file);

    std::string line; 
    while (std::getline(fs, line)) {
        sensor_t s;
        sscanf_s(line.c_str(), "Sensor at x=%d, y=%d: closest beacon is at x=%d, y=%d\n", &s.sensor.x, &s.sensor.y, &s.beacon.x, &s.beacon.y);
        ret.push_back(s);
    }

    return ret;
}

int manhatten(const pos_t& a,const pos_t& b){
    return std::abs(a.x-b.x) + std::abs(a.y-b.y);
}

struct range_t{
    int low = 0;
    int high = 0;
};

bool operator<(const range_t& a, const range_t& b) {
    return std::make_tuple(a.low,a.high) < std::make_tuple(b.low,b.high);
}

struct interval_set_t
{
    void insert(const range_t& range)
    {
        auto after = ranges.upper_bound(range.low);
        auto insert_range = after;

        if(after == ranges.begin() || std::prev(after)->second < range.low) {
            insert_range = ranges.insert(after, {range.low,range.high+1}); // +1 since inclusive
        }else{
            insert_range = std::prev(after);
            if (insert_range->second >= range.high+1) {
                return;
            }else{
                insert_range->second = range.high+1;
            }   
        }   

        while(after != ranges.end() && range.high+1 >= after->first) {
            insert_range->second = std::max(after->second, insert_range->second);
            after = ranges.erase(after);
        }   
    }

    std::map<int,int> ranges; // stupid std::set iterators are not mutable, so lets abuse map
};


std::optional<range_t> row_sensor_intersection(const sensor_t& sen, int row){
    /*int vert_dist = std::abs(sen.sensor.y - row);
    int manhatten_dist = manhatten(sen.sensor, sen.beacon);
    int diff = manhatten_dist - vert_dist;*/
    
    int extent = manhatten(sen.sensor, sen.beacon) - std::abs(sen.sensor.y - row);
    if(extent >= 0){
        return range_t{ sen.sensor.x - extent, sen.sensor.x + extent };
    }else{
        return {};
    }
}

auto part1(const sensors_t& sensor_list, int row) 
{  
    interval_set_t intervals;

    for(auto& sen : sensor_list){
        auto in = row_sensor_intersection(sen, row);
        if(in){
            intervals.insert(*in);
        }
    }

    int empty = 0;
    for(auto& [low,high] : intervals.ranges){
        empty += high - low;
    }

    std::unordered_set<int> beacons;
    for(auto& sen : sensor_list){
        if(sen.beacon.y == row){
            beacons.insert(sen.beacon.x);
        }
    }

    return empty - beacons.size();
}

auto part2(const sensors_t& sensor_list) 
{  
    int min_y = INT_MAX;
    int max_y = INT_MIN;
    for(auto& sen : sensor_list){
        min_y = std::min(sen.sensor.y, min_y);
        max_y = std::max(sen.sensor.y, max_y);
    }

    for(int y=min_y; y<=max_y; ++y){
        interval_set_t intervals;

        for(auto& sen : sensor_list){
            auto in = row_sensor_intersection(sen, y);
            if(in){
                intervals.insert(*in);
            }
        }

        if(intervals.ranges.size() > 1){
            for(auto& [low,high] : intervals.ranges){
                return high * 4'000'000ULL + y;
            }
        }
    }

    return 0ULL;
}

void main()
{
    auto test_values = load_input("../src/day15/test_input.txt");
    auto actual_values = load_input("../src/day15/input.txt");

    /*std::cout << "part1: " << part1(test_values, 10) << std::endl;
    std::cout << "part1: " << part1(actual_values, 2'000'000) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;*/

    if(!(part1(test_values, 10) == 26)){
        std::cout << "FAIL0" << std::endl;
    }

    if(!(part1(actual_values, 2'000'000) == 5144286)){
        std::cout << "FAIL1" << std::endl;
    }

    if(!(part2(test_values) == 56000011)){
        std::cout << "FAIL2" << std::endl;
    }

    if(!(part2(actual_values) == 10229191267339)){
        std::cout << "FAIL3" << std::endl;
    }
    /*assert(part1(actual_values, 2'000'000) == 5144286);

    assert(part2(test_values) == 56000011);
    assert(part2(actual_values) == 10229191267339);*/

    std::cout << "------------------------------PASS----------------------------" << std::endl;
}