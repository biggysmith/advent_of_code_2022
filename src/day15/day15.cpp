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
#include "timer.hpp"

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
        auto after = intervals.upper_bound(interval.begin);
        auto insert_interval = after;

        if(after == intervals.begin() || std::prev(after)->second < interval.begin) {
            insert_interval = intervals.insert(after, {interval.begin, interval.end});
        }else{
            insert_interval = std::prev(after);
            if (insert_interval->second >= interval.end) {
                return;
            }else{
                insert_interval->second = interval.end;
            }   
        }   

        while(after != intervals.end() && interval.end >= after->first) {
            insert_interval->second = std::max(after->second, insert_interval->second);
            after = intervals.erase(after);
        }   
    }

    std::map<int,int> intervals; // stupid std::set iterators are not mutable, so lets abuse map
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
    interval_set_t interval_set;

    for(auto& sen : sensor_list){
        if(auto in = row_sensor_intersection(sen, row)){
            interval_set.insert(*in);
        }
    }

    int empty = 0;
    for(auto& [xlow,xhigh] : interval_set.intervals){
        empty += xhigh - xlow + 1;
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
    uint64_t result;
    volatile bool flag = false;

    #pragma omp parallel for
    for(int y=0; y<=4'000'000; ++y){
        if(flag){
            continue;
        }

        interval_set_t interval_set;

        for(auto& sen : sensor_list){
            if(auto in = row_sensor_intersection(sen, y)){
                interval_set.insert(*in);
            }
        }

        if(interval_set.intervals.size() > 1){
            for(auto& [xlow,xhigh] : interval_set.intervals){
                flag = true;
                result = (xhigh+1) * 4'000'000ULL + y; 
                break;
            }
        }
    }


    return result;
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