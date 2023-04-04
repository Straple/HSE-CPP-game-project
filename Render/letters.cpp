﻿const char* small_letters[][7] = {
    "",
    "",
    " 000", // a
    "    0",
    " 0000",
    "0   0",
    " 0000",

    "0", // b
    "0",
    "0 00",
    "00  0",
    "0   0",
    "0   0",
    "0000",

    "",
    "",
    " 000", // c
    "0",
    "0",
    "0   0",
    " 000",

    "    0", // d
    "    0",
    " 00 0",
    "0  00",
    "0   0",
    "0   0",
    " 0000",

    "",
    "",
    " 000", // e
    "0   0",
    "00000",
    "0",
    " 000",

    "  00", // f
    " 0  0",
    " 0",
    "000",
    " 0",
    " 0",
    " 0",

    " 0000", // g
    "0   0",
    "0   0",
    " 0000",
    "    0",
    "    0",
    " 000",

    "0", // h
    "0",
    "0 00",
    "00  0",
    "0   0",
    "0   0",
    "0   0",

    "  0", // i
    "",
    "  0",
    "  0",
    "  0",
    "  0",
    "00000",

    "    0", // j
    "",
    "  000",
    "    0",
    "    0",
    " 0  0",
    "  00",

    "0", // k
    "0",
    "0  0 ",
    "0 0",
    "00",
    "0 0",
    "0  0",

    "000", // l
    "  0",
    "  0",
    "  0",
    "  0",
    "  0",
    "00000",

    "",
    "",
    "00 0", // m
    "0 0 0",
    "0 0 0",
    "0   0",
    "0   0",

    "",
    "",
    "0 00", // n
    "00  0",
    "0   0",
    "0   0",
    "0   0",

    "",
    "",
    " 000", // o
    "0   0",
    "0   0",
    "0   0",
    " 000",

    "0000", // p
    "0   0",
    "0   0",
    "0000",
    "0",
    "0",
    "0",

    " 000", // q
    "0   0",
    "0   0",
    "0   0",
    " 000",
    "   0",
    "  0",

    "",
    "",
    "0 00", // r
    "00  0",
    "0",
    "0",
    "0",

    "",
    "",
    " 000", // s
    "0",
    " 000",
    "    0",
    "0000",

    " 0", // t
    " 0",
    "0000",
    " 0",
    " 0",
    " 0",
    "  000",

    "",
    "",
    "0   0", // u
    "0   0",
    "0   0",
    "0   0",
    " 000",

    "",
    "",
    "0   0", // v
    "0   0",
    "0   0",
    " 0 0",
    "  0",

    "",
    "",
    "0   0", // w
    "0   0",
    "0 0 0",
    "0 0 0",
    " 0 0",

    "",
    "",
    "0   0", // x
    " 0 0",
    "  0",
    " 0 0",
    "0   0",

    "0   0", // y
    "0   0",
    "0   0",
    " 0000",
    "    0",
    "    0",
    " 000",

    "",
    "",
    "00000", // z
    "   0",
    "  0",
    " 0",
    "00000",
};
const char* letters[][7] = {
    " 000", // A
    "0   0",
    "0   0",
    "0   0",
    "00000",
    "0   0",
    "0   0",

    "0000", // B
    "0   0",
    "0   0",
    "0000",
    "0   0",
    "0   0",
    "0000",

    " 000", // C
    "0   0",
    "0",
    "0",
    "0",
    "0   0",
    " 000",


    "0000", // D
    "0   0",
    "0   0",
    "0   0",
    "0   0",
    "0   0",
    "0000",

    "00000", // E
    "0",
    "0",
    "0000",
    "0",
    "0",
    "00000",

    "00000", // F
    "0",
    "0",
    "0000",
    "0",
    "0",
    "0",

    " 000", // G
    "0   0",
    "0",
    "0 000",
    "0   0",
    "0   0",
    " 000",

    "0   0", // H
    "0   0",
    "0   0",
    "00000",
    "0   0",
    "0   0",
    "0   0",

    "00000", // I
    "  0",
    "  0",
    "  0",
    "  0",
    "  0",
    "00000",

    "  000", // J
    "    0",
    "    0",
    "    0",
    "    0",
    "0   0",
    " 000",

    "0   0", // K
    "0  0",
    "0 0",
    "00",
    "0 0",
    "0  0",
    "0   0",

    "0", // L
    "0",
    "0",
    "0",
    "0",
    "0",
    "00000",


    "0   0", // M
    "00 00",
    "0 0 0",
    "0   0",
    "0   0",
    "0   0",
    "0   0",

    "0   0", // N
    "0   0",
    "00  0",
    "0 0 0",
    "0  00",
    "0   0",
    "0   0",


    " 000 ", // O
    "0   0",
    "0   0",
    "0   0",
    "0   0",
    "0   0",
    " 000",


    "0000", // P
    "0   0",
    "0   0",
    "0000",
    "0",
    "0",
    "0",

    " 000", // Q
    "0   0",
    "0   0",
    "0   0",
    "0 0 0",
    "0  0",
    " 00 0",


    "0000", // R
    "0   0",
    "0   0",
    "0000",
    "0 0",
    "0  0",
    "0   0",


    " 0000", // S
    "0",
    "0",
    " 000",
    "    0",
    "    0",
    "0000",


    "00000", // T
    "  0",
    "  0",
    "  0",
    "  0",
    "  0",
    "  0",

    "0   0", // U
    "0   0",
    "0   0",
    "0   0",
    "0   0",
    "0   0",
    " 000",


    "0   0", // V
    "0   0",
    "0   0",
    "0   0",
    " 0 0",
    " 0 0",
    "  0",

    "0   0", // W
    "0   0",
    "0   0",
    "0 0 0",
    "0 0 0",
    "0 0 0",
    " 0 0",


    "0   0", // X
    "0   0",
    " 0 0",
    "  0",
    " 0 0",
    "0   0",
    "0   0",


    "0   0", // Y
    "0   0",
    "0   0",
    " 0 0",
    "  0",
    "  0",
    "  0",

    "00000", // Z
    "    0",
    "   0",
    "  0",
    " 0",
    "0",
    "00000",
};
const char* digits[][7] = {
    " 000", // 0
    "0   0",
    "0  00",
    "0 0 0",
    "00  0",
    "0   0",
    " 000",

    "  0", // 1
    " 00",
    "  0",
    "  0",
    "  0",
    "  0",
    "00000",

    " 000", // 2
    "0   0",
    "    0",
    "   0",
    "  0",
    " 0",
    "00000",

    " 000", // 3
    "0   0",
    "    0",
    "  00",
    "    0",
    "0   0",
    " 000",

    "   0", // 4
    "  00",
    " 0 0",
    "0  0",
    "00000",
    "   0",
    "   0",

    "00000", // 5
    "0",
    "0000",
    "    0",
    "    0",
    "0   0",
    " 000",

    " 000", // 6
    "0   0",
    "0",
    "0000",
    "0   0",
    "0   0",
    " 000",

    "00000", // 7
    "    0",
    "    0",
    "   0",
    "   0",
    "  0",
    "  0",

    " 000", // 8
    "0   0",
    "0   0",
    " 000",
    "0   0",
    "0   0",
    " 000",

    " 000", // 9
    "0   0",
    "0   0",
    " 0000",
    "    0",
    "0   0",
    " 000",
};
const char* symbols[][7] = {
    "", // -
    "",
    "",
    " 000 ",
    "",
    "",
    "",

    "", // +
    "",
    "  0",
    " 000 ",
    "  0",
    "",
    "",

    "", // .
    "",
    "",
    "",
    "",
    "",
    " 0 ",

    "  0  ", // !
    "  0  ",
    "  0  ",
    "  0  ",
    "  0  ",
    "",
    "  0  ",

    " 000", // ?
    "0   0",
    "    0",
    "   0",
    "  0",
    "",
    "  0",

    "  0  ", // |
    "  0  ",
    "  0  ",
    "  0  ",
    "  0  ",
    "  0  ",
    "  0  ",

    "    00", // (
    "   0  ",
    "  0   ",
    "  0   ",
    "  0   ",
    "   0  ",
    "    00",

    "00    ", // )
    "  0   ",
    "   0  ",
    "   0  ",
    "   0  ",
    "  0   ",
    "00    ",

    "", // ,
    "",
    "",
    "",
    " 00 ",
    "  0 ",
    " 0  ",
};
const char* unused_character[7] = {
    "00000",
    "0 0 0",
    "0 0 0",
    "00000",
    "00 00",
    "0 0 0",
    "00000",
};

