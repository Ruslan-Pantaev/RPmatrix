/* Author:		    Ruslan Pantaev <arpiseaQ>
 * License:		    MIT License
 * Start_Date:		6/21/2017
 * Last_modified:	7/4/2017
 * Compilation:		<g++ rpmatrix.cpp -std=c++11 -lpthread -lncurses -Wall -o <pgm_name>> (use -g flag for valgrind debugging)
 * Note:		      Containing all structs / funcs in one file for portability, ease of use and simpler compilation
 * Known Bugs:		* Rare floating point exceptions / segfaults with smaller term win sizes
 */

#include <iostream>
#include <vector>
#include <map>								// For Matrix symbols hashmap
#include <string>
#include <unistd.h>							// For sleep()
#include <sys/ioctl.h>							// For getting Terminal Console dimensions
#include <ncurses.h>							// For future updates
#include <iomanip>							// For std::hex and setw()
#include <thread>        						// std::this_thread::sleep_for	Alt to sleep()
#include <chrono>							// std::chrono::seconds 	Alt to sleep()
#include <pthread.h>							// POSIX threads for dynamic user input
#include <boost/random.hpp>						// For random gen <random_int()>
#include <boost/generator_iterator.hpp>
#include <random>
#include <sstream>
// #include <cstdio>							// Might be redundant w/ c++14
// #include <cstdlib>							// Might be redundant w/ c++14
// #include <future>							// For async thread, NOT IN USE, using pthreads instead
// #include <codecvt>							// For wide_char conversions, NOT IN USE
// #include <cwchar>							// For extended ASCII / wide_char, NOT IN USE
// #include <linux/kd.h>						// For KDMKTONE

using namespace std;

	
struct Matrix {
	int speed = 20000;
	int terminal_width;
	bool rainbow_on = false;
	// To change duration of each color
	int rainbow_mod = 30;
	// <matrix> by default
	string color = "matrix";

