#include <iostream>
#include <vector>
#include "bitboard.cpp"
#include <map>
#include <string>
#include <stack>
#include "perft.cpp"

using namespace std;

struct Board {
    map<char, bb> bitboards;
    bool castlingBK;
    bool castlingBQ;
    bool castlingWK;
    bool castlingWQ;
    char player;
    int enPassantSquare;
    int plySinceCapture;
    int moveNumber;

    Board(map<char, bb> _bitboards, bool _castlingWK, bool _castlingWQ, bool _castlingBK,bool _castlingBQ, char _player, int _enPassantSquare, int _plySinceCapture, int _moveNumber) {
        bitboards = _bitboards;
        castlingBK = _castlingBK;
        castlingBQ = _castlingBQ;
        castlingWK = _castlingWK;
        castlingWQ = _castlingWQ;
        player = _player;
        enPassantSquare = _enPassantSquare;
        plySinceCapture = _plySinceCapture;
        moveNumber = _moveNumber;
    }
};

class ChessBoard {
public:
    map<char, bb> bitboards;
    map<int, char> pieceListWhite;
    map<int, char> pieceListBlack;
    stack<Board> boardStack;

    bool castlingBK;
    bool castlingBQ;
    bool castlingWK;
    bool castlingWQ;
    char player;
    int enPassantSquare;
    int plySinceCapture;
    int moveNumber;

    ChessBoard(string fen) {
        initPosition(fen);
    }

    void initPosition(string fen) {
        // Split string in segments by spaces
        vector<string> words = splitString(fen, " ");
        string fenSegmentBoard = words[0];
        string fenSegmentPlayer = words[1];
        string fenSegmentCastlingRights = words[2];
        string fenSegmentEnPassantSquare = words[3];

        if (words.size() >= 5) {
            string fenSegmentPlySinceCapture = words[4];
            cout << fenSegmentPlySinceCapture << endl;
            plySinceCapture = stoi(fenSegmentPlySinceCapture);
        } else {
            plySinceCapture = 0;
        }

        if (words.size() >= 6) {
            string fenSegmentMoveNumber = words[5];
            moveNumber = stoi(fenSegmentMoveNumber);
        } else {
            moveNumber = 0;
        }


        // Init bitboards
        for(char piece: PIECE_CODES_ALL) {
            bitboards[piece] = bbZero;
        }

        // Board
        int rank = 7;
        int file = 0;
        for (char c: fenSegmentBoard) {
            // New Rank
            if (c == '/') {
                rank -= 1;
                file = 0;
                continue;
            }

            // Char is number -> n empty squares
            if (isdigit(c)) {
                file += c-48; // Digit ascii to int -> -48
            } else {
                // Sit bit in corresponding bitboard
                int square = rank*8 + file;
                bbSetBit(bitboards[c], square);
                file++;
            }
        }

        // Generate piece Lists
        generatePieceLists();

        // Additional game state
        player = fenSegmentPlayer[0];
        castlingWK = stringContains(fenSegmentCastlingRights, 'K');
        castlingWQ = stringContains(fenSegmentCastlingRights, 'Q');
        castlingBK = stringContains(fenSegmentCastlingRights, 'k');
        castlingBQ = stringContains(fenSegmentCastlingRights, 'q');
        enPassantSquare = (fenSegmentEnPassantSquare == "-") ? -1 : SQUARE_NOTATION[fenSegmentEnPassantSquare];

    }

    void printBoard() {
        // Convert bitboards to board with pieces as chars
        char board[64] {};
        fill_n (board, 64, ' ');
        for (char pieceType: PIECE_CODES_ALL) {
            for (int idx: bbGetTrueBits(bitboards[pieceType])) {
                board[idx] = pieceType;
            }
        }

        // Print board to console
        cout << "+----------------+" << endl;
        for (int rank = 7; rank >= 0; rank--) {
            cout << "|";
            for (int file = 0; file < 8; file++) {
                int square = rank*8 + file;
                //cout << board[square] << " ";
                cout << PIECE_SYMBOLS[board[square]] << " ";
            }
            cout << "|" << rank+1 << endl;
        }
        cout << "+----------------+" << endl << " a b c d e f g h" << endl;
    }

