#include <iostream>
#include <vector>
#include <array>
#include <string>
#include <utility> //
#include <cstdio>

#include <queue>

//if fh
#include <fstream>

//#include<bits/stdc++.h> when I'm feeling lazy

#define fileStream 1
#define filename_glo "sample.txt"

#define rCoordinates std::pair<std::pair<int, int>, std::pair<int, int>>

//later convert std::array to normal array;

void test_storage(std::pair<std::array<int, 5> ,std::vector< rCoordinates >> data_stored){
    std::cout << data_stored.first[0] << '\n';
    std::cout << data_stored.first[1] << '\n';
    std::cout << data_stored.first[2] << '\n';
    std::cout << data_stored.first[3] << '\n';
    std::cout << data_stored.first[4] << '\n';

    for(int i = 0; i < data_stored.first[0]; i++){
        std::cout << data_stored.second[i].first.first << ' ';
        std::cout << data_stored.second[i].second.first << ' ';
        std::cout << data_stored.second[i].first.second  << ' ';
        std::cout << data_stored.second[i].second.second << '\n';
    }
}

std::pair<std::array<int, 5> ,std::vector< rCoordinates >> usingFH(std::string filename){
    std::pair<std::array<int, 5> ,std::vector< rCoordinates >> data;
    
    std::fstream fs(filename);

    /*
    0 - num of shapes
    1 - x spacing
    2 - y spacing
    3 - dimension 
    4 - dim
    */
    std::string temp;
    fs >> temp;
    data.first[0] = std::stoi(temp.substr(10));
    fs >> temp;
    data.first[1] = std::stoi(temp.substr(9));
    fs >> temp;
    data.first[2] = std::stoi(temp.substr(9));
    fs >> temp;
    std::string xtra;
    xtra = temp.substr(10);
    int xloc = xtra.find("x");
    data.first[3] = std::stoi(xtra.substr(0, xloc));
    data.first[4] = std::stoi(xtra.substr(xloc + 1));

    int xLL, yLL, xUR, yUR;
    std::string delim = ",";
    rCoordinates specs;
    for(int i = 0; i < data.first[0]; i++){
        fs >> xLL >> delim >> yLL >> delim >> xUR >> delim >> yUR;
        specs.first.first = xLL;
        specs.second.first = yLL;
        specs.first.second = xUR;
        specs.second.second = yUR;
        data.second.emplace_back(specs);
    }

    return data;
}

std::pair<std::array<int, 5> ,std::vector< rCoordinates >> inputRdr(){
    std::pair<std::array<int, 5> ,std::vector< rCoordinates >> data;

    std::string stat;
    std::cin >> stat;
    data.first[0] = std::stoi(stat.substr(10));
    std::cin >> stat;
    data.first[1] = std::stoi(stat.substr(9));
    std::cin >> stat;
    data.first[2] = std::stoi(stat.substr(9));

    std::cin >> stat;
    std::string xtra;
    xtra = stat.substr(10);
    int xloc = xtra.find("x");
    data.first[3] = std::stoi(xtra.substr(0, xloc));
    data.first[4] = std::stoi(xtra.substr(xloc + 1));

    int xLL, yLL, xUR, yUR;
    rCoordinates specs;
    while(scanf("%d[^,], %d[^,], %d[^,], %d", &xLL, &yLL, &xUR, &yUR) == 4){
        specs.first.first = xLL;
        specs.second.first = yLL;
        specs.first.second = xUR;
        specs.second.second = yUR;
        data.second.emplace_back(specs);
    }

    return data;
}

//rectangle shiz

class RecNode{
    private:
        //rect x range and y range
        rCoordinates rectangle;
        int max;
        RecNode *left;
        RecNode *right;

    public:
        RecNode(rCoordinates rectangle);
        friend class RecTree;
};

RecNode::RecNode(rCoordinates rectangle) : rectangle(rectangle), max(rectangle.first.second), left(nullptr), right(nullptr) {}

class RecTree{
    private:
        RecNode *root;
        size_t size;
        int xGap;
        int yGap;
        //void recursivedestroy(RecNode* curr_node, )
        void RTInsert(RecNode* curr_node, rCoordinates rect_val);
        void recursiveDestroy(RecNode* curr_node);
        bool intersect(rCoordinates rRef, rCoordinates rQuery);

    public:
        RecTree(int xGap, int yGap);
        ~RecTree();
        size_t getSize() {return size;}
        void addRect(rCoordinates rect_val);
        void levelTraverse();
        void printRect(rCoordinates rect_val);
        bool Overlap(rCoordinates rQuery);
};

RecTree::RecTree(int xGap, int yGap) : root(nullptr), size(0){
    this->xGap = xGap;
    this->yGap = yGap;
}

RecTree::~RecTree(){
    if (this->root == nullptr){
        return;
    }
    else{
        this->recursiveDestroy(this->root);
    }
}

