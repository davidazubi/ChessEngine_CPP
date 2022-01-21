#include <iostream>
#include <memory>
#include "move.cpp"
#include "engine.cpp"

using namespace std;

int main() {
    // Precompile
    bb_precompile();

    // Init board
    ChessBoard game(FEN_PERF_2);

    // Print board
    game.printDebugInfo();

    // Generate moves
    //auto moves = game.generateMoves();

    // Game Loop
    while (false) {
       string moveNotation;
       cout << "Enter move: ";
       cin >> moveNotation;

        Move move = game.parseMoveAN(moveNotation);
        move.print();
        game.doMove(move);
        game.printBoard();

        // Quit
        if (moveNotation == "q") {
            break;
        }
    }


    // Test move generation
   // testMoveGeneration(FEN_INITIAL, 1);
   // testMoveGeneration(FEN_INITIAL, 2);
   // testMoveGeneration(FEN_INITIAL, 3);
   // testMoveGeneration(FEN_PERF_2, 1);

    // Test
    //cout << game.isSquareAttacked(4, 'b') << endl;


    return 0;
}