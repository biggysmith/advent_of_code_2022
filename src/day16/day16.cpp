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
#include <omp.h>

struct valve_t{
    std::string name;
    int flow_rate;
    std::set<std::string> tunnels;
};

using valves_t = std::map<std::string,valve_t>;

valves_t load_input(const std::string& file){
    valves_t ret;
    std::ifstream fs(file);

    std::string line; 
    while (std::getline(fs, line)) {
        valve_t valve;

        std::string src = line.substr(6, 2);

        auto s = line.find_first_of('='); 
        auto e = line.find_first_of(';');
        valve.flow_rate = std::stoi(line.substr(s+1, e-s));

        s = line.find_last_of("valve");
        if(line[s+1] == 's'){
            s++;
        }

        line = line.substr(s+1);
        std::stringstream ss(line); std::string token;
        while (std::getline(ss, token, ',')) {
            valve.tunnels.insert(token.substr(1));
        }

        valve.name = src;
        ret[src] = valve;
    }

    return ret;
}


int flow(const valves_t& valves, const std::set<std::string>& opened){
    int sum = 0;
    for(auto& v : opened){
        sum += valves.at(v).flow_rate;
    }
    return sum;
}

using dist_graph_t = std::map<std::string, std::map<std::string, int>>;

void dfs(const valves_t& valves, const dist_graph_t& graph, const std::string& node, std::set<std::string>& opened, int min, int pressure, int num_important_valves, int limit, int& max_pressure)
{
    if(min == limit)
    {
        max_pressure = std::max(pressure, max_pressure);
    }
    else
    {
        for(auto& [name, dist] : graph.at(node))
        {
            if(!opened.count(name) && dist!=0 && valves.at(name).flow_rate){
                if(min + dist + 1 >= limit){ // hit the time limit and won't be able to open next valve
                    int new_pressure = pressure + (limit-min) * flow(valves, opened);
                    dfs(valves, graph, name, opened, limit, new_pressure, num_important_valves, limit, max_pressure);
                }else{
                    int new_pressure = pressure + (dist+1) * flow(valves, opened);
                    opened.insert(name); // open next valve
                    dfs(valves, graph, name, opened, min+dist+1, new_pressure, num_important_valves, limit, max_pressure);
                    opened.erase(name);
                }
            }

            if(num_important_valves == opened.size()){
                int new_pressure = pressure + (limit-min) * flow(valves, opened);
                dfs(valves, graph, name, opened, limit, new_pressure, num_important_valves, limit, max_pressure);
            }
        }
    }
}

dist_graph_t floyd_warshall(const valves_t& valves)
{
    dist_graph_t dist;

    for(auto& [valve0, valves0] : valves){
        for(auto& [valve1, _] : valves){       
            if(valve0 == valve1){ // distance to self is zero
                dist[valve0][valve1] = 0;
            }else if(valves0.tunnels.count(valve1)){ // can move
                dist[valve0][valve1] = 1;
            }else{
                dist[valve0][valve1] = (int)valves.size()+1; // can't be longer than this
            }
        }
    }

    for(auto& [k, _] : valves){
        for(auto& [i, _] : valves){
            for(auto& [j, _] : valves){
                if (dist[i][j] > dist[i][k] + dist[k][j]) {
					dist[i][j] = dist[i][k] + dist[k][j];
				}
            }
        }
    }

    return dist;
};

int count_important_valves(const valves_t& valves){
    int num_important_valves = 0;
    for(auto& [_, valve] : valves){
        if(valve.flow_rate){
            num_important_valves++;
        }
    }
    return num_important_valves;
}

auto part1(const valves_t& valves) 
{
    dist_graph_t dist = floyd_warshall(valves);

    int max_pressure = 0;
    std::set<std::string> opened;
    dfs(valves, dist, "AA", opened, 0, 0, count_important_valves(valves), 30, max_pressure);

    return max_pressure;
}

struct route_t{
    int pressure;
    uint32_t opened = 0;
};

void dfs2(const valves_t& valves, const dist_graph_t& graph, const std::string& node, std::set<std::string>& opened, int min, int pressure, int num_important_valves, int limit, const std::map<std::string,int>& index_map, std::vector<route_t>& routes)
{
    if(min == limit)
    {
        uint32_t iroute = 0;
        for(auto& v : opened){
            iroute |= (1 << index_map.at(v)); 
        }      
        routes.push_back({pressure,iroute}); 
    }
    else
    {
        for(auto& [name, dist] : graph.at(node))
        {
            if(!opened.count(name) && dist!=0 && valves.at(name).flow_rate){
                if(min + dist + 1 >= limit){ // hit the time limit and won't be able to open next valve
                    int new_pressure = pressure + (limit-min) * flow(valves, opened);
                    dfs2(valves, graph, name, opened, limit, new_pressure, num_important_valves, limit, index_map, routes);
                }else{
                    int new_pressure = pressure + (dist+1) * flow(valves, opened);
                    opened.insert(name); // open next valve
                    dfs2(valves, graph, name, opened, min+dist+1, new_pressure, num_important_valves, limit, index_map, routes);
                    opened.erase(name);
                }
            }

            if(num_important_valves == opened.size()){
                int new_pressure = pressure + (limit-min) * flow(valves, opened);
                dfs2(valves, graph, name, opened, limit, new_pressure, num_important_valves, limit, index_map, routes);
            }
        }
    }
}

bool is_disjoint(const route_t& a, const route_t& b){
    return (a.opened & b.opened) == 0;
}

auto part2(const valves_t& valves) 
{  
    int max_path = part1(valves);

    int idx = 0;
    std::map<std::string,int> index_map;
    for(auto& [name,valve] : valves){
        if(valve.flow_rate){
            index_map[name] = idx++;
        }
    }

    dist_graph_t dist = floyd_warshall(valves);

    std::vector<route_t> routes;
    std::set<std::string> opened;
    dfs2(valves, dist, "AA", opened, 0, 0, count_important_valves(valves), 26, index_map, routes);

    std::vector<route_t> routes2;
    for(auto& r : routes){
        if(r.pressure > max_path/2){
            routes2.push_back(r);
        }
    }

    std::vector<int> maxes(omp_get_max_threads(), 0);

    #pragma omp parallel for
    for(int i=0; i<routes2.size(); ++i){
        for(int j=i+1; j<routes2.size(); ++j){
            if(is_disjoint(routes2[i], routes2[j])){
                maxes[omp_get_thread_num()] = std::max(maxes[omp_get_thread_num()], routes2[i].pressure + routes2[j].pressure);
            }
        }
    }

    int max_pressure = 0;
    for(auto thread_max : maxes){
        max_pressure = std::max(max_pressure, thread_max);
    }

    return max_pressure;
}

void main()
{
    auto test_values = load_input("../src/day16/test_input.txt");
    auto actual_values = load_input("../src/day16/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}