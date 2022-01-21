#include <iostream>
using namespace std;

class Move {
private:
    // 16 bits
    unsigned short bits;

public:
    Move(int from, int to, bool promotion, bool capture, bool special1, bool special0) {
        bits = 0;
        bits |= from;
        bits |= to << 6;
        bits |= promotion << 15;
        bits |= capture << 14;
        bits |= special1 << 13;
        bits |= special0 << 12;
    }

    int from() {
        // Bit 0-5
        return bits & 63;
    }

    int to(){
        // Bit 6-11
        return (bits >> 6) & 63;
    }

    bool promotion() {
        // Bit 15
        return (bits >> 15) & 1;
    }

    bool capture() {
        // Bit 14
        return (bits >> 14) & 1;
    }

    bool special1() {
        // Bit 13
        return (bits >> 13) & 1;
    }

    bool special0() {
        // Bit 12
        return (bits >> 12) & 1;
    }

    bool castleKingside() {
        return code() == 2;
    }

    bool castleQueenside() {
        return code() == 3;
    }

    int code() {
        return bits >> 12;
    }

    void print() {
        cout << "From: " << from() << " To: " << to() << " Capture: " << capture() << " Promo: " << promotion() << " s1: " << special1() << " s0: " << special0() << endl;
    }

    void set_from(int from) {
        bits &= 65472;
        bits |= from;
    }
    void set_to(int to) {
        bits &= 61503;
        bits |= to << 6;
    }
    void set_promotion(bool promotion) {
        bits |= promotion << 15;
    }
    void set_capture(bool capture) {
        bits |= capture << 14;
    }
    void set_special1(bool  special1) {
        bits |= special1 << 13;
    }
    void set_special0(bool special0) {
        bits |= special0 << 12;
    }

};


