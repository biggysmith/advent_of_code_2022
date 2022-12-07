#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <numeric>
#include <set>
#include <map>

enum file_type_enum { e_dir=0, e_file };

using instructions_t = std::vector<std::string>;

instructions_t load_input(const std::string& file){
    instructions_t ret;
    std::ifstream fs(file);

    std::string line; 
    while (std::getline(fs, line)) {
        ret.push_back(line);
    }

    return ret;
}

struct node_t {
    std::string name;
    file_type_enum type;
    int size;

    node_t* parent_dir = nullptr;
    std::map<std::string,node_t> nodes;
};

node_t fill_tree(const instructions_t& instructions)
{
    node_t root;
    node_t* current = &root;
    std::string cwd = "";
        
    for(auto& in : instructions){
        if(in[0] == '$'){
            if(in.substr(2,2) == "cd"){
                std::string dir = in.substr(5);
                if(dir == ".."){
                    cwd = cwd.substr(0,cwd.find_last_of("/"));
                    current = current->parent_dir;
                }else{
                    cwd += "/" + dir;
                    current->nodes[cwd] = { cwd, e_dir, 0, current };
                    current = &current->nodes[cwd];
                }
            }
        }else if(isdigit(in[0])){
            int file_size = std::stoi(in.substr(0,in.find_first_of(' ')));
            std::string file_name = cwd + "/" + in.substr(in.find_first_of(' ')+1);

            current->nodes[file_name] = { file_name, e_file, file_size, current };
        }
    }

    return root;
}

int calculate_dir_size(const node_t& dir, std::map<std::string,int>& dir_sizes){
    int dir_size = 0;

    for(auto& [name,node] : dir.nodes){
        if(node.type == e_file){
            dir_size += node.size;
        }else{
            dir_sizes[node.name] = calculate_dir_size(node, dir_sizes);
            dir_size += dir_sizes[node.name];
        }
    }

    return dir_size;
}

auto part1(const instructions_t& instructions) 
{
    node_t root = fill_tree(instructions);

    std::map<std::string,int> dir_size_map;
    calculate_dir_size(root, dir_size_map);

    int sum = 0;
    for(auto& [name,size] : dir_size_map){
        if(size <= 100000){
            sum += size;
        }
    }

    return sum;
} 

auto part2(const instructions_t& instructions) 
{  
    node_t root = fill_tree(instructions);

    int total_space = 70000000;
    int needed_space = 30000000;

    std::map<std::string,int> dir_size_map;
    int space_used = calculate_dir_size(root, dir_size_map);

    int unused_space = total_space - space_used;

    std::set<int> dir_sizes;
    for(auto& [name,size] : dir_size_map){
        dir_sizes.insert(size);
    }

    int smallest_dir_size = 0;
    for(auto& size : dir_sizes){
        int updated_unused_space = unused_space + size;
        if(updated_unused_space >= needed_space){
            smallest_dir_size = size;
            break;
        }
    }

    return smallest_dir_size;
}

void main()
{
    auto test_values = load_input("../src/day07/test_input.txt");
    auto actual_values = load_input("../src/day07/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}