	map<string, vector<string>> colors = {
		/* start-FONTS */
		// Feel free to play around with these and add custom color configs!
		{"matrix",
			{
			"\e[1;33m",	// COUT_0 bold fire 	<strong orange/yellow>
			"\e[1;32m",	// COUT_1 bold main 	<dark green>
			"\e[1;37m",	// COUT_2 bold header 	<bright white>
			"\e[0;92m",	// COUT_3 alt mid main 	<mid green>
			"\e[1;32m",	// COUT_4 bold main 	<dark green>
			"\e[0;32m",	// COUT_5 light main 	<light green>
			"\e[1;33m",	// COUT_6 bold fire 	<strong orange/yellow>
			"\e[0;33m",	// COUT_7 light fire 	<light orange/yellow>
			"\e[0;32m",	// COUT_8 light main 	<light green>
			"\e[40m"	// [9] BACKGROUND
			}
		}, {"black",
			{
			"\e[1;33m",	// COUT_0 bold fire
			"\e[1;30m",	// COUT_1 bold main
			"\e[1;90m",	// COUT_2 bold header
			"\e[0;30m",	// COUT_3 alt mid main
			"\e[1;30m",	// COUT_4 bold main
			"\e[1;30m",	// COUT_5 light main
			"\e[1;33m",	// COUT_6 bold fire
			"\e[0;33m",	// COUT_7 light fire
			"\e[1;30m",	// COUT_8 light main
			"\e[40m"	// [9] BACKGROUND
			}
		}, {"red",
			{
			"\e[1;33m",	// COUT_0 bold fire
			"\e[1;31m",	// COUT_1 bold main
			"\e[1;37m",	// COUT_2 bold header
			"\e[0;91m",	// COUT_3 alt mid main
			"\e[1;31m",	// COUT_4 bold main
			"\e[0;31m",	// COUT_5 light main
			"\e[1;33m",	// COUT_6 bold fire
			"\e[0;33m",	// COUT_7 light fire
			"\e[0;31m",	// COUT_8 light main
			"\e[40m"	// [9] BACKGROUND
			}
		}, {"green",
			{
			"\e[1;33m",	// COUT_0 bold fire
			"\e[1;32m",	// COUT_1 bold main
			"\e[1;37m",	// COUT_2 bold header
			"\e[0;92m",	// COUT_3 alt mid main
			"\e[1;32m",	// COUT_4 bold main
			"\e[0;32m",	// COUT_5 light main
			"\e[1;33m",	// COUT_6 bold fire
			"\e[0;33m",	// COUT_7 light fire
			"\e[0;32m",	// COUT_8 light main
			"\e[40m"	// [9] BACKGROUND
			}
		}, {"yellow",
			{
			"\e[1;33m",	// COUT_0 bold fire
			"\e[1;33m",	// COUT_1 bold main
			"\e[1;37m",	// COUT_2 bold header
			"\e[0;93m",	// COUT_3 alt mid main
			"\e[1;33m",	// COUT_4 bold main
			"\e[0;33m",	// COUT_5 light main
			"\e[1;33m",	// COUT_6 bold fire
			"\e[0;33m",	// COUT_7 light fire
			"\e[0;33m",	// COUT_8 light main
			"\e[40m"	// [9] BACKGROUND
			}
		}, {"blue",
			{
			"\e[1;33m",	// COUT_0 bold fire
			"\e[1;34m",	// COUT_1 bold main
			"\e[1;37m",	// COUT_2 bold header
			"\e[0;94m",	// COUT_3 alt mid main
			"\e[1;34m",	// COUT_4 bold main
			"\e[0;34m",	// COUT_5 light main
			"\e[1;33m",	// COUT_6 bold fire
			"\e[0;33m",	// COUT_7 light fire
			"\e[0;34m",	// COUT_8 light main
			"\e[40m"	// [9] BACKGROUND
			}
		}, {"purple",
			{
			"\e[1;33m",	// COUT_0 bold fire
			"\e[1;35m",	// COUT_1 bold main
			"\e[1;37m",	// COUT_2 bold header
			"\e[0;95m",	// COUT_3 alt mid main
			"\e[1;35m",	// COUT_4 bold main
			"\e[0;35m",	// COUT_5 light main
			"\e[1;33m",	// COUT_6 bold fire
			"\e[0;33m",	// COUT_7 light fire
			"\e[0;35m",	// COUT_8 light main
			"\e[40m"	// [9] BACKGROUND
			}
		}, {"cyan",
			{
			"\e[1;33m",	// COUT_0 bold fire
			"\e[1;36m",	// COUT_1 bold main
			"\e[1;37m",	// COUT_2 bold header
			"\e[0;96m",	// COUT_3 alt mid main
			"\e[1;36m",	// COUT_4 bold main
			"\e[0;36m",	// COUT_5 light main
			"\e[1;33m",	// COUT_6 bold fire
			"\e[0;33m",	// COUT_7 light fire
			"\e[0;36m",	// COUT_8 light main
			"\e[40m"	// [9] BACKGROUND
			}
		}, {"white",
			{
			"\e[1;33m",	// COUT_0 bold fire
			"\e[1;37m",	// COUT_1 bold main
			"\e[1;37m",	// COUT_2 bold header
			"\e[0;97m",	// COUT_3 alt mid main
			"\e[1;37m",	// COUT_4 bold main
			"\e[0;37m",	// COUT_5 light main
			"\e[1;33m",	// COUT_6 bold fire
			"\e[0;33m",	// COUT_7 light fire
			"\e[0;37m",	// COUT_8 light main
			"\e[40m"	// [9] BACKGROUND
			}
		}, {"inverse",
			{
			"\e[7m",
			"\e[7m",
			"\e[7m",
			"\e[7m",
			"\e[7m",
			"\e[7m",
			"\e[7m",
			"\e[7m",
			"\e[7m",
			"\e[7m"
			}
		}
	};

