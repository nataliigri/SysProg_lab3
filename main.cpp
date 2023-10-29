#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>

// Типи лексем
enum TokenType {
    NUMBER,
    STRING,
    DIRECTIVE,
    IDENTIFIER,
    COMMENT,
    OPERATOR,
    PUNCTUATION,
    ERROR
};

// Клас лексеми
struct Token {
    std::string lexeme;
    TokenType type;
};

// Функція для визначення типу числа
TokenType getNumberType(const std::string& token) {
    bool hasDecimal = false;

    for (char ch : token) {
        if (!isdigit(ch)) {
            if (ch == '.' && !hasDecimal) {
                hasDecimal = true;
            } else {
                return IDENTIFIER;
            }
        }
    }

    return hasDecimal ? NUMBER : IDENTIFIER;

}

// Функція для визначення типу лексеми
TokenType getTokenType(const std::string& token) {
    if (isdigit(token[0])) {
        return getNumberType(token);
    } else if (token.front() == '"' && token.back() == '"') {
        return STRING;
    } else if (token[0] == '#') {
        return DIRECTIVE;
    } else if (token.substr(0, 2) == "//" || token.substr(0, 2) == "/*") {
        return COMMENT;
    } else if (isalpha(token[0]) || token[0] == '_') {
        return IDENTIFIER;
    } else if (token.size() == 1 && ispunct(token[0])) {
        return OPERATOR;
    } else if (ispunct(token[0])) {
        return PUNCTUATION;
    } else {
        return ERROR;
    }
}

// Функція для розділення рядка на лексеми
std::vector<Token> tokenize(const std::string& input) {
    std::vector<Token> tokens;
    std::string token;
    bool inSingleLineComment = false;
    bool inMultiLineComment = false;
    bool inStringLiteral = false;

    for (size_t i = 0; i < input.size(); ++i) {
        if (inSingleLineComment) {
            // Пропуск символів під час однорядкового коментаря
            if (input[i] == '\n' || i == input.size() - 1) {
                inSingleLineComment = false;
                tokens.push_back({token, COMMENT});
                token.clear();
            } else {
                token += input[i];
            }
        } else if (inMultiLineComment) {
            // Обробка багаторядкового коментаря
            token += input[i];
            if (input.substr(i, 2) == "*/") {
                inMultiLineComment = false;
                tokens.push_back({token, COMMENT});
                token.clear();
                i += 1;
            }
        } else if (inStringLiteral) {
            // Обробка строкового літералу
            token += input[i];
            if (input[i] == '"' && input[i - 1] != '\\') {
                inStringLiteral = false;
                tokens.push_back({token, STRING});
                token.clear();
            }
        } else if (isspace(input[i])) {
            // Пропуск пробілів
            if (!token.empty()) {
                TokenType type = getTokenType(token);
                tokens.push_back({token, type});
                token.clear();
            }
        } else if (input.substr(i, 2) == "//") {
            inSingleLineComment = true;
            token += input[i];
        } else if (input.substr(i, 2) == "/*") {
            inMultiLineComment = true;
            token += input[i];
            tokens.push_back({token, COMMENT});
            token.clear();
        } else if (input[i] == '"') {
            inStringLiteral = true;
            token += input[i];
        } else if (ispunct(input[i])) {
            // Додавання символу розділового знаку або оператора
            if (!token.empty()) {
                TokenType type = getTokenType(token);
                tokens.push_back({token, type});
                token.clear();
            }

            token = input[i];
            TokenType type;

            if (input[i] == '+' || input[i] == '-' || input[i] == '*' || input[i] == '/' || input[i] == '%' ||
                input[i] == '<' || input[i] == '>' || input[i] == '=' || input[i] == '&' || input[i] == '|' ||
                input[i] == '!' || input[i] == '^') {
                type = OPERATOR;
            } else {
                type = PUNCTUATION;
            }

            tokens.push_back({token, type});
            token.clear();
        } else {
            token += input[i];
        }
    }

    if (!token.empty()) {
        TokenType type = getTokenType(token);
        tokens.push_back({token, type});
    }

    return tokens;
}

// Функція для виводу результатів
void printTokens(const std::vector<Token>& tokens) {
    for (const auto& token : tokens) {
        std::cout << "< " << token.lexeme << ", ";

        switch (token.type) {
            case NUMBER:
                std::cout << "NUMBER";
                break;
            case STRING:
                std::cout << "STRING";
                break;
            case DIRECTIVE:
                std::cout << "DIRECTIVE";
                break;
            case IDENTIFIER:
                std::cout << "IDENTIFIER";
                break;
            case OPERATOR:
                std::cout << "OPERATOR";
                break;
            case COMMENT:
                std::cout << "COMMENT";
                break;
            case PUNCTUATION:
                std::cout << "PUNCTUATION";
                break;
            case ERROR:
                std::cout << "ERROR";
                break;
        }

        std::cout << " >" << std::endl;
    }
}

int main() {
    std::string filename;
    std::cout << "Enter the name of the file containing C++ code: ";
    std::cin >> filename;

    std::ifstream file("../"+filename);

    if (!file.is_open()) {
        std::cerr << "Error opening file " << filename << std::endl;
        return 1;
    }

    std::string input((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    file.close();

    std::vector<Token> tokens = tokenize(input);
    printTokens(tokens);

    return 0;
}

