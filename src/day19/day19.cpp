#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <numeric>
#include <functional>

struct resources_t{
    int ore = 0;
    int clay = 0;
    int obsidian = 0;
    int geode = 0;
};

struct blueprint_t{
    int id;
    resources_t ore_bot_cost;
    resources_t clay_bot_cost;
    resources_t obsidian_bot_cost;
    resources_t geode_bot_cost;
    int max_ore_bots;
    int max_clay_bots;
    int max_obsidian_bots;
};

using blueprints_t = std::vector<blueprint_t>;

blueprints_t load_input(const std::string& file){
    blueprints_t ret;
    std::ifstream fs(file);

    std::string line; 
    while(std::getline(fs, line)){
        blueprint_t bp;
        sscanf_s(
            line.c_str(), 
            "Blueprint %d: Each ore robot costs %d ore. Each clay robot costs %d ore. Each obsidian robot costs %d ore and %d clay. Each geode robot costs %d ore and %d obsidian.", 
            &bp.id, &bp.ore_bot_cost.ore, &bp.clay_bot_cost.ore, &bp.obsidian_bot_cost.ore, &bp.obsidian_bot_cost.clay, &bp.geode_bot_cost.ore, &bp.geode_bot_cost.obsidian
        );
        bp.max_ore_bots = std::max(bp.ore_bot_cost.ore, std::max(bp.clay_bot_cost.ore, std::max(bp.obsidian_bot_cost.ore, bp.geode_bot_cost.ore)));
        bp.max_clay_bots = bp.obsidian_bot_cost.clay;
        bp.max_obsidian_bots = bp.geode_bot_cost.obsidian;
        ret.push_back(bp);
    }
    return ret;
}

enum moves_enum { e_build_ore_bot=0, e_build_clay_bot, e_build_obsidian_bot, e_build_geode_bot, e_build_none };

struct state_t{
    resources_t resources;
    int ore_bots = 1;
    int clay_bots = 0;
    int obsidian_bots = 0;
    int geode_bots = 0;
    int min = 0;
};

bool can_build(const state_t& state,const resources_t& cost){
    return state.resources.ore >= cost.ore && state.resources.clay >= cost.clay && state.resources.obsidian >= cost.obsidian;
}

state_t update(const state_t& state, moves_enum move, const resources_t& cost=resources_t()){
    state_t new_state = state;
    new_state.resources.ore += state.ore_bots - cost.ore;
    new_state.resources.clay += state.clay_bots - cost.clay;
    new_state.resources.obsidian += state.obsidian_bots - cost.obsidian;
    new_state.resources.geode += state.geode_bots - cost.geode;
    switch(move){
        case e_build_ore_bot:       new_state.ore_bots++;           break;
        case e_build_clay_bot:      new_state.clay_bots++;          break;
        case e_build_obsidian_bot:  new_state.obsidian_bots++;      break;
        case e_build_geode_bot:     new_state.geode_bots++;         break;
    };
    new_state.min++;
    return new_state;
}

void dfs(const blueprint_t& blueprint, state_t state, moves_enum move, int limit, int& max_geodes)
{
    // no point in building more lesser bots than needed, we only want enough to build a new geode bot every min
    if(move==e_build_ore_bot && state.ore_bots >= blueprint.max_ore_bots){
        return;
    }else if(move==e_build_clay_bot && state.clay_bots >= blueprint.max_clay_bots){
        return;
    }else if(move==e_build_obsidian_bot && state.obsidian_bots >= blueprint.max_obsidian_bots){
        return;
    }

    // if we were to build geode bots for the remaining time and still have less than max_geodes then we ok to skip
    auto integer_seq_sum = [](int a,int b){ return (b-a)*(a+b)/2; };
    int time_remaining = limit-state.min;
    if(state.resources.geode + integer_seq_sum(state.geode_bots, state.geode_bots+time_remaining) <= max_geodes){
        return;
    }

    while(state.min < limit)
    {
        auto build_next_bot = [&](const resources_t& cost){
            if(can_build(state, cost)){
                for(moves_enum next_move : {e_build_ore_bot, e_build_clay_bot, e_build_obsidian_bot, e_build_geode_bot}){
                    state_t new_state = update(state, move, cost);
                    dfs(blueprint, new_state, next_move, limit, max_geodes);
                }
                return true;
            }
            return false;
        };

        if(move==e_build_ore_bot && build_next_bot(blueprint.ore_bot_cost)){
            return;
        }else if(move==e_build_clay_bot && build_next_bot(blueprint.clay_bot_cost)){
            return;
        }else if(move==e_build_obsidian_bot && build_next_bot(blueprint.obsidian_bot_cost)){
            return;
        }else if(move==e_build_geode_bot && build_next_bot(blueprint.geode_bot_cost)){
            return;
        }

        state = update(state, e_build_none);
    }
    
    max_geodes = std::max(state.resources.geode, max_geodes);
}


auto part1(const blueprints_t& blueprints)
{
    int quality = 0;
    for(int i=0; i<blueprints.size(); ++i){
        state_t state;
        int max_geode = 0;
        for(moves_enum move : {e_build_ore_bot, e_build_clay_bot, e_build_obsidian_bot, e_build_geode_bot}){
            dfs(blueprints[i], state, move, 24, max_geode);
        }
        quality += blueprints[i].id * max_geode;
    }

    return quality;
}

auto part2(const blueprints_t& blueprints)
{
    int product = 1;
    for(int i=0; i<3; ++i){
        state_t state;
        int max_geode = 0;
        for(moves_enum move : {e_build_ore_bot, e_build_clay_bot, e_build_obsidian_bot, e_build_geode_bot}){
            dfs(blueprints[i], state, move, 32, max_geode);
        }
        product *= max_geode;
    }

    return product;
}

void main()
{
    auto test_values = load_input("../src/day19/test_input.txt");
    auto actual_values = load_input("../src/day19/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(actual_values) << std::endl;
}
