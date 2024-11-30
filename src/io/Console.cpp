#include "io/Console.hpp"
#include <limits>
#include <algorithm>
#include <cctype>

const char* Console::PIECE_SYMBOLS[] = {
    "P", "N", "R", "B", "Q", "K",  // White pieces
    "p", "n", "r", "b", "q", "k"   // Black pieces
};

const char* Console::COLOR_NAMES[] = {"White", "Black"};

const std::string Console::BOARD_BORDER = "+---+---+---+---+---+---+---+---+";
const std::string Console::COLUMN_LABELS = "  a   b   c   d   e   f   g   h";
const std::string Console::HELP_TEXT = R"(
Available commands:
- move <from> <to>  (e.g., 'move e2 e4')
- show moves <pos>  (e.g., 'show moves e2')
- resign
- draw offer
- draw accept
- draw decline
- help
- quit
)";

Console::Console(Game* gamePtr) : game(gamePtr), isRunning(true) {
    if (!game) {
        throw std::runtime_error("Game pointer cannot be null");
    }
}

void Console::start() {
    showMainMenu();
    while (isRunning) {
        playGame();
    }
}

void Console::showMainMenu() {
    clearScreen();
    std::cout << "Chess Game\n"
              << "1. Start new game\n"
              << "2. Help\n"
              << "3. Quit\n"
              << "Choose option: ";
    
    std::string input = getStringInput("");
    if (input == "1") {
        game->reset();
    } else if (input == "2") {
        showHelp();
        showMainMenu();
    } else if (input == "3") {
        isRunning = false;
    } else {
        showError("Invalid option");
        showMainMenu();
    }
}

void Console::playGame() {
    while (!game->isGameOver() && isRunning) {
        clearScreen();
        displayBoard();
        displayGameStatus();
        
        std::string from, to;
        if (getMove(from, to)) {
            if (isPromotionMove(from, to)) {
                handlePromotion(from, to);
            }
            
            if (!game->makeMove(from, to)) {
                showError("Invalid move!");
                continue;
            }
            
            moveHistory.push_back(formatMove(from, to));
            
            if (game->isCheck()) {
                showMessage("Check!");
            }
        }
        
        if (game->isGameOver()) {
            handleGameOver();
        }
    }
}

void Console::displayBoard() const {
    std::cout << "\nPosition:\n";
    for (int rank = 7; rank >= 0; --rank) {
        std::cout << (rank + 1) << " ";
        for (int file = 0; file < 8; ++file) {
            Position pos(file, rank);
            const Square* square = game->getBoard()->getSquare(pos);
            
            if (!square) {
                std::cout << "? "; 
                continue;
            }
            
            if (square->isOccupied() && square->getPiece()) {
                std::cout << square->getPiece()->getSymbol() << " ";
            } else {
                std::cout << ((file + rank) % 2 == 0 ? "." : " ") << " ";
            }
        }
        std::cout << " " << (rank + 1) << "\n";
    }
    std::cout << "  a b c d e f g h  \n\n";
    std::cout.flush(); 
}

void Console::displayGameStatus() const {
    std::cout << "\nTurn: " << colorToString(game->getCurrentTurn()) << "\n";
    
    if (!moveHistory.empty()) {
        std::cout << "Last move: " << moveHistory.back() << "\n";
    }
    
    if (game->getGameState()->isDrawOffered()) {
        std::cout << "Draw offered by " 
                 << colorToString(game->getGameState()->getDrawOfferingColor()) 
                 << "\n";
    }
}