const char **get_symbol(char symbol) {
    if (is_between('a', symbol, 'z')) {  // small letter
        return small_letters[symbol - 'a'];
    } else if (is_between('A', symbol, 'Z')) {  // letter
        return letters[symbol - 'A'];
    } else if (is_between('0', symbol, '9')) {  // digit
        return digits[symbol - '0'];
    } else {  // symbol
        switch (symbol) {
            case '-': {
                return symbols[0];
            }
            case '+': {
                return symbols[1];
            }
            case '.': {
                return symbols[2];
            }
            case '!': {
                return symbols[3];
            }
            case '?': {
                return symbols[4];
            }
            case '|': {
                return symbols[5];
            }
            case '(': {
                return symbols[6];
            }
            case ')': {
                return symbols[7];
            }
            case ',': {
                return symbols[8];
            }
            default: {
                return unused_character;
            }
        }
    }
}

int symbol_len_in_pixels(char symbol) {
    if (symbol == ' ') {
        return 4;
    } else {
        int right_x = -1;

        auto memory = get_symbol(symbol);
        for (int i = 0; i < 7; i++) {
            right_x =
                std::max<int>(right_x, static_cast<int>(strlen(memory[i])));
        }
        return right_x + 1;
    }
}

int text_len_in_pixels(const char *text) {
    int max_len = 0;
    int len = 0;
    for (; *text; text++) {
        if (*text == '\n') {
            max_len = std::max(max_len, len);
            len = 0;
        } else {
            len += symbol_len_in_pixels(*text);
        }
    }
    max_len = std::max(max_len, len);
    return max_len;
}

/*int sprite_len(sprite_t sprite) {
    int right_x = -1;
    auto &pixels = Sprites[sprite];

    for (int i = 0; i < pixels.height(); i++) {
        for (int j = 0; j < pixels.width(); j++) {
            if (is_draw(pixels[i][j]) && right_x < j) {
                right_x = j;
            }
        }
    }
    return right_x + 2;
}*/
