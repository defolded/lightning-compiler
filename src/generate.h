#pragma once

#include <string>
#include <sstream>

#include "parse.h"

class Generate {
private:
    const NodeReturn m_root;

public:
    explicit Generate(NodeReturn root)
        : m_root{ std::move(root) }
    {}

    [[nodiscard]] std::string generate() const {
         std::stringstream out;
        out << "global _start\n_start:\n";
        out << "    mov rax, 60\n";
        out << "    mov rdi, " << m_root.expr.int_lit.value.value() << '\n';
        out << "    syscall";
        return out.str();
    }
};