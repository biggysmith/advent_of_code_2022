#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <numeric>
#include <functional>
#include <map>

struct monkey_t{
    std::string name;
    std::string left;
    std::string right;
    char symbol = ' ';
    int64_t value = 0;
};

using monkeys_t = std::map<std::string,monkey_t>;

monkeys_t load_input(const std::string& file){
    monkeys_t ret;
    std::ifstream fs(file);

    std::string line;
    while(std::getline(fs, line)){
        monkey_t monkey;
        monkey.name = line.substr(0,4);
        if(isdigit(line[6])){
            monkey.value = std::stoi(line.substr(5));
        }else{
            monkey.left = line.substr(6,4);
            monkey.symbol = line.substr(11,1).front();
            monkey.right = line.substr(13,4);
        }
        ret[monkey.name] = monkey;
    }
    return ret;
}

struct node_t{
    char symbol = ' ';
    int64_t value = 0;
    bool flagged = false;
    node_t* left = nullptr;
    node_t* right = nullptr;
};

using tree_t = node_t*;

bool is_operator(char op){
    return op=='+' || op=='-' || op=='*' || op=='/';
}

void construct(tree_t& tree, const std::string& monkey_name, const monkeys_t& monkeys) {
    auto& monkey = monkeys.at(monkey_name);
    if(is_operator(monkey.symbol)){
        tree = new node_t;
        tree->symbol = monkey.symbol;
        construct(tree->left, monkey.left, monkeys);
        construct(tree->right, monkey.right, monkeys);
    }else{
        tree = new node_t;
        tree->value = monkey.value;
        if(monkey_name == "humn"){
            tree->symbol = 'h'; 
        }
    }
}

int64_t monkey_op(int64_t left, char op, int64_t right){
    switch(op){
        case '+':   return left + right;;
        case '-':   return left - right;;
        case '*':   return left * right;;
        case '/':   return left / right;;
        default:    return op;
    };
}

int64_t evaluate(tree_t& tree) {
    if(is_operator(tree->symbol)){
        return monkey_op(evaluate(tree->left), tree->symbol, evaluate(tree->right));
    }else{
        return tree->value;
    }
}

char opposite_left(char op){
    switch(op){
        case '+':   return '-';
        case '-':   return '+';
        case '*':   return '/';
        case '/':   return '*';
        default:    return op;
    };
}

char opposite_right(char op){
    switch(op){
        case '+':   return '-';
        case '-':   return '-';
        case '*':   return '/';
        case '/':   return '/';
        default:    return op;
    };
}

std::ostream& operator<<(std::ostream& os, tree_t tree) {
    if (tree) {
        if(!is_operator(tree->symbol)) {
            os << tree->value;
        }else {
            os << "(" << tree->left << tree->symbol << tree->right << ")";            
        }
    }
    return os;
}

bool has_symbol(tree_t& tree, char symbol){
    if(!tree){
        return false;
    }
    if(tree->symbol == symbol){
        return true;
    }
    if(has_symbol(tree->left, symbol) || has_symbol(tree->right, symbol)){
        return true;
    }
    return false;
}

void rearrange_for_symbol(tree_t& tree, char symbol, tree_t& new_node){
    if(tree){
        rearrange_for_symbol(tree->left, symbol, new_node);
        rearrange_for_symbol(tree->right, symbol, new_node);

        tree->flagged = tree->symbol == symbol || (tree->left && tree->left->flagged) || (tree->right && tree->right->flagged);
        if(tree->flagged){
            new_node->right = new node_t;
            new_node->left = new node_t;

            if((tree->left && tree->left->flagged)){
                new_node->symbol = opposite_left(tree->symbol);
                new_node->right = tree->right;
                new_node = new_node->left;
            }else if((tree->right && tree->right->flagged)){
                // gotta be careful with ops arrangement here when on right
                new_node->symbol = opposite_right(tree->symbol);
                if(tree->symbol == '+' || tree->symbol == '*'){
                    new_node->right = tree->left;
                    new_node = new_node->left;
                }else{
                    new_node->left = tree->left;
                    new_node = new_node->right;
                }
            }
        }
    }
}

std::pair<tree_t,tree_t> rearrange_for_symbol(tree_t& tree, char symbol){
    tree_t new_tree = new node_t;
    tree_t symbol_node = new_tree;
    rearrange_for_symbol(tree, symbol, symbol_node);
    return { new_tree, symbol_node };
}

auto part1(monkeys_t monkeys) 
{
    tree_t tree = nullptr;
    construct(tree, "root", monkeys);

    return evaluate(tree);
}

auto part2(monkeys_t monkeys)
{
    tree_t tree = nullptr;
    construct(tree, "root", monkeys);

    bool symbol_on_left = has_symbol(tree->left, 'h');

    auto [new_tree, symbol_node] = rearrange_for_symbol(symbol_on_left ? tree->left : tree->right, 'h');
    symbol_node->value = evaluate(symbol_on_left ? tree->right : tree->left);
    return evaluate(new_tree);
}

void main()
{
    auto test_values = load_input("../src/day21/test_input.txt");
    auto actual_values = load_input("../src/day21/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}