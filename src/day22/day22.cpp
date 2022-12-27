#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <numeric>
#include <functional>
#include <regex>
#include <map>
#include <optional>

#define LEFT -1
#define RIGHT -2

enum turn_enum { e_trun_left=-2, e_turn_right=-1 };
enum dir_enum { e_right=0, e_down, e_left, e_up };
enum side_enum { e_top_face=0, e_bottom_face, e_front_face, e_back_face, e_left_face, e_right_face };

struct pos_t{
    int x,y,z;
};

pos_t operator+(const pos_t&a, const pos_t& b){
    return pos_t{a.x+b.x, a.y+b.y, a.z+b.z};
}

bool operator==(const pos_t&a, const pos_t& b){
    return std::tuple(a.x,a.y,a.z) == std::tuple(b.x,b.y,b.z);
}

bool operator<(const pos_t&a, const pos_t& b){
    return std::tuple(a.x,a.y,a.z) < std::tuple(b.x,b.y,b.z);
}

int dot(const pos_t& a,const pos_t& b){
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

struct map_t{
    std::vector<char> data;
    int width = 0;
    int height = 0;

    bool empty() const {
        return data.empty();
    }

    char& get(int x,int y,int z){
        return data[y*width + x];
    }
};

using path_t = std::vector<int>;

struct note_t{
    map_t map;
    path_t path;
};

note_t load_input(const std::string& file){
    note_t ret;

    {
        std::ifstream fs(file);
        int w = 0;
        int h = 0;
        std::string line;
        while (std::getline(fs, line)) {
            if (line.empty()) {
                break;
            }
            w = std::max(w, (int)line.size());
            h++;
        }

        ret.map.width = w;
        ret.map.height = h;
        ret.map.data.resize(w*h*1, ' ');
    }

    std::ifstream fs(file);

    int h = 0;
    bool part1 = true;
    std::string line;
    while(std::getline(fs, line)){
        if(line.empty()){
            part1 = false;
            continue;
        }

        if(part1){
            for(int i=0; i<line.size(); ++i){
                ret.map.get(i, h, 0) = line[i];
            }
            h++;
        }else{
            std::regex word_regex("\\d+|\\D+");
            for(auto it=std::sregex_iterator(line.begin(), line.end(), word_regex); it!=std::sregex_iterator(); it++){
                if(it->str() == "L"){
                    ret.path.push_back(LEFT);
                }else if(it->str() == "R"){
                    ret.path.push_back(RIGHT);
                }else{
                    ret.path.push_back(std::stoi(it->str()));
                }
            }
        }

    }
    return ret;
}

std::ostream& operator<<(std::ostream& os, const pos_t& pos){
    os << pos.x << ", " << pos.y << ", " << pos.z;
    return os;
}

std::ostream& operator<<(std::ostream& os, map_t& map){
    for(int y=0; y<map.height; ++y){
        for(int x=0; x<map.width; ++x){
            std::cout << map.get(x,y,0);
        }
        std::cout << std::endl;
    }
    return os;
}

int mod(int a, int b) { return (a % b + b) % b; }

auto part1(note_t note) 
{
    pos_t pos { 0, 0, 0 };
    std::vector<pos_t> dirs {
        { 1, 0, 0 },
        { 0, 1, 0 },
        { -1, 0, 0 },
        { 0, -1, 0 }
    };
    int dir_idx = 0;

    while(note.map.get(pos.x, pos.y, pos.z) != '.'){
        pos.x++;
    }

    note.map.get(pos.x,pos.y,0) = '>';

    for(auto in : note.path){
        if(in == LEFT){
            dir_idx = mod(dir_idx-1, 4);
        }else if(in == RIGHT){
            dir_idx = mod(dir_idx+1, 4);
        }else{
            for(int i=0; i<in; ++i){
                pos_t new_pos = pos + dirs[dir_idx];

                if(new_pos.x >= note.map.width || (dir_idx==0 && note.map.get(new_pos.x,new_pos.y,0) == ' ')){
                    new_pos.x = 0;
                    while(note.map.get(new_pos.x, new_pos.y, new_pos.z) == ' '){
                        new_pos.x++;
                    }
                }else if(new_pos.x < 0 || (dir_idx==2 && note.map.get(new_pos.x,new_pos.y,0) == ' ')){
                    new_pos.x = note.map.width-1;
                    while(note.map.get(new_pos.x, new_pos.y, new_pos.z) == ' '){
                        new_pos.x--;
                    }
                }else if(new_pos.y >= note.map.height || (dir_idx==1 && note.map.get(new_pos.x,new_pos.y,0) == ' ')){
                    new_pos.y = 0;
                    while(note.map.get(new_pos.x, new_pos.y, new_pos.z) == ' '){
                        new_pos.y++;
                    }
                }else if(new_pos.y < 0 || (dir_idx==3 && note.map.get(new_pos.x,new_pos.y,0) == ' ')){
                    new_pos.y = note.map.height-1;
                    while(note.map.get(new_pos.x, new_pos.y, new_pos.z) == ' '){
                        new_pos.y--;
                    }
                }

                if(note.map.get(new_pos.x, new_pos.y, new_pos.z) != '#'){
                    pos = new_pos;
                }
            }
        }
    }
 
    return 1000 * (pos.y+1) + 4 * (pos.x+1) + dir_idx;
}

template<typename T>
struct grid_t{
    std::vector<T> data;
    int width = 0;
    int height = 0;

    grid_t() {}
    grid_t(int w,int h,const T& val=T()) : data(w*h,val), width(w), height(h) {} 
    grid_t(int w,int h,const std::vector<T>& d) : data(d), width(w), height(h) {} 

    bool empty() const {
        return data.empty();
    }

    bool within(int x,int y) const{
        return x>=0 && y>=0 && x<width && y<height;
    }

    decltype(auto) operator()(int x,int y){
        return data[y*width + x];
    }

    decltype(auto) operator()(int x,int y) const{
        return data[y*width + x];
    }
};

template<typename T>
std::ostream& operator<<(std::ostream& os, grid_t<T>& grid){
    for(int y=0; y<grid.height; ++y){
        for(int x=0; x<grid.width; ++x){
            std::cout << grid(x,y);
        }
        std::cout << std::endl;
    }
    return os;
}

using side_t = grid_t<char>;
using sides_t = grid_t<side_t>;

struct cube_t{
    std::vector<side_t> sides;
};

using faces_t = std::map<side_enum, side_t>;

std::map<side_enum, std::vector<side_enum>> neighbours = {
    { e_top_face,    { e_right_face, e_front_face,  e_left_face,   e_back_face  } },
    { e_front_face,  { e_right_face, e_bottom_face, e_left_face,   e_top_face   } },
    { e_bottom_face, { e_right_face, e_back_face,   e_left_face,   e_front_face } },
    { e_back_face,   { e_left_face,  e_bottom_face, e_right_face,  e_top_face   } },
    { e_left_face,   { e_front_face, e_bottom_face, e_back_face,   e_top_face   } },
    { e_right_face,  { e_top_face,   e_back_face,   e_bottom_face, e_front_face } }
};

enum axis_enum { e_xaxis_90=0, e_xaxis_270, e_yaxis_90, e_yaxis_270};

#define COS90 0
#define SIN90 1

#define COS270 0
#define SIN270 -1

struct orien_t{
    pos_t x_dir = { 1, 0, 0 };
    pos_t y_dir = { 0, 1, 0 };
    pos_t z_dir = { 0, 0, 1 };
};

pos_t rotate(const pos_t& n, axis_enum axis){
    if(axis == e_xaxis_270){
        return { dot({1,0,0},n),dot({0,COS270,-SIN270},n),dot({0,SIN270,COS270},n) };
    }else if(axis == e_xaxis_90){
        return { dot({1,0,0},n),dot({0,COS90,-SIN90},n),dot({0,SIN90,COS90},n) };
    }else if(axis == e_yaxis_90){
        return { dot({COS90,0,SIN90},n),dot({0,1,0},n),dot({-SIN90,0,COS90},n) };
    }else /*if(axis == e_zaxis)*/{
        return { dot({COS270,0,SIN270},n),dot({0,1,0},n),dot({-SIN270,0,COS270},n) };
    }
}

orien_t rotate(const orien_t& o, axis_enum axis){
    return { rotate(o.x_dir, axis), rotate(o.y_dir, axis), rotate(o.z_dir, axis) };
}

std::map<pos_t, side_enum> dir_to_face_map;

struct pos_orien_t{
    pos_t grid_pos;
    orien_t orien;
};

void build_cube(std::map<side_enum,pos_orien_t>& faces, side_enum face, const grid_t<int>& face_present, int x, int y, const orien_t& orien)
{
    if(!faces.count(face) && face_present.within(x,y) && face_present(x,y))
    {
        faces[face] = { pos_t{x,y}, orien };

        build_cube(faces, neighbours[face][0], face_present, x+1, y  , rotate(orien, e_yaxis_90));
        build_cube(faces, neighbours[face][1], face_present, x,   y+1, rotate(orien, e_xaxis_270));
        build_cube(faces, neighbours[face][2], face_present, x-1, y  , rotate(orien, e_yaxis_270));
        build_cube(faces, neighbours[face][3], face_present, x,   y-1, rotate(orien, e_xaxis_90));
    }
}

template<typename T>
bool operator==(const grid_t<T>& a,const grid_t<T>& b){
    return std::tuple(a.width,a.height,a.data) == std::tuple(b.width,b.height,b.data);
}

using net_t = grid_t<int>;
using nets_t = std::vector<net_t>;

nets_t nets = {
    net_t(4, 3, { 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0 }),
    net_t(4, 3, { 0, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0 }),
    net_t(4, 3, { 0, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0 }),
    net_t(4, 3, { 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0 }),
    net_t(4, 3, { 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0 }),
    net_t(5, 2, { 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1 }),
    net_t(4, 3, { 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0 }),
    net_t(4, 3, { 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1 }),
    net_t(4, 3, { 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1 }),
    net_t(4, 3, { 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1 }),
    net_t(4, 3, { 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1 })
};

net_t rotate90(const net_t& net){
    net_t ret(net.height, net.width);
    for(int y=0; y<net.height; ++y){
        for(int x=0; x<net.width; ++x){
            ret(y,x) = net(x,y);
        }
    }
    return ret;
}

net_t flip(const net_t& net){
    net_t ret(net.width, net.height);
    if((ret.width % 2)==0){
        for(int y=0; y<net.height; ++y){
            for(int x=0; x<net.width; ++x){
                if((ret.width % 2)==0){
                    ret(x,y) = net(ret.width-x-1,y);
                }else{
                    ret(x,y) = net(x,ret.height-x-y);
                }
            }
        }
    }
    return ret;
}

bool equivalent_net(const net_t& a,const net_t& b){
    if(a == b){
        return true;
    }

    net_t r = a;
    for(int i=1; i<4; ++i){
        r = rotate90(r);
        if(r == b){
            return true;
        }
    }

    r = flip(a);
    for(int i=0; i<4; ++i){
        r = rotate90(r);
        if(r == b){
            return true;
        }
    }

    return false;
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
    dir_enum side;
    bool inverted;
};

struct face_info_t{
    int x;
    int y;
    int dims;

    int xstart() const{ return x; }
    int xend() const{ return x+dims; }
    int ystart() const{ return y; }
    int yend() const{ return y+dims; }

    edge_t edges[4];
};

char dir_to_char(dir_enum dir){
    switch(dir){
        case e_left:    return '<';
        case e_right:   return '>';
        case e_up:      return '^';
        case e_down:    return 'v';
        default:        return ' ';
    };
}

cube_t load_input2(const std::string& file, int dims){
    cube_t ret;

    /*for(auto& side : ret.sides){
        side = { std::vector<char>(dims*dims), dims, dims };
    }*/


    std::vector<std::string> lines;
    std::ifstream fs(file);
    std::string line;
    while (std::getline(fs, line)) {
        if (line.empty()) {
            break;
        }
        lines.push_back(line);
    }

    std::vector<int> path;
    std::getline(fs, line);
    std::regex word_regex("\\d+|\\D+");
    for(auto it=std::sregex_iterator(line.begin(), line.end(), word_regex); it!=std::sregex_iterator(); it++){
        if(it->str() == "L"){
            path.push_back(LEFT);
        }else if(it->str() == "R"){
            path.push_back(RIGHT);
        }else{
            path.push_back(std::stoi(it->str()));
        }
    }

    int w = 0;
    int h = 0;
    for(auto& l : lines){
        w = std::max(w, (int)l.size());
        h++;
    }

    map_t map = { std::vector<char>(w*h), w, h };

    int xgrid = w / dims;
    int ygrid = h / dims;

    grid_t<int> face_present(xgrid, ygrid, false);

    grid_t<side_t> sides(xgrid, ygrid);

    for(int y=0; y<h; ++y){
        for(int x=0; x<std::min(w,(int)lines[y].size()); ++x){
            map.get(x,y,0) = lines[y][x];
        }
    }

    for(int gy=0; gy<ygrid; ++gy){
        for(int gx=0; gx<xgrid; ++gx){
            int sx = gx * dims;
            int sy = gy * dims;
            if(sx < lines[sy].size() && lines[sy][sx] != ' '){
                face_present(gx,gy) = true;
                sides(gx,gy) = side_t(dims, dims);
                for(int y=0; y<dims; ++y){
                    for(int x=0; x<dims; ++x){
                        sides(gx,gy)(x,y) = map.get(sx+x,sy+y,0); 
                    }
                }
            }
        }
    }

    for(int i=0; i<nets.size(); ++i){
        if(equivalent_net(face_present, nets[i])){
            std::cout << i << std::endl;
            break;
        }
    }

    int cx = 0;
    int cy = 0;
    for(; cx<xgrid; ++cx){
        if(face_present(cx,0)){
            break;
        }
    }

    /*orien_t orien0;
    auto left_x = rotate(orien0, e_yaxis_270);
    auto right_x = rotate(orien0, e_yaxis_90);
    auto up_y = rotate(orien0, e_xaxis_90);
    auto dwnn_y = rotate(orien0, e_xaxis_270);*/


    orien_t orien;
    std::map<side_enum, pos_orien_t> face_pos_map;
    //faces[e_top_face] = sides(cx,cy);
    build_cube(face_pos_map, e_top_face, face_present, cx, cy, orien);

    std::map<side_enum, side_t> faces;
    for(auto& [face, pos] : face_pos_map){
        
        faces[face] = side_t(dims,dims);
        for(int y=0; y<dims; ++y){
            for(int x=0; x<dims; ++x){
                faces[face](x,y) = lines[pos.grid_pos.y*dims+y][pos.grid_pos.x*dims+x];
            }
        }

    }

    for(auto& [face,side] : faces){
        if(face == e_top_face){
            std::cout << "top_face" << std::endl;
        }else if(face == e_front_face){
            std::cout << "front_face" << std::endl;
        }else if(face == e_bottom_face){
            std::cout << "bottom_face" << std::endl;
        }else if(face == e_back_face){
            std::cout << "back_face" << std::endl;
        }else if(face == e_left_face){
            std::cout << "left_face" << std::endl;
        }else if(face == e_right_face){
            std::cout << "right_face" << std::endl;
        }
        std::cout << side << std::endl;
    }

    grid_t<face_info_t*> face_info(face_present.width, face_present.height, nullptr);
    for(int y=0; y<face_present.height; ++y){
        for(int x=0; x<face_present.width; ++x){
            if(face_present(x,y)){
                face_info(x,y) = new face_info_t;
                face_info(x,y)->x = dims*x;
                face_info(x,y)->y = dims*y;
                face_info(x,y)->dims = dims;
            }
        }
    }

    /*struct edge_t{
        face2_t* face;
        dir_enum side;
        bool inverted;
    };*/

    // cube net 1-3-2 pattern 3 equivalent
    std::map<side_enum,face_info_t*> face_info_map;
    if(dims == 4)
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

    face_info_t* current_face = face_info_map[e_top_face];

    pos_t pos { 0, 0, 0 };
    std::vector<pos_t> dirs {
        { 1, 0, 0 },
        { 0, 1, 0 },
        { -1, 0, 0 },
        { 0, -1, 0 }
    };
    int dir_idx = 0;

    while(map.get(pos.x, pos.y, pos.z) != '.'){
        pos.x++;
    }

    map.get(pos.x,pos.y,0) = '>';

    for(auto in : path){
        if(in == LEFT){
            dir_idx = mod(dir_idx-1, 4);
        }else if(in == RIGHT){
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
                    edge_idx = pos.y % dims;
                }else if(new_pos.x <  current_face->xstart()){
                    to_edge = current_face->edges[e_left];
                    edge_idx = pos.y % dims;
                }else if(new_pos.y >= current_face->yend()){
                    to_edge = current_face->edges[e_down];
                    edge_idx = pos.x % dims;
                }else if(new_pos.y < current_face->ystart()){
                    to_edge = current_face->edges[e_up];
                    edge_idx = pos.x % dims;
                }else{
                    transitioning = false;
                }

                if(transitioning)
                {
                    if(to_edge.inverted){
                        edge_idx = dims - edge_idx - 1;
                    }

                    if(to_edge.side == e_right)
                    {
                        new_pos.x = to_edge.face->x + dims - 1;
                        new_pos.y = to_edge.face->y + edge_idx;
                        new_dir_idx = e_left;
                    }
                    else if(to_edge.side == e_down)
                    {
                        new_pos.x = to_edge.face->x + edge_idx;
                        new_pos.y = to_edge.face->y + dims - 1;
                        new_dir_idx = e_up;
                    }
                    else if(to_edge.side == e_left)
                    {
                        new_pos.x = to_edge.face->x;
                        new_pos.y = to_edge.face->y + edge_idx;
                        new_dir_idx = e_right;
                    }
                    else if(to_edge.side == e_up)
                    {
                        new_pos.x = to_edge.face->x + edge_idx;
                        new_pos.y = to_edge.face->y;
                        new_dir_idx = e_down;
                    }
                }

                if(map.get(new_pos.x, new_pos.y, new_pos.z) != '#'){
                    pos = new_pos;
                    dir_idx = new_dir_idx;
                    if(transitioning){
                        current_face = to_edge.face;
                    }
                    map.get(new_pos.x, new_pos.y, new_pos.z) = dir_to_char((dir_enum)dir_idx);
                }

                //std::cout << std::endl;
                //std::cout << map << std::endl;
                int ggg = 0;
            }
        }
    }
 
    //std::cout << map << std::endl;
    std::cout << "answer:" << 1000 * (pos.y+1) + 4 * (pos.x+1) + dir_idx << std::endl;



    return ret;
}


auto part2(note_t note)
{
    return 0;
}

void main()
{
    //auto test_values = load_input2("../src/day22/test_input.txt", 4);
    auto test_values = load_input2("../src/day22/input.txt", 50);

    //auto test_values = load_input("../src/day22/test_input.txt");
    //auto actual_values = load_input("../src/day22/input.txt");

    //std::cout << "part1: " << part1(test_values) << std::endl;
    //std::cout << "part1: " << part1(actual_values) << std::endl;

    //std::cout << "part2: " << part2(test_values) << std::endl;
    //std::cout << "part2: " << part2(actual_values) << std::endl;
}