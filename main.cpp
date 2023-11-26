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
    size_t line;  // Додатковий параметр для збереження рядкового номеру лексеми
};

// Клас помилки
class Error {
public:
    std::string message;
    size_t line;

    Error(const std::string& msg, size_t ln) : message(msg), line(ln) {}
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
    } else if (token == "$") {
        return ERROR;  // Treat '$' as ERROR
    } else if (isalpha(token[0]) || token[0] == '_') {
        return IDENTIFIER;
    } else if (token.size() == 1 && ispunct(token[0]) && token != "$") {
        return PUNCTUATION;
    } else {
        return ERROR;  // Default case for unrecognized tokens
    }
}

// Функція для розділення рядка на лексеми
std::pair<std::vector<Token>, std::vector<Error>> tokenize(const std::string& input) {
    std::vector<Token> tokens;
    std::vector<Error> errors;
    std::string token;
    bool inSingleLineComment = false;
    bool inMultiLineComment = false;
    bool inStringLiteral = false;
    size_t currentLine = 1;

    for (size_t i = 0; i < input.size(); ++i) {
        if (input[i] == '\n') {
            ++currentLine;
        }

        if (inSingleLineComment) {
            // Пропуск символів під час однорядкового коментаря
            if (input[i] == '\n' || i == input.size() - 1) {
                inSingleLineComment = false;
                tokens.push_back({token, COMMENT, currentLine});
                token.clear();
            } else {
                token += input[i];
            }
        } else if (inMultiLineComment) {
            // Обробка багаторядкового коментаря
            token += input[i];
            if (input.substr(i, 2) == "*/") {
                inMultiLineComment = false;
                tokens.push_back({token, COMMENT, currentLine});
                token.clear();
                i += 1;
            }
        } else if (inStringLiteral) {
            // Обробка строкового літералу
            token += input[i];
            if (input[i] == '"' && input[i - 1] != '\\') {
                inStringLiteral = false;
                tokens.push_back({token, STRING, currentLine});
                token.clear();
            }
        } else if (isspace(input[i])) {
            // Пропуск пробілів
            if (!token.empty()) {
                TokenType type = getTokenType(token);
                if (type == ERROR) {
                    errors.push_back({token, currentLine});
                } else {
                    tokens.push_back({token, type, currentLine});
                }
                token.clear();
            }
        } else if (input.substr(i, 2) == "//") {
            inSingleLineComment = true;
            token += input[i];
        } else if (input.substr(i, 2) == "/*") {
            inMultiLineComment = true;
            token += input[i];
            tokens.push_back({token, COMMENT, currentLine});
            token.clear();
        } else if (input[i] == '"') {
            inStringLiteral = true;
            token += input[i];
        } else if (ispunct(input[i])) {
            // Додавання символу розділового знаку або оператора
            if (!token.empty()) {
                TokenType type = getTokenType(token);
                if (type == ERROR) {
                    errors.push_back({token, currentLine});
                } else {
                    tokens.push_back({token, type, currentLine});
                }
                token.clear();
            }

            token = input[i];
            TokenType type;

            if (input[i] == '+' || input[i] == '-' || input[i] == '*' || input[i] == '/' || input[i] == '%' ||
                input[i] == '<' || input[i] == '>' || input[i] == '=' || input[i] == '&' || input[i] == '|' ||
                input[i] == '!' || input[i] == '^') {
                type = OPERATOR;
            } else if (input[i] == '$') {
                type = ERROR;
            } else {
                type = PUNCTUATION;
            }

            if (type == ERROR) {
                errors.push_back({token, currentLine});
            } else {
                tokens.push_back({token, type, currentLine});
            }
            token.clear();
        } else {
            token += input[i];
        }
    }

    if (!token.empty()) {
        TokenType type = getTokenType(token);
        if (type == ERROR) {
            errors.push_back({token, currentLine});
        } else {
            tokens.push_back({token, type, currentLine});
        }
    }

    return {tokens, errors};
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
        std::cout << ", Line: " << token.line << " >" << std::endl;
    }
}

// Функція для виводу помилок
void printErrors(const std::vector<Error>& errors) {
    if (errors.empty()) {
        std::cout << "No errors found." << std::endl;
    } else {
        std::cout << "Errors:" << std::endl;
        for (const auto& error : errors) {
            std::cout << "Error: " << error.message << ", Line: " << error.line << std::endl;
        }
    }
}

int main() {
    std::string filename;
    std::cout << "Enter the name of the file containing C++ code: ";
    std::cin >> filename;

    std::ifstream file("../" + filename);

    if (!file.is_open()) {
        std::cerr << "Error opening file " << filename << std::endl;
        return 1;
    }

    std::string input((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    file.close();

    auto result = tokenize(input);
    printTokens(result.first);
    printErrors(result.second);

    return 0;
}
