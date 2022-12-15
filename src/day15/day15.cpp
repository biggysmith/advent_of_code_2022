#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <numeric>
#include <functional>
#include <optional>
#include <set>
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

struct interval_t{
    int begin = 0;
    int end = 0;
};

struct interval_set_t
{
    void insert(const interval_t& interval)
    {
        auto after = ranges.upper_bound(interval.begin);
        auto insert_range = after;

        if(after == ranges.begin() || std::prev(after)->second < interval.begin) {
            insert_range = ranges.insert(after, {interval.begin, interval.end+1}); // +1 since inclusive
        }else{
            insert_range = std::prev(after);
            if (insert_range->second >= interval.end+1) {
                return;
            }else{
                insert_range->second = interval.end+1;
            }   
        }   

        while(after != ranges.end() && interval.end+1 >= after->first) {
            insert_range->second = std::max(after->second, insert_range->second);
            after = ranges.erase(after);
        }   
    }

    std::map<int,int> ranges; // stupid std::set iterators are not mutable, so lets abuse map
};


std::optional<interval_t> row_sensor_intersection(const sensor_t& s, int row){
    int extent = manhatten(s.sensor, s.beacon) - std::abs(s.sensor.y - row);
    if(extent >= 0){
        return interval_t{ s.sensor.x - extent, s.sensor.x + extent };
    }else{
        return {};
    }
}

auto part1(const sensors_t& sensor_list, int row) 
{  
    interval_set_t intervals;

    for(auto& sen : sensor_list){
        if(auto in = row_sensor_intersection(sen, row)){
            intervals.insert(*in);
        }
    }

    int empty = 0;
    for(auto& [xlow,xhigh] : intervals.ranges){
        empty += xhigh - xlow;
    }

    std::set<int> beacons;
    for(auto& sen : sensor_list){
        if(sen.beacon.y == row){
            beacons.insert(sen.beacon.x);
        }
    }

    return empty - beacons.size();
}

auto part2(const sensors_t& sensor_list) 
{  
    int max_y = INT_MIN;
    for(auto& sen : sensor_list){
        max_y = std::max(sen.sensor.y, max_y);
    }

    for(int y=0; y<=max_y; ++y){
        interval_set_t intervals;

        for(auto& sen : sensor_list){
            if(auto in = row_sensor_intersection(sen, y)){
                intervals.insert(*in);
            }
        }

        if(intervals.ranges.size() > 1){
            for(auto& [xlow,xhigh] : intervals.ranges){
                return xhigh * 4'000'000ULL + y;
            }
        }
    }

    return 0ULL;
}

void main()
{
    auto test_values = load_input("../src/day15/test_input.txt");
    auto actual_values = load_input("../src/day15/input.txt");

    std::cout << "part1: " << part1(test_values, 10) << std::endl;
    std::cout << "part1: " << part1(actual_values, 2'000'000) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}