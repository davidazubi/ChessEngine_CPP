#include <string>
#include <iostream>
#include <vector>
#include <limits>
#include <map>
#include <math.h>

using namespace std;

// Bitboard
typedef unsigned long long bb;
const bb bbONE = 1ULL;
const bb bbZero = 0ULL;
const bb bbMax = numeric_limits<bb>::max();

// FEN Position
const string FEN_INITIAL = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
const string FEN_TEST_1 = "rnbqkbnr/pppppppp/8/8/8/8/8/RNBQKBNR w KQkq - 0 1";
const string FEN_TEST_2 = "8/8/8/2k5/4K3/8/8/8 w KQkq - 0 1";
const string FEN_TEST_3 = "rnbqkbnr/pppppppp/8/8/5R1N/8/8/RNBQKB2 w - - 0 1";
const string FEN_TEST_4 = "r1bq2k1/2ppbppp/p1n2n2/1pR1p1r1/P3P3/1B1PBN2/1PPNQPPP/4R1K1 b - - 0 1";
const string FEN_TEST_5 = "r2q2k1/2pp1ppp/p1n2n2/1pR1p1rb/2b5/8/2B1P2P/3K4 b - - 0 1";

const string FEN_PERF_2 = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -";

// Color
const char WHITE = 'w';
const char BLACK = 'b';

// Piece symbols
map<char, string> PIECE_SYMBOLS {{'P', "\u2659"},
                                 {'R', "\u2656"},
                                 {'N', "\u2658"},
                                 {'B', "\u2657"},
                                 {'Q', "\u2655"},
                                 {'K', "\u2654"},
                                 {'p', "\u265F"},
                                 {'r', "\u265C"},
                                 {'n', "\u265E"},
                                 {'b', "\u265D"},
                                 {'q', "\u265B"},
                                 {'k', "\u265A"},
                                 {' ', " "}};

// Piece codes
const char PIECE_CODES_ALL[12] = {'P', 'R', 'N', 'B', 'Q', 'K', 'p', 'r', 'n', 'b', 'q', 'k'};
const vector<char> PIECE_CODES_WHITE = {'P', 'R', 'N', 'B', 'Q', 'K'};
const vector<char> PIECE_CODES_BLACK = {'p', 'r', 'n', 'b', 'q', 'k'};

// Masks for files
const bb maskFiles[8] = {72340172838076673,
                         144680345676153346,
                         289360691352306692,
                         578721382704613384,
                         1157442765409226768,
                         2314885530818453536,
                         4629771061636907072,
                         9259542123273814144};

// Masks for Ranks
const bb maskRanks[8] = {255,
                         65280,
                         16711680,
                         4278190080,
                         1095216660480,
                         280375465082880,
                         71776119061217280,
                         18374686479671623680};

// Masks for Diagnals
const bb maskDiagonals[16] = {9241421688590303745,
                              4620710844295151872,
                              2310355422147575808,
                              1155177711073755136,
                              577588855528488960,
                              288794425616760832,
                              144396663052566528,
                              72057594037927936,
                              0,
                              128,
                              32832,
                              8405024,
                              2151686160,
                              550831656968,
                              141012904183812,
                              36099303471055874};

// Masks for Anti diagonals
const bb maskAntiDiagonals[16] = {72624976668147840,
                                  283691315109952,
                                  1108169199648,
                                  4328785936,
                                  16909320,
                                  66052,
                                  258,
                                  1,
                                  0,
                                  9223372036854775808,
                                  4647714815446351872,
                                  2323998145211531264,
                                  1161999622361579520,
                                  580999813328273408,
                                  290499906672525312,
                                  145249953336295424};

// Not on file constants for attak mask generation
const bb not_h_File =  9187201950435737471;
const bb not_a_File =  18374403900871474942;
const bb not_hg_File = 4557430888798830399;
const bb not_ab_File = 18229723555195321596;