void RecTree::recursiveDestroy(RecNode* curr_node){
    if (curr_node == nullptr){
        return;
    }
    else{
        this->recursiveDestroy(curr_node->left);
        this->recursiveDestroy(curr_node->right);
        delete curr_node;
    }
}

void RecTree::RTInsert(RecNode* curr_node, rCoordinates rect_val){
    if(curr_node->rectangle.first.first >= rect_val.first.first){
        if(curr_node->left != nullptr){
            this->RTInsert(curr_node->left, rect_val);
        }
        else{
            curr_node->left = new RecNode(rect_val);
        }
    }
    else{
        if(curr_node->right != nullptr){
            this->RTInsert(curr_node->right, rect_val);
        }
        else{
            curr_node->right = new RecNode(rect_val);
        }
    }

    //max update
    if(curr_node->max < rect_val.first.second){
        curr_node->max = rect_val.first.second;
    }
}

void RecTree::addRect(rCoordinates rect_val){ //key is low x
    //if tree empty
    if(this->root == NULL){
        this->root = new RecNode(rect_val);
    }
    else{
        this->RTInsert(this->root, rect_val);
    }

    this->size++;
}

void RecTree::levelTraverse(){
    //print out content by level
    std::queue<RecNode*> q;
    RecNode *temp = this->root;
    if(temp == nullptr){
        std::cout << "empty tree\n";
        return;
    }
    //not empty
    q.push(temp);
    int lvlcnt = 1;
    int sz = this->size;

    while(sz > 0){
        std::cout << "Level " << lvlcnt << ": ";
        //do i have something print
        int nodeCnt = q.size();

        while(nodeCnt > 0){//how many nodes were in that level
            RecNode *curr_node = q.front();
            std::cout << "[" << curr_node->rectangle.first.first << " " 
                      << curr_node->rectangle.first.second << "]"
                      << "(" << curr_node->max << ") ";
            q.pop();
            sz--;
            //put their children in queue
            if(curr_node->left){
                q.push(curr_node->left);
            }
            if(curr_node->right){
                q.push(curr_node->right);
            }
            nodeCnt--;
        }
        std::cout << "\n";
        lvlcnt++;
    }

}

void RecTree::printRect(rCoordinates rect_val){
    std::cout << "culprit\n";
    std::cout << "[" << rect_val.first.first << " " 
                     << rect_val.second.first << " "
                     << rect_val.first.second << " "
                     << rect_val.second.second << "]\n";
}

bool RecTree::intersect(rCoordinates rRef, rCoordinates rQuery){
    //[low, high] 
    //   [refx, refy] 
    //completely left
    if(rQuery.first.second <= rRef.first.first - this->xGap && rQuery.first.first < rRef.first.first - this->xGap){
        return 0;
    }
    //right
    if(rQuery.first.first >= rRef.first.second + this-> xGap && rQuery.first.second > rRef.first.second + this->xGap){
        return 0;
    }
    //lower tho
    if(rQuery.second.second <= rRef.second.first - this->yGap && rQuery.second.first < rRef.second.first - this->yGap){
        return 0;
    }
    //higher tho
    if(rQuery.second.first >= rRef.second.second + this-> yGap && rQuery.second.second > rRef.second.second + this->yGap){
        return 0;
    }

    return 1;
}


bool RecTree::Overlap(rCoordinates rQuery){
    RecNode* curr_node = this->root;
    while(curr_node != nullptr){
        if(intersect(curr_node->rectangle, rQuery)){
            //printRect(curr_node->rectangle);
            return 1;
        }
        else if(curr_node->left == nullptr){
            curr_node = curr_node->right;
        }
        else if(curr_node->left->max < rQuery.first.first){
            curr_node = curr_node->right;
        }
        else{
            curr_node = curr_node->left;
        }
    }

    return 0;
}

void printPCB(std::pair<std::array<int, 5> ,std::vector< rCoordinates >> data_stored){
    int shapeNum = data_stored.first[0];
    RecTree side1(data_stored.first[1], data_stored.first[2]);
    RecTree side2(data_stored.first[1], data_stored.first[2]);
    for(int i = 0; i < shapeNum; i++){
        if(!side1.Overlap(data_stored.second[i])){
            side1.addRect(data_stored.second[i]);
            std::cout << "SIDE1\n";
        }
        else if(!side2.Overlap(data_stored.second[i])){    
            side2.addRect(data_stored.second[i]);
            std::cout << "SIDE2\n";
        }
        else{
            std::cout << "NONE\n";  
        }
    }

}

//

int main(void){
    //if input rdr 0 
    //jic nagfail use 1
    #if 1
    std::pair<std::array<int, 5> ,std::vector< rCoordinates >> components;
    components = usingFH(filename_glo);
    //test_storage(components);
    #else
    std::pair<std::array<int, 5> ,std::vector< rCoordinates >> components;
    components = inputRdr();
    //test_storage(components);
    #endif

    printPCB(components);

    return 0;
}