    void printDebugInfo() {
        cout << "Board: " << endl;
        printBoard();
        cout << "Player: " << player << endl;
        cout << "Move number: " << moveNumber << endl;
        cout << "Castling White Kingside: " << castlingWK << endl;
        cout << "Castling White Queenside: " << castlingWQ << endl;
        cout << "Castling Black Kingside: " << castlingBK << endl;
        cout << "Castling Black Queenside: " << castlingBQ << endl;
        cout << "En Passant Square: " << enPassantSquare << endl;
        cout << "Ply since last Capture or pawn moves: " << plySinceCapture << endl;
        cout << endl;
    }

    bb getBoardPiecesWhite() {
        bb bbPieces = bbZero;
        for (char pieceCode: PIECE_CODES_WHITE) {
            bbPieces |= bitboards[pieceCode];
        }
        return bbPieces;
    }

    bb getBoardPiecesBlack() {
        bb bbPieces = bbZero;
        for (char pieceCode: PIECE_CODES_BLACK) {
            bbPieces |= bitboards[pieceCode];
        }
        return bbPieces;
    }

    void generatePieceLists() {
        // Generate piece list for player 1
        for (char pieceCode: PIECE_CODES_ALL) {
            vector<int> squares = bbGetTrueBits(bitboards[pieceCode]);
            for (int square: squares) {
                if (isupper(pieceCode)) {
                    pieceListWhite[square] = pieceCode;
                } else{
                    pieceListBlack[square] = pieceCode;
                }
            }
        }

    }

    char getNextPlayer() {
        return (player == WHITE) ? BLACK : WHITE;
    }

    void doMove(Move move) {
        // Add current position squareTo stack
        boardStack.push(Board(bitboards, castlingWK, castlingWQ, castlingBK, castlingBQ, player, enPassantSquare, plySinceCapture, moveNumber));
        bool promotion = move.promotion();
        bool capture = move.capture();
        bool special1 = move.special1();
        bool special0 = move.special0();
        int moveCode = move.code();

        // Assign piece Lists
        map<int, char> pieceListPlayer1 = (player == WHITE) ? pieceListWhite : pieceListBlack;
        map<int, char> pieceListPlayer2 = (player == BLACK) ? pieceListWhite : pieceListBlack;

        // From
        int squareFrom = move.from();
        char pieceTypeFrom = pieceListPlayer1[squareFrom];
        bbResetBit(bitboards[pieceTypeFrom], squareFrom);

        // To
        int squareTo = (moveCode == MOVE_CODE_EP_CAPTURE) ? enPassantSquare : move.to();
        char pieceTypeTo = (promotion) ? moveCodePromoType[player][moveCode] : pieceTypeFrom;
        bbSetBit(bitboards[pieceTypeTo], squareTo);

        // Remove
        if (capture) {
            int squareCapture = move.to();
            char pieceTypeCapture = pieceListPlayer2[squareCapture];
            bbResetBit(bitboards[pieceTypeCapture], squareCapture);
        }

        // Castling
        if (move.castleKingside() | move.castleQueenside()) {
            // Move the Rook from initial square to the other side of the king
            int squareRookFrom = rookInitialLocation[player][moveCode];
            int squareRookTo = rookTargetCastling[squareRookTo];
            char rookCode = pieceListPlayer1[squareRookFrom];
            bbResetBit(bitboards[rookCode], squareRookFrom);
            bbSetBit(bitboards[rookCode], squareRookTo);
        }

        // King or rook moved -> Remove castling rights
        if ((squareFrom == 4) | (squareFrom == 7) | (squareTo == 7)) { castlingWK = false;}
        if ((squareFrom == 4) | (squareFrom == 0) | (squareTo == 0) ) { castlingWQ = false;}
        if ((squareFrom == 60) | (squareFrom == 63) | (squareTo == 63) ) { castlingBK = false;}
        if ((squareFrom == 60) | (squareFrom == 56) | (squareTo == 56) ) { castlingBQ = false;}

        // Set next player;
        player = getNextPlayer();

        // Generate new piece Lists
        generatePieceLists();

        // Move Number
        moveNumber++;
    }

