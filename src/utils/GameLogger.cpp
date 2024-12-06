#include "GameLogger.hpp"
#include <sstream>
#include <iostream>

GameLogger::GameLogger(Game* game) : game(game) {}

char GameLogger::getPieceNotation(Piece::Type type) const {
    switch (type) {
        case Piece::Type::King:   return 'K';
        case Piece::Type::Queen:  return 'Q';
        case Piece::Type::Rook:   return 'R';
        case Piece::Type::Bishop: return 'B';
        case Piece::Type::Knight: return 'N';
        case Piece::Type::Pawn:   return 'P';
        default:                  return '?';
    }
}
const std::string GameLogger::SAVE_DIRECTORY = "../resources/saved_games/";


std::string GameLogger::moveToNotation(const std::string& from, const std::string& to,
                                     Piece::Type pieceType, Piece::Color pieceColor,
                                     bool isCapture, bool isCheck,
                                     bool isCheckmate, Piece::Type promotionPiece) {
    std::string notation;
    
    if (pieceType != Piece::Type::Pawn) {
        notation += getPieceNotation(pieceType);
    }
    
    notation += from;
    
    if (isCapture) {
        notation += "x";
    } else {
        notation += "-";
    }
    
    notation += to;
    
    if (pieceType == Piece::Type::Pawn && 
        (to[1] == '8' || to[1] == '1')) {
        notation += "=";
        notation += getPieceNotation(promotionPiece);
    }
    
    if (isCheckmate) {
        notation += "#";
    } else if (isCheck) {
        notation += "+";
    }
    
    return notation;
}

void GameLogger::logMove(const std::string& from, const std::string& to,
                        Piece::Type pieceType, Piece::Color pieceColor,
                        bool isCapture, bool isCheck,
                        bool isCheckmate, Piece::Type promotionPiece) {
    std::string notation = moveToNotation(from, to, pieceType, pieceColor,
                                        isCapture, isCheck, isCheckmate,
                                        promotionPiece);
    moveHistory.push_back(notation);
}

bool GameLogger::saveGame(const std::string& filename) {
    #ifdef _WIN32
        int result1 = mkdir("../resources");
        int result2 = mkdir("../resources/saved_games");
    #else
        int result1 = mkdir("../resources", 0777);
        int result2 = mkdir("../resources/saved_games", 0777);
    #endif

    // Проверяем, что директории созданы или уже существуют
    if ((result1 != 0 && errno != EEXIST) || 
        (result2 != 0 && errno != EEXIST)) {
        std::cerr << "Error creating directories" << std::endl;
        return false;
    }

    std::string fullPath = SAVE_DIRECTORY + filename;
    std::ofstream file(fullPath);
    if (!file.is_open()) {
        return false;
    }
    
    for (size_t i = 0; i < moveHistory.size(); ++i) {
        if (i % 2 == 0) {
            file << (i / 2 + 1) << ". ";
        }
        file << moveHistory[i] << " ";
        if (i % 2 == 1) {
            file << "\n";
        }
    }
    
    if (moveHistory.size() % 2 == 1) {
        file << "\n";
    }
    
    return true;
}


bool GameLogger::parseNotation(const std::string& notation, std::string& from, 
                             std::string& to, Piece::Type& pieceType) {
    //std::cout << "Parsing notation: " << notation << std::endl;
    
    if (notation.length() < 4) {
        std::cerr << "Notation too short" << std::endl;
        return false;
    }
    
    size_t startPos = 0;
    pieceType = Piece::Type::Pawn;
    
    if (std::isupper(notation[0])) {
        //std::cout << "Found piece prefix: " << notation[0] << std::endl;
        switch (notation[0]) {
            case 'K': pieceType = Piece::Type::King; break;
            case 'Q': pieceType = Piece::Type::Queen; break;
            case 'R': pieceType = Piece::Type::Rook; break;
            case 'B': pieceType = Piece::Type::Bishop; break;
            case 'N': pieceType = Piece::Type::Knight; break;
            default: 
                std::cerr << "Invalid piece prefix" << std::endl;
                return false;
        }
        startPos = 1;
    }
    
    from = notation.substr(startPos, 2);
    //std::cout << "Extracted 'from' position: " << from << std::endl;
    
    if (!isValidSquare(from)) {
        std::cerr << "Invalid 'from' square: " << from << std::endl;
        return false;
    }
    
    size_t moveSymbol = notation.find('-');
    if (moveSymbol == std::string::npos) {
        moveSymbol = notation.find('x');
        if (moveSymbol == std::string::npos) {
            std::cerr << "No move symbol found" << std::endl;
            return false;
        }
    }
    
    to = notation.substr(moveSymbol + 1, 2);
    //std::cout << "Extracted 'to' position: " << to << std::endl;
    
    if (!isValidSquare(to)) {
        std::cerr << "Invalid 'to' square: " << to << std::endl;
        return false;
    }
    
    //std::cout << "Successfully parsed move: " << from << " to " << to << std::endl;
    return true;
}



bool GameLogger::loadGame(const std::string& filename) {
    if (!game) {
        std::cerr << "Game pointer is null" << std::endl;
        return false;
    }
    
    std::string fullPath = SAVE_DIRECTORY + filename;
    std::ifstream file(fullPath);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << fullPath << std::endl;
        return false;
    }
    
    game->reset();
    game->initialize();
    
    std::string line;
    while (std::getline(file, line)) {
        while (!line.empty() && std::isspace(line.back())) {
            line.pop_back();
        }
        
        if (line.empty()) continue;
        
        std::istringstream iss(line);
        std::string token;
        
        if (!(iss >> token) || token.back() != '.') continue;
        
        std::string notation;
        if (iss >> notation) {  
            std::string from, to;
            Piece::Type pieceType;
            if (parseNotation(notation, from, to, pieceType)) {
                if (!game->makeMove(from, to)) {
                    return false;
                }
            }
            
            if (iss >> notation) {  
                if (parseNotation(notation, from, to, pieceType)) {
                    if (!game->makeMove(from, to)) {
                        return false;
                    }
                }
            }
        }
    }
    
    return true;
}

bool GameLogger::loadPositionFromFile(const std::string& filename) {
    return loadGame(filename);
}
