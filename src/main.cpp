#include "io/Console.hpp"
#include "game/Game.hpp"
#include "ai/AI.hpp"
#include "utils/Timer.hpp"
#include "utils/GameLogger.hpp"
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
        logger(std::make_unique<GameLogger>(game.get())),
        isPlayerWhite(true),
        timer(nullptr)
        {
        }

    void startNewGame() {
        showWelcomeMessage();
        choosePlayerColor();
        setupTimer();
        playGame();
    }

    void start() {
        showMainMenu();
    }
    
private:
    std::unique_ptr<Game> game;
    std::unique_ptr<AI> ai;
    Console console;
    bool isPlayerWhite;
    std::unique_ptr<Timer> timer;
    std::unique_ptr<GameLogger> logger;


     void showWelcomeMessage() {
        console.clearScreen();
        std::cout << "Welcome to Chess Game!\n";
        std::cout << "===================\n\n";
        std::cout << "Controls:\n";
        std::cout << "- Enter moves in format: e2e4 or e2 e4\n";
        std::cout << "- Type 'quit' to exit\n";
        std::cout << "- Type 'help' for more commands\n\n";
        std::cout << "Press Enter to start the game...";
        std::string dummy;
        std::getline(std::cin, dummy);
    }


    void showMainMenu() {
        while (true) {
            console.clearScreen();
            std::cout << "Chess Game Menu\n";
            std::cout << "===============\n";
            std::cout << "1. New Game\n";
            std::cout << "2. Load Game from File\n";
            std::cout << "3. Exit\n\n";
            std::cout << "Enter your choice (1-3): ";
            
            std::string input;
            std::getline(std::cin, input);
            
            if (input == "1") {
                startNewGame();
                break;
            } else if (input == "2") {
                loadGameFromFile();
                break;
            } else if (input == "3") {
                exit(0);
            }
            
            std::cout << "Invalid choice. Press Enter to try again...";
            std::getline(std::cin, input);
        }
    }

    

    void loadGameFromFile() {
        console.clearScreen();
        std::cout << "Enter filename to load (or press Enter to return): ";
        std::string filename;
        std::getline(std::cin, filename);
        
        if (filename.empty()) {
            showMainMenu();
            return;
        }
        
        if (logger->loadGame(filename)) {
            std::cout << "\nGame loaded successfully!\n";
            
            choosePlayerColor();
            
            playLoadedGame();
        } else {
            std::cout << "\nError loading game!\n";
            std::cout << "Press Enter to return to menu...";
            std::string dummy;
            std::getline(std::cin, dummy);
            showMainMenu();
        }
    }

    void playLoadedGame() {
        setupTimer();
        console.setTimer(timer.get());
        console.setPlayerColor(isPlayerWhite);

        while (!game->isGameOver()) {
            console.clearScreen();
            console.displayBoard();
            console.displayGameStatus();

            bool isPlayerTurn = (game->getCurrentTurn() == Piece::Color::White) == isPlayerWhite;

            if (isPlayerTurn) {
                std::cout << "\nYour turn!\n";
                handlePlayerMove();

                if (game->getCurrentTurn() == Piece::Color::White && isPlayerWhite ||
                    game->getCurrentTurn() == Piece::Color::Black && !isPlayerWhite) {
                    continue;
                }
            } else {
                handleAIMove();
            }

            console.clearScreen();
            console.displayBoard();
            console.displayGameStatus();

            checkGameState();
        }

        announceResult();
    }

    void setupTimer() {
        int minutes;
        while (true) {
            std::cout << "Enter time limit in minutes: ";
            std::string input;
            std::getline(std::cin, input);
            try {
                minutes = std::stoi(input);
                if (minutes > 0) {
                    break;
                }
                std::cout << "Please enter a positive and integer number.\n";
            } catch (...) {
                std::cout << "Invalid input. Please enter a number.\n";
            }
        }
        timer = std::make_unique<Timer>(minutes);
        console.setTimer(timer.get());  
        console.setPlayerColor(isPlayerWhite);  
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

                if (game->getCurrentTurn() == Piece::Color::White && isPlayerWhite ||
                game->getCurrentTurn() == Piece::Color::Black && !isPlayerWhite) {
                continue; }
            } else {
                handleAIMove();
            }

            console.clearScreen();
            console.displayBoard();
            console.displayGameStatus();

            checkGameState();

            /*if (!isPlayerTurn && !game->isGameOver()) {
                std::cout << "\nPress Enter to continue...";
                std::string dummy;
                std::getline(std::cin, dummy);
            }*/
        }

        announceResult();
    }

    void handlePlayerMove() {
        std::string from, to;
        timer->start();
        bool invalidMove = false;  
        
        while (true) {
            if (timer->isTimeUp()) {
                std::cout << "\nTime's up! You lost the game.\n";
                game->resign(isPlayerWhite ? Piece::Color::White : Piece::Color::Black);
                return;
            }
            
            if (!invalidMove) {  
                console.clearScreen();
            }
            console.displayBoard();
            console.displayGameStatus();
            
            if (invalidMove) {
                std::cout << "Invalid move! Please try again.\n";
                invalidMove = false;
            }
            
            if (console.getMove(from, to)) {
                const Square* fromSquare = game->getBoard()->getSquare(Position(from));
                if (fromSquare && fromSquare->isOccupied()) {
                    const Piece* piece = fromSquare->getPiece();
                    const Square* toSquare = game->getBoard()->getSquare(Position(to));
                    const bool isCapture = toSquare && toSquare->isOccupied() &&
                            toSquare->getPiece()->getColor() != piece->getColor();

                    Piece::Type capturedType = Piece::Type::Pawn;
                    Piece::Color capturedColor = Piece::Color::White;
                    if (isCapture && toSquare->isOccupied()) {
                        capturedType = toSquare->getPiece()->getType();
                        capturedColor = toSquare->getPiece()->getColor();
                    }

                    if (game->makeMove(from, to)) {
                        timer->stop();

                            logger->logMove(from, to, piece->getType(), piece->getColor(),
                                            isCapture, game->isCheck(), game->isCheckmate());
        
                        console.addMoveToHistory(from, to, piece->getType(), 
                                            piece->getColor(), isCapture,
                        capturedType, capturedColor);
                        console.addMoveToHistory(from, to, piece->getType(), 
                                        piece->getColor(), isCapture,
                                        capturedType, capturedColor);
                        return;
                    }
                }

                invalidMove = true;  
            }
        }
    } 
    

    void handleAIMove() {
        //std::cout << "\nAI is thinking...\n";
    
        Move aiMove = ai->getMove(game->getBoard(), game->getCurrentTurn());
    
        if (aiMove.getFrom().isValid() && aiMove.getTo().isValid()) {
            const std::string from = aiMove.getFrom().toAlgebraic();
            const std::string to = aiMove.getTo().toAlgebraic();
        
            const Square* fromSquare = game->getBoard()->getSquare(aiMove.getFrom());
            if (fromSquare && fromSquare->isOccupied()) {
                const Piece* piece = fromSquare->getPiece();
                const Square* toSquare = game->getBoard()->getSquare(aiMove.getTo());
                bool isCapture = toSquare && toSquare->isOccupied() && 
                        toSquare->getPiece()->getColor() != piece->getColor();

                Piece::Type capturedType = Piece::Type::Pawn;
                Piece::Color capturedColor = Piece::Color::White;
                if (isCapture && toSquare->isOccupied()) {
                    capturedType = toSquare->getPiece()->getType();
                    capturedColor = toSquare->getPiece()->getColor();
                }

                std::cout << "AI moves: " << from << " to " << to << "\n";
            
                if (game->makeMove(from, to)) {
                    logger->logMove(from, to, piece->getType(), piece->getColor(),
                                    isCapture, game->isCheck(), game->isCheckmate());
                        
                console.addMoveToHistory(from, to, piece->getType(), 
                        piece->getColor(), isCapture,
                        capturedType, capturedColor);
                console.clearScreen();
                console.displayBoard();
                console.displayGameStatus();
                }
            }
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

        if (timer && timer->isTimeUp()) {
            std::cout << "Time's up! " << (isPlayerWhite ? "Black" : "White") << " wins!\n";
        } else {
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
        }

        std::cout << "\nWould you like to save this game? (y/n): ";
        std::string response;
        std::getline(std::cin, response);
        
        if (!response.empty() && std::tolower(response[0]) == 'y') {
            std::cout << "Enter filename to save: ";
            std::string filename;
            std::getline(std::cin, filename);
            
            if (!filename.empty()) {
                if (logger->saveGame(filename)) {
                    std::cout << "Game saved successfully!\n";
                } else {
                    std::cout << "Error saving game!\n";
                }
            }
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