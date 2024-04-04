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
    ident,
    _if
};

struct Token {
    TokenType type;
    std::optional<std::string> value{};
};

static inline const std::vector<std::string> op {"+", "-", "/", "*", "^"};

std::string substituteVariableForValue(std::vector<std::string>& temp, size_t& i, std::vector<Token>& tokens, size_t& nestedIndex) {
    auto it = temp.begin()-1 + static_cast<int>(nestedIndex);
    int v = 0;
    while (it != temp.begin() - 1) {
        if (*it == temp[nestedIndex]) {
//                    std::cout << temp[(nestedIndex - v - 1) + 2] << " was added to solver (VARIABLE FOUND)\n";
            std::cout << "BITCH IS " << temp[(nestedIndex - v - 1) + 2] << '\n';
            return temp[(nestedIndex - v - 1) + 2];
        } else {
            ++v;
            --it;
        }
    }
}

std::optional<int> calculateExpression(std::vector<std::string>& temp, size_t& i, std::vector<Token>& tokens, int doPushBackToken = 1) {
    std::string in;
    size_t nestedIndex{};
    if (doPushBackToken) {
        nestedIndex = i + 1; // set nestedIndex to the first num in the expr
    } else {
        nestedIndex = 0; // set nestedIndex to the first num in the expr
    }

    while (temp[nestedIndex] != ")") {
        if (temp[nestedIndex] >= "0" && temp[nestedIndex] <= "9999") {
//            std::cout << temp[nestedIndex] << " was added to solver\n";
            in += temp[nestedIndex];
        } else if (std::find(op.begin(), op.end(), temp[nestedIndex]) == op.end()
                   && temp[nestedIndex] != "("
                   && temp[nestedIndex] != ")") {
            std::string r = substituteVariableForValue(temp, i, tokens, nestedIndex);
            in += r;
        } else if (std::find(op.begin(), op.end(), temp[nestedIndex]) != op.end()){
//            std::cout << temp[nestedIndex] << " was added to solver\n";
            in += temp[nestedIndex];
        }
        nestedIndex += 1;
    }

    ShuntingYard::RPN rpn = ShuntingYard::reversePolishNotation(in.c_str());
    ShuntingYard::Node *tree = ShuntingYard::parse(rpn);
    auto res = static_cast<int>(ShuntingYard::eval(tree));

    if (doPushBackToken) {
        std::cout << std::to_string(res) << " pushed back (SOLVER)\n";
        tokens.push_back({ .type = TokenType::int_lit, .value = std::to_string(res) });
        temp.erase(temp.begin() + static_cast<int>(i) + 1, temp.begin() + static_cast<int>(nestedIndex) + 1);
        temp[i] = std::to_string(res);
    } else {
        return res;
    }
}

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
                    tokens.push_back({ .type = TokenType::int_lit, .value = temp[i + 1] });
                    i += 1;
                }
            } else if (temp[i] >= "0" && temp[i] <= "9999") { // std::isdigit()
                std::cout << temp[i] << " pushed back\n";
                tokens.push_back({ .type = TokenType::int_lit, .value = temp[i] });
            } else if (temp[i] == "якщо") {
                std::cout << temp[i] << " pushed back\n";
                tokens.push_back({ .type = TokenType::_if });
            } else if (temp[i] == "то") {
                std::cout << temp[i] << "\n";
            } else if (temp[i] == "повторити") {
                std::cout << temp[i] << "\n";
                // Calculate loop iterations
                ++i;
                calculateExpression(temp, i, tokens);
                ++i;
                std::vector<std::string> expr;
                while (temp[i] != ")") {
                    expr.push_back(temp[i]);
                    ++i;
                }
                expr.emplace_back(")");
                auto j = std::stoi(temp[i-expr.size()]);
                ++i;
                int res{ 0 };
                while (j != 0) {
                    if (auto n = calculateExpression(expr, i, tokens, 0)) {
                        res += *n;
                    }
                    --j;
                }
                std::cout << "hello";


            } else if (temp[i] == "(") {
                calculateExpression(temp, i, tokens);
            } else {
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