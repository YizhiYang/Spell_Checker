#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#define MAX_SIZE 256
#define WORDLENGTH 50
#define MAX_MISSPELLED_WORDS 5

#define USAGE(return_code) do { \
    fprintf(stderr, "%s\n", \
        "Usage: spell [-h] [-o OUT_FILE] [-i IN_FILE] [-d DICT_FILE] [-An]\n" \
        "Spell Checker using a custom dictionary. Auto corrects any known misspellings in the text.\n" \
        "Additional function to add new words and randomized misspellings to the dictionary.\n\n" \
        "\t-h\tDisplays this usage.\n" \
        "\t-o\tOUT_FILE filename, if omitted output to stdout\n" \
        "\t-i\tIN_FILE filename, if omitted input comes from stdin (CTRL+D to end input)\n" \
        "\t-d\tfor the dictionary filename, if omitted use default 'rsrc/dictionary.txt'\n" \
        "\t-An\tAutomatically add n (in range 0-5) random misspellings for any word not in the dictionary.\n"); \
        exit(return_code);\
} while (0)


//p ichar DEFAULT_DICT_FILE[]= "dictionary.txt";
FILE* DEFAULT_INPUT;
FILE* DEFAULT_OUTPUT;
struct dictionary* dict;
struct misspelled_word* m_list;


struct Args{
    bool d;
    bool i;
    bool o;

    char dictFile[MAX_SIZE];
    char input[MAX_SIZE];
    char output[MAX_SIZE];
};

struct dictionary{
    int num_words;
    struct dict_word* word_list;
};

struct dict_word{
    char word[WORDLENGTH];
    int misspelled_count; // initialize to 0
    int num_misspellings; // initialize to 0
    struct misspelled_word* misspelled[MAX_MISSPELLED_WORDS];
    struct dict_word* next;
};

struct misspelled_word{
    char word[WORDLENGTH];
    int misspelled; // initialize to 0
    struct dict_word* correct_word;
    struct misspelled_word* next;
};



/**
 * @brief      read in dictionary
 *
 * @param      f     opened dictionary file
 */
void processDictionary(FILE* f);


/**
 * @brief      Adds a word.
 *
 * @param      dWord  The d word
 * @param      word   The word
 */
void addWord(struct dict_word* dWord, char* word);



/**
 * @brief      Adds a misspelled word.
 *
 * @param      misspelledWord  The misspelled word
 * @param      correctWord     The correct word
 * @param      word            The word
 */
void addMisspelledWord(struct misspelled_word * misspelledWord, struct dict_word* correctWord, char* word);


/**
 * @brief      free the words
 *
 * @param      word  The word
 */
void freeWords(struct dict_word* word);



/**
 * @brief      Print the words
 *
 * @param      word  The word
 * @param      f     { parameter_description }
 */
void printWords(struct dict_word* word, FILE* f);



/**
 * @brief      Process a word
 *
 * @param      inputWord  The input word
 */
void processWord(char* inputWord, int is_a_specified, int n, char *dict_changed);



/**
 * @brief      find the misspelling
 *
 * @param      inputWord  The input word
 *
 * @return     boolean
 */
bool foundMisspelledMatch(char* inputWord);



/**
 * @brief      find match in dictionary
 *
 * @param      inputWord  The input word
 *
 * @return     boolean
 */
bool foundDictMatch(char* inputWord);

void freeMisspelledWord(struct misspelled_word* currWord);

int delete_punct_of_word(char *word, char *new_string_with_malloc_space);

char* creat_new_file_name(char *copy_of_file_name);

int get_total_number_of_misspelled(struct misspelled_word *m_list_ptr);

void find_the_top_3(struct dict_word*, struct dict_word*);

void put_back_to_the_original_input_string(char *original_string, char *new_string, int start_index, int previous_valid_string_size, char* new_string_to_put_back);
