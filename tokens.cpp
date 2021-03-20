#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <algorithm>

class Grammer {
    std::vector<std::vector<std::string>> grammer;
    std::set<std::string> vt;
    std::set<std::string> vn;

public:
    std::set<std::string> terminable_tokens() {
        std::set<std::string> s;
        s = vt;
        int isChange = 0;
        while(!isChange) {
            int size_init = s.size();
            for(auto &p: grammer) {
                int i = 1;
                int state = 0;
                for(; i < p.size(); ++i) {
                    if(!s.count(p[i])) {
                        state = 1;
                        break;
                    }
                }
                if(0 == state) {
                    s.insert(p[0]);
                }
            }
            int size_end = s.size();
            isChange = size_init != size_end;
        }
        return s;
    }

    std::set<std::string> reachable_tokens() {
        std::set<std::string> s;
        s.insert(grammer[0][0]);
        int isChange = 0;
        while(!isChange) {
            int size_init = s.size();
            for(auto &p: grammer) {
                if(s.count(p[0])) {
                    for(int i = 1; i < p.size(); ++i) {
                        s.insert(p[i]);
                    }
                }
            }
            int size_end = s.size();
            isChange = size_init != size_end;
        }
    }

    void min_grammer() {
        auto a = this->reachable_tokens();
        auto b = this->terminable_tokens();
        std::vector<std::vector<std::string>> s;
        std::set<std::string> c;
        std::set_intersection(a.begin(), a.end(), b.begin(), b.end(), std::back_inserter(c));
        for(auto &p: grammer) {
            int state = 0;
            for(auto item: p) {
                if(!c.count(item)) {
                    state = 1;
                    break;
                }
            }
            if(!state) {
                s.push_back(p);
            }
        }
        grammer = s;
    }
};

int main() {
    std::cout << "hello world" << std::endl;
    return 0;
}
