#include "io/Console.hpp"
#include "game/Game.hpp"
#include "ai/AI.hpp"
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <memory>

class ChessGame {
public:
    ChessGame() : 
        game(std::make_unique<Game>()),
        ai(std::make_unique<AI>()),
        console(game.get()),
        isPlayerWhite(true) {}

    void start() {
        showWelcomeMessage();
        choosePlayerColor();
        playGame();
    }

private:
    std::unique_ptr<Game> game;
    std::unique_ptr<AI> ai;
    Console console;
    bool isPlayerWhite;

    void showWelcomeMessage() {
        console.clearScreen();
        std::cout << "Welcome to Chess Game!\n";
        std::cout << "===================\n\n";
        std::cout << "Controls:\n";
        std::cout << "- Enter moves in format: e2e4 or e2 e4\n";
        std::cout << "- Type 'quit' to exit\n";
        std::cout << "- Type 'help' for more commands\n\n";
    }

    void choosePlayerColor() {
        while (true) {
            std::cout << "Choose your color (w/b): ";
            std::string input;
            std::getline(std::cin, input);
            
            if (!input.empty()) {
                char choice = std::tolower(input[0]);
                if (choice == 'w' || choice == 'b') {
                    isPlayerWhite = (choice == 'w');
                    break;
                }
            }
            std::cout << "Invalid input. Please enter 'w' for White or 'b' for Black.\n";
        }
        std::cout << "\nYou are playing as " << (isPlayerWhite ? "White" : "Black") << ".\n";
        std::cout << "Press Enter to start the game...";
        std::string dummy;
        std::getline(std::cin, dummy);
    }

    void playGame() {
        game->initialize();
        
        while (!game->isGameOver()) {
            console.clearScreen();
            console.displayBoard();
            console.displayGameStatus();

            bool isPlayerTurn = (game->getCurrentTurn() == Piece::Color::White) == isPlayerWhite;

            if (isPlayerTurn) {
                std::cout << "\nYour turn!\n";
                handlePlayerMove();
            } else {
                handleAIMove();
            }

            console.clearScreen();
            console.displayBoard();
            console.displayGameStatus();

            checkGameState();

            if (!isPlayerTurn && !game->isGameOver()) {
                std::cout << "\nPress Enter to continue...";
                std::string dummy;
                std::getline(std::cin, dummy);
            }
        }

        announceResult();
    }

    void handlePlayerMove() {
        std::string from, to;
        while (true) {
            if (console.getMove(from, to)) {
                if (game->makeMove(from, to)) {
                    console.clearScreen();
                    console.displayBoard();
                    return;
                }
                std::cout << "Invalid move! Please try again.\n";
                console.displayBoard();
            }
        }
    }

    void handleAIMove() {
        std::cout << "\nAI is thinking...\n";
        
        Move aiMove = ai->getMove(game->getBoard(), game->getCurrentTurn());
        
        if (aiMove.getFrom().isValid() && aiMove.getTo().isValid()) {
            std::cout << "AI moves: " << aiMove.getFrom().toAlgebraic() 
                     << " to " << aiMove.getTo().toAlgebraic() << "\n";
            
            game->makeMove(
                aiMove.getFrom().toAlgebraic(), 
                aiMove.getTo().toAlgebraic()
            );
            
            console.clearScreen();
            console.displayBoard();
        } else {
            std::cout << "AI couldn't make a move!\n";
        }
    }

    void checkGameState() {
        if (game->isCheck()) {
            std::cout << "\nCheck!\n";
        }

        if (game->isCheckmate()) {
            std::cout << "\nCheckmate!\n";
        } else if (game->isStalemate()) {
            std::cout << "\nStalemate!\n";
        }
    }

    void announceResult() {
        console.clearScreen();
        console.displayBoard();
        std::cout << "\n=== Game Over! ===\n";

        GameState::Result result = game->getResult();
        switch (result) {
            case GameState::Result::WhiteWin:
                std::cout << "White wins";
                if (!isPlayerWhite) std::cout << " (AI wins)";
                std::cout << "!\n";
                break;
            case GameState::Result::BlackWin:
                std::cout << "Black wins";
                if (isPlayerWhite) std::cout << " (AI wins)";
                std::cout << "!\n";
                break;
            case GameState::Result::Draw:
                std::cout << "Game is drawn!\n";
                break;
            case GameState::Result::Stalemate:
                std::cout << "Game ends in stalemate!\n";
                break;
            default:
                std::cout << "Game ended with unknown result!\n";
                break;
        }

        std::cout << "\nPress Enter to exit...";
        std::string dummy;
        std::getline(std::cin, dummy);
    }
};

int main() {
    try {
        ChessGame chessGame;
        chessGame.start();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred!" << std::endl;
        return 1;
    }
    return 0;
}