bool Console::getMove(std::string& from, std::string& to) {
    
    std::string input = getStringInput("Enter move (e.g., 'e2e4' or 'e2 e4' or 'h7 h8q' for promotion): ");
    std::istringstream iss(input);
    std::string command;
    iss >> command;
    
    clearScreen();

    if (command == "quit") {
        if (getYesNoInput("Are you sure you want to quit? (y/n): ")) {
            isRunning = false;
            std::cout << "\nGame ended by player.\n";
            std::cout << "Press Enter to exit...";
            std::string dummy;
            std::getline(std::cin, dummy);
            exit(0);
        }
        displayBoard();
        displayGameStatus();
        return false;
    } 
    else if (command == "help") {
        showHelp();
        displayBoard();
        displayGameStatus();
        return false;
    } 
    else if (command == "resign") {
        handleResignation();
        if (game->isGameOver()) {
            handleGameOver();
        } else {
            displayBoard();
            displayGameStatus();
        }
        return false;
    } 
    else if (command == "draw") {
        handleDraw();
        if (game->isGameOver()) {
            handleGameOver();
        } else {
            displayBoard();
            displayGameStatus();
        }
        return false;
    } 
    else if (command == "show") {
        std::string subCommand, pos;
        iss >> subCommand >> pos;
        if (subCommand == "moves" && isValidPosition(pos)) {
            displayLegalMoves(pos);
            std::cout << "\nPress Enter to continue...";
            std::string dummy;
            std::getline(std::cin, dummy);
            //clearScreen();
            displayBoard();
            displayGameStatus();
        }
        return false;
    }

    if (command.length() >= 4) {
        from = command.substr(0, 2);
        if (command.length() == 4) {
            to = command.substr(2, 2);
        } 
        else if (command.length() == 5) {
            to = command.substr(2, 3);
        } 
        else {
            displayBoard();
            displayGameStatus();
            return false;
        }
        return isValidMoveInput(from) && isValidMoveInput(to.substr(0, 2));
    }
    
    iss.clear();
    iss.str(input);
    iss >> from >> to;
    
    std::string promotion;
    if (iss >> promotion) {
        if (promotion.length() == 1 && isPromotionPiece(promotion[0])) {
            to += promotion;
        } 
        else {
            displayBoard();
            displayGameStatus();
            return false;
        }
    }
    
    if (!isValidMoveInput(from) || !isValidMoveInput(to.substr(0, 2))) {
        displayBoard();
        displayGameStatus();
        return false;
    }
    
    return true;
}

void Console::handlePromotion(const std::string& from, std::string& to) {
    if (to.length() == 3) {
        return;
    }
    
    char piece = getPromotionPiece();
    if (isPromotionPiece(piece)) {
        to += piece;
    }
}


char Console::getPromotionPiece() const {
    while (true) {
        std::string input = getStringInput(
            "Choose promotion piece (q:Queen, r:Rook, b:Bishop, n:Knight): ");
        if (!input.empty()) {
            char piece = std::tolower(input[0]);
            if (piece == 'q' || piece == 'r' || piece == 'b' || piece == 'n') {
                return piece;
            }
        }
        showError("Invalid piece choice!");
    }
}

void Console::handleGameOver() {
    displayBoard();
    GameState::Result result = game->getResult();
    
    std::cout << "\nGame Over! " << resultToString(result) << "\n";
    
    if (getYesNoInput("Play again? (y/n): ")) {
        game->reset();
        moveHistory.clear();
    } else {
        isRunning = false;
    }
}

bool Console::getYesNoInput(const std::string& prompt) const {
    while (true) {
        std::string input = getStringInput(prompt);
        if (!input.empty()) {
            char response = std::tolower(input[0]);
            if (response == 'y' || response == 'n') {
                return response == 'y';
            }
        }
        showError("Please enter 'y' or 'n'");
    }
}

std::string Console::getStringInput(const std::string& prompt) const {
    if (!prompt.empty()) {
        std::cout << prompt;
    }
    std::string input;
    std::getline(std::cin, input);
    return input;
}

void Console::clearScreen() const {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
    std::cout.flush(); 
}

void Console::showError(const std::string& message) const {
    std::cout << "Error: " << message << "\n";
}

void Console::showMessage(const std::string& message) const {
    std::cout << message << "\n";
}

bool Console::isValidPosition(const std::string& pos) const {
    return pos.length() == 2 && 
           pos[0] >= 'a' && pos[0] <= 'h' && 
           pos[1] >= '1' && pos[1] <= '8';
}

bool Console::isValidMoveInput(const std::string& input) const {
    return isValidPosition(input);
}


