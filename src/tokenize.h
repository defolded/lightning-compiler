#pragma once

#include <vector>
#include <string>
#include <optional>
#include <iostream>

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
            if (temp[i] == "повернути") { // return statement
                std::cout << temp[i] << " pushed back\n";
                tokens.push_back({ .type = TokenType::_return });
            } else if (temp[i] >= "0" && temp[i] <= "9999") { // std::isdigit()
                std::cout << temp[i] << " pushed back\n";
                tokens.push_back({ .type = TokenType::int_lit, .value = temp[i] });
            } else {
                if (i < (temp.size() - 2) && temp[i + 1] == "буде") { // See if the word ahead is a variable declaration
                    std::cout << temp[i] << " pushed back\n";
                    tokens.push_back({ .type = TokenType::ident, .value = temp[i] });
                    std::cout << temp[i + 1] << " pushed back\n";
                    tokens.push_back({ .type = TokenType::eq });
                    std::cout << temp[i + 2] << " pushed back\n";
                    tokens.push_back({ .type = TokenType::int_lit, .value = temp[i + 2] });
                    i += 2;
                } else {
                    std::cout << temp[i] << " pushed back\n";
                    tokens.push_back({ .type = TokenType::ident, .value = temp[i] });
                }
            }
        }

        return tokens;
    }
};