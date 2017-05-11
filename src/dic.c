#include "dic.h"
#include <ctype.h>
#include "mispelling.h"

/* Great filename. */

int first_m_list_flag = 0;
int first_currentWord_flag = 0;


void processDictionary(FILE* f){
    dict->num_words = 0;

    while(true)
    {
        /* Initialize the current word */
        struct dict_word* currWord;

        if((currWord = (struct dict_word*) malloc(sizeof(struct dict_word))) == NULL)
        {
            printf("OUT OF MEMORY.\n");
            return;
        }

        /* Set up associated info*/
        currWord->num_misspellings = 0;
        currWord->misspelled_count = 0;

        char word[MAX_SIZE];
        char* wdPtr = word;
        char line[MAX_SIZE];
        char* character = line;

        int counter = 0;
        int firstWord = 1;

        /* Read a line and then place it into line ptr*/
        void *x = fgets(line, MAX_SIZE+1, f);
        if( x == NULL){
            free(currWord);
            break;
        }

        /* If no space at the end of the line, put one there*/
        if((line[strlen(line)-2] != ' ' && line[strlen(line)-1] == '\n')
            || (line[strlen(line)-1] != ' ' && line[strlen(line)-1] != '\n')){

            line[strlen(line) - 1 ] = ' ';
            line[strlen(line)] = '\n';
        }


        while(*character != '\n')
        {
            if(counter >= MAX_MISSPELLED_WORDS + 1)
                break;

            /* If the char is a space, add that word into word_list */
            if(*character == ' ')
            {
                *wdPtr = *character;
                wdPtr = word;
                if(firstWord)
                {
                    addWord(currWord, wdPtr);
                    dict->num_words++;

                    firstWord = 0;
                    if(first_currentWord_flag == 0){
                        currWord -> next = NULL;
                        first_currentWord_flag = 1;
                    }

                }
                else
                {
                    struct misspelled_word* currMisspelling;
                    if((currMisspelling = (struct misspelled_word*) malloc(sizeof(struct misspelled_word))) == NULL)
                    {
                        printf("ERROR: OUT OF MEMORY.");
                        return;
                    }

                    addMisspelledWord(currMisspelling, currWord, wdPtr);
                    if(first_m_list_flag == 0){
                        m_list -> next = NULL;
                        first_m_list_flag = 1;
                    }

                }
            }

            else if(*character != '\n'){
                *wdPtr = *character;
                wdPtr++;
            }
            character++;
        }
    }

}

void addWord(struct dict_word* dWord, char* word){


    char temp_correct_word[MAX_SIZE];
    char *temp_new_word = temp_correct_word;

    while(*word != ' ' && *word != '\0'){
        *temp_new_word = *word;
        word++;
        temp_new_word ++;
    }
    *temp_new_word = '\0';


    dWord->misspelled_count = 0;
    dWord->num_misspellings = 0;
    dWord->next = dict->word_list;
    strcpy(dWord->word, temp_correct_word);
    dict->word_list = dWord;
}

void addMisspelledWord(struct misspelled_word* misspelledWord, struct dict_word* correctWord, char* word){

    char temp_misspelled_word[MAX_SIZE];
    char *temp_misspelled_word_ptr = temp_misspelled_word;
    while(*word != ' ' && *word != '\0'){
        *temp_misspelled_word_ptr = *word;
        word++;
        temp_misspelled_word_ptr++;
    }
    *temp_misspelled_word_ptr = '\0';

    strcpy(misspelledWord->word, temp_misspelled_word);
    misspelledWord->misspelled = 0;
    misspelledWord->correct_word = correctWord;
    misspelledWord->next = m_list;
    (correctWord->misspelled)[correctWord->num_misspellings++] = misspelledWord;
    m_list = misspelledWord;
}

void freeWords(struct dict_word* currWord){
    if(currWord != NULL)
    {
        printf("FREED %s\n", currWord->word);
        free(currWord);
    }
}

void freeMisspelledWord(struct misspelled_word* currWord){
    if(currWord != NULL)
    {
        free(currWord);
    }
}



