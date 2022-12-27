#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <numeric>
#include <functional>
#include <regex>
#include <map>

enum turn_enum { e_turn_right=-2, e_turn_left=-1 };
enum dir_enum { e_right=0, e_down, e_left, e_up };
enum face_enum { e_top_face=0, e_bottom_face, e_front_face, e_back_face, e_left_face, e_right_face };

struct pos_t{
    int x,y;
};

pos_t operator+(const pos_t&a, const pos_t& b){
    return pos_t{a.x+b.x, a.y+b.y};
}

template<typename T>
struct grid_t{
    std::vector<T> data;
    int width = 0;
    int height = 0;

    grid_t() {}
    grid_t(int w,int h,const T& val=T()) : data(w*h,val), width(w), height(h) {} 

    decltype(auto) operator()(int x,int y){
        return data[y*width + x];
    }

    decltype(auto) operator()(int x,int y) const{
        return data[y*width + x];
    }
};

using path_t = std::vector<int>;
using map_t = grid_t<char>;

struct note_t{
    map_t map;
    path_t path;
    int dims;
};

note_t load_input(const std::string& file, int dims)
{
    note_t ret;

    std::vector<std::string> lines;
    std::ifstream fs(file);
    std::string line;
    while (std::getline(fs, line)) {
        if (line.empty()) {
            break;
        }
        lines.push_back(line);
    }

    std::getline(fs, line);
    std::regex word_regex("\\d+|\\D+");
    for(auto it=std::sregex_iterator(line.begin(), line.end(), word_regex); it!=std::sregex_iterator(); it++){
        if(it->str() == "L"){
            ret.path.push_back(e_turn_left);
        }else if(it->str() == "R"){
            ret.path.push_back(e_turn_right);
        }else{
            ret.path.push_back(std::stoi(it->str()));
        }
    }

    int w = 0;
    int h = 0;
    for(auto& l : lines){
        w = std::max(w, (int)l.size());
        h++;
    }

    ret.map = map_t(w, h, ' ');
    for(int y=0; y<h; ++y){
        for(int x=0; x<std::min(w,(int)lines[y].size()); ++x){
            ret.map(x,y) = lines[y][x];
        }
    }

    ret.dims = dims;
    return ret;
}


int mod(int a, int b) { return (a % b + b) % b; }

std::vector<pos_t> dirs {
    { 1, 0 },
    { 0, 1 },
    { -1, 0 },
    { 0, -1 }
};

auto part1(note_t note) 
{
    pos_t pos { 0, 0};
    int dir_idx = 0;

    while(note.map(pos.x, pos.y) != '.'){
        pos.x++;
    }

    for(auto in : note.path){
        if(in == e_turn_left){
            dir_idx = mod(dir_idx-1, 4);
        }else if(in == e_turn_right){
            dir_idx = mod(dir_idx+1, 4);
        }else{
            for(int i=0; i<in; ++i){
                pos_t new_pos = pos + dirs[dir_idx];

                if(new_pos.x >= note.map.width || (dir_idx==0 && note.map(new_pos.x,new_pos.y) == ' ')){
                    new_pos.x = 0;
                    while(note.map(new_pos.x, new_pos.y) == ' '){
                        new_pos.x++;
                    }
                }else if(new_pos.x < 0 || (dir_idx==2 && note.map(new_pos.x,new_pos.y) == ' ')){
                    new_pos.x = note.map.width-1;
                    while(note.map(new_pos.x, new_pos.y) == ' '){
                        new_pos.x--;
                    }
                }else if(new_pos.y >= note.map.height || (dir_idx==1 && note.map(new_pos.x,new_pos.y) == ' ')){
                    new_pos.y = 0;
                    while(note.map(new_pos.x, new_pos.y) == ' '){
                        new_pos.y++;
                    }
                }else if(new_pos.y < 0 || (dir_idx==3 && note.map(new_pos.x,new_pos.y) == ' ')){
                    new_pos.y = note.map.height-1;
                    while(note.map(new_pos.x, new_pos.y) == ' '){
                        new_pos.y--;
                    }
                }

                if(note.map(new_pos.x, new_pos.y) != '#'){
                    pos = new_pos;
                }
            }
        }
    }

    return 1000 * (pos.y+1) + 4 * (pos.x+1) + dir_idx;
}

struct neighbour_edge_t{
    int x;
    int y;
    int edge;
    bool invert;
};

struct face_info_t;

struct edge_t{
    face_info_t* face = nullptr;
    dir_enum edge_dir;
    bool inverted;
};

struct face_info_t{
    int x;
    int y;
    int dims;
    edge_t edges[4];

    face_info_t(int px,int py,int pdims) : x(px), y(py), dims(pdims) {}

    int xstart() const { return x; }
    int xend() const { return x+dims; }
    int ystart() const { return y; }
    int yend() const { return y+dims; }
};

using face_info_map_t = std::map<face_enum,face_info_t*>;

