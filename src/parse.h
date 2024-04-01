#pragma once

#include <vector>
#include <string>
#include <optional>
#include <iostream>
#include <variant>

#include "./tokenize.h"

struct NodeStmt;


struct NodeExprIntLit {
    Token int_lit;
};

struct NodeExprIdent {
    Token ident;
};

struct NodeExpr {
    std::variant<NodeExprIntLit, NodeExprIdent> var;
};

struct NodeStmtReturn {
    NodeExpr expr;
};

struct NodeStmtEq {
    Token ident;
    NodeExpr expr;
};

struct NodeStmtTernary {
    NodeExpr expr;
    std::vector<NodeStmt> stmts;
};

struct NodeStmt {
    std::variant<NodeStmtReturn, NodeStmtEq, NodeStmtTernary> var;
};

struct NodeProgram {
    std::vector<NodeStmt> stmts;
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
                .var = NodeExprIntLit { .int_lit = consume() }
            };
        } else if (peek().has_value() && peek().value().type == TokenType::ident) {
            return NodeExpr { .var = NodeExprIdent { .ident = consume() } };
        }
        return {};
    }

    std::optional<NodeStmt> parse_stmt() {
        if (peek().has_value() && peek().value().type == TokenType::_return) {
            consume();
            NodeStmtReturn stmt_return;
            if (auto node_expr = parse_expr()) {
                stmt_return = { .expr = node_expr.value() };
            }
            return NodeStmt { .var = stmt_return };
        } else if (peek().has_value() && peek().value().type == TokenType::ident && peek(1).has_value()
                    && peek(1).value().type == TokenType::eq && peek(2).has_value()
                    && peek(2).value().type == TokenType::int_lit) {
            auto stmt_eq = NodeStmtEq { .ident = consume() };
            consume();
            if (auto expr = parse_expr()) {
                stmt_eq.expr = expr.value();
            } else {
                std::cerr << "\n\nParse expression failed.\n\n";
                exit(EXIT_FAILURE);
            }
            return NodeStmt { .var = stmt_eq };
        } else if (peek().has_value() && peek().value().type == TokenType::_if) {
            consume();
            NodeStmtTernary stmt_ternary;
            if (auto node_expr = parse_expr()) {
                stmt_ternary = { .expr = node_expr.value() };
            }
            if (auto stmt = parse_stmt()) {
                stmt_ternary.stmts.push_back(stmt.value());
            }
            return NodeStmt { .var = stmt_ternary };
        } else {
            return {};
        }
    }

    std::optional<NodeProgram> parse_prog() {
        NodeProgram prog;
        while(peek().has_value()) {
            if (auto stmt = parse_stmt()) {
                prog.stmts.push_back(stmt.value());
            } else {
                std::cerr << "\n\nParsing failed.\n\n";
                exit(EXIT_FAILURE);
            }
        }
        return prog;
    }
};