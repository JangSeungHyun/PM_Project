#include <iostream>
#include <fstream>
#include <Windows.h>
#include <cwchar>
#include <cstdlib>
#include <cctype>
using namespace std;

const int TILE_BOARD_LENGTH = 4;
const int BELT_LENGTH = 20;
const int BELT_TILE_WIDTH = 3;
const int PLAYER_START_LOCATION_IDX = 9;
const int NUM_CHANCE = 2;
const int CHANCE_1_DURATION_SECONDS = 10;

enum SUIT {
	SPADE = 9824,  //L'¢¼'
	DIAMOND = 9670,  //L'¡ß'
	HEART = 9829,  //L'¢¾'
	CLOVER = 9827  //L'¢À'
};
enum COLOR { BLACK = 0, GREEN = 2, BLUE = 9, RED = 12, YELLOW = 14, WHITE = 15 };
const char TILE_CHARACTERS[TILE_BOARD_LENGTH][TILE_BOARD_LENGTH] = { {'A', 'B', 'C', 'D'}, {'E','F', 'G', 'H'}, {'I', 'J', 'K', 'L'}, {'M', 'N', 'O', 'P'} };

void textcolor(int foreground, int background) {
	int color = foreground + background * 16;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

class Pattern {
public:
	Pattern();
	Pattern(const char description[]);
	bool operator== (const Pattern& other) const;
	bool operator!= (const Pattern& other) const;
private:
	COLOR fg_color;
	COLOR bg_color;
	SUIT suit;
	friend wostream& operator<<(wostream& output, const Pattern& p); 
};

Pattern::Pattern() {
	fg_color = BLACK;
	bg_color = BLACK;
	suit = SPADE;
}

Pattern::Pattern(const char description[]) {
	switch (description[0]) {
	case 'G':
		fg_color = WHITE;
		bg_color = GREEN;
		break;
	case 'Y':
		fg_color = YELLOW;
		bg_color = BLACK;
		break;
	case 'R':
		fg_color = WHITE;
		bg_color = RED;
		break;
	case 'B':
		fg_color = WHITE;
		bg_color = BLUE;
		break;
	}
	switch (description[1]) {
	case 'H':
		suit = HEART;
		break;
	case 'S':
		suit = SPADE;
		break;
	case 'D':
		suit = DIAMOND;
		break;
	case 'C':
		suit = CLOVER;
		break;
	}
}

bool Pattern::operator == (const Pattern& other) const {
	if (bg_color == other.bg_color && suit == other.suit) {
		return true;
	}
	else {
		return false;
	}
}

bool Pattern::operator != (const Pattern& other) const {
	if (bg_color != other.bg_color || suit != other.suit) {
		return true;
	}
	else {
		return false;
	}
}

wostream& operator <<(wostream& output, const Pattern& p) {
	textcolor(p.fg_color, p.bg_color);
	wchar_t pic = p.suit;
	output << pic;
	textcolor(WHITE, BLACK);
	return output;
}

class Board {
public:
	Board() {};
	Board(const char characters[][TILE_BOARD_LENGTH], const Pattern patterns[]);
	Pattern get_pattern(char character) const;
	void print() const;
	void print_one_flipped(char flipped_character) const;
	void print_all_flipped() const;
private:
	pair<char, Pattern> tiles[TILE_BOARD_LENGTH][TILE_BOARD_LENGTH]; 
};

Board::Board(const char characters[][TILE_BOARD_LENGTH], const Pattern patterns[]) {
	for (int i = 0; i < TILE_BOARD_LENGTH; i++) {
		for (int j = 0; j < TILE_BOARD_LENGTH; j++) {
			tiles[i][j].first = characters[i][j];
			tiles[i][j].second = patterns[4 * i + j];
		}
	}
}

Pattern Board::get_pattern(char character) const {
	int char_num = 0;
	if (isupper(character)) {
		char_num = character - 65;
	}
	else if (!isupper(character)) {
		char_num = character - 97;
	}
	int row = char_num / 4;
	int col = char_num % 4;
	return tiles[row][col].second;
}

void Board::print() const {
	cout << "====== BOARD ======" << endl;
	for (int i = 0; i < TILE_BOARD_LENGTH; i++) {
		cout << "-------------------" << endl;
		cout << " ";
		for (int j = 0; j < TILE_BOARD_LENGTH; j++) {
			cout << "| ";
			cout << tiles[i][j].first;
			cout << " ";
		}
		cout << "|" << endl;
	}
	cout << "-------------------" << endl;
}

void Board::print_one_flipped(char flipped_character) const {
	int char_num = 0;
	if (isupper(flipped_character)) {
		char_num = flipped_character - 65;
	}
	else if (!isupper(flipped_character)) {
		char_num = flipped_character - 97;
	}
	int row = char_num / 4;
	int col = char_num % 4;
	system("CLS");
	cout << "====== BOARD ======" << endl;
	for (int i = 0; i < TILE_BOARD_LENGTH; i++) {
		cout << "-------------------" << endl;
		cout << " ";
		for (int j = 0; j < TILE_BOARD_LENGTH; j++) {
			if (i == row && j == col) {
				cout << "| ";
				wcout << this->get_pattern(flipped_character);
				cout << " ";
			}
			else {
				cout << "| ";
				cout << tiles[i][j].first;
				cout << " ";
			}
		}
		cout <<"|" << endl;
	}
	cout << "-------------------" << endl;
}

void Board::print_all_flipped() const {
	for (int i = 10; i > 0; i < i--) {
		system("CLS");
		cout << "====== BOARD ======" << endl;
		for (int i = 0; i < TILE_BOARD_LENGTH; i++) {
			cout << "-------------------" << endl;
			cout << " ";
			for (int j = 0; j < TILE_BOARD_LENGTH; j++) {
				cout << "| ";
				wcout << tiles[i][j].second;
				cout << " ";
			}
			cout << "|";
			cout << endl;
		}
		cout << "-------------------" << endl;
		cout << i << "second(s) left!";
		Sleep(1000);
	}
}

class Belt {
public:
	Belt() {};
	Belt(const Pattern patterns[]);
	void initialize_tiles();
	void roll_forward(); 
	void roll_backward(); 
	bool out_of_bounds() const;
	int get_player_location() const;
	Pattern get_pattern_at_current_location() const;
private:
	int player_location = PLAYER_START_LOCATION_IDX; 
	Pattern tiles[BELT_LENGTH];
	Pattern pattern_bag[TILE_BOARD_LENGTH * TILE_BOARD_LENGTH];
	friend wostream& operator<<(wostream& output, const Belt& belt);
};

Belt::Belt(const Pattern patterns[]) {
	for (int i = 0; i < TILE_BOARD_LENGTH * TILE_BOARD_LENGTH; i++) {
		pattern_bag[i] = patterns[i];
	}
	initialize_tiles();
}

int Belt::get_player_location() const {
	return player_location;
}

void Belt::initialize_tiles() {
	this->tiles[0] = this->pattern_bag[rand() % 16];
	Pattern temp = this->pattern_bag[rand() % 16];
	for (int i = 1; i < BELT_LENGTH; i++) {
		while (this->tiles[i-1] == temp) {
			temp = this->pattern_bag[rand() % 16];
		}
		tiles[i] = temp;
	}
}

void Belt::roll_forward() {
	player_location++;
}

void Belt::roll_backward() {
	player_location = player_location - 2;
	for (int i = 0; i < BELT_LENGTH-1; i++) {
		tiles[i] = tiles[i + 1];
	}
	Pattern temp = this->pattern_bag[rand() % 16];
	while (tiles[BELT_LENGTH - 2] == temp) {
		temp = pattern_bag[rand() % 16];
	}
	tiles[BELT_LENGTH - 1] = temp;
}

bool Belt::out_of_bounds() const {
	if (player_location < 0 || player_location >= BELT_LENGTH) {
		return false;
	}
	else {
		return true;
	}
}

Pattern Belt::get_pattern_at_current_location() const {
	return tiles[player_location];
}

wostream& operator << (wostream& output, const Belt& belt) {
	Pattern black = Pattern();
	for (int i = 0; i < BELT_TILE_WIDTH+2; i++) {
		if (i < BELT_TILE_WIDTH) {
			for (int j = 0; j < 20; j++) {
				for (int k = 0; k < BELT_TILE_WIDTH; k++) {
					output << belt.tiles[j];
				}
				output << '|';
			}
		}
		else if(i == BELT_TILE_WIDTH){
			for (int j = 0; j < 20; j++) {
				if (j == belt.player_location) {
					output << "======";
				}
				else {
					for (int k = 0; k < BELT_TILE_WIDTH; k++) {
						output << black;
					}
					output << " ";
				}
			}
		}
		else {
			for (int j = 0; j < 20; j++) {
				if (j == belt.player_location) {
					output << " Here ";
				}
				else {
					for (int k = 0; k < BELT_TILE_WIDTH; k++) {
						output << black;
					}
					output << " ";
				}
			}
		}
		cout << endl;
	}
	return output;
}

class Player {
public:
	Player() {};
	Player(const Pattern patterns[]);
	void print_belt() const; 
	bool check_pattern_with_belt(Pattern p); 
	bool did_win() const;
	bool did_lose() const;
	void print_chance_list() const; 
	void use_chance(int chance_idx);
	bool has_any_remaining_chance() const; 
	bool has_remaining_chance(int chance_idx) const;
private:
	Belt belt;
	bool win;
	bool lose;
	int chance_count[NUM_CHANCE];
};

Player::Player(const Pattern patterns[]): win(true), lose(true) {
	belt = Belt(patterns);
	chance_count[0] = 1;
	chance_count[1] = 2;
}

void Player::print_belt() const {
	wcout << belt;
	cout << endl;
}

bool Player::check_pattern_with_belt(Pattern p) {
	if (p == belt.get_pattern_at_current_location()) {
		belt.roll_forward();
		return true;
	}
	else {
		belt.roll_backward();
		return false;
	}
}

bool Player::did_win() const {
	if (belt.out_of_bounds()&&belt.get_player_location()>=BELT_LENGTH) {
		return win;
	}
	else {
		return false;
	}
	
}

bool Player::did_lose() const {
	if (belt.out_of_bounds()&&belt.get_player_location() < 0) {
		return lose;
	}
	else {
		return false;
	}

}

void Player::print_chance_list() const{
	cout << "Chance List : ";
	for (int i = 0; i < NUM_CHANCE; i++) {
		if (chance_count[i] != 0) {
			cout << chance_count[i] << "	";
		}
	}
	cout << endl;
	if (chance_count[0] != 0) {
		cout << "Chance 1 : Show back sides of all alphabet tiles." << endl;
	}
	if (chance_count[1] != 0) {
		cout << "Chance 2 : Pass your turn" << endl;
	}

}

void Player::use_chance(int chance_idx) {
	chance_count[chance_idx] = 0;
}

bool Player::has_any_remaining_chance() const {
	if (chance_count[0] == 0 && chance_count[1] == 0) {
		return false;
	}
	else {
		return true;
	}
}

bool Player::has_remaining_chance(int chance_idx) const {
	if (chance_count[chance_idx]!=0) {
		return true;
	}
	else {
		false;
	}
}

class Game {
public:
	Game(Pattern patterns[]);
	void run(); 
	bool play_turn(); 
private:
	int current_player_idx; 
	Player players[2];
	Board board;
};

Game::Game(Pattern patterns[]) : current_player_idx(0) {
	players[0] = Player(patterns);
	players[1] = Player(patterns);
	board = Board(TILE_CHARACTERS, patterns);
}

void Game::run() {
	system("CLS");
	cout << "Game Start!";
	Sleep(2000);
	while (true) {
		char input = 0;
		if (players[0].did_win() || players[1].did_lose()) {
			cout << "Player 1's Win!" << endl;
			cout << "ReGame? Y(y)/N(n)" << endl;
			cout << ">>";
			break;
		}
		else if (players[0].did_lose() || players[1].did_win()) {
			cout << "Player 2's Win!" << endl;
			cout << "ReGame? Y(y)/N(n)" << endl;
			cout << ">>";
			break;
		}
		system("CLS");
		board.print();
		cout << "Player 1" << endl;
		players[0].print_belt();
		cout << endl;
		cout << "Player 2" << endl;
		players[1].print_belt();
		cout << endl;
		if (!play_turn()) {
			if (current_player_idx == 0) {
				current_player_idx = 1;
			}
			else {
				current_player_idx = 0;
			}
		}
	}
}

bool Game::play_turn() {
	char input;
	if (current_player_idx == 0) {
		cout << "Player 1's Turn!" << endl;
		cout << endl;
		cout << "Enter either chance number to use chance or alphabet tile to flip on board" << endl;
		if (players[0].has_any_remaining_chance()) {
			players[0].print_chance_list();
		}
		cout << ">> ";
		cin >> input;
		if (!isdigit(input)) {
			system("CLS");
			board.print_one_flipped(input);
			cout << "Player 1" << endl;
			players[0].print_belt();
			cout << endl;
			cout << "Player 2" << endl;
			players[1].print_belt();
			cout << endl;
			cout << "Player 1's Turn!" << endl;
			cout << endl;
			cout << "Player 1 picks '" << input << "' tile." << endl;
			Sleep(2000);
			if (this->players[current_player_idx].check_pattern_with_belt(board.get_pattern(input))) {
				return true;
			}
			else {
				return false;
			}
		}
		else {
			if (input == '1' && players[0].has_remaining_chance(0)) {
				players[0].use_chance(0);
				board.print_all_flipped();
				return true;
			}
			else if (input == '2' && players[0].has_remaining_chance(1)) {
				players[0].use_chance(1);
				return false;
			}
			else {
				cout << "Wrong Chance Number!" << endl;
			}
		}
	}
	else if (current_player_idx == 1) {
		cout << "Player 2's Turn!" << endl;
		cout << endl;
		cout << "Enter either chance number to use chance or alphabet tile to flip on board" << endl;
		if (players[1].has_any_remaining_chance()) {
			players[1].print_chance_list();
		}
		cout << ">> ";
		cin >> input;
		if (!isdigit(input)) {
			system("CLS");
			board.print_one_flipped(input);
			cout << "Player 1" << endl;
			players[0].print_belt();
			cout << endl;
			cout << "Player 2" << endl;
			players[1].print_belt();
			cout << endl;
			cout << "Player 2's Turn!" << endl;
			cout << endl;
			cout << "Player 2 picks '" << input << "' tile." << endl;
			Sleep(2000);
			if (this->players[current_player_idx].check_pattern_with_belt(board.get_pattern(input))) {
				return true;
			}
			else {
				return false;
			}
		}
		else {
			if (input == '1' && players[0].has_remaining_chance(0)) {
				players[1].use_chance(0);
				board.print_all_flipped();
				return true;
			}
			else if (input == '2' && players[0].has_remaining_chance(1)) {
				players[1].use_chance(1);
				return false;
			}
			else {
				cout << "Wrong Chance Number!" << endl;
			}
		}
	}
}

int main(void) {
	setlocale(LC_ALL, "");
	system("mode con lines=35 cols=145");
	srand(3);
	//srand((unsigned int)time(NULL));
	ifstream ifs("Tile.txt");
	if (ifs.fail()) {
		cout << "Input tile text not found! Quitting..." << endl;
		exit(1);
	}
	char description[TILE_BOARD_LENGTH * TILE_BOARD_LENGTH][2];
	Pattern pattern[TILE_BOARD_LENGTH * TILE_BOARD_LENGTH];
	for (int i = 0; i < TILE_BOARD_LENGTH * TILE_BOARD_LENGTH; i++) {
		for (int j = 0; j < 2; j++) {
			ifs >> description[i][j];
		}
		pattern[i] = Pattern(description[i]);
	}
	char regame;
	do {
		Game start = Game(pattern);
		start.run();
		cin >> regame;
	} while (regame == 'Y' || regame == 'y');
	cout << "Thanks for playing the game!" << endl;
	return 0;
}