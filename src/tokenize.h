#pragma once

#include <vector>
#include <string>
#include <optional>
#include <iostream>

#include "./lib/shunting_yard.hpp"

enum class TokenType {
    _return,
    int_lit,
    eq,
    ident
};

struct Token {
    TokenType type;
    std::optional<std::string> value{};
};

class Tokenize {
private:
    std::string m_src;
    const std::vector<std::string> op {"+", "-", "/", "*", "^"};

public:
    explicit Tokenize(std::string str)
        : m_src{ std::move(str) }
    {}

    std::vector<Token> tokenizeString() {

        std::vector<Token> tokens;
        std::vector<std::string> temp;

        size_t start = 0;
        for (size_t i = 0; i < m_src.length(); ++i) {
            if (std::isspace(m_src[i]) || m_src[i] == '.' || m_src[i] == ',') {
                // Token found, extract substring
                if (start != i) { // Check if there's content to extract
                    temp.push_back(m_src.substr(start, i - start));
                }
                start = i + 1; // Update start for next token
            }
        }

        for (size_t i = 0; i < temp.size(); ++i) {
            if (temp[i] == "повернути") { // end program statement
                std::cout << temp[i] << " pushed back\n";
                tokens.push_back({ .type = TokenType::_return });
                if (temp[i + 1] != "(") {
                    std::cout << temp[i + 1] << " pushed back\n";
                    tokens.push_back({ .type = TokenType::ident, .value = temp[i + 1] });
                    i += 2;
                }
            } else if (temp[i] >= "0" && temp[i] <= "9999") { // std::isdigit()
                std::cout << temp[i] << " pushed back\n";
                tokens.push_back({ .type = TokenType::int_lit, .value = temp[i] });
            } else if (temp[i] == "(") {
                std::string in;
                size_t nestedIndex = i + 1; // set nestedIndex to the first num in the expr
                while (temp[nestedIndex] != ")") {
                    if (temp[nestedIndex] >= "0" && temp[nestedIndex] <= "9999") {
                        std::cout << temp[nestedIndex] << " was added to solver\n";
                        in += temp[nestedIndex];
                    } else if (std::find(op.begin(), op.end(), temp[nestedIndex]) == op.end()
                               && temp[nestedIndex] != "("
                               && temp[nestedIndex] != ")") {
                        auto it = temp.begin()-1 + static_cast<int>(nestedIndex);
                        int v = 0;
                        while (it != temp.begin() - 1) {
                            if (*it == temp[nestedIndex]) {
                                std::cout << temp[(nestedIndex - v - 1) + 2] << " was added to solver (VARIABLE FOUND)\n";
                                in += temp[(nestedIndex - v - 1) + 2];
                                break;
                            } else {
                                ++v;
                                --it;
                            }
                        }
                    } else if (std::find(op.begin(), op.end(), temp[nestedIndex]) != op.end()){
                        std::cout << temp[nestedIndex] << " was added to solver\n";
                        in += temp[nestedIndex];
                    }
                    nestedIndex += 1;
                }

                ShuntingYard::RPN rpn = ShuntingYard::reversePolishNotation(in.c_str());
                ShuntingYard::Node *tree = ShuntingYard::parse(rpn);
                auto res = static_cast<int>(ShuntingYard::eval(tree));
                std::cout << std::to_string(res) << " pushed back\n";
                tokens.push_back({ .type = TokenType::int_lit, .value = std::to_string(res) });
                temp.erase(temp.begin() + static_cast<int>(i) + 1, temp.begin() + static_cast<int>(nestedIndex) + 1);
                temp[i] = std::to_string(res);
            }
            else {
                if (i < (temp.size() - 2) && temp[i + 1] == "буде") { // See if the word ahead is a variable declaration
                    std::cout << temp[i] << " pushed back\n";
                    tokens.push_back({ .type = TokenType::ident, .value = temp[i] });
                    std::cout << temp[i + 1] << " pushed back\n";
                    tokens.push_back({ .type = TokenType::eq });
                    if (temp[i + 2] != "(") {
                        std::cout << temp[i + 2] << " pushed back\n";
                        tokens.push_back({ .type = TokenType::int_lit, .value = temp[i + 2] });
                        i += 2;
                    }
                }
            }
        }

        return tokens;
    }
};