bool Console::isPromotionMove(const std::string& from, const std::string& to) const {
    if (!game || !game->getBoard()) {
        return false;
    }

    const Square* fromSquare = game->getBoard()->getSquare(Position(from));
    if (!fromSquare || !fromSquare->isOccupied()) {
        return false;
    }
    
    Piece* piece = fromSquare->getPiece();
    if (piece->getType() != Piece::Type::Pawn) {
        return false;
    }
    
    Position toPos(to.substr(0, 2));
    int promotionRank = (piece->getColor() == Piece::Color::White) ? 7 : 0;
    return toPos.getY() == promotionRank;
}

bool Console::isPromotionPiece(char c) const {
    c = std::tolower(c);
    return c == 'q' || c == 'r' || c == 'b' || c == 'n';
}

void Console::handleDraw() {
    std::string subCommand = getStringInput("Enter draw command (offer/accept/decline): ");
    if (subCommand == "offer") {
        game->offerDraw(game->getCurrentTurn());
        showMessage("Draw offered");
    } else if (subCommand == "accept") {
        game->acceptDraw();
        showMessage("Draw accepted");
    } else if (subCommand == "decline") {
        game->declineDraw();
        showMessage("Draw declined");
    } else {
        showError("Invalid draw command");
    }
}

void Console::handleResignation() {
    if (getYesNoInput("Are you sure you want to resign? (y/n): ")) {
        game->resign(game->getCurrentTurn());
        showMessage(colorToString(game->getCurrentTurn()) + " resigns!");
    }
}

std::string Console::colorToString(Piece::Color color) const {
    return COLOR_NAMES[static_cast<int>(color)];
}

std::string Console::resultToString(GameState::Result result) const {
    switch (result) {
        case GameState::Result::WhiteWin:
            return "White wins!";
        case GameState::Result::BlackWin:
            return "Black wins!";
        case GameState::Result::Draw:
            return "Game is drawn";
        case GameState::Result::Stalemate:
            return "Stalemate";
        default:
            return "Unknown result";
    }
}

void Console::showHelp() const {
    std::cout << HELP_TEXT << "\n";
    getStringInput("Press Enter to continue...");
}

void Console::displayLegalMoves(const std::string& position) const {
    auto moves = game->getLegalMoves(position);
    if (moves.empty()) {
        showMessage("No legal moves from " + position);
    } else {
        std::cout << "Legal moves from " << position << ":\n";
        for (const auto& move : moves) {
            std::cout << formatPosition(move.getTo()) << " ";
        }
        std::cout << "\n";
    }
    //getStringInput("Press Enter to continue...");
}

std::string Console::formatMove(const std::string& from, const std::string& to) const {
    std::stringstream ss;
    ss << from << "-" << to;
    
    const Square* fromSquare = game->getBoard()->getSquare(Position(from));
    if (fromSquare && fromSquare->isOccupied()) {
        Piece* piece = fromSquare->getPiece();
        
        if (piece->getType() != Piece::Type::Pawn) {
            int index = static_cast<int>(piece->getType()) + 
                       (piece->getColor() == Piece::Color::Black ? 6 : 0);
            ss << " " << PIECE_SYMBOLS[index];
        }
        
        const Square* toSquare = game->getBoard()->getSquare(Position(to));
        if (toSquare && toSquare->isOccupied()) {
            ss << " captures";
        }
    }
    
    return ss.str();
}

std::string Console::formatPosition(const Position& pos) const {
    if (!pos.isValid()) {
        return "invalid";
    }
    return std::string(1, static_cast<char>('a' + pos.getX())) + 
           std::string(1, static_cast<char>('1' + pos.getY()));
}

void Console::displayMoveHistory() const {
    if (moveHistory.empty()) {
        showMessage("No moves made yet.");
        return;
    }

    std::cout << "\nMove History:\n";
    for (size_t i = 0; i < moveHistory.size(); ++i) {
        if (i % 2 == 0) {
            std::cout << (i/2 + 1) << ". ";
        }
        std::cout << moveHistory[i];
        if (i % 2 == 0) {
            std::cout << " ";
        } else {
            std::cout << "\n";
        }
    }
    if (moveHistory.size() % 2 != 0) {
        std::cout << "\n";
    }
}