	vector<string> symbols = {
		"A","B","C","D","E","F","G","H","I","J","K","L","M","N",
		"O","P","Q","R","S","T","U","V","W","X","Y","Z",

		"a","b","c","d","e","f","g","H","i","J","k","l","m","n",
		"o","p","q","r","s","t","u","v","w","x","y","z",

		// Repeated for increased probability of digits
		"0","1","2","3","4","5","6","7","8","9",
		"0","1","2","3","4","5","6","7","8","9",
		"0","1","2","3","4","5","6","7","8","9",
		"0","1","2","3","4","5","6","7","8","9",

		"0","0","0","0","0","0","0","0","0","0","0","0","0","0",
		"0","0","0","0","0","0","0","0","0","0","0","0","0","0",
		"0","0","0","0","0","0","0","0","0","0","0","0","0","0",
		"0","0","0","0","0","0","0","0","0","0","0","0","0","0",
		"1","1","1","1","1","1","1","1","1","1","1","1","1","1",
		"1","1","1","1","1","1","1","1","1","1","1","1","1","1",
		"1","1","1","1","1","1","1","1","1","1","1","1","1","1",
		"1","1","1","1","1","1","1","1","1","1","1","1","1","1",

		"¡","¢","£","¥","¦","§","±","µ","¿","À","Á","Ä","Æ","Ç",
		"É","Ê","Ï","Ð","Ñ","Ò","Õ","Ö","Ø","Ú","Ý","Þ","ß","à",
		"á","â","ã","å","æ","ç","é","ê","ë","î","ï","ð","ô","õ",
		"õ","ø","û","ü","ý","þ","ÿ","ą","Ď","Đ","ę","ğ","Ħ","ħ",
		"ķ","ĸ","Ŋ","ŋ","ő","Œ","œ","Ŕ","ŕ","Ŗ","ŗ","Ř","ř","Ş",
		"ş","Ţ","ţ","Ť","Ŧ","ŧ","ƀ","Ƃ","Ɔ","Ƌ","ƌ","ƍ","Ǝ","Ə",
		"Ɛ","Ƒ","ƒ","Ƙ","ƙ","ƚ","ƛ","Ɯ","Ɲ","ƞ","Ɵ","Ơ","ơ","Ƣ",
		"ƣ","Ƥ","ƥ","Ʀ","Ƨ","ƨ","Ʃ","ƪ","Ʊ","ƴ","Ƶ","ƶ","Ʒ","Ƹ",
		"ƹ","ƺ","ƻ","Ƽ","ƽ","ƾ","ǂ","Ǣ","ǣ","ǭ","Ǯ","ǯ","ȱ","ȵ",
		"ȸ","ȹ","Ⱥ","Ȼ","ȼ","Ƚ","Ⱦ","ɀ","Ɂ","?","Ƀ","Ʌ","Ɇ","ɇ",
		"Ɉ","Ɏ","ɐ","ɔ","ɕ","ɘ","ə","ɚ","ɝ","ɦ","ɧ","ɮ","ɰ","ɶ",
		"ɷ","ɸ","ʍ","ʨ","Π",

		// uncomment extra spaces for more random effect
		" "," "," "," ",
		// " "," "," "," "," "," "," "," "," "," "," "," "," "," ",
		// " "," "," "," "," "," "," "," "," "," "," "," "," "," ",
		// " "," "," "," "," "," "," "," "," "," "," "," "," "," ",
		// " "," "," "," "," "," "," "," "," "," "," "," "," "," ",
		// " "," "," "," "," "," "," "," "," "," "," "," "," "," ",

		// alert beeps not working
		// "\a ","\a ","\a ","\a ","\a ","\a ","\a ","\a ","\a ",

		// These Japanese chars take up two spaces; this creates error in cout
		// TODO fix this to incorporate these extended chars
		// "ぁ","げ", "ぁ","げ", "ぁ","げ", "ぁ"
	};
};

/* ------------------------------------------------------------------------------------
 * CENTER STRINGS
 * Credit to Kevin Ballard
 * see -> <https://stackoverflow.com/questions/14861018/center-text-in-fixed-width-field-with-stream-manipulators-in-c>
 */
template<typename charT, typename traits = std::char_traits<charT>>
class center_helper {
		std::basic_string<charT, traits> str_;
	public:
		center_helper(std::basic_string<charT, traits> str) : str_(str) {}
		template<typename a, typename b>
		friend std::basic_ostream<a, b>& operator<<(std::basic_ostream<a, b>& s,
			const center_helper<a, b>& c);
	};

	template<typename charT, typename traits = std::char_traits<charT>>
	center_helper<charT, traits> centered(std::basic_string<charT, traits> str) {
		return center_helper<charT, traits>(str);
	}

	// Redeclare for std::string directly so we can support anything that implicitly converts to std::string
	center_helper<std::string::value_type, std::string::traits_type> centered(const std::string& str) {
		return center_helper<std::string::value_type, std::string::traits_type>(str);
	}

	template<typename charT, typename traits>
	std::basic_ostream<charT, traits>& operator<<(std::basic_ostream<charT, traits>& s,
		const center_helper<charT, traits>& c) {
		
		std::streamsize w = s.width();
		if (w > (unsigned)c.str_.length()) {
			std::streamsize left = (w + c.str_.length()) / 2;
			s.width(left);
			s << c.str_;
			s.width(w - left);
			s << "";
		} else {
			s << c.str_;
		}
		return s;
}
/* end-CENTER STRINGS
 * ------------------------------------------------------------------------------------
 */