    void undoMove() {
        // Pop last board from stack
        Board board = boardStack.top();
        boardStack.pop();

        // Set board to old board
        bitboards = board.bitboards;
        castlingBK = board.castlingBK;
        castlingBQ = board.castlingBQ;
        castlingWK = board.castlingWK;
        castlingWQ = board.castlingWQ;
        player = board.player;
        enPassantSquare = board.enPassantSquare;
        plySinceCapture = board.plySinceCapture;
        moveNumber = board.moveNumber;

        // Generate new piece Lists
        generatePieceLists();
    }

    bool isSquareAttacked(int square, char attacker) {
        // Check if square is attacked
        // player1 -> Attacker
        // player 2 -> Attacked

        // Generate bitboards for occupied squares
        bb bbPiecesPlayer1 = (player == attacker) ? getBoardPiecesWhite() : getBoardPiecesBlack();
        bb bbPiecesPlayer2 = (player == attacker) ? getBoardPiecesBlack() : getBoardPiecesWhite();
        bb bbPiecesAll = bbPiecesPlayer1 | bbPiecesPlayer2;

        // Which enemy squares can be attack from target square
        vector<char> pieceCodesAttacker = (attacker == WHITE) ? PIECE_CODES_WHITE : PIECE_CODES_BLACK;
        for (char pieceCode: pieceCodesAttacker) {
            cout << pieceCode << endl;
            bb targets = getTargetSquares(square, pieceCode, bbPiecesAll, bbPiecesPlayer1, bbPiecesPlayer2, attacker);
            bbPrintBoard(targets);
            if (targets & bitboards[pieceCode] != bbZero) {
                return true;
            }
        }
        // Square is not attacked
        return false;
    }

    bool pathCastlingFree(string side) {
        cout << "Check path: " << endl;
        // Check if king is not in check and doesn't castle trough or in check
        char attacker = getNextPlayer();
        for (int sq: kingPathCastling[side]) {
            cout << "Path: " << sq << endl;
            if (isSquareAttacked(sq, attacker)) {
                return false;
            }
        }
        return true;
    }

    int getKingPosition(char color) {
        char pieceCodeKing = (color == WHITE) ? 'K' : 'k';
        return bbGetTrueBits(bitboards[pieceCodeKing])[0];
    }

    Move parseMoveAN (string notation) {
        Move move(0, 0, false, false, false, false);
        int len = notation.length();

        // Assign piece Lists
        map<int, char> pieceListPlayer1 = (player == WHITE) ? pieceListWhite : pieceListBlack;
        map<int, char> pieceListPlayer2 = (player == BLACK) ? pieceListWhite : pieceListBlack;

        // Pawn push
        if (len == 2) {
            if (SQUARE_NOTATION.count(notation)) {
                int toSquare = SQUARE_NOTATION[notation];
                int fromSquare;

                int offset = pawSinglepushOffset[player];
                int pawnFromSingle = toSquare - offset;
                int pawnFromDouble = toSquare - offset*2;
                if (pieceListPlayer1.count(pawnFromSingle)) {
                    fromSquare = pawnFromSingle;
                } else if (pieceListPlayer1.count(pawnFromDouble)) {
                    fromSquare = pawnFromDouble;
                }
                cout << fromSquare << " " << toSquare << endl;


                move.set_from(fromSquare);
                move.set_to(toSquare);

            }
        }





        return move;
    }

