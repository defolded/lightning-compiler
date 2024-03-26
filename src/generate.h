#pragma once

#include <string>
#include <sstream>

#include "parse.h"

class Generate {
private:
    const NodeProgram m_prog;
    std::stringstream m_out;

public:
    explicit Generate(NodeProgram prog)
        : m_prog{ std::move(prog) }
    {}

    void generate_expr(const NodeExpr& expr) {
        struct ExprVisitor {
            Generate* g;

           void operator()(const NodeExprIntLit& expr_int_lit) const {
               g->m_out << "    mov rax, " << expr_int_lit.int_lit.value.value() << '\n';
               g->m_out << "    push rax\n";
            }

            void operator()(const NodeExprIdent& expr_ident) {

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
                g->m_out << "    pop rdi\n";
                g->m_out << "    syscall\n";
            }

            void operator()(const NodeStmtEq& stmt_let) {

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