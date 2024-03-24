#pragma once

#include <vector>
#include <string>
#include <optional>
#include <iostream>

enum class TokenType {
    _return,
    int_lit,
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

        for (const std::string& token : temp) {
            if (token == "повернути") {
                tokens.push_back({ .type = TokenType::_return });
            } else if (token >= "0" && token <= "9999") {
                tokens.push_back({ .type = TokenType::int_lit, .value = token });
            }
        }

        return tokens;
    }
};