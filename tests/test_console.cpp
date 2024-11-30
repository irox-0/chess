#include <gtest/gtest.h>
#include "io/Console.hpp"
#include <sstream>

class ConsoleTest : public ::testing::Test {
protected:
    void SetUp() override {
        Game game;
        console = new Console(&game);
        originalCin = std::cin.rdbuf();
        originalCout = std::cout.rdbuf();
    }

    void TearDown() override {
        std::cin.rdbuf(originalCin);
        std::cout.rdbuf(originalCout);
        delete console;
    }

    void simulateInput(const std::string& input) {
        inputStream.str(input);
        inputStream.clear();
        std::cin.rdbuf(inputStream.rdbuf());
    }

    std::string captureOutput() {
        outputStream.str("");
        outputStream.clear();
        std::cout.rdbuf(outputStream.rdbuf());
        return outputStream.str();
    }

    Console* console;
    std::stringstream inputStream;
    std::stringstream outputStream;
    std::streambuf* originalCin;
    std::streambuf* originalCout;
};

TEST_F(ConsoleTest, GetYesNoInput) {
    simulateInput("y\n");
    EXPECT_TRUE(console->getYesNoInput("Test prompt: "));

    simulateInput("n\n");
    EXPECT_FALSE(console->getYesNoInput("Test prompt: "));

    simulateInput("invalid\ny\n");
    EXPECT_TRUE(console->getYesNoInput("Test prompt: "));
}

TEST_F(ConsoleTest, GetStringInput) {
    simulateInput("test input\n");
    EXPECT_EQ(console->getStringInput("Enter text: "), "test input");

    simulateInput("\n");
    EXPECT_EQ(console->getStringInput("Enter text: "), "");
}

TEST_F(ConsoleTest, DisplayBoard) {
    captureOutput();
    console->displayBoard();
    std::string output = outputStream.str();

    EXPECT_NE(output.find("8"), std::string::npos);
    EXPECT_NE(output.find("1"), std::string::npos);
    EXPECT_NE(output.find("a"), std::string::npos);
    EXPECT_NE(output.find("h"), std::string::npos);
    EXPECT_NE(output.find("+---+"), std::string::npos);
}

TEST_F(ConsoleTest, GetPromotionPiece) {
    simulateInput("q\n");
    EXPECT_EQ(console->getPromotionPiece(), 'q');

    simulateInput("r\n");
    EXPECT_EQ(console->getPromotionPiece(), 'r');

    simulateInput("x\nq\n");
    EXPECT_EQ(console->getPromotionPiece(), 'q');
}

TEST_F(ConsoleTest, DisplayGameStatus) {
    captureOutput();
    console->displayGameStatus();
    std::string output = outputStream.str();

    EXPECT_NE(output.find("Turn:"), std::string::npos);
}

TEST_F(ConsoleTest, DisplayLegalMoves) {
    captureOutput();
    console->displayLegalMoves("e2");
    std::string output = outputStream.str();

    EXPECT_NE(output.find("Legal moves"), std::string::npos);
}

TEST_F(ConsoleTest, ShowError) {
    captureOutput();
    console->showError("Test error");
    std::string output = outputStream.str();

    EXPECT_NE(output.find("Error: Test error"), std::string::npos);
}

TEST_F(ConsoleTest, ShowMessage) {
    captureOutput();
    console->showMessage("Test message");
    std::string output = outputStream.str();

    EXPECT_NE(output.find("Test message"), std::string::npos);
}

TEST_F(ConsoleTest, ShowHelp) {
    captureOutput();
    simulateInput("\n");  
    console->showHelp();
    std::string output = outputStream.str();

    EXPECT_NE(output.find("Available commands"), std::string::npos);
    EXPECT_NE(output.find("move"), std::string::npos);
    EXPECT_NE(output.find("help"), std::string::npos);
    EXPECT_NE(output.find("quit"), std::string::npos);
}

TEST_F(ConsoleTest, GetMove) {
    std::string from, to;
    
    simulateInput("e2 e4\n");
    EXPECT_TRUE(console->getMove(from, to));
    EXPECT_EQ(from, "e2");
    EXPECT_EQ(to, "e4");

    simulateInput("quit\n");
    EXPECT_FALSE(console->getMove(from, to));

    simulateInput("help\n");
    EXPECT_FALSE(console->getMove(from, to));

    simulateInput("e2 e9\n");
    EXPECT_FALSE(console->getMove(from, to));
}

TEST_F(ConsoleTest, DisplayMoveHistory) {
    captureOutput();
    simulateInput("\n");  
    console->displayMoveHistory();
    std::string output = outputStream.str();

    EXPECT_NE(output.find("No moves made yet"), std::string::npos);
}

TEST_F(ConsoleTest, ClearScreen) {
    EXPECT_NO_THROW(console->clearScreen());
}