// Random int gen
int random_int(unsigned int min, unsigned int max);

// Reset default colors to terminal
void reset();

void print_man(int width, Matrix &matrix);

// Persistent loop to capture user input
void capture_input(string &s);

// Wrapper to simultaneously run func on another thread
void *capture_input_wrapper(void *arg);

// Parse user input
int check_str(string &s, Matrix &matrix);

// Randomly populate bold and light zones with col_index, start & length values
void populate_zones(vector<int> &zones, int width, int row, int &SZ, bool bold);

// Check populated zones and print accordingly
// Using a bool for printed to pass on to other if / else if checkers outside of func loop
// int index here refers to which symbol to print
// bool bold checks if it should print a bold char
void check_zones(vector<int> zones, int outer_index, int inner_index,
	int index, bool &printed, Matrix &matrix, int SZ, bool bold);


int main() {
	// Clear screen
	system("clear");
	system("setterm -cursor off");
	// Turn echo off
	system("stty -echo");

	/* Basic ncurses commands
	 * 	Might go this route, like cmatrix
	 *	UPDATE 7/1/2017		ncurses allows invisible cursor, but prevents the user input thread from functioning
	 */
	// initscr();
	// // raw();
	// echo();
	// curs_set(0);		// Using <system("setterm -cursor off")> instead
	// endwin();

	// These sound commands are not working, check kernel
	// int ms = 5000;
	// int freq = 440;
	// ioctl(STDOUT_FILENO, KDMKTONE, (ms<<16 | 1193180/freq));
	// beep();		// an ncurses command
	

	/* Using pthread
	 * 	Is there any reason here to do pthread_join()
	 *	Do I always want seperate proccesses to be independent?
	 */

	// Create Matrix struct object
	Matrix matrix;

	// Create and call the capture_input wrapped in capture_input_wrapper function to receive user input
	string input_buffer = "";
	pthread_t input_Pthread;
	pthread_create(&input_Pthread, NULL, capture_input_wrapper, &input_buffer);

	// For obtaining Linux / Mac Terminal size
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

	print_man(w.ws_col, matrix);
	this_thread::sleep_for(chrono::seconds(3));

	// For <rainbow> to iterate / cycle thru colors
	map<string, vector<string>>::iterator map_it = matrix.colors.begin();

	while (1) {
		// Checking user input
		// If "q" or "exit", then check func returns 0 & pgm exits (calling the reset func)
		int esc = check_str(input_buffer, matrix);
		if (esc == 0) {
			atexit(reset);
			return 0;
		}
		// For updating Linux / Mac Terminal size
		ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
		// Storing width in matrix obj for possible use by funcs or other threads
		matrix.terminal_width = w.ws_col;

		// <row> sets the limit for the outer for-loop
		// It is based on terminal screen size
		// int row = random_int(w.ws_row, w.ws_row*3);
		// OR
		// Increase coefficient for sparser output (downside is it takes longer to update terminal width output)
		// Decrease coefficient for denser output
		int row = w.ws_row*10;

		// These vectors contain the data for standard downward flowing chars
		vector<int> bold_zones;
		vector<int> light_zones;
		int bold_SZ;
		int light_SZ;
		populate_zones(bold_zones, w.ws_col, row, bold_SZ, true);
		populate_zones(light_zones, w.ws_col, row, light_SZ, false);

		/* ---------------------------------------------------------------------------------------
		 * start-INIT GLITCH ZONE
		 * The glitch chars pop out at random positions on the screen, seemingly going against the current upwards
		 *
		 * TODO put into functions (tried but got weird behavior; will need to come back to this)
		 * 	run on sep. pthread?
		 */
		// ROWS
		const int glitch_SZ = 8;
		string glitch_row_indexes[glitch_SZ];
		// <ss> for converting ints to string
		stringstream ss;

		for (int k=0; k<glitch_SZ; ++k) {
			ss.str(string());				// flushing / clearing stringstream <ss>
			int glitch_index = random_int(0, w.ws_row);
			ss << glitch_index;
			glitch_row_indexes[k] = ss.str();
		}

		// COLUMNS
		string glitch_col_indexes[8];
		for (int k=0; k<8; ++k) {
			ss.str(string());
			int glitch_index = random_int(0, w.ws_col);
			ss << glitch_index;
			glitch_col_indexes[k] = ss.str();
		}
		/* ---------------------------------------------------------------------------------------
		 * end-INIT GLITCH ZONE
		 */

		// Orange Fire sporadic lines
		int fire_col = random_int(0, w.ws_col-1);
		int fire_length = random_int(10, row);
		
		for (int j=0; j<row; ++j) {
			// For updating Linux / Mac Terminal size
			ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

			// Checking user input
			// If "q" or "exit", then check func returns 0 & pgm exits (calling the reset func)
			esc = check_str(input_buffer, matrix);
			if (esc == 0) {
				atexit(reset);
				return 0;
			}

			// This reverses the printing from top to bottom!
			cout << "\e[H";
			cout << "\eM";

			this_thread::sleep_for(chrono::microseconds(matrix.speed));

			/* ---------------------------------------------------------------------------------------
			 * start-RAINBOW EFFECT
			 */

			if (matrix.rainbow_on && j % matrix.rainbow_mod == 0 /*Use <-> or <+> change duration of each color*/) {
				// avoiding printing the inverse; it doesn't look great
				if (map_it->first == "inverse" /*|| map_it == matrix.colors.end()*/) {
					map_it = matrix.colors.begin();
				}
				matrix.color = map_it->first;
				map_it++;
			}

			/* ---------------------------------------------------------------------------------------
			 * end-RAINBOW EFFECT
			 */

			/* ---------------------------------------------------------------------------------------
			 * start-GLITCH ZONE PRINT
			 * TODO put into functions (tried but it didn't work as expected; will need to come back to this)
			 */
			// ROWS
			// Creates gaps & varies height / starting point
			// Play around with mod values for varied effect
			
			ss.str(string());					// flushing / clearing stringstream <ss>
			int glitch_index = random_int(0, w.ws_row);
			ss << glitch_index;

			if (j % 30 == 0) {
				glitch_row_indexes[0] = ss.str();
			} else if (j % 20 == 0) {
				glitch_row_indexes[1] = ss.str();
			} else if (j % 15 == 0) {
				glitch_row_indexes[2] = ss.str();
			} else if (j % 80 == 0) {
				glitch_row_indexes[3] = ss.str();
			} else if (j % 40 == 0) {
				glitch_row_indexes[4] = ss.str();
			} else if (j % 17 == 0) {
				glitch_row_indexes[5] = ss.str();
			} else if (j % 19 == 0) {
				glitch_row_indexes[6] = ss.str();
			} else if (j % 13 == 0) {
				glitch_row_indexes[7] = ss.str();
			}

			// COLUMNS
			// Goes to new index along terminal width <w.ws_col>
			// Play around with mod values for varied effect
			
			ss.str(string());
			glitch_index = random_int(0, w.ws_col);
			ss << glitch_index;

			if (j % 90 == 0) {
				glitch_col_indexes[0] = ss.str();
			} else if (j % 50 == 0) {
				glitch_col_indexes[1] = ss.str();
			} else if (j % 70 == 0) {
				glitch_col_indexes[2] = ss.str();
			} else if (j % 90 == 0) {
				glitch_col_indexes[3] = ss.str();
			} else if (j % 90 == 0) {
				glitch_col_indexes[4] = ss.str();
			} else if (j % 120 == 0) {
				glitch_col_indexes[5] = ss.str();
			} else if (j % 160 == 0) {
				glitch_col_indexes[6] = ss.str();
			} else if (j % 210 == 0) {
				glitch_col_indexes[7] = ss.str();
			}

			// Flushing just in case
			ss.str(string());

			// PRINT glitch rows & columns
			for (int k=0; k<glitch_SZ; ++k) {
				// select random position on screen
				cout << "\e[" + glitch_row_indexes[k] + ";" + glitch_col_indexes[k] +"H";
				int index = random_int(0, matrix.symbols.size()-1);
				// <index> specifies which symbol to print
				if (k == 0) {
					// fire
					// COUT_0
					cout << matrix.colors[matrix.color][0] << matrix.colors[matrix.color][9] << matrix.symbols[index];
				} else {
					// reg bold
					// COUT_1
					cout << matrix.colors[matrix.color][1] << matrix.colors[matrix.color][9] << matrix.symbols[index];
				}
			}

			// Reset cursor to top
			// This might be slowing down the pgm a lot...
			cout << "\e[H";
			/* ---------------------------------------------------------------------------------------
			 * end-GLITCH ZONE PRINT
			 */

			// Inner loop to print standard downward flowing chars along the full width
			for (int i=0; i<w.ws_col; ++i) {
				// For updating Linux / Mac Terminal size
				ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

				// Checking user input again, for more reliable input capture
				// If "q" or "exit", then check func returns 0 & pgm exits (calling the reset func)
				esc = check_str(input_buffer, matrix);
				if (esc == 0) {
					atexit(reset);
					return 0;
				}

				// TODO encapsulate <index>, <printed>, et al that's appropriate in Matrix struct
				//	Make Matrix struct a class to add populate / check_zones funcs?

				// <index> specifies which symbol to print
				int index = random_int(0, matrix.symbols.size()-1);
				// <printed> checks if we should evaluate the following if / else if statements
				bool printed = false;

				check_zones(bold_zones, j, i, index, printed, matrix, bold_SZ, true);
				check_zones(light_zones, j, i, index, printed, matrix, light_SZ, false);

				// Fire column
				if ( (i == fire_col) && (j == 0) && (!printed) ) {
					// COUT_6
					cout << setw(2) << matrix.colors[matrix.color][6] << matrix.colors[matrix.color][9] << matrix.symbols[index];
				} else if ( (i == fire_col) && (j < fire_length)
					&& (!printed) ) {

					// COUT_7
					cout << setw(2) << matrix.colors[matrix.color][7] << matrix.colors[matrix.color][9] << matrix.symbols[index];
				// Spaces
				} else if (!printed) {
					// COUT_8
					cout << setw(2) << matrix.colors[matrix.color][8] << matrix.colors[matrix.color][9] << " ";
				}
			}
		}
	}
	// Should never reach here
	return 0;
}

