#include <iostream>
#include <ostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <cctype>
#include <direct.h>

// ----------------------------------------------------------------
// TODO: read file line by line
/**
 * @brief read file line by line
 * 
 * @param file: std::string 
 * @return std::vector<std::string> lines in file 
 */
std::vector<std::string> read_file_lines(const std::string & file) {
    std::fstream in_file(file, std::ios::in);
    std::vector<std::string> lines;
    std::string line;
    if(!in_file.is_open()) {
        std::cerr << "\n---------------------\n";
        std::cerr << "    file not found" << std::endl;
        std::cerr << "---------------------\n";
        char path[128];
        getcwd(path, 128);
        std::cerr << "the current working directory is " << path << std::endl;
        std::cerr << "---------------------\n";
        std::exit(1);
    }
    
    while(std::getline(in_file, line)) {
        if(line.length() > 0) {
            lines.push_back(line);
        }
    }

    in_file.close();
    return lines;
}

// ---------------------------------------------------------------
// TODO: 分隔符 '|' 前后插入空格
// DONE
/**
 * @brief add stpace around '|'
 * 
 * @param str 
 */
void preprocess_add_space(std::string & str) {

    std::string::iterator it = str.begin();
    while(it != str.end()) {
        switch(*it) {
            case ';':
            case '|':
                it = str.insert(it, ' ');
                it = str.insert(it + 2, ' ');
                break;
            default:
                it++;
        }
    }

}

// ----------------------------------------------------------------
// TODO: return words of sentence
// DONE
/**
 * @brief parse a string to an array of words
 * 
 * @param str 
 * @return std::vector<std::string> 
 */
std::vector<std::string> read_words(const std::string & str) {
    std::vector<std::string> words;
    std::string word;
    int i = 0;
    do {
        while(std::isspace(str[i]) && i < str.length()) ++i;
        while(!std::isspace(str[i]) && i < str.length()) {
            word += str[i++];
        }
        if(word.length() != 0) words.push_back(word);
        word.clear();
    } while(i < str.length());
    return words;
}

// ----------------------------------------------------------------
// TODO: 分离VT, VN
class Grammer {
    std::vector<std::string> Lines; // 保存初始输入行内容
    std::vector<std::vector<std::string>> grammer; // G[S], 保存词素
    std::set<std::string> vt;
    std::set<std::string> vn;
public:
    void read_from_file(const std::string& file) {
        auto lines = read_file_lines(file);
        read_from_lists(lines);
    }
    /**
     * @brief 初始化vt、vn、Lines，同时获取每一行的词素grammar
     * 
     * @param lines 
     */
    void read_from_lists(const std::vector<std::string>& lines) {

        Lines = lines;
        vt.clear();
        vn.clear();
        std::vector<std::string> tmp;
        for(auto line: lines) {
            std::vector<std::string> p;
            preprocess_add_space(line);
            auto words = read_words(line);
            int state = 0;
            for(auto word: words) {
                switch(state) {
                    // 状态0表示读到首字母
                case 0:
                    if("->" == word) {
                        state = 1;
                    } else {
                        vn.insert(word);
                        p.push_back(word);
                    }
                    break;
                    // 状态1表示读到"->"后有效元素
                case 1:
                    if("|" != word && ";" != word) {
                        tmp.push_back(word);
                        p.push_back(word);
                    }
                    break;
                }
            }
            grammer.push_back(p);
        }
        for(auto t: tmp) {
            if(!vn.count(t)) {
                vt.insert(t);
            }
        }
    }
    /**
     * @brief 化简语法
     * 
     */
    void minor() {
        auto record = min_grammer();
        std::vector<std::string > tmp;
        for(auto i: record) {
            tmp.push_back(Lines[i]);
        }
        Lines = tmp;
    }
    void show() {
        std::cout << std::endl << "--------------------------------VN--------------------------------" << std::endl;
        for(auto item: vn) {
            std::cout << item << std::endl;
        }
        std::cout << std::endl << "--------------------------------VT--------------------------------" << std::endl;
        for(auto item: vt) {
            std::cout << item << std::endl;
        }
        std::cout << std::endl << "--------------------------------grammer--------------------------------" << std::endl;
        for(auto line: Lines) {
            std::cout << line << std::endl;
        }
    }
private:
    std::set<std::string> terminable_tokens() {
        std::set<std::string> s;
        s = vt;
        int isChange = 1;
        // 不断判断，直到集合不改变大小
        while(isChange) {
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
        // 初始化为开始符号
        s.insert(grammer[0][0]);
        int isChange = 1;
        while(isChange) {
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
        return s;
    }

    /**
     * @brief 化简句子，返回的结果为化简前句子行数的索引
     * 
     * @return std::vector<int> 
     */
    std::vector<int> min_grammer() {
        std::vector<int> record;
        auto a = this->reachable_tokens();
        auto b = this->terminable_tokens();
        std::vector<std::vector<std::string>> s;
        std::set<std::string> c;
        std::set_intersection(a.begin(), a.end(), b.begin(), b.end(), std::inserter(c, c.begin()));
        int i = 0;
        // 化简表达式
        for(auto &p: grammer) {
            int state = 0;
            for(auto item: p) {
                if(!c.count(item)) {
                    state = 1;
                    break;
                }
            }
            if(!state) {
                record.push_back(i);
                s.push_back(p);
            }
            ++i;
        }
        grammer = s;
        // 化简vt, VN
        std::set<std::string> tmp;
        std::set_intersection(c.begin(), c.end(), vt.begin(), vt.end(), std::inserter(tmp, tmp.begin()));
        vt = tmp;
        tmp.clear();
        std::set_intersection(c.begin(), c.end(), vn.begin(), vn.end(), std::inserter(tmp, tmp.begin()));
        vn = tmp;
        tmp.clear();
        return record;
    }
};

int main(int, char**) {
    Grammer grammar;
    grammar.read_from_file("../G.txt");
    grammar.minor();
    grammar.show();


    return 0;
}