// Pawn singlepush offset
map<char, int>pawSinglepushOffset = {{'w', 8}, {'b', -8}};
map<char, int>pawDoublepushOffset = {{'w', 16}, {'b', -16}};

// Bitboard mask castling
const bb maskCastlingWK = 240;
const bb maskCastlingWQ = 31;
const bb maskCastlingBK = 17293822569102704640;
const bb maskCastlingBQ = 2233785415175766016;
const bb initCastlingWK = 144;
const bb initCastlingWQ = 17;
const bb initCastlingBK = 10376293541461622784;
const bb initCastlingBQ = 1224979098644774912;

// Move codes
const int MOVE_CODE_QUIET_MOVE = 0;
const int MOVE_CODE_DOUBLE_PAWN_PUSH = 1;
const int MOVE_CODE_KING_CASTLE = 2;
const int MOVE_CODE_QUEEN_CASTLE = 3;
const int MOVE_CODE_CAPTURE = 4;
const int MOVE_CODE_EP_CAPTURE = 5;
const int MOVE_CODE_KNIGHT_PROMO = 8;
const int MOVE_CODE_BISHOP_PROMO = 9;
const int MOVE_CODE_ROOK_PROMO = 10;
const int MOVE_CODE_QUEEN_PROMO = 11;
const int MOVE_CODE_KNIGHT_PROMO_CAPTURE = 12;
const int MOVE_CODE_BISHOP_PROMO_CAPTURE = 13;
const int MOVE_CODE_ROOK_PROMO_CAPTURE = 14;
const int MOVE_CODE_QUEEN_PROMO_CAPTURE = 15;

// Promo types from move code
map<int, char> moveCodePromoTypeWHITE = {{MOVE_CODE_KNIGHT_PROMO, 'N'},
                                    {MOVE_CODE_BISHOP_PROMO, 'B'},
                                    {MOVE_CODE_ROOK_PROMO, 'R'},
                                    {MOVE_CODE_QUEEN_PROMO, 'Q'},
                                    {MOVE_CODE_KNIGHT_PROMO_CAPTURE, 'N'},
                                    {MOVE_CODE_BISHOP_PROMO_CAPTURE, 'B'},
                                    {MOVE_CODE_ROOK_PROMO_CAPTURE, 'R'},
                                    {MOVE_CODE_QUEEN_PROMO_CAPTURE, 'Q'}};

map<int, char> moveCodePromoTypeBLACK = {{MOVE_CODE_KNIGHT_PROMO, 'n'},
                                         {MOVE_CODE_BISHOP_PROMO, 'b'},
                                         {MOVE_CODE_ROOK_PROMO, 'r'},
                                         {MOVE_CODE_QUEEN_PROMO, 'q'},
                                         {MOVE_CODE_KNIGHT_PROMO_CAPTURE, 'n'},
                                         {MOVE_CODE_BISHOP_PROMO_CAPTURE, 'b'},
                                         {MOVE_CODE_ROOK_PROMO_CAPTURE, 'r'},
                                         {MOVE_CODE_QUEEN_PROMO_CAPTURE, 'q'}};

map<char, map<int, char>> moveCodePromoType = {{WHITE, moveCodePromoTypeWHITE}, {BLACK, moveCodePromoTypeBLACK}};

// Rook target squares castling
map<int, int> rookTargetCastling = {{0, 3}, {7, 5}, {56, 59}, {63, 61}};

// Rook initial locations castling
map<char, map<int, int>> rookInitialLocation = {{WHITE, {{MOVE_CODE_KING_CASTLE, 7}, {MOVE_CODE_QUEEN_CASTLE, 0}}},
                                                {BLACK, {{MOVE_CODE_KING_CASTLE, 63}, {MOVE_CODE_QUEEN_CASTLE, 57}}}};

// King path castling
map<string, vector<int>> kingPathCastling = {{"wk", {4, 5, 6}}, {"wq", {4, 3, 2}}, {"bk", {60, 61, 62}}, {"bq", {60, 59, 58}}};