int random_int(unsigned int min, unsigned int max) {
	random_device rd;		// I think I prefer seeding with random device vs ctime
	mt19937 rng(rd()); 		// seed with Mersenne-Twister engine VS ctime
	uniform_int_distribution<int> uni(min, max);
	return uni(rng);
}

void reset() {
	system("setterm -cursor on");
	// cout << "\e[H";
	system("clear");
	cout << "\e[0m" << "\n\nSTATE RESET \n\n";
	// Turn echo back on (seemed to need to go at the end for colors to reset)
	system("stty echo");
	// Seems to be a bug fix to reset bg correctly when <inverse> is called
	system("clear");
}

void print_man(int width, Matrix &matrix) {
	for (int i=0; i<width; ++i) {
		cout << matrix.colors[matrix.color][9] << " \n";
	}
	cout << "\e[H";
	cout << "\n\n\n" << matrix.colors[matrix.color][2] << setw(width) << centered("WELCOME TO THE MATRIX, NEO") << "\n\n\n";
	cout << "\n\n\n" << matrix.colors[matrix.color][2] << setw(width) << centered("type <exit> or <q> & hit enter to leave the MATRIX\n");
	cout << "\n" << matrix.colors[matrix.color][2] << setw(width) << centered("<p> to pause / unpause screen\n");
	cout << "\n" << matrix.colors[matrix.color][2] << setw(width) << centered("type <color_name> to change colors (ex: <cyan>, <red>, <matrix>)\n");
	cout << "\n" << matrix.colors[matrix.color][2] << setw(width) << centered("<inverse> or <i> to invert colors\n");
	cout << "\n" << matrix.colors[matrix.color][2] << setw(width) << centered("<rainbow> or <bow> to cycle colors (use <-> or <+> to change duration)\n");
	cout << "\n" << matrix.colors[matrix.color][2] << setw(width) << centered("<fastest> for zero thread sleep\n");
	cout << "\n" << matrix.colors[matrix.color][2] << setw(width) << centered("<slowest> for inspection\n");
	cout << "\n" << matrix.colors[matrix.color][2] << setw(width) << centered("<s> or <f> for slower/faster to (in/de)crement speed\n");
	cout << "\n" << matrix.colors[matrix.color][2] << setw(width) << centered("<reset_speed> or <rs> for default speed\n");
	cout << "\n" << matrix.colors[matrix.color][2] << setw(width) << centered("<reset> or <r> for all default settings\n");
	cout << "\n" << matrix.colors[matrix.color][2] << setw(width) << centered("<man> to display list of commands\n");
}

