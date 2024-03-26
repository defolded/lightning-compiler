#pragma once

#include <string>
#include <sstream>
#include <unordered_map>

#include "parse.h"

class Generate {
private:
    const NodeProgram m_prog;
    std::stringstream m_out;
    size_t m_stack_size { 0 };

    void push(const std::string& reg) {
        m_out << "    push " << reg << '\n';
        ++m_stack_size;
    }

    void pop(const std::string& reg) {
        m_out << "    pop " << reg << '\n';
        --m_stack_size;
    }

    struct Var {
        size_t stack_loc;
    };

    std::unordered_map<std::string, Var> m_vars{};

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
               if (!g->m_vars.contains(expr_ident.ident.value.value())) {
                   exit(EXIT_FAILURE);
               }
               const auto& v = g->m_vars.at(expr_ident.ident.value.value());
               std::stringstream ss;
               ss << "QWORD [rsp + " << (g->m_stack_size - v.stack_loc - 1) * 8 << "]\n";
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
                if (g->m_vars.contains(stmt_eq.ident.value.value())) {
                    exit(EXIT_FAILURE);
                }
                g->m_vars.insert({ stmt_eq.ident.value.value(), Var { .stack_loc = g->m_stack_size }});
                g->generate_expr(stmt_eq.expr);
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