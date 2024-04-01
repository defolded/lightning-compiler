#pragma once

#include <string>
#include <sstream>
#include <ranges>
#include <cassert>

#include "parse.h"

class Generate {
private:
    const NodeProgram m_prog;
    std::stringstream m_out;
    size_t m_stack_size { 0 };
    int m_label_count { 0 };

    void push(const std::string& reg) {
        m_out << "    push " << reg << '\n';
        ++m_stack_size;
    }

    void pop(const std::string& reg) {
        m_out << "    pop " << reg << '\n';
        --m_stack_size;
    }

    std::string create_label() {
        std::stringstream ss;
        ss << "label" << m_label_count++;
        return ss.str();
    }

    struct Var {
        std::string name;
        size_t stack_loc;
    };

    std::vector<Var> m_vars{};

public:
    explicit Generate(NodeProgram prog)
        : m_prog{ std::move(prog) }
    {}

    void generate_expr(const NodeExpr& expr) {
        struct ExprVisitor {
            Generate* g;

           void operator()(const NodeExprIntLit& expr_int_lit) const {
               g->m_out << "    mov rax, " << expr_int_lit.int_lit.value.value() << '\n';
               g->push("rax");
            }

            void operator()(const NodeExprIdent& expr_ident) const {
                const auto it = std::ranges::find_if(g->m_vars, [&](const Var& var) {
                    return var.name == expr_ident.ident.value.value();
                });
               if (it == g->m_vars.end()) {
                   std::cerr << "Cannot find value in expr_ident\n";
                   exit(EXIT_FAILURE);
               }
               std::stringstream ss;
               ss << "QWORD [rsp + " << (g->m_stack_size - it->stack_loc - 1) * 8 << "]\n";
               g->push(ss.str());
            }
        };

        ExprVisitor visitor{ .g = this };
        std::visit(visitor, expr.var);
    }

    void generate_stmts(const NodeStmt& stmt_return) {
        struct StmtVisitor {
            Generate* g;

            void operator()(const NodeStmtReturn& stmt_return) const {
                g->generate_expr(stmt_return.expr);
                g->m_out << "    mov rax, 60\n";
                g->pop("rdi");
                g->m_out << "    syscall\n";
            }

            void operator()(const NodeStmtEq& stmt_eq) const {
                g->m_vars.push_back({ .name = stmt_eq.ident.value.value(), .stack_loc = g->m_stack_size });
                const auto it = std::ranges::find_if(g->m_vars, [&](const Var& var) {
                    return var.name == stmt_eq.ident.value.value();
                });
                if (it == g->m_vars.end()) {
                    std::cerr << "Cannot find value in stmt_eq\n";
                    exit(EXIT_FAILURE);
                }

                g->m_vars.push_back({ .name = stmt_eq.ident.value.value(), .stack_loc = g->m_stack_size });
                g->generate_expr(stmt_eq.expr);
            }
            void operator()(const NodeStmtTernary& stmt_ternary) const {
                g->generate_expr(stmt_ternary.expr);
                g->pop("rax");
                const std::string label = g->create_label();
                g->m_out << "    test rax, rax\n";
                g->m_out << "    jz " << label << '\n';
                for (const NodeStmt& stmt : stmt_ternary.stmts) {
                    g->generate_stmts(stmt);
                }
                g->m_out << label << ":\n";
            }
        };

        StmtVisitor visitor{ .g = this };
        std::visit(visitor, stmt_return.var);
    }

    [[nodiscard]] std::string generate() {
        m_out << "global _start\n_start:\n";

        for (const NodeStmt& stmt : m_prog.stmts) {
            generate_stmts(stmt);
        }

        m_out << "    mov rax, 60\n";
        m_out << "    mov rdi, 0\n";
        m_out << "    syscall\n";
        return m_out.str();
    }
};