// Square names Arithmetic notation
map<string, int> SQUARE_NOTATION = {{"a1", 0}, {"b1", 1}, {"c1", 2}, {"d1", 3}, {"e1", 4}, {"f1", 5}, {"g1", 6}, {"h1", 7},
                                 {"a2", 8}, {"b2", 9}, {"c2", 10}, {"d2", 11}, {"e2", 12}, {"f2", 13}, {"g2", 14}, {"h2", 15},
                                 {"a3", 16}, {"b3", 17}, {"c3", 18}, {"d3", 19}, {"e3", 20}, {"f3", 21}, {"g3", 22}, {"h3", 23},
                                 {"a4", 24}, {"b4", 25}, {"c4", 26}, {"d4", 27}, {"e4", 28}, {"f4", 29}, {"g4", 30}, {"h4", 31},
                                 {"a5", 32}, {"b5", 33}, {"c5", 34}, {"d5", 35}, {"e5", 36}, {"f5", 37}, {"g5", 38}, {"h5", 39},
                                 {"a6", 40}, {"b6", 41}, {"c6", 42}, {"d6", 43}, {"e6", 44}, {"f6", 45}, {"g6", 46}, {"h6", 47},
                                 {"a7", 48}, {"b7", 49}, {"c7", 50}, {"d7", 51}, {"e7", 52}, {"f7", 53}, {"g7", 54}, {"h7", 55},
                                 {"a8", 56}, {"b8", 57}, {"c8", 58}, {"d8", 59}, {"e8", 60}, {"f8", 61}, {"g8", 62}, {"h8", 63}};

//======================================================================================================================

// Bitboard Locic
vector<int> bitlocationByte[256];
map<bb, bb> targetSetRank[64];
map<bb, bb> targetSetFile[64];
map<bb, bb> targetSetDiagonal[64];
map<bb, bb> targetSetAntiDiagonal[64];
bb attackSetKnight[64];
bb attackSetKing[64];

map<char, vector<bb>> attackSetPawn;
map<char, vector<bb>> pawnMoveSet;

void bbSetBit(bb &board, int idx) {
    board |= bbONE << idx;
}

void bbResetBit(bb &board, int idx) {
    board ^= bbONE << idx;
}

bool bbGetBit(bb board, int idx) {
    return board & bbONE << idx;
}

string bbToString(bb board) {
    string s = "";
    for (int i = 0; i < 64; i++ ) {
        s.append(bbGetBit(board, i) ? "1" : "0");
    }
    return s;
}

bb getKeyFile(int file, bb bbPieces) {
    return (bbPieces & maskFiles[file]);
}

bb getKeyRank(int rank, bb bbPieces) {
    return bbPieces & maskRanks[rank];
}

bb getKeyDiagonal(int diagonal, bb bbPieces) {
    return bbPieces & maskDiagonals[diagonal];
}

bb getKeyAntiDiagonal(int antidiagonal, bb bbPieces) {
    return bbPieces & maskAntiDiagonals[antidiagonal];
}

int diagonalIndex(int square) {
    // Return diagonal number
    int file = square & 7;
    int rank = square >> 3;
    return (rank - file) & 15;
}

int antiDiagonalndex(int square) {
    // Return antidiagonal numer
    int file = square & 7;
    int rank = square >> 3;
    return (rank + file) ^ 7;
}

vector<int> bbGetTrueBits(bb board) {
    // Get the indices of the True bits as vector
    vector<int> bits;
    for (int i = 0; i < 8; i++ ) {
        // Mask the first byte
        bb currentByte = board >> 8*i & 255;

        // Add the bit locations from precompiled table
        for (auto idx: bitlocationByte[currentByte]) {
            bits.push_back(idx+i*8);
        }
    }
    return bits;
}

