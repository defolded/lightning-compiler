#include <iostream>
#include <fstream>
#include <sstream>
#include <optional>
#include <vector>

#include "generate.h"
#include "parse.h"
#include "tokenize.h"

int main(int argc, char* argv[]) {
    if (argc != 2)
        return EXIT_FAILURE;

    std::string contents;
    std::stringstream contents_stream;
    std::fstream input(argv[1], std::ios::in);
    contents_stream << input.rdbuf();
    contents = contents_stream.str();
    input.close();

    Tokenize tokenize(std::move(contents));
    std::vector<Token> tokens = tokenize.tokenizeString();

    Parse parse(std::move(tokens));
    std::optional<NodeReturn> tree = parse.parse();

    if (!tree.has_value()) {
        exit(EXIT_FAILURE);
    }

    Generate generate(tree.value());
    std::fstream file("out.asm", std::ios::out);
    file << generate.generate();
    file.close();

    system("nasm -felf64 out.asm");
    system("ld -o out out.o");

    return EXIT_SUCCESS;
}