#include <iostream>
#include <vector>
#include <tuple>
#include <array>
#include <algorithm>

// using bb = __int128_t;
using namespace std;


const int H = 6;
const int W = 11;

const int NPIECES = 11;
const int SIZE = 5;

using piece = vector<tuple<int, int>>;
using board = array<array<int, W>, H>;

const vector<vector<string>> piece_data = {
  { " #   ",
    "###  ",
    " #   ",
    " #   ",
    "     "},
  { "##   ",
    "#    ",
    "#    ",
    "#    ",
    "#    "},
  { "##   ",
    " #   ",
    "##   ",
    "#    ",
    "     "},
  { " #   ",
    "###  ",
    "#    ",
    "#    ",
    "     "},
  { "##   ",
    "#    ",
    "###  ",
    "     ",
    "     "},
  { "  #  ",
    " ##  ",
    "###  ",
    "     ",
    "     "},
  { " #   ",
    " #   ",
    " #   ",
    "##   ",
    "#    "},
  { "#    ",
    "##   ",
    "#    ",
    "#    ",
    "#    "},
  { " #   ",
    "##   ",
    "##   ",
    "#    ",
    "     "},
  { "# #  ",
    "###  ",
    " #   ",
    "     ",
    "     "},
  { "#    ",
    "##   ",
    "#    ",
    "##   ",
    "     "},
};

vector<vector<piece>> P(11);
vector<vector<vector<tuple<int, piece>>>> M(H, vector<vector<tuple<int, piece>>>(W));



piece read_piece_string(const vector<string> & input) {
  piece v;
  for (int y = 0; y < SIZE; ++y) {
    for (int x = 0; x < SIZE; ++x) {
      if (input[y][x] == '#') {
        v.push_back(make_tuple(x, y));
      }
    }
  }
  return v;
}

piece rotate(const piece & p) {
  piece v;
  for (auto t : p) {
    int x = get<0>(t);
    int y = get<1>(t);
    v.push_back(make_tuple(y, SIZE - 1 - x));
  }
  return v;
}

bool can_place(const board & b, const piece & p) {
  for (auto t : p) {
    int x = get<0>(t);
    int y = get<1>(t);
    if (b[y][x] != 0) {
      return false;
    }
  }
  return true;
}

void place(int color, board & b, const piece & p) {
  for (auto t : p) {
    int x = get<0>(t);
    int y = get<1>(t);
    b[y][x] = color;  
  }
}

void unplace(board & b, const piece & p) {
  place(0, b, p);
}

bool valid_piece(const piece & p) {
  if (p.size() <= 0) {
    return false;
  }

  for (auto t : p) {
    int x = get<0>(t);
    int y = get<1>(t);
    if (!(0 <= x && 0 <= y && x < W && y < H)) {
      return false;
    }
  }
  return true;
}

piece shift(const piece & p, int dx, int dy) {
  piece v;
  for (auto t : p) {
    int x = get<0>(t);
    int y = get<1>(t);
    v.push_back(make_tuple(x + dx, y + dy));
  }
  return v;
}

void init_pieces() {
  piece pcs[4];
  for (int i = 0; i < NPIECES; i++) {
    pcs[0] = read_piece_string(piece_data[i]);
    for (int k = 1; k < 4; k++) {
      pcs[k] = rotate(pcs[k-1]);
    }

    for (int k = 0; k < 4; k++) {
      for (int y = -SIZE + 1; y < H; y++) {
        for (int x = -SIZE + 1; x < W; x++) {
          piece p = shift(pcs[k], x, y);
          if (valid_piece(p)) {
            P[i].push_back(p);
          }
        }
      }
    }
  }
}

void print_board(const board & b) {
  for (int y = 0; y < H; y++) {
    for (int x = 0; x < W; x++) {
      cout << std::hex << b[y][x] << ' ';
    }
    cout << endl;
  }
  cout << std::dec;
}

void print_piece(const piece & p) {
  board b {};

  for (auto t : p) {
    int x = get<0>(t);
    int y = get<1>(t);
    b[y][x] = 1;
  }

  for (int y = 0; y < H; y++) {
    for (int x = 0; x < W; x++) {
      cout << (b[y][x] == 1 ? '#' : '.') << ' ';
    }
    cout << endl;
  }
}

int cnt = 0;

bool search1(board & b, int i) {
  if (!(i < NPIECES)) {
    print_board(b);
    return true;
  }

  for (auto & p : P[i]) {
    if (!can_place(b, p)) {
      continue;
    }
    // color should not be zero, so increment it
    place(i + 1, b, p);
    cnt++;

    if (search1(b, i + 1)) {
      return true;
    } else {
      unplace(b, p);
    }

    if (cnt % (65536 * 10) == 0) {
      cout << cnt << endl;
    }
  }

  return false;
}

void init_map() {
  for (int y = 0; y < H; y++) {
    for (int x = 0; x < W; x++) {
      vector<tuple<int, piece>> v;

      for (int c = 0; c < NPIECES; c++) {
        for (auto & p : P[c]) {
          if (std::find(p.begin(), p.end(), make_tuple(x, y)) != p.end()) {
            v.push_back(make_tuple(c, p));
          }
        }
      }

      M[y][x] = v;
    }
  }
}

bool search2(board & b, vector<int> & used, int y, int x) {
  if (!(y < H)) {
    print_board(b);
    cout << "---" << endl;
    return false;
  }

  if (!(x < W)) {
    return search2(b, used, y + 1, 0);
  }

  if (b[y][x] != 0) {
    return search2(b, used, y, x + 1);
  }

  // select
  for (const auto & tuple : M[y][x]) {
    // cnt++;

    // if (cnt % (65536 * 16) == 0) {
    //   cout << cnt << ": (" << y << ", " << x << ")" << endl;
    // }

    const int c = get<0>(tuple);
    const piece & p = get<1>(tuple);

    if (used[c] || !can_place(b, p)) {
      continue;
    }

    place(c + 1, b, p);
    used[c] = 1;

    if (search2(b, used, y, x + 1)) {
      return true;
    } else {
      used[c] = 0;
      unplace(b, p);
    }
  }
  return false; 
}

int main()
{
  init_pieces();
  init_map();

  board b {};
  vector<int> used(NPIECES, 0);
  search2(b, used, 0, 0);
}










