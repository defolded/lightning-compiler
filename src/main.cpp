#include <iostream>
#include <fstream>
#include <sstream>
#include <optional>
#include <vector>

enum class TokenType {
    _return,
    int_lit
};

struct Token {
    TokenType type;
    std::optional<std::string> value{};
};

static std::vector<Token> tokenized;

std::vector<Token> tokenize(std::string& str) {
    std::vector<std::string> tokens;
    std::vector<Token> t;

    size_t start = 0;
    for (size_t i = 0; i < str.length(); ++i) {
        if (std::isspace(str[i])) {
            // Token found, extract substring
            if (start != i) { // Check if there's content to extract
                tokens.push_back(str.substr(start, i - start));
            }
            start = i + 1; // Update start for next token
        }
    }

    for (const std::string& token : tokens) {
        if (token == "повернути") {
            t.push_back({ .type = TokenType::_return });
            std::cout << token << " pushed back\n";
        } else if (token >= "0" && token <= "999") {
            t.push_back({ .type = TokenType::int_lit, .value = token });
            std::cout << token << " pushed back\n";
        }
    }

    return t;
}

std::string tokens_to_asm(const std::vector<Token>& tokens) {
    std::stringstream out;
    out << "global _start\n_start:\n";
    for (int i = 0; i < tokens.size(); ++i) {
        const Token& token = tokens.at(i);
        if (token.type == TokenType::_return) {
            if (tokens.at(i + 1).type == TokenType::int_lit) {
                out << "    mov rax, 60\n";
                out << "    mov rdi, " << tokens.at(i + 1).value.value() << '\n';
                out << "    syscall";
            }
        }
    }
    return out.str();
}

int main(int argc, char* argv[]) {
    if (argc != 2)
        return EXIT_FAILURE;

    std::ifstream file(argv[1], std::ios::in);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            tokenized = tokenize(line);
        }
        file.close();
    }

    std::fstream output("out.asm", std::ios::out);
    output << tokens_to_asm(tokenized);
    output.close();


    system("nasm -felf64 out.asm");
    system("ld -o out out.o");

    return EXIT_SUCCESS;
}