void capture_input(string &s) {
	while (getline(cin, s, '\n')) {
		// If I use ncurses, I might do this or convert s to char*
		// getstr(<*char>);

		this_thread::sleep_for(chrono::milliseconds(1000));
		cin.clear();
	}
}

void *capture_input_wrapper(void *arg) {
	string *s = (string *)arg;
	capture_input(*s);
	return NULL;
}

int check_str(string &s, Matrix &matrix) {
	if (s == "exit" || s == "q") {
		return 0;
	} else if (s == "reset" || s == "r") {
		matrix.speed = 20000;
		matrix.rainbow_on = false;
		matrix.rainbow_mod = 30;
		matrix.color = "matrix";
	} else if (s == "reset_speed" || s == "rs") {
		matrix.speed = 20000;
	} else if (s == "f" && matrix.speed >= 10000) {
		matrix.speed -= 10000;
	} else if (s == "s") {
		matrix.speed += 10000;
	} else if (s == "fastest") {
		matrix.speed = 0;
	} else if (s == "slowest") {
		matrix.speed = 1000000;
	} else if (s == "p") {
		// To PAUSE output
		cin.get();
	} else if (s == "man") {
		cout << "\e[H";
		system("clear");
		print_man(matrix.terminal_width, matrix);
		cin.get();
	} else if (s == "rainbow" || s == "bow") {
		matrix.rainbow_on = true;
	} else if (s == "-" && matrix.rainbow_on) {
		if (matrix.rainbow_mod > 5) {
			matrix.rainbow_mod -= 1;
		} else if (matrix.rainbow_mod > 1) {
			matrix.rainbow_mod--;
		}
	} else if ((s == "+" || s == "=") && matrix.rainbow_on) {
		matrix.rainbow_mod += 1;
	} else if (s != "") {
		if (s == "i") {
			s = "inverse";
		}
		map<string, vector<string>>::iterator it = matrix.colors.find(s);
		if (it != matrix.colors.end()) {
			matrix.color = s;
		}
	}
	// might be a bug fix
	s = "";
	return 1;
}

