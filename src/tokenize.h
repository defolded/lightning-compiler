#pragma once

#include <vector>
#include <string>
#include <optional>
#include <iostream>

enum class TokenType {
    _return,
    int_lit,
    var,
    ident
};

struct Token {
    TokenType type;
    std::optional<std::string> value{};
};

class Tokenize {
private:
    std::string m_src;

public:
    explicit Tokenize(std::string str)
        : m_src{ std::move(str) }
    {}

    std::vector<Token> tokenizeString() {

        std::vector<Token> tokens;
        std::vector<std::string> temp;

        size_t start = 0;
        for (size_t i = 0; i < m_src.length(); ++i) {
            if (std::isspace(m_src[i]) || m_src[i] == '.') {
                // Token found, extract substring
                if (start != i) { // Check if there's content to extract
                    temp.push_back(m_src.substr(start, i - start));
                }
                start = i + 1; // Update start for next token
            }
        }

//        for (const std::string& token : temp) {
//            if (token == "повернути") {
//                tokens.push_back({ .type = TokenType::_return });
//            } else if (token >= "0" && token <= "9999") {
//                if (tokens+1 = "буде")
//                tokens.push_back({ .type = TokenType::int_lit, .value = token });
//            }
//        }

        for (size_t i = 0; i < temp.size(); ++i) {
            if (temp[i] == "повернути") {
                std::cout << temp[i] << " pushed back\n";
                tokens.push_back({ .type = TokenType::_return });
            } else if (temp[i] >= "0" && temp[i] <= "9999") {
                std::cout << temp[i] << " pushed back\n";
                tokens.push_back({ .type = TokenType::int_lit, .value = temp[i] });
            } else {
                if (i < (temp.size() - 2) && temp[i + 1] == "буде") {
                    std::cout << temp[i] << " pushed back\n";
                    tokens.push_back({ .type = TokenType::ident, .value = temp[i] });
                    tokens.push_back({ .type = TokenType::var });
                    tokens.push_back({ .type = TokenType:: });
                    i += 2;
                }
            }
        }

        return tokens;
    }
};