#pragma once

#include <vector>
#include <string>
#include <optional>
#include <iostream>

#include "./lib/shunting_yard.hpp"

auto expression(const std::vector<std::string>& tokens, const auto iterator) {
    auto i = iterator;

    std::string in;
    in += tokens[i];

    in += tokens[i + 1];

//    if (tokens[i + 2] >= "0" && tokens[i + 2] <= "9999") {
        in += tokens[i + 2];
//    } else {
//        auto it = std::find(tokens.begin(), tokens.end(), tokens[i + 2]);
//        int index = it - tokens.begin();
//        ShuntingYard::variables[tokens[i + 2]] = std::stoi(tokens[index + 2]);
//    }

    ShuntingYard::RPN rpn = ShuntingYard::reversePolishNotation(in.c_str());
    ShuntingYard::Node *tree = ShuntingYard::parse(rpn);

    return static_cast<int>(ShuntingYard::eval(tree));
}

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
                std::cout << temp[i + 1] << " pushed back\n";
                tokens.push_back({ .type = TokenType::ident, .value = temp[i + 1] });
                i += 2;
            } else if (temp[i] >= "0" && temp[i] <= "9999") { // std::isdigit()
                if (std::find(op.begin(), op.end(), temp[i + 1]) != op.end()) {
                    auto result = expression(temp, i);
                    std::cout << "res: " << result << " pushed back\n";
                    tokens.push_back({ .type = TokenType::int_lit, .value = std::to_string(result) });
                    i += 2;
                } else {
                    std::cout << temp[i] << " pushed back\n";
                    tokens.push_back({ .type = TokenType::int_lit, .value = temp[i] });
                }
            } else {
                if (i < (temp.size() - 2) && temp[i + 1] == "буде") { // See if the word ahead is a variable declaration
                    std::cout << temp[i] << " pushed back\n";
                    tokens.push_back({ .type = TokenType::ident, .value = temp[i] });
                    std::cout << temp[i + 1] << " pushed back\n";
                    tokens.push_back({ .type = TokenType::eq });
                    if (temp[i + 2] == "(") {
                        // we have an expression here
                        std::string in;
                        size_t nestedIndex = i + 3; // set nestedIndex to the first item in the expr
                        while (temp[nestedIndex] != ")") {
                            if (temp[nestedIndex] >= "0" && temp[nestedIndex] <= "9999") {
                                std::cout << temp[nestedIndex] << " was added to solver\n";
                                in += temp[nestedIndex];
                            } else if (std::find(op.begin(), op.end(), temp[nestedIndex]) == op.end()
                                        && temp[nestedIndex] != "("
                                        && temp[nestedIndex] != ")") {
                                auto it = std::find(temp.rbegin(), temp.rend() - 1, temp[nestedIndex]);
                                int previous_index = std::distance(temp.rbegin(), it);
                                std::cout << temp[previous_index] << " was added to solver (VARIABLE FOUND)\n";
                                in += temp[previous_index];
                            } else if (std::find(op.begin(), op.end(), temp[nestedIndex]) != op.end()){
                                std::cout << temp[nestedIndex] << " was added to solver\n";
                                in += temp[nestedIndex];
                            }

                            nestedIndex += 1;
                        }

                        ShuntingYard::RPN rpn = ShuntingYard::reversePolishNotation(in.c_str());
                        ShuntingYard::Node *tree = ShuntingYard::parse(rpn);
                        auto res = static_cast<int>(ShuntingYard::eval(tree));
                        tokens.push_back({ .type = TokenType::int_lit, .value = std::to_string(res) });

//                        std::cout << "i index is: " << i << " nested index is: " << nestedIndex << "\n\n\n";
                        i = nestedIndex;
                    } else {
                        std::cout << temp[i + 2] << " pushed back\n";
                        tokens.push_back({ .type = TokenType::int_lit, .value = temp[i + 2] });
                        i += 2;
                    }
//                    if (i < (temp.size() - 4)) {
//                        if (std::find(op.begin(), op.end(), temp[i + 3]) == op.end()) {
//
//                        } else if (std::find(temp.begin(), temp.begin() + static_cast<int>(i), temp[i + 2]) != temp.end()
//                                    && temp[i] <= "0" && temp[i] >= "9999") {
//                            std::cout << "here is + 2: " << temp[i+2] << '\n';
//                            auto result = expression(temp, i);
//                            std::cout << "res: " << result << " pushed back\n";
//                            tokens.push_back({.type = TokenType::int_lit, .value = std::to_string(result)});
//                        }
//                        i += 2;
//                    }
//                    i += 1;
                }
            }
        }

        return tokens;
    }
};