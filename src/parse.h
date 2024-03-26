#pragma once

#include <vector>
#include <string>
#include <optional>
#include <iostream>

#include "./tokenize.h"

struct NodeExpr {
    Token int_lit;
};

struct NodeReturn {
    NodeExpr expr;
};

class Parse {
private:
    const std::vector<Token> m_tokens;
    size_t m_index{ 0 };

    auto consume() {
        return m_tokens.at(m_index++);
    }

    [[nodiscard]] std::optional<Token> peek(const int offset = 0) const
    {
        if (m_index + offset >= m_tokens.size()) {
            return {};
        }
        return m_tokens.at(m_index + offset);
    }

public:
    explicit Parse(std::vector<Token> tokens)
        : m_tokens{ std::move(tokens) }
    {}

    std::optional<NodeExpr> parse_expr() {
        if(peek().has_value() && peek().value().type == TokenType::int_lit) {
            return NodeExpr {
                .int_lit = consume()
            };
        }
    }

    std::optional<NodeReturn> parse() {
        std::optional<NodeReturn> return_node;
        while(peek().has_value()) {
            if (peek().value().type == TokenType::_return) {
                consume();
                if (auto node_expr = parse_expr()) {
                    return_node = NodeReturn { .expr = node_expr.value() };
                } else {
                    exit(EXIT_FAILURE);
                }
            } else if (peek(2).has_value() && peek(1).value().type == TokenType::var) {
                
            }
        }
        m_index = 0;
        return return_node;
    }
};