int process(note_t note, const face_info_map_t& face_info_map)
{
    face_info_t* current_face = face_info_map.at(e_top_face);

    pos_t pos { 0, 0 };
    int dir_idx = 0;

    while(note.map(pos.x, pos.y) != '.'){
        pos.x++;
    }

    for(auto in : note.path){
        if(in == e_turn_left){
            dir_idx = mod(dir_idx-1, 4);
        }else if(in == e_turn_right){
            dir_idx = mod(dir_idx+1, 4);
        }else{
            for(int i=0; i<in; ++i){
                pos_t new_pos = pos + dirs[dir_idx];
                int new_dir_idx = dir_idx;

                bool transitioning = true;
                edge_t to_edge;
                int edge_idx = 0;

                if(new_pos.x >= current_face->xend()){
                    to_edge = current_face->edges[e_right];
                    edge_idx = pos.y % note.dims;
                }else if(new_pos.x <  current_face->xstart()){
                    to_edge = current_face->edges[e_left];
                    edge_idx = pos.y % note.dims;
                }else if(new_pos.y >= current_face->yend()){
                    to_edge = current_face->edges[e_down];
                    edge_idx = pos.x % note.dims;
                }else if(new_pos.y < current_face->ystart()){
                    to_edge = current_face->edges[e_up];
                    edge_idx = pos.x % note.dims;
                }else{
                    transitioning = false;
                }

                if(transitioning)
                {
                    if(to_edge.inverted){
                        edge_idx = note.dims - edge_idx - 1;
                    }

                    if(to_edge.edge_dir == e_right)
                    {
                        new_pos.x = to_edge.face->x + note.dims - 1;
                        new_pos.y = to_edge.face->y + edge_idx;
                        new_dir_idx = e_left;
                    }
                    else if(to_edge.edge_dir == e_down)
                    {
                        new_pos.x = to_edge.face->x + edge_idx;
                        new_pos.y = to_edge.face->y + note.dims - 1;
                        new_dir_idx = e_up;
                    }
                    else if(to_edge.edge_dir == e_left)
                    {
                        new_pos.x = to_edge.face->x;
                        new_pos.y = to_edge.face->y + edge_idx;
                        new_dir_idx = e_right;
                    }
                    else if(to_edge.edge_dir == e_up)
                    {
                        new_pos.x = to_edge.face->x + edge_idx;
                        new_pos.y = to_edge.face->y;
                        new_dir_idx = e_down;
                    }
                }

                if(note.map(new_pos.x, new_pos.y) != '#'){
                    pos = new_pos;
                    dir_idx = new_dir_idx;
                    if(transitioning){
                        current_face = to_edge.face;
                    }
                }
            }
        }
    }

    return 1000 * (pos.y+1) + 4 * (pos.x+1) + dir_idx;
}

