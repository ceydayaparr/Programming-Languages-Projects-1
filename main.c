#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define MAX_INT_SIZE 8
#define MAX_IDENTIFIER_SIZE 10
#define MAX_TOKEN_LENGTH 258
int isKeyword(const char *identifier);
void Identifiers(FILE *sta, FILE *lex, char char_);
void IntConsts(FILE *sta, FILE *lex, char char_);
void Comments(FILE *sta, FILE *lex, char char_);
void Operators(FILE *sta, FILE *lex, char char_);
void StringConsts(FILE *sta, FILE *lex, char char_);
void Brackets(FILE *lex, char char_);
void EndOfLine(FILE *lex, char char_);
void Comma(FILE *lex, char char_);

int main() {
    char char_;

    FILE *sta= fopen("code.sta", "r");
    FILE *lex = fopen("code.lex", "w");

    if (!lex) {
        printf("Error: Cannot open output file code.lex\n");
        return 1;
    }
    if (!sta) {
        printf("Error: Cannot open source file code.sta\n");
        return 1;
    }


    while ((char_ = fgetc(sta)) != EOF) {
        if (isalpha(char_)) {
            Identifiers(sta, lex, char_);
        } else if (isdigit(char_)) {
            IntConsts(sta, lex, char_);
        } else if (char_ == '/') {
            Comments(sta, lex, char_);
        }else if (char_ == '+' || char_ == '-' || char_ == '*' || char_ == '/' || char_ == ':') {
            Operators(sta, lex, char_);
        } else if (char_ == '"') {
            StringConsts(sta, lex, char_);
        } else if (char_ == '(' || char_ == '[' || char_ == '{' || char_ == ')' || char_ == ']' || char_ == '}') {
            Brackets(lex, char_);
        } else if (char_ == ';') {
            EndOfLine(lex, char_);
        }
    }

    fclose(sta);
    fclose(lex);

    return 0;
}

int isKeyword(const char *identifier) {
    const char *keywords[] = {
            "int", "text", "is", "loop", "times", "read", "write", "newLine"
    };
    int num_keywords = sizeof(keywords) / sizeof(char *);

    for (int i = 0; i < num_keywords; i++) {
        if (strcmp(identifier, keywords[i]) == 0) {
            return 1;
        }
    }

    return 0;
}

void Identifiers(FILE *sta, FILE *lex, char char_) {
    int count = 0;
    char identifier[MAX_IDENTIFIER_SIZE + 1] = "";

    while (isalnum(char_) || char_ == '\0') {
        if (count < MAX_IDENTIFIER_SIZE) {
            identifier[count] = tolower(char_);
        }
        count++;
        char_ = fgetc(sta);
    }

    ungetc(char_, sta); // put back the non-alphanumeric character

    if (count > MAX_IDENTIFIER_SIZE) {
        fprintf(lex, "Error: Identifier maximum limit of 30 characters\n", identifier);
    } else if (isKeyword(identifier)) {
        fprintf(lex, "Token: Keyword(%s)\n", identifier);
    } else {
        fprintf(lex, "Token: Identifier(%s)\n", identifier);
    }
}

void IntConsts(FILE *sta, FILE *lex, char char_) {
    int count = 0;
    char integer_const[MAX_INT_SIZE + 1] = "";

    while (isdigit(char_)) {
        if (count < MAX_INT_SIZE) {
            integer_const[count] = char_;
        }
        count++;
        char_ = fgetc(sta);
    }

    ungetc(char_, sta); // put back the non-digit character

    if (count > MAX_INT_SIZE) {
        fprintf(lex, "Error: Integer constant maximum limit of 10 digits\n", integer_const);
    } else {
        fprintf(lex, "Token: IntConst(%s)\n", integer_const);
    }
}

void Comments(FILE *sta, FILE *lex, char char_) {
    char next_char_ = fgetc(sta);
    if (next_char_ == '*') {
        do {
            while ((char_ = fgetc(sta)) != '*') {
                if (char_ == EOF) {
                    fprintf(lex, "Error: Comment doesn't terminate before end of file\n");
                    return;
                }
            }
            char_ = fgetc(sta);
            if (char_ == EOF) {
                fprintf(lex, "Error: Comment doesn't terminate before end of file\n");
                return;
            }
        } while (char_ != '/');
    } else {
        ungetc(next_char_, sta);
    }
}

void Operators(FILE *sta, FILE *lex, char char_) {
    char operator[3] = "";
    operator[0] = char_;
    operator[1] = fgetc(sta);

    if (operator[1] != '+' && operator[1] != '=' && operator[1] != '-') {
        ungetc(operator[1], sta); // put back the non-operator character
        operator[1] = '\0'; // null
    }

    fprintf(lex, "Token: Operator(%s)\n", operator);
}

void StringConsts(FILE *sta, FILE *lex, char char_) {
    char string_const[9999] = "";
    int count = 0;

    while ((char_ = fgetc(sta)) != '"') {
        if (char_ == EOF) {
            fprintf(lex, "Error: String constant doesn't terminate before end of file.\n");
            return;
        }
        string_const[count++] = char_;
    }

    string_const[count] = '\0'; // null
    fprintf(lex, "Token: StringConst(%s)\n", string_const);
}

void Brackets(FILE *lex, char char_) {
    const char *token_;
    switch (char_) {
        case '{':
            token_ = "LeftCurlyBracket";
            break;
        case '}':
            token_ = "RightCurlyBracket";
            break;
        default:
            return;
    }
    fprintf(lex, "Token: %s\n", token_);
}

void Comma(FILE *lex, char char_) {
    const char *token_ = "Comma";
    fprintf(lex, "Token: %s\n", token_);
}



void EndOfLine(FILE *lex, char char_) {
    if (char_ == ';') {
        fprintf(lex, "Token: EndOfLine\n");
    }
}