void printWords(struct dict_word* currWord, FILE* f){
    if(currWord != NULL)
    {
        if(currWord ->next != NULL){
            printWords(currWord->next, f);
        }

        char line[MAX_SIZE];
        int i;

        sprintf(line, "%s\n", currWord->word);
        fwrite(line, strlen(line)+1, 1, f);

        sprintf(line, "\tNUMBER OF TIMES WORD IS MISSPELLED: %d\n", currWord->misspelled_count); // puts string into buffer
        fwrite(line, strlen(line)+1, 1, f);

        sprintf(line, "\tNUMBER OF MISSPELLINGS: %d\n", currWord->num_misspellings);
        fwrite(line, strlen(line)+1, 1, f);

        for(i = 0; i<currWord->num_misspellings; i++)
        {
            sprintf(line, "\tMISPELLED WORD #%d: %s\n", i,((currWord->misspelled)[i])->word);
            fwrite(line, strlen(line)+1, 1, f);

            sprintf(line,"\t\tMISPELLED?: %d\n", ((currWord->misspelled)[i])->misspelled);
            fwrite(line, strlen(line)+1, 1, f);

            //sprintf(line, "\t\tACTUAL WORD: %s\n", ((currWord->misspelled)[i])->correct_word);
            fwrite(line, strlen(line)+1, 1, f);

            if(((currWord->misspelled)[i])->next->word != NULL)
            {
                sprintf(line, "\t\tNEXT MISPELLED WORD: %s\n", ((currWord->misspelled)[i])->next->word);
                fwrite(line, strlen(line)+1, 1, f);
            }
        }

        if((currWord->next)->word != NULL)
        {
            sprintf(line,"\tNEXT WORD: %s\n", (currWord->next)->word);
            fwrite(line, strlen(line)+1, 1, f);
        }
    }
}

void processWord(char* inputWord, int is_a_spefified, int n, char *dict_changed){

    char *temp_char = inputWord;

    while(*temp_char != '\0'){
        *temp_char = tolower(*temp_char);
        temp_char++;
    }

    if(foundMisspelledMatch(inputWord))
        return;
    if(foundDictMatch(inputWord))
        return;

    if(is_a_spefified == 1){

        struct dict_word* newWord;

        if((newWord = (struct dict_word*) malloc(sizeof(struct dict_word))) == NULL)
        {
            printf("ERROR: OUT OF MEMORY.\n");
            return;
        }
        addWord(newWord, inputWord);
        *dict_changed = 'Y';
        dict -> num_words++;

        char **new_misspelling_word_linked_list = gentypos(n, inputWord);


        int counter = 0;
        for(int i = n; i > 0; i--){

            struct misspelled_word* currMisspelling;

            if((currMisspelling = (struct misspelled_word*) malloc(sizeof(struct misspelled_word))) == NULL)
            {
                printf("ERROR: OUT OF MEMORY.");
                return;
            }
            currMisspelling -> misspelled = 0;
            addMisspelledWord(currMisspelling, dict->word_list, new_misspelling_word_linked_list[counter]);

            counter++;
        }

        for(int i = n - 1; i >=0; i--){
            char *temp_new_misspelling_linked_list = new_misspelling_word_linked_list[i];
            free(temp_new_misspelling_linked_list);
        }

        free(new_misspelling_word_linked_list);


    }

    else if(is_a_spefified == 0){

        return;
    }

}

bool foundMisspelledMatch(char* inputWord){
    struct misspelled_word* listPtr = NULL;
    listPtr = m_list;
    while(listPtr != NULL)
    {
        if(strcasecmp(inputWord, listPtr->word) == 0)
        {
            strcpy(inputWord, listPtr->correct_word->word);
            listPtr->misspelled = 1;
            listPtr->correct_word->misspelled_count++;
            return true;
        }
        listPtr = listPtr->next;
    }
    return false;
}

bool foundDictMatch(char* inputWord){
    struct dict_word* listPtr = NULL;
    listPtr = dict->word_list;

    while(listPtr != NULL)
    {
        if(strcasecmp(inputWord, listPtr->word) == 0)
            return true;
        listPtr = listPtr->next;
    }
    return false;
}