void populate_zones(vector<int> &zones, int width, int row, int &SZ, bool bold) {
	/* -----------------------------------------------------------------------------
	 * Splitting the columns to be printed into zones (based on 24 screen divisions)
	 *
	 * bold_zones[i] 	= index along w.ws_col
	 * bold_zones[i+1]	= bold_start_i
	 * bold_zones[i+2]	= bold_length_i
	 */
	
	// These values in range 30 to 300 are arbitrary based on what looked pretty goog
	// Tweak if you like (use couts to see what resolution your terminal is at)
	if (width < 118 /*1280x720 & lower*/) {
	 	// cout << "1280x720 & lower\n";
		if (bold) {
			SZ = 30;
		} else {
			SZ = 60;
		}
	} else if (width >= 118 && width < 142 /*1344x756*/) {
		// cout << "1344x756\n";
		if (bold) {
			SZ = 30;
		} else {
			SZ = 60;
		}
	} else if (width >= 142 && width < 171 /*1600x900*/) {
		// cout << "1600x900\n";
		if (bold) {
			SZ = 60;
		} else {
			SZ = 90;
		}
	} else if (width >= 171 && width < 182 /*1920x1080*/) {
		// cout << "1920x1080\n";
		if (bold) {
			SZ = 60;
		} else {
			SZ = 90;
		}
	} else if (width >= 182 && width < 200 /*2048x1152*/) {
		// cout << "2048x1152\n";
		if (bold) {
			SZ = 60;
		} else {
			SZ = 90;
		}
	} else if (width >= 200 && width < 230 /*2304x1296*/) {
		// cout << "2304x1296\n";
		if (bold) {
			SZ = 60;
		} else {
			SZ = 90;
		}
	} else if (width >= 230 && width < 231 /*2560x1440*/) {
		// cout << "2560x1440\n";
		if (bold) {
			SZ = 90;
		} else {
			SZ = 150;
		}
	} else if (width >= 231 && width < 272 /*3008x1692*/) {
		// cout << "3000x1692\n";
		if (bold) {
			SZ = 120;
		} else {
			SZ = 240;
		}
	} else if (width >= 272 && width < 289 /*3200x1800*/) {
		// cout << "3200x1800\n";
		if (bold) {
			SZ = 120;
		} else {
			SZ = 240;
		}
	} else if (width >= 289 && width < 304 /*3360x1890*/) {
		// cout << "3360x1890\n";
		if (bold) {
			SZ = 120;
		} else {
			SZ = 270;
		}
	} else if (width >= 304 /*3840x2160 & higher*/) {
		// cout << "3840x2160 & higher\n";
		if (bold) {
			SZ = 120;
		} else {
			SZ = 300;
		}
	}
	// Below is to accomodate stretching row*20
	if (bold) {
		SZ *= 20;
	} else {
		SZ *= 30;
	}
	zones.clear();
	zones.reserve(SZ*3);
	
	for (int i=0; i<SZ; ++i) {
		zones.push_back(random_int(0, width));					// index along w.ws_col
		zones.push_back(random_int(0, row-90));					// bold_start_i
		zones.push_back(random_int(zones.back()+17, zones.back()+100));		// bold_length_i
	}
}

