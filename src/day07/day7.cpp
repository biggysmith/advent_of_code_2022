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

bool is_cmd(const std::string& in){
    return in[0] == '$';
}

bool is_cd(const std::string& in){
    return in.substr(2,2) == "cd";
}

bool is_file(const std::string& in){
    return isdigit(in[0]);
}

std::string get_dir(const std::string& in){
    return in.substr(5);
}

std::string move_out(const std::string& cwd){
    return cwd.substr(0,cwd.find_last_of("/"));
}

std::string move_in(const std::string& cwd,const std::string& dir){
    return cwd + "/" + dir;
}

std::string file_name(const std::string& in){
    return in.substr(in.find_first_of(' ')+1);
}

int file_size(const std::string& in){
    return std::stoi(in.substr(0,in.find_first_of(' ')));
}

node_t fill_tree(const instructions_t& instructions)
{
    node_t root;
    node_t* current = &root;
    std::string cwd = "";
        
    for(auto& in : instructions){
        if(is_cmd(in)){
            if(is_cd(in)){
                std::string dir = get_dir(in);
                if(dir == ".."){
                    cwd = move_out(cwd);
                    current = current->parent_dir;
                }else{
                    cwd = move_in(cwd, dir);
                    current->nodes[cwd] = { cwd, e_dir, 0, current };
                    current = &current->nodes[cwd];
                }
            }
        }else if(is_file(in)){
            std::string name = cwd + "/" + file_name(in);
            current->nodes[name] = { name, e_file, file_size(in), current };
        }
    }

    return root;
}

int calculate_dir_size(const node_t& dir, std::map<std::string,int>& dir_sizes)
{
    int size = 0;

    for(auto& [name,node] : dir.nodes){
        if(node.type == e_file){
            size += node.size;
        }else{
            dir_sizes[node.name] = calculate_dir_size(node, dir_sizes);
            size += dir_sizes[node.name];
        }
    }

    return size;
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

    std::set<int> dir_sizes;
    for(auto& [name,size] : dir_size_map){
        dir_sizes.insert(size);
    }

    int space_we_need_to_delete = space_used - (total_space - needed_space);
    return *std::lower_bound(dir_sizes.begin(), dir_sizes.end(), space_we_need_to_delete);
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