    vector<Move> generateMoves() {
        char player1 = player;
        char player2 = getNextPlayer();

        vector<Move> legalMoves;
        vector<Move> pseudolegalMoves;

        // Assign active player piece codes, piece lists
        vector<char> pieceCodesPlayer1 = (player == WHITE) ? PIECE_CODES_WHITE : PIECE_CODES_BLACK;
        vector<char> pieceCodesPlayer2 = (player == BLACK) ? PIECE_CODES_WHITE : PIECE_CODES_BLACK;

        // Generate bitboards for occupied squares
        bb bbPiecesPlayer1 = (player == WHITE) ? getBoardPiecesWhite() : getBoardPiecesBlack();
        bb bbPiecesPlayer2 = (player == BLACK) ? getBoardPiecesWhite() : getBoardPiecesBlack();
        bb bbPiecesAll = bbPiecesPlayer1 | bbPiecesPlayer2;

        // Generate moves for each piece
        map<int, char> pieceListPlayer1 = (player == WHITE) ? pieceListWhite : pieceListBlack;
        map<int, char> pieceListPlayer2 = (player == BLACK) ? pieceListWhite : pieceListBlack;
        for (auto const& x : pieceListPlayer1) {
            int square = x.first;
            char pieceType = toupper(x.second);

            // Get bitboard with target squares
            bb targets = getTargetSquares(square, pieceType, bbPiecesAll, bbPiecesPlayer1, bbPiecesPlayer2, player);

            // Pawn
            if (pieceType == 'P') {
                bb pushes = pawnMoveSet[player][square] & ~bbPiecesAll;
                for (int i = 0; i < 64; i++) {
                    // Captures
                    if (bbGetBit(targets, i)) {;
                        char captureType = pieceListPlayer2[i];

                        // Pawn capture Promotion
                        if ((i >= 0 && i <= 7) || (i >= 56 && i <=63)) {
                            // Add all promotion options as separate move
                            pseudolegalMoves.push_back(Move(square, i, true, true, false, false));
                            pseudolegalMoves.push_back(Move(square, i, true, true, false, true));
                            pseudolegalMoves.push_back(Move(square, i, true, true, true, false));
                            pseudolegalMoves.push_back(Move(square, i, true, true, true, true));
                        } else {
                            // Pawn capture
                            pseudolegalMoves.push_back(Move(square, i, false, true, false, false));
                        }

                    }

                    // Pawn pushes
                    if (bbGetBit(pushes, i)) {;
                        // Promotion
                        if ((i >= 0 && i <= 7) || (i >= 56 && i <=63)) {
                            // Add all promotion options as separate move
                            pseudolegalMoves.push_back(Move(square, i, true, false, false, false));
                            pseudolegalMoves.push_back(Move(square, i, true, false, false, true));
                            pseudolegalMoves.push_back(Move(square, i, true, false, true, false));
                            pseudolegalMoves.push_back(Move(square, i, true, false, true, true));
                        } else {
                            // In case of doublepush the skipped square has to be free
                            int sqSinglepush = square + pawSinglepushOffset[player];
                            if (i == sqSinglepush | !bbGetBit(bbPiecesAll, sqSinglepush)) {
                                pseudolegalMoves.push_back(Move(square, i, false, false, false, false));
                            }

                        }
                    }
                }
            }

            // Not pawn
            if (pieceType != 'P') {
                for (int i: bbGetTrueBits(targets)) {
                    bool capture = bbGetBit(bbPiecesPlayer2, i);
                    pseudolegalMoves.push_back(Move(square, i, false, capture, false, false));
                }
            }

            // En passant
            if (enPassantSquare != -1) {
                // Could a opposite color pawn on the enPassant square capture something?
                char otherPawn = (player == WHITE) ? 'p' : 'P';
                targets = getTargetSquares(enPassantSquare, 'P', bbPiecesAll, bbPiecesPlayer1, bbPiecesPlayer2,player2);
                targets &= bitboards[otherPawn];

                for (int i: bbGetTrueBits(targets)) {
                    pseudolegalMoves.push_back(Move(square, i, false, true, false, true));
                }
            }

            // Castling: Check that king is not in check and doesn't castle trough or in check && King + Rook are in initial position + haven't moved
            if (player == WHITE && pieceType == 'K') {
                // White castling kingside
                if ((bbPiecesAll & maskCastlingWK) == initCastlingWK && castlingWK) {
                    if (pathCastlingFree("wk")) {
                        pseudolegalMoves.push_back(Move(4, 6, false, false, true, false));
                    }
                }

                // White castling queenside
                if ((bbPiecesAll & maskCastlingWQ) == initCastlingWQ && castlingWQ) {
                    if (pathCastlingFree("wq")) {
                        pseudolegalMoves.push_back(Move(4, 2, false, false, true, false));
                    }
                }
            } else if (player == BLACK && pieceType == 'K'){
                // Black castling kingside
                if ((bbPiecesAll & maskCastlingBK) == initCastlingBK && castlingBK) {
                    if (pathCastlingFree("bk")) {
                        pseudolegalMoves.push_back(Move(60, 62, false, false, true, false));
                    }
                }

                // Black castling queenside
                if ((bbPiecesAll & maskCastlingBQ) == initCastlingBQ && castlingBQ) {
                    if (pathCastlingFree("bq")) {
                        pseudolegalMoves.push_back(Move(60, 58, false, false, true, true));
                    }
                }
            }

        }

        // Filter illegal moves (When king is in check after move)
        for (Move move: pseudolegalMoves) {
            // Make move
            doMove(move);

            // King is not in check -> Add to legal moves
            if (!isSquareAttacked(getKingPosition(player1), player2)) {
                legalMoves.push_back(move);
            }
            // Undo move
            undoMove();
        }


        // Debugging
        /*
        cout << "Move: " << pseudolegalMoves.size() << endl;
        for (int i = 0; i <pseudolegalMoves.size(); i++) {

            cout << "Move: " << i << endl;
            doMove(pseudolegalMoves[i]);
            printBoard();
            undoMove();

        }
         */


        /*
        Move move = pseudolegalMoves[2];
        move.print();
        doMove(move);
        printBoard();
        */
        return legalMoves;
    }

};