void check_zones(vector<int> zones, int outer_index, int inner_index,
	int index, bool &printed, Matrix &matrix, int SZ, bool bold) {
	
	for (int i=0; i<SZ; i+=3) {
		// Check for head of bold column
		// if head, print gray white symbol
		if ( (inner_index == zones[i]) && (outer_index == zones[i+1])
			&& (outer_index < zones[i+2])
			&& (printed == false) && (bold) ) {

			// COUT_2
			cout << setw(2) << matrix.colors[matrix.color][2] << matrix.colors[matrix.color][9] << matrix.symbols[index];
			printed = true;
			break;
		} else if ( (inner_index == zones[i]) && (outer_index > zones[i+1])
			&& (outer_index < zones[i+2])
			&& (printed == false) ) {

			if (bold) {
				// For introducing sporadic gaps in bold w/ dif shade
				int flip_die = random_int(0,9);
				if (flip_die == 0) {
					// COUT_3
					cout << setw(2) << matrix.colors[matrix.color][3] << matrix.colors[matrix.color][9] << matrix.symbols[index];
				} else {
					// COUT_4
					cout << setw(2) << matrix.colors[matrix.color][4] << matrix.colors[matrix.color][9] << matrix.symbols[index];
				}
			} else {
				// COUT_5
				cout << setw(2) << matrix.colors[matrix.color][5] << matrix.colors[matrix.color][9] << matrix.symbols[index];
			}
			printed = true;
			break;
		}
	}
}

/* Adding extended ASCII
 *	see --> <https://www.joelonsoftware.com/2003/10/08/the-absolute-minimum-every-software-developer-absolutely-positively-must-know-about-unicode-and-character-sets-no-excuses/>
 *	ran into too many conflicts
 *	opted to explicitly populate symbols hashmap
 */
// locale loc("en_US.UTF-8"); // or "" for system default
// for (int i=48; i<122; ++i) {
// 	// stringstream stream;
// 	// stream << "0x" << hex << i;
// 	// string result = stream.str();
// 	symbols.push_back(i);
// }

/* Alternative Method to Execute lambda asyncronously
 * Did not use this method because the main while function
 * 	depeneded on receiving a future_status::ready call
 *	and once recieved, the while loop would terminate.
 *	This was good for a basic pgm termination.
 */
// string s;
// auto f = async(launch::async, [&s] () -> string {
// 	if (cin >> s) {
// 		if (s == "exit") {
// 			atexit(reset);
// 			return 0;
// 		}
// 	}
// });
// // while (f.wait_for(0s) != future_status::ready) {
