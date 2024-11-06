#include <iostream>
#include <vector>
#include <ctime>
#include <algorithm>
#include <random>
#include <fstream>
#include <windows.h>

#define R "\033[32m"  
#define G "\033[33m"  
#define B "\033[35m"  
#define N "\033[0m"   

using namespace std;

struct Card {
    char symbol;
    bool isFlipped;
};

struct Game {
    int rows;
    int cols;
    vector<vector<Card>> field;
    int moves;
    int score;
    time_t startTime;
};


void initializeCards(Game& game) {
    int totalCards = game.rows * game.cols;
    vector<char> symbols(totalCards / 2);
    for (int i = 0; i < symbols.size(); ++i) {
        symbols[i] = 'A' + i;
    }
    symbols.insert(symbols.end(), symbols.begin(), symbols.end());  
    shuffle(symbols.begin(), symbols.end(), mt19937(random_device()()));  

    game.field.resize(game.rows, vector<Card>(game.cols));
    int index = 0;
    for (int i = 0; i < game.rows; ++i) {
        for (int j = 0; j < game.cols; ++j) {
            game.field[i][j].symbol = symbols[index++];  
            game.field[i][j].isFlipped = false;
        }
    }
}


void displayField(const Game& game, int revealTime = 0) {
    if (revealTime > 0) {
        cout << "Запам'ятайте розташування карток:\n";
    }
    else {
        cout << "Знайдіть парні картки:\n";
    }
    cout << "   ";
    for (int j = 0; j < game.cols; ++j) cout << j + 1 << " ";
    cout << endl;

    for (int i = 0; i < game.rows; ++i) {
        cout << static_cast<char>('a' + i) << "  ";
        for (int j = 0; j < game.cols; ++j) {
            if (game.field[i][j].isFlipped || revealTime > 0) {
                cout << game.field[i][j].symbol << " ";
            }
            else {
                cout << B << "*" << N << " ";
            }
        }
        cout << endl;
    }
    if (revealTime > 0) {
        Sleep(revealTime);
        system("cls");
    }
    cout << "Рахунок: " << game.score << " | Кількість ходів: " << game.moves << endl;
}


bool flipCard(Game& game, int row, int col) {
    if (!game.field[row][col].isFlipped) {
        game.field[row][col].isFlipped = true;  
        return true;
    }
    return false;
}


bool checkMatch(const Game& game, int firstRow, int firstCol, int secondRow, int secondCol) {
    return game.field[firstRow][firstCol].symbol == game.field[secondRow][secondCol].symbol;
}


void saveStatistics(const Game& game) {
    ofstream file("statistics.txt", ios::app);
    if (file.is_open()) {
        double duration = difftime(time(nullptr), game.startTime);  
        file << fixed << setprecision(2);
        file << "Рахунок: " << game.score << ", Ходи: " << game.moves << ", Час: " << duration << " секунд\n";
        file.close();
    }
}


bool parseInput(const string& input, int& row, int& col, int maxRows, int maxCols) {
    if (input.length() < 2) return false;
    row = tolower(input[0]) - 'a';
    col = input[1] - '1';
    return row >= 0 && row < maxRows && col >= 0 && col < maxCols;
}

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    Game game;
    int choice;
    cout << "Виберіть розмір поля:\n1. 4x4\n2. 5x6\n";
    cin >> choice;
    if (choice == 1) {
        game.rows = 4;
        game.cols = 4;
    }
    else if (choice == 2) {
        game.rows = 5;
        game.cols = 6;
    }
    else {
        cout << "Неправильний вибір. Завершення програми...\n";
        return 1;
    }

    initializeCards(game);
    game.moves = 0;
    game.score = 0;
    game.startTime = time(nullptr);
    displayField(game, 5000);

    string firstInput, secondInput;
    int firstRow, firstCol, secondRow, secondCol;
    bool gameComplete = false;

    while (!gameComplete) {
        displayField(game);
        cout << "Оберіть першу картку (наприклад, a1): ";
        cin >> firstInput;
        cout << "Оберіть другу картку (наприклад, b2): ";
        cin >> secondInput;

        if (parseInput(firstInput, firstRow, firstCol, game.rows, game.cols) &&
            parseInput(secondInput, secondRow, secondCol, game.rows, game.cols)) {
            if (flipCard(game, firstRow, firstCol) && flipCard(game, secondRow, secondCol)) {
                if (checkMatch(game, firstRow, firstCol, secondRow, secondCol)) {
                    cout << R << "Пара знайдена!\n" << N;
                    game.score++;
                }
                else {
                    cout << G << "Немає пари. Спробуйте ще раз.\n" << N;
                    game.field[firstRow][firstCol].isFlipped = false;
                    game.field[secondRow][secondCol].isFlipped = false;
                }
                game.moves++;
            }
            else {
                cout << "Некоректний вибір. Спробуйте ще раз.\n";
            }
        }
        else {
            cout << "Неправильний формат введення. Спробуйте ще раз.\n";
        }

      
        gameComplete = true;
        for (int i = 0; i < game.rows; ++i) {
            for (int j = 0; j < game.cols; ++j) {
                if (!game.field[i][j].isFlipped) {
                    gameComplete = false;
                    break;
                }
            }
            if (!gameComplete) break;
        }
    }

    cout << "\n\n\tГра завершена! Підсумковий рахунок: " << game.score << endl;
    saveStatistics(game);
    return 0;
}