class PerftCounter {
public:
    int nodes = 0;
    int captures = 0;
    int enPassant = 0;
    int castles = 0;

    void print() {
        cout << "Nodes: " << nodes << " Captures: " << captures << " enPassant: " << enPassant << " Castles: " << castles << endl;
    }
};

void perft(ChessBoard position, PerftCounter &counter, int depth) {
    // Depth reached -> return
    if (depth == 0) {
        return;
    }

    // Generate all legal moves
    vector<Move> moves = position.generateMoves();
    //cout << "Depth: " << depth << " Moves: " << moves.size() << endl;
    for(Move move: moves) {
        position.doMove(move);
        move.print();
        cout << move.code() << endl;

        // Count flags when depth is reached
        if (depth == 1) {
            counter.nodes++;
            if (move.capture()) {
                counter.captures++;
            }

            if (move.code() == MOVE_CODE_EP_CAPTURE) {
                counter.enPassant++;
            }

            if (move.castleKingside() | move.castleQueenside()) {
                counter.castles++;
            }
        }

        // Call perf recursively for new positions
        perft(position, counter, depth-1);

        // Undo move
        position.undoMove();
    }
}

void testMoveGeneration(string fen, int depth) {
    // https://www.chessprogramming.org/Perft_Results

    // Create initial position
    ChessBoard position(fen);
    PerftCounter counter;
    cout << "Performance test: FEN: " << fen << " , Depth: " << depth << endl;

    // Perform test
     perft(position, counter, depth);

    // Log results
    counter.print();
    cout << endl;


}

