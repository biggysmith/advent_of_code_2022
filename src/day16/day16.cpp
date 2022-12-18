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
#include <queue>

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

void dfs(const valves_t& valves, const dist_graph_t& graph, const std::string& node, std::set<std::string>& opened, int min, int pressure, int num_valves, int& max_pressure)
{
    if(min == 30)
    {
        if(pressure > max_pressure){
            max_pressure = std::max(pressure, max_pressure);
        }
    }
    else
    {
        for(auto& [name, dist] : graph.at(node)){
            if(!opened.count(name) && dist!=0 && valves.at(name).flow_rate){
                if(min + dist + 1 >= 30){
                    int new_pressure = pressure+(30-min)*flow(valves, opened);
                    dfs(valves, graph, name, opened, 30, new_pressure, num_valves, max_pressure);
                }else{
                    int new_pressure = pressure+(dist+1)*flow(valves, opened);
                    auto opened2 = opened;
                    opened2.insert(name);
                    dfs(valves, graph, name, opened2, min+dist+1, new_pressure, num_valves, max_pressure);
                }
            }

            if(opened.size() == num_valves){
                int flow_sum = flow(valves, opened);
                int new_pressure = pressure+(30-min)*flow_sum;
                dfs(valves, graph, name, opened, 30, new_pressure, num_valves, max_pressure);
            }
        }
    }
}

auto part1(const valves_t& valves) 
{
    dist_graph_t graph;

    for(auto& [name1,v1] : valves){
        for(auto& [name2,v2] : valves){
        
            if(name1 == name2){
                graph[name1][name2] = 0;
            }else if(v1.tunnels.count(name2)){
                graph[name1][name2] = 1;
            }else{
                graph[name1][name2] = 65535;
            }

        }
    }

    for(auto& [k,v1] : valves){
        for(auto& [i,v2] : valves){
            for(auto& [j,v3] : valves){
                if (graph[i][j] > graph[i][k] + graph[k][j]) {
					graph[i][j] = graph[i][k] + graph[k][j];
				}
            }
        }
    }

    int num_valves = 0;
    for(auto& [name,valve] : valves){
        if(valve.flow_rate){
            num_valves++;
        }
    }

    int max_pressure = 0;
    std::set<std::string> opened;
    dfs(valves, graph, "AA", opened, 0, 0, num_valves, max_pressure);

    return max_pressure;
}

auto part2(const valves_t& valves) 
{  
    return 0;
}

void main()
{
    auto test_values = load_input("../src/day16/test_input.txt");
    auto actual_values = load_input("../src/day16/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    //std::cout << "part2: " << part2(test_values) << std::endl;
    //std::cout << "part2: " << part2(actual_values) << std::endl;
}