void bbPrintBoard(bb board) {
    cout << "+----------------+" << endl;
    for (int rank = 7; rank >= 0; rank--) {
        cout << "|";
        for (int file = 0; file < 8; file++) {
            int square = rank*8 + file;
            char symbol = (bbGetBit(board, square)) ? 'X' : ' ';
            cout << symbol << " ";
        }
        cout << "|" << rank+1 << endl;
    }
    cout << "+----------------+" << endl << " a b c d e f g h" << endl;
}

bb getTargetSquares(int square, char pieceCode, bb bbPiecesAll, bb bbPiecesPlayer1, bb bbPiecesPlayer2, char actPlayer) {
    bb targets = bbZero;
    int file = square & 7;
    int rank = square >> 3;


    // Generate bitboard with target squares
    switch(toupper(pieceCode)) {
        case 'R':
            // Rook: Generate bitboard with possible target squares
            targets |= targetSetRank[square][getKeyRank(rank, bbPiecesAll)];
            targets |= targetSetFile[square][getKeyFile(file, bbPiecesAll)];
            targets &= ~bbPiecesPlayer1;
            break;

        case 'B':
            // Bishop: Generate bitboard with possible target squares
            targets |= targetSetDiagonal[square][getKeyDiagonal(diagonalIndex(square), bbPiecesAll)];
            targets |= targetSetAntiDiagonal[square][getKeyAntiDiagonal(antiDiagonalndex(square), bbPiecesAll)];
            targets &= ~bbPiecesPlayer1;
            break;

        case 'N':
            // Knight: Generate bitboard with possible target squares
            targets |= attackSetKnight[square] &~bbPiecesPlayer1;
            break;

        case 'Q':
            // Queen: Generate bitboard with possible target squar
            targets |= targetSetRank[square][getKeyRank(rank, bbPiecesAll)];
            targets |= targetSetFile[square][getKeyFile(file, bbPiecesAll)];
            targets |= targetSetDiagonal[square][getKeyDiagonal(diagonalIndex(square), bbPiecesAll)];
            targets |= targetSetAntiDiagonal[square][getKeyAntiDiagonal(antiDiagonalndex(square), bbPiecesAll)];
            targets &= ~bbPiecesPlayer1;
            break;

        case 'K':
            targets |= attackSetKing[square] & ~bbPiecesPlayer1;
            break;

        case 'P':
            targets |= attackSetPawn[actPlayer][square] & bbPiecesPlayer2;
            break;

     default:
        break;

    }




    return targets;
}

vector<string> splitString(string s, string del = " ") {
    vector<string> words;
    int start = 0;
    int end = s.find(del);
    while (end != -1) {
        words.push_back(s.substr(start, end - start));
        start = end + del.size();
        end = s.find(del, start);
    }
    words.push_back(s.substr(start, end - start));
    return words;
}

bool stringContains(string s, char c) {
    return s.find(c) != std::string::npos;
}


//=====================================================================================================================

void bb_precompile() {
    // Precompile index of True bits in all possible bytes
    for(int i = 0; i < 256; i++) {
        // Get True bits in byte
        for (int j = 0; j < 8; j++) {
            if (i & 1 << j) {
                bitlocationByte[i].push_back(j);
            }
        }
    }

    // Generate for every rank
    for (int r = 0; r < 8; r++) {
        // Generate for evey square on the file
        for (int f = 0; f < 8; f++) {
            int square = r*8 + f;
            // Generate for evey possible state of the rank
            for (int p = 0; p < 256; p++) {
                // Create bitboard
                bb key = bbZero;
                int permutation = p | (1 << f);

                for (int bit = 0; bit < 8; bit++) {
                    if (permutation & (1 << bit)) {
                        bbSetBit(key, bit + (8 * r));
                    }
                }

                // Targets
                bb targets = bbZero;


                // Targets left
                for (int sq = square  - 1; sq >= r * 8; sq--) {
                    bbSetBit(targets, sq);
                    if (bbGetBit(key, sq)) {
                        break;
                    }
                }

                // Targets right
                for (int sq = square  + 1; sq <= r * 8 + 7; sq++) {
                    bbSetBit(targets, sq);
                    if (bbGetBit(key, sq)) {
                        break;
                    }
                }

                targetSetRank[square][key] = targets;

            }

        }
    }

    // Generate for every file
    for (int f = 0; f < 8; f++) {
        // Generate for evey square on the rank
        for (int r = 0; r < 8; r++) {
            int square = r*8 + f;
            // Generate for evey possible state of the file
            for (int p = 0; p < 256; p++) {
                // Create bitboard
                bb key = bbZero;
                int permutation = p | (1 << r);

                for (int bit = 0; bit < 8; bit++) {
                    if (permutation & (1 << bit)) {
                        bbSetBit(key, (bit * 8) + f);
                    }
                }

                // Targets
                bb targets = bbZero;
                // Targets up
                for (int sq = square + 8; sq <= 56+f; sq+=8) {
                    bbSetBit(targets, sq);
                    if (bbGetBit(key, sq)) {
                        break;
                    }
                }

                // Targets down
                for (int sq = square - 8; sq >= f; sq-=8) {
                    bbSetBit(targets, sq);
                    if (bbGetBit(key, sq)) {
                        break;
                    }
                }

                targetSetFile[square][key] = targets;
            }

        }
    }

    // Generate for every diagonal
    for (int square = 0; square < 64; square++) {
        // Diagonal index
        int dIdx = diagonalIndex(square);

        // Generate for evey possible state of the diagonal
        bb diagonal = maskDiagonals[dIdx];
        vector<int> diagonalSquares = bbGetTrueBits(diagonal);
        int diagonalLength = diagonalSquares.size();
        int nPermutations = pow(2, diagonalLength);

        // Find start index
        int startIdx;
        for (int i = 0; i < diagonalLength; i++) {
            if (diagonalSquares[i] == square) {
                startIdx = i;
            }
        }

        // Generate for all permutations
        for (int p = 0; p < nPermutations; p++) {
            // Create bitboard
            bb key = bbZero;
            int permutation = p | (1 << startIdx);

            // Set bits in permutation
            for (int bit = 0; bit < diagonalLength; bit++) {
                if (permutation & (1 << bit)) {
                    bbSetBit(key, diagonalSquares[bit]);
                }
            }

            // Targets
            bb targets = bbZero;

            // Targets up
            for (int i = startIdx+1; i < diagonalLength; i++) {
                bbSetBit(targets, diagonalSquares[i]);
                if (bbGetBit(key, diagonalSquares[i])) {
                    break;
                }
            }

            // Targets down
            for (int i = startIdx-1; i >= 0; i--) {
                bbSetBit(targets, diagonalSquares[i]);
                if (bbGetBit(key, diagonalSquares[i])) {
                    break;
                }
            }

            targetSetDiagonal[square][key] = targets;
        }
    }

    // Generate for every diagonal
    for (int square = 0; square < 64; square++) {
        // Diagonal index
        int dIdx = antiDiagonalndex(square);

        // Generate for evey possible state of the diagonal
        bb diagonal = maskAntiDiagonals[dIdx];
        vector<int> diagonalSquares = bbGetTrueBits(diagonal);
        int diagonalLength = diagonalSquares.size();
        int nPermutations = pow(2, diagonalLength);

        // Find start index
        int startIdx;
        for (int i = 0; i < diagonalLength; i++) {
            if (diagonalSquares[i] == square) {
                startIdx = i;
            }
        }

        for (int p = 0; p < nPermutations; p++) {
            // Create bitboard
            bb key = bbZero;
            int permutation = p | (1 << startIdx);

            // Set bits in permutation
            for (int bit = 0; bit < diagonalLength; bit++) {
                if (permutation & (1 << bit)) {
                    bbSetBit(key, diagonalSquares[bit]);
                }
            }

            // Targets
            bb targets = bbZero;

            // Targets up
            for (int i = startIdx+1; i < diagonalLength; i++) {
                bbSetBit(targets, diagonalSquares[i]);
                if (bbGetBit(key, diagonalSquares[i])) {
                    break;
                }
            }

            // Targets down
            for (int i = startIdx-1; i >= 0; i--) {
                bbSetBit(targets, diagonalSquares[i]);
                if (bbGetBit(key, diagonalSquares[i])) {
                    break;
                }
            }
            targetSetAntiDiagonal[square][key] = targets;
        }
    }

    // Attack set Knight: Generate attack bitboard for every square
    for (int i = 0; i < 64; i++) {
        // Initialize bitboard with knight on square i
        bb bitboard = bbZero;
        bbSetBit(bitboard, i);

        // Set bits for the square that can be attacked
        bb attacks = bbZero;
        attacks |= (bitboard << bb(17)) & not_a_File;
        attacks |= (bitboard << bb(10)) & not_ab_File;
        attacks |= (bitboard >> bb(6 )) & not_ab_File;
        attacks |= (bitboard >> bb(15)) & not_a_File;
        attacks |= (bitboard << bb(15)) & not_h_File;
        attacks |= (bitboard << bb(6 )) & not_hg_File;
        attacks |= (bitboard >> bb(10)) & not_hg_File;
        attacks |= (bitboard >> bb(17)) & not_h_File;

        // Add bitboard to array
        attackSetKnight[i] = attacks;
    }

    // Attack set King: Generate attack bitboard for every square
    for (int i = 0; i < 64; i++) {
        // Initialize bitboard with king on square i
        bb bitboard = bbZero;
        bbSetBit(bitboard, i);

        // Set bits for the square that can be attacked
        bb attacks = bbZero;
        attacks |= (bitboard >> bb(8));
        attacks |= (bitboard >> bb(9)) & not_h_File;
        attacks |= (bitboard >> bb(7)) & not_a_File;
        attacks |= (bitboard >> bb(1)) & not_h_File;
        attacks |= (bitboard << bb(8));
        attacks |= (bitboard << bb(9)) & not_a_File;
        attacks |= (bitboard << bb(7)) & not_h_File;
        attacks |= (bitboard << bb(1)) & not_a_File;

        // Add bitboard to array
        attackSetKing[i] = attacks;
    }

    // Attack set Pawn: Generate attack bitboard for every square and color
    for (int i = 0; i < 64; i++) {
        // Initialize bitboard with pawn on square i
        bb bitboard = bbZero;
        bbSetBit(bitboard, i);

        // Set bit for squares that can be attacked
        bb attacksWhite = bbZero;
        bb attacksBlack = bbZero;

        // Generate for white or black pawns
            attacksWhite |= (bitboard << bb(7)) & not_h_File;
            attacksWhite |= (bitboard << bb(9)) & not_a_File;
            attacksBlack |= (bitboard >> bb(7)) & not_a_File;
            attacksBlack |= (bitboard >> bb(9)) & not_h_File;

        // Add bitboard to array
        attackSetPawn['w'].push_back(attacksWhite);
        attackSetPawn['b'].push_back(attacksBlack);
    }

    // Move set Pawn: Generate attack bitboard for every square and color
    for (int i = 0; i < 64; i++) {
        // Initialize bitboard with pawn on square i
        bb bitboard = bbZero;
        bbSetBit(bitboard, i);

        // Set bit for squares the pawn can move to
        bb movesWhite = bbZero;
        bb movesBlack = bbZero;

        // Generate for white or black pawns{
        movesWhite |= (bitboard << bb(8));
        movesWhite |= (bitboard << bb(16)) & maskRanks[3];
        movesBlack |= (bitboard >> bb(8));
        movesBlack |= (bitboard >> bb(16)) & maskRanks[4];

        // Add bitboard to array
        pawnMoveSet['w'].push_back(movesWhite);
        pawnMoveSet['b'].push_back(movesBlack);
    }


}