auto part2(const note_t& note) 
{
    int xgrid = note.map.width / note.dims;
    int ygrid = note.map.height / note.dims;

    grid_t<int> face_presence(xgrid, ygrid, false);

    for(int gy=0; gy<ygrid; ++gy){
        for(int gx=0; gx<xgrid; ++gx){
            int sx = gx * note.dims;
            int sy = gy * note.dims;
            if(note.map(sx,sy) != ' '){
                face_presence(gx,gy) = true;
            }
        }
    }

    int cx = 0;
    int cy = 0;
    for(; cx<xgrid; ++cx){
        if(face_presence(cx,0)){
            break;
        }
    }

    grid_t<face_info_t*> face_info(face_presence.width, face_presence.height, nullptr);
    for(int y=0; y<face_presence.height; ++y){
        for(int x=0; x<face_presence.width; ++x){
            if(face_presence(x,y)){
                face_info(x,y) = new face_info_t(x*note.dims, y*note.dims, note.dims);
            }
        }
    }

    face_info_map_t face_info_map;
    if(note.dims == 4)
    {
        // cube net 1-3-2 pattern 3 equivalent
        face_info_map[e_top_face]    = face_info(2,0);
        face_info_map[e_front_face]  = face_info(2,1);
        face_info_map[e_bottom_face] = face_info(2,2);
        face_info_map[e_right_face]  = face_info(3,2);
        face_info_map[e_left_face]   = face_info(1,1);
        face_info_map[e_back_face]   = face_info(0,1);

        // top face
        face_info_map[e_top_face]->edges[e_right] = { face_info_map[e_right_face], e_right, true };
        face_info_map[e_top_face]->edges[e_down]  = { face_info_map[e_front_face], e_up, false };
        face_info_map[e_top_face]->edges[e_left]  = { face_info_map[e_left_face], e_up, false };
        face_info_map[e_top_face]->edges[e_up]    = { face_info_map[e_back_face], e_up, true };

        // front face
        face_info_map[e_front_face]->edges[e_right] = { face_info_map[e_right_face], e_up, true };
        face_info_map[e_front_face]->edges[e_down]  = { face_info_map[e_bottom_face], e_up, false };
        face_info_map[e_front_face]->edges[e_left]  = { face_info_map[e_left_face], e_right, false };
        face_info_map[e_front_face]->edges[e_up]    = { face_info_map[e_top_face], e_down, false };

        // bottom face
        face_info_map[e_bottom_face]->edges[e_right] = { face_info_map[e_right_face], e_left, false };
        face_info_map[e_bottom_face]->edges[e_down]  = { face_info_map[e_back_face], e_down, true };
        face_info_map[e_bottom_face]->edges[e_left]  = { face_info_map[e_left_face], e_down, true };
        face_info_map[e_bottom_face]->edges[e_up]    = { face_info_map[e_top_face], e_down, false };

        // right face
        face_info_map[e_right_face]->edges[e_right] = { face_info_map[e_top_face], e_right, true };
        face_info_map[e_right_face]->edges[e_down]  = { face_info_map[e_back_face], e_left, true };
        face_info_map[e_right_face]->edges[e_left]  = { face_info_map[e_bottom_face], e_right, false };
        face_info_map[e_right_face]->edges[e_up]    = { face_info_map[e_front_face], e_right, true };

        // left face
        face_info_map[e_left_face]->edges[e_right] = { face_info_map[e_front_face], e_left, false };
        face_info_map[e_left_face]->edges[e_down]  = { face_info_map[e_bottom_face], e_left, true };
        face_info_map[e_left_face]->edges[e_left]  = { face_info_map[e_back_face], e_right, false };
        face_info_map[e_left_face]->edges[e_up]    = { face_info_map[e_top_face], e_left, false };

        // back face
        face_info_map[e_back_face]->edges[e_right] = { face_info_map[e_left_face], e_left, false };
        face_info_map[e_back_face]->edges[e_down]  = { face_info_map[e_bottom_face], e_down, true };
        face_info_map[e_back_face]->edges[e_left]  = { face_info_map[e_right_face], e_down, true };
        face_info_map[e_back_face]->edges[e_up]    = { face_info_map[e_top_face], e_up, true };
    }
    else
    {
        // cube net 1-3-2 pattern 1 equivalent
        face_info_map[e_top_face]    = face_info(1,0);
        face_info_map[e_front_face]  = face_info(1,1);
        face_info_map[e_bottom_face] = face_info(1,2);
        face_info_map[e_right_face]  = face_info(2,0);
        face_info_map[e_left_face]   = face_info(0,2);
        face_info_map[e_back_face]   = face_info(0,3);

        // top face
        face_info_map[e_top_face]->edges[e_right] = { face_info_map[e_right_face], e_left, false };
        face_info_map[e_top_face]->edges[e_down]  = { face_info_map[e_front_face], e_up, false };
        face_info_map[e_top_face]->edges[e_left]  = { face_info_map[e_left_face], e_left, true };
        face_info_map[e_top_face]->edges[e_up]    = { face_info_map[e_back_face], e_left, false };

        // front face
        face_info_map[e_front_face]->edges[e_right] = { face_info_map[e_right_face], e_down, false };
        face_info_map[e_front_face]->edges[e_down]  = { face_info_map[e_bottom_face], e_up, false };
        face_info_map[e_front_face]->edges[e_left]  = { face_info_map[e_left_face], e_up, false };
        face_info_map[e_front_face]->edges[e_up]    = { face_info_map[e_top_face], e_down, false };

        // bottom face
        face_info_map[e_bottom_face]->edges[e_right] = { face_info_map[e_right_face], e_right, true };
        face_info_map[e_bottom_face]->edges[e_down]  = { face_info_map[e_back_face], e_right, false };
        face_info_map[e_bottom_face]->edges[e_left]  = { face_info_map[e_left_face], e_right, false };
        face_info_map[e_bottom_face]->edges[e_up]    = { face_info_map[e_front_face], e_down, false };

        // right face
        face_info_map[e_right_face]->edges[e_right] = { face_info_map[e_bottom_face], e_right, true };
        face_info_map[e_right_face]->edges[e_down]  = { face_info_map[e_front_face], e_right, false };
        face_info_map[e_right_face]->edges[e_left]  = { face_info_map[e_top_face], e_right, false };
        face_info_map[e_right_face]->edges[e_up]    = { face_info_map[e_back_face], e_down, false };

        // left face
        face_info_map[e_left_face]->edges[e_right] = { face_info_map[e_bottom_face], e_left, false };
        face_info_map[e_left_face]->edges[e_down]  = { face_info_map[e_back_face], e_up, false };
        face_info_map[e_left_face]->edges[e_left]  = { face_info_map[e_top_face], e_left, true };
        face_info_map[e_left_face]->edges[e_up]    = { face_info_map[e_front_face], e_left, false };

        // back face
        face_info_map[e_back_face]->edges[e_right] = { face_info_map[e_bottom_face], e_down, false };
        face_info_map[e_back_face]->edges[e_down]  = { face_info_map[e_right_face], e_up, false };
        face_info_map[e_back_face]->edges[e_left]  = { face_info_map[e_top_face], e_up, false };
        face_info_map[e_back_face]->edges[e_up]    = { face_info_map[e_left_face], e_down, false };
    }

    return process(note, face_info_map);
}

void main()
{
    auto test_values = load_input("../src/day22/test_input.txt", 4);
    auto actual_values = load_input("../src/day22/input.txt", 50);

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}