#include <iostream>
#include <string>
#include <queue>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <locale>

using namespace std;

// Custom function to read lines of wstring from a wifstream
std::wstring wgetline(std::wifstream& wif) {
    std::wstring line;
    std::getline(wif, line);
    return line;
}

class ACNode {
public:
    bool isEndOfWord;
    unordered_map<wchar_t, ACNode*> children;
    ACNode* failure;
    wstring word;

    ACNode() {
        isEndOfWord = false;
        failure = nullptr;
        word = L"";
    }
};

class ACFilter {
private:
    ACNode* root;

    void insertWord(const wstring& word) {
        ACNode* current = root;

        for (wchar_t c : word) {
            if (current->children.find(c) == current->children.end()) {
                current->children[c] = new ACNode();
            }
            current = current->children[c];
        }

        current->isEndOfWord = true;
        current->word = word;
    }

    void buildFailureLinks() {
        queue<ACNode*> q;

        for (auto& pair : root->children) {
            ACNode* child = pair.second;
            child->failure = root;
            q.push(child);
        }

        while (!q.empty()) {
            ACNode* current = q.front();
            q.pop();

            for (auto& pair : current->children) {
                wchar_t c = pair.first;
                ACNode* child = pair.second;
                ACNode* failure = current->failure;

                while (failure != nullptr && failure->children.find(c) == failure->children.end()) {
                    failure = failure->failure;
                }

                if (failure != nullptr) {
                    child->failure = failure->children[c];
                }
                else {
                    child->failure = root;
                }

                q.push(child);
            }
        }
    }

    void replaceSensitiveWords(wstring& text) {
        for (size_t i = 0; i < text.length();) {
            ACNode* current = root;
            size_t j = i;
            size_t wordLength = 0;

            while (j < text.length() && current->children.find(text[j]) != current->children.end()) {
                current = current->children[text[j]];
                wordLength++;

                if (current->isEndOfWord) {
                    for (size_t k = i; k < i + wordLength; k++) {
                        text[k] = L'*';
                    }
                    break;
                }

                j++;
            }

            // 如果没有找到敏感词，则移动到下一个字符
            if (wordLength == 0) {
                i++;
            }
            else {
                i += wordLength;
            }
        }
    }

public:
    ACFilter() {
        root = new ACNode();
    }

    void addSensitiveWord(const wstring& word) {
        insertWord(word);
    }

    void initializeFromFile() {
        wifstream file("sensitive_words_lines.txt"); // Use wifstream for wide characters
        if (!file) {
            wcout << L"Error opening file: sen.txt" << endl;
            return;
        }

        wstring word;
        while (!file.eof()) {
            word = wgetline(file);
            if (!word.empty())
                addSensitiveWord(word);
        }
    }

    wstring filterText(const wstring& text) {
        wstring filteredText = text;
        replaceSensitiveWords(filteredText);
        return filteredText;
    }
};

int main() {
    ACFilter filter;
    locale::global(locale("C"));
    wcout.imbue(locale());
    //std::locale::global(std::locale("C"));


    filter.initializeFromFile();

    while (true) {
        wstring text;
        wcout << L"请输入一句话（输入q退出）：";
        getline(wcin, text);

        if (text == L"q") {
            break;
        }

        wstring filteredText = filter.filterText(text);
        wcout << L"过滤后的结果：" << filteredText << endl;
    }

    return 0;
}
