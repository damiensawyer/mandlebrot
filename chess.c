#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BOARD_SIZE 8

typedef struct {
    char board[BOARD_SIZE][BOARD_SIZE];
} ChessBoard;

void initBoard(ChessBoard *chessBoard) {
    const char *initialBoard[BOARD_SIZE] = {
        "rnbqkbnr",
        "pppppppp",
        "        ",
        "        ",
        "        ",
        "        ",
        "PPPPPPPP",
        "RNBQKBNR"
    };

    for (int i = 0; i < BOARD_SIZE; ++i) {
        strncpy(chessBoard->board[i], initialBoard[i], BOARD_SIZE);
    }
}

void printBoard(const ChessBoard *chessBoard) {
    printf("  a b c d e f g h\n");
    for (int i = 0; i < BOARD_SIZE; ++i) {
        printf("%d ", BOARD_SIZE - i);
        for (int j = 0; j < BOARD_SIZE; ++j) {
            printf("%c ", chessBoard->board[i][j]);
        }
        printf("\n");
    }
}

int isValidMove(const ChessBoard *chessBoard, int startX, int startY, int endX, int endY) {
    if (startX < 0 || startX >= BOARD_SIZE || startY < 0 || startY >= BOARD_SIZE ||
        endX < 0 || endX >= BOARD_SIZE || endY < 0 || endY >= BOARD_SIZE) {
        return 0;
    }

    if (startX == endX && startY == endY) {
        return 0;
    }

    char piece = chessBoard->board[startX][startY];
    char target = chessBoard->board[endX][endY];

    if (piece == ' ' || (target != ' ' && ((piece >= 'a' && target >= 'a') || (piece <= 'Z' && target <= 'Z')))) {
        return 0;
    }

    int dx = endX - startX;
    int dy = endY - startY;

    switch (tolower(piece)) {
        case 'p':
            if (piece == 'P') {
                if ((dx == -1 && dy == 0 && target == ' ') || 
                    (dx == -1 && abs(dy) == 1 && target != ' ') || 
                    (startX == 6 && dx == -2 && dy == 0 && chessBoard->board[startX-1][startY] == ' ' && target == ' ')) {
                    return 1;
                }
            } else {
                if ((dx == 1 && dy == 0 && target == ' ') || 
                    (dx == 1 && abs(dy) == 1 && target != ' ') || 
                    (startX == 1 && dx == 2 && dy == 0 && chessBoard->board[startX+1][startY] == ' ' && target == ' ')) {
                    return 1;
                }
            }
            break;
        case 'r':
            if ((dx == 0 || dy == 0) && target != piece) {
                int stepX = dx ? dx / abs(dx) : 0;
                int stepY = dy ? dy / abs(dy) : 0;
                for (int i = startX + stepX, j = startY + stepY; i != endX || j != endY; i += stepX, j += stepY) {
                    if (chessBoard->board[i][j] != ' ') return 0;
                }
                return 1;
            }
            break;
        case 'n':
            if ((abs(dx) == 2 && abs(dy) == 1) || (abs(dx) == 1 && abs(dy) == 2)) {
                return 1;
            }
            break;
        case 'b':
            if (abs(dx) == abs(dy)) {
                int stepX = dx / abs(dx);
                int stepY = dy / abs(dy);
                for (int i = startX + stepX, j = startY + stepY; i != endX; i += stepX, j += stepY) {
                    if (chessBoard->board[i][j] != ' ') return 0;
                }
                return 1;
            }
            break;
        case 'q':
            if ((abs(dx) == abs(dy)) || (dx == 0 || dy == 0)) {
                int stepX = dx ? dx / abs(dx) : 0;
                int stepY = dy ? dy / abs(dy) : 0;
                for (int i = startX + stepX, j = startY + stepY; i != endX || j != endY; i += stepX, j += stepY) {
                    if (chessBoard->board[i][j] != ' ') return 0;
                }
                return 1;
            }
            break;
        case 'k':
            if (abs(dx) <= 1 && abs(dy) <= 1) {
                return 1;
            }
            break;
    }
    return 0;
}

void movePiece(ChessBoard *chessBoard, int startX, int startY, int endX, int endY) {
    chessBoard->board[endX][endY] = chessBoard->board[startX][startY];
    chessBoard->board[startX][startY] = ' ';
}

int main() {
    ChessBoard chessBoard;
    initBoard(&chessBoard);

    char move[6];
    int startX, startY, endX, endY;
    while (1) {
        printBoard(&chessBoard);
        printf("Enter your move (e.g., e2 e4): ");
        fgets(move, sizeof(move), stdin);
        if (strlen(move) < 5) {
            printf("Invalid move format. Try again.\n");
            continue;
        }

        startY = move[0] - 'a';
        startX = BOARD_SIZE - (move[1] - '0');
        endY = move[3] - 'a';
        endX = BOARD_SIZE - (move[4] - '0');

        if (isValidMove(&chessBoard, startX, startY, endX, endY)) {
            movePiece(&chessBoard, startX, startY, endX, endY);
        } else {
            printf("Invalid move. Try again.\n");
        }
    }

    return 0;
}
