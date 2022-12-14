#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <numeric>
#include <functional>
#include <stack>
#include <variant>

struct packet_t : std::vector<std::variant<int,packet_t>>{ 
    using std::vector<std::variant<int,packet_t>>::vector;
};

using packets_t = std::vector<packet_t>;

packet_t parse_line(const std::string& line){
    packet_t packets;
    std::stack<packet_t*> ptr_stack;
    ptr_stack.push(&packets);

    for(int i = 0; i < line.size(); i++){
        const char c = line[i];
        if(c == '['){
            ptr_stack.top()->push_back(packet_t());
            ptr_stack.push(&std::get<packet_t>(ptr_stack.top()->back()));
        }else if(c == ']'){
            ptr_stack.pop();
        }else if(c == ','){

        }else{
            if(line[i+1] == ',' || line[i+1] == ']'){
                ptr_stack.top()->push_back(std::stoi(line.substr(i,1)));
            }else if(line[i+2] == ',' || line[i+2] == ']'){
                ptr_stack.top()->push_back(std::stoi(line.substr(i,2)));
                i++;
            }
        }
    }

    return packets;
}

packets_t load_input(const std::string& file){
    packets_t ret;
    std::ifstream fs(file);

    std::string line; 
    while (std::getline(fs, line)) {
        if(line != ""){
            ret.push_back(parse_line(line));
        }
    }

    return ret;
}

template<typename... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<typename... Ts> overloaded(Ts...) -> overloaded<Ts...>;

enum result_enum { e_wrong_order=0, e_correct_order, e_equal_order };

result_enum compare_packets(const packet_t& a, const packet_t& b)
{
    result_enum ret = e_equal_order;
    for(int i=0; i<std::max(a.size(),b.size()) && ret==e_equal_order; ++i){
        if(i == a.size()){
            return e_correct_order;
        }else if(i == b.size()){
            return e_wrong_order;
        }else{
            ret = std::visit(overloaded{
                [](const packet_t& a, const packet_t& b) { return compare_packets(a, b); },
                [](const packet_t& a, int b) { return compare_packets(a, {b}); },
                [](int a, const packet_t& b) { return compare_packets({a}, b); },
                [](int a, int b) {
                    if(a < b){
                        return e_correct_order;
                    }else if(a > b){
                        return e_wrong_order;
                    }else{
                        return e_equal_order;
                    }
                }
            }, a[i], b[i]);
        }
    }

    return ret;
}


auto part1(const packets_t& packets) 
{  
    int sum = 0;
    for(int i=0; i<packets.size(); i+=2){
        if(compare_packets(packets[i], packets[i+1])){
            sum += i/2 + 1;
        }
    }
    return sum;
}

auto part2(packets_t packets) 
{  
    packet_t two = parse_line("[[2]]");
    packet_t six = parse_line("[[6]]");

    packets.push_back(two);
    packets.push_back(six);

    std::sort(packets.begin(), packets.end(), [](auto& a,auto& b){
        return compare_packets(a,b);
    });

    auto get_index = [&](const packet_t& packet){
        return std::distance(packets.begin(), std::find(packets.begin(), packets.end(), packet)) + 1;
    };

    return get_index(two) * get_index(six);
}

void main()
{
    auto test_values = load_input("../src/day13/test_input.txt");
    auto actual_values = load_input("../src/day13/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}