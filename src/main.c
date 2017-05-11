#include "dic.h"


int main(int argc, char *argv[]){
    DEFAULT_INPUT = stdin;
    DEFAULT_OUTPUT = stdout;

    char dicted_changed = 'N';

    /* Create dictionary*/
    if((dict = (struct dictionary*) malloc(sizeof(struct dictionary))) == NULL)
    {
        printf("ERROR: OUT OF MEMORY.\n");
        return EXIT_FAILURE;
    }


    if((m_list = (struct misspelled_word*) malloc(sizeof(struct misspelled_word))) == NULL)
    {
        printf("ERROR: OUT OF MEMORY.\n");
        return EXIT_FAILURE;
    }

    struct misspelled_word *duplicate_for_m_list = m_list;

    struct Args args;


    args.d = false;
    args.i = false;
    args.o = false;

    int i;
    char line[MAX_SIZE];

    FILE* dFile;
    FILE* iFile = DEFAULT_INPUT;
    FILE* oFile = DEFAULT_OUTPUT;

    char temp_char;

    int valid_o = 0;
    int valid_i = 0;
    int valid_d = 0;
    int valid_a = 0;
    int valid_h = 0;

    while( (temp_char = (getopt(argc, argv, "hd:o:i:A:"))) != -1){
        switch(temp_char){

            case 'd':
            valid_d = 1;
            break;
            case 'o':
            valid_o = 1;
            break;
            case'i':
            valid_i = 1;
            break;
            case 'h':
            valid_h = 1;
            break;
            case 'A':
            valid_a  = 1;
            break;
            case '?':
            USAGE(EXIT_FAILURE);
            break;

        }
    }

    if(valid_h == 1){
        USAGE(EXIT_SUCCESS);
    }

    int default_dict = 1;

    char opt = '\0';
    int amount_of_n = 0;
    for(i = 1; i< argc; i++)
    {
        char* currArg = argv[i];

        opt = *(currArg + 1);
        //there's a flag
        if(opt != '\0')
        {
            if(opt == 'd' && valid_d == 1)
            {
                default_dict = 0;

                if(strcmp(argv[i+1], "-h") == 0){
                    USAGE(EXIT_SUCCESS);
                }

                strcpy(args.dictFile, (argv[i + 1]));
                args.d = true;
                i++;
            }
            if(opt == 'i' && valid_i == 1)
            {
                if(strcmp(argv[i+1], "-h") == 0){
                    USAGE(EXIT_SUCCESS);
                }

                strcpy(args.input, (argv[i + 1]));
                args.i = true;
                iFile = fopen(args.input, "r");
                i++;
            }
            if(opt == 'o' && valid_o == 1)
            {
                if(strcmp(argv[i+1], "-h") == 0){
                    USAGE(EXIT_SUCCESS);
                }

                strcpy(args.output, argv[i + 1]);
                args.o = true;
                oFile = fopen(args.output, "w");
                i++;
            }
            if(opt == 'A' && valid_a == 1){
                if(*(currArg + 2) != 0){
                    char temp_char = (*(currArg + 2));
                    amount_of_n = temp_char - '0';
                }
            }
            if(opt == 'h' && valid_h == 1){

                if((strcmp(argv[i], "-h")) == 0){
                    USAGE(0);
                }
            }

            opt = '\0';
        }
    }

    if(default_dict == 1){

        strcpy(args.dictFile, "rsrc/dictionary.txt\0");
        args.d = true;
    }


    if(amount_of_n < 0 || amount_of_n >5){
        USAGE(EXIT_FAILURE);
    }

    dFile = fopen(args.dictFile, "r");

    if(iFile == NULL && args.i == true)
    {
        printf("Unable to open: %s.\n", args.input);
        USAGE(EXIT_FAILURE);
        //return EXIT_FAILURE;
    }
    if(dFile == NULL)
    {
        printf("Unable to open: %s.\n", args.dictFile);
        USAGE(EXIT_FAILURE);
    }
    else
    {
        processDictionary(dFile);

    }

    strcpy(line,"\n--------INPUT FILE WORDS--------\n");
    fwrite(line, strlen(line) + 1, 1, oFile);


    int previous_read_in_is_a_valid_char = 0;

    while(!feof(iFile))
    {
        char word[MAX_SIZE];
        char* wdPtr = word;
        //char line[MAX_SIZE];
        char* character = line;

        void *temp_void = fgets(line, MAX_SIZE+1, iFile);
        if(temp_void == NULL)
            break;


        if((line[strlen(line)-1] != ' ') && (line[strlen(line)-1] != '\n'))
            strcat(line, " ");


        while(*character != '\0')
        {
            if((*character == ' ' || *character == '\n') && previous_read_in_is_a_valid_char ==1)
            {

                *wdPtr = '\0';
                wdPtr = word;

                char *new_string = (char*) malloc(WORDLENGTH);


                int start_index = delete_punct_of_word(wdPtr, new_string);

                int size_of_new_string = 0;
                size_of_new_string = strlen(new_string);
                if(size_of_new_string != 0){
                    processWord(new_string, valid_a, amount_of_n, &dicted_changed);

                }

                char *new_string_to_putback = (char*) malloc(WORDLENGTH);


                put_back_to_the_original_input_string(wdPtr, new_string, start_index, size_of_new_string, new_string_to_putback);

                int size_of_actual_new_string_after_process = strlen(new_string);

                if(size_of_new_string == size_of_actual_new_string_after_process){

                    if(*character == '\n')
                        strcat(wdPtr, "\n");
                    else
                        strcat(wdPtr, " ");
                    fwrite(wdPtr, strlen(wdPtr)+1, 1, oFile);

                }
                else{
                    if(*character == '\n')
                        strcat(new_string_to_putback, "\n");
                    else
                        strcat(new_string_to_putback, " ");
                    fwrite(new_string_to_putback, strlen(new_string_to_putback)+1, 1, oFile);

                }
                previous_read_in_is_a_valid_char = 0;
                free(new_string_to_putback);
                free(new_string);
            }
            else
            {
                if(*character == '\n'){
                    fwrite(character, 1, 1, oFile);
                }

                *wdPtr = *character;
                wdPtr++;
                previous_read_in_is_a_valid_char = 1;
            }
            character++;
        }

        if(iFile == stdin)
            break;
    }
    struct misspelled_word *copy_for_printing_the_misspelled_list = m_list;


    if(dicted_changed == 'Y'){
        char *dict_old_name = args.dictFile;

        char * brand_new_file_name = creat_new_file_name (dict_old_name);

        FILE* new_dict_pointer = fopen(brand_new_file_name, "w");

        struct dict_word *copy_of_dict_word = dict -> word_list;
        struct misspelled_word *copy_of_m_list = m_list;

        while(copy_of_dict_word != NULL){

            if(copy_of_dict_word -> num_misspellings != 0){
                fprintf(new_dict_pointer,"%s ", copy_of_dict_word -> word);
            }
            else
                fprintf(new_dict_pointer,"%s", copy_of_dict_word -> word);

            int number_of_wrong_word = copy_of_dict_word -> num_misspellings;

            if(number_of_wrong_word == 0){
                fprintf(new_dict_pointer, "\n");
            }


            while(copy_of_m_list != NULL && number_of_wrong_word != 0){

                if(number_of_wrong_word != 1){
                    fprintf(new_dict_pointer,"%s ", copy_of_m_list -> word);
                    copy_of_m_list = copy_of_m_list -> next;
                }
                else{
                    fprintf(new_dict_pointer,"%s\n", copy_of_m_list -> word);
                    copy_of_m_list = copy_of_m_list -> next;
                }

                number_of_wrong_word--;

            }
            copy_of_dict_word = copy_of_dict_word -> next;
        }
        fclose(new_dict_pointer);
    }


    fprintf(stderr, "Total number of words in dictionary: %d\n", dict-> num_words);
    int sizeof_dic = sizeof(struct dictionary) + sizeof(struct dict_word) * dict->num_words;
    fprintf(stderr, "Size of dictionary (in bytes): %d\n", sizeof_dic);

    struct misspelled_word *temp_ptr_for_traversal = m_list;
    int counter = 0;

    while(temp_ptr_for_traversal != NULL){
        counter++;
        temp_ptr_for_traversal  = temp_ptr_for_traversal -> next;
    }

    int size_of_missp = sizeof(struct misspelled_word) * counter;

    fprintf(stderr, "Size of misspelled word list (in bytes): %d\n", size_of_missp);

    int count_misspelled = 0;
    while(copy_for_printing_the_misspelled_list != NULL){

        if(copy_for_printing_the_misspelled_list -> misspelled == 1){
            count_misspelled ++;
        }
        copy_for_printing_the_misspelled_list = copy_for_printing_the_misspelled_list -> next;
    }

    fprintf(stderr, "Total number of misspelled words: %d\n", count_misspelled);

    fprintf(stderr, "Top 3 misspelled words:");

    struct dict_word array_of_top_3[3];

    struct dict_word *copy_of_dict_word_for_printing = dict -> word_list;

    find_the_top_3(array_of_top_3, copy_of_dict_word_for_printing);

    fprintf(stderr, "\n%s ", array_of_top_3[0].word);
    fprintf(stderr, "(%d times): ", array_of_top_3[0].misspelled_count);
    int counter_1 = 0;

    for(int i = array_of_top_3[0].num_misspellings - 1; i >= 0; i--){
        if(array_of_top_3[0].misspelled[i]->misspelled ==1){
            if(counter_1 != 0)
                fprintf(stderr, ", %s", array_of_top_3[0].misspelled[i]->word);
            else{
                fprintf(stderr, "%s", array_of_top_3[0].misspelled[i]->word);
                counter_1 ++;
            }
        }
    }


    fprintf(stderr, "\n%s ", array_of_top_3[1].word);
    fprintf(stderr, "(%d times): ", array_of_top_3[1].misspelled_count);
    int counter_2 = 0;

    for(int i = array_of_top_3[1].num_misspellings - 1; i >= 0; i--){
        if(array_of_top_3[1].misspelled[i]->misspelled ==1){
            if(counter_2 != 0)
                fprintf(stderr, ", %s", array_of_top_3[1].misspelled[i]->word);
            else{
                fprintf(stderr, "%s", array_of_top_3[1].misspelled[i]->word);
                counter_2++;
            }
        }
    }

    fprintf(stderr, "\n%s ", array_of_top_3[2].word);
    fprintf(stderr, "(%d times): ", array_of_top_3[2].misspelled_count);
    int counter_3 = 0;

    for(int i = array_of_top_3[2].num_misspellings - 1; i >= 0; i--){
        if(array_of_top_3[2].misspelled[i]->misspelled ==1){
            if(counter_3 != 0)
                fprintf(stderr, ", %s", array_of_top_3[2].misspelled[i]->word);

            else{
                fprintf(stderr, "%s", array_of_top_3[2].misspelled[i]->word);
                counter_3++;
            }
        }
    }
    fprintf(stderr, "\n");


    struct misspelled_word *next_wrong_word = m_list -> next;
    int counter_for_freeing_word_list = dict -> num_words;

    while(m_list -> next != NULL){
        free(m_list);
        m_list = next_wrong_word;
        next_wrong_word = next_wrong_word -> next;
        //counter_for_freeing_word_list --;
        counter --;
    }

    free(duplicate_for_m_list);


    struct dict_word *word_list_for_freeing = NULL;
    word_list_for_freeing = dict -> word_list;
    struct dict_word *next_dict_word = dict -> word_list -> next;
    int counter_for_freeing = 0;

    while(word_list_for_freeing ->next != NULL){

        free(word_list_for_freeing);
        word_list_for_freeing = next_dict_word;
        next_dict_word = next_dict_word -> next;
        counter_for_freeing++;
        counter_for_freeing_word_list --;
    }

    free(word_list_for_freeing);

    free(dict);

    fclose(dFile);
    fclose(iFile);
    fclose(oFile);
    return EXIT_SUCCESS;
}


int delete_punct_of_word(char *word, char *new_string_with_malloc_space){


    int size_of_word = strlen(word);
    int counter_of_starting_offset = 0;
    int counter_from_the_end = 0;

    char *copied_of_new_string_with_malloc_space = new_string_with_malloc_space;

    char *start = word;
    char *end_of_word = NULL;
    end_of_word = word + size_of_word - 1;;

    while((*start < 65 || *start > 90) && (*start < 97 || *start > 122)){
        start++;
        counter_of_starting_offset++;
    }

    while((*end_of_word < 65 || *end_of_word > 90) && (*end_of_word < 97 || *end_of_word > 122)){
        end_of_word--;
        counter_from_the_end++;
    }

    int valid_string_size = strlen(word) - counter_of_starting_offset - counter_from_the_end;

    for(int i = valid_string_size; i > 0; i--){
        // for the existence case the next one is
        *copied_of_new_string_with_malloc_space = *start;
        copied_of_new_string_with_malloc_space++;
        start++;
    }

    copied_of_new_string_with_malloc_space = copied_of_new_string_with_malloc_space - valid_string_size;
    *(copied_of_new_string_with_malloc_space + valid_string_size) = '\0';


    return counter_of_starting_offset;
}

void put_back_to_the_original_input_string(char *original_string, char *new_string, int start_index, int previous_valid_string_size, char *new_string_to_putback){

    char *temp_new_string = new_string;

    char *copy_of_new_string_to_put_back = new_string_to_putback;

    int new_string_length = strlen(new_string);

    if(previous_valid_string_size == new_string_length){
        original_string = original_string + start_index;
        for(int i = strlen(new_string); i > 0; i--){

            *original_string = *temp_new_string;
            temp_new_string++;
            original_string++;
        }
    }
    else{
        for(int i = start_index; i > 0; i--){

            *copy_of_new_string_to_put_back = *original_string;
            copy_of_new_string_to_put_back ++;
            original_string++;
        }

        for(int i = strlen(new_string); i > 0; i--){
            *copy_of_new_string_to_put_back = *temp_new_string;
            copy_of_new_string_to_put_back++;
            temp_new_string++;
        }

        original_string = original_string + previous_valid_string_size;
        while(*original_string != '\0'){
            *copy_of_new_string_to_put_back = *original_string;
            copy_of_new_string_to_put_back++;
            original_string++;
        }
    }
}

char* creat_new_file_name(char *copy_of_file_name){

    char *temp_file_name = copy_of_file_name;

    temp_file_name = temp_file_name + strlen(copy_of_file_name) - 1;

    int counter = strlen(copy_of_file_name);

    while(*temp_file_name != '/' && counter != 0){
        temp_file_name --;
        counter--;

    }
    temp_file_name++;

    int lenght_of_file_name = strlen(temp_file_name);

    char * ONLY_FILE_NAME = (char*) malloc(sizeof(char) * (lenght_of_file_name + 1));


    char *temp_ONLY_FILE_NAME = ONLY_FILE_NAME;

    strcpy(temp_ONLY_FILE_NAME, temp_file_name);

    char *new_word_to_be_appended = "new_\0";

    while(* new_word_to_be_appended != '\0'){
        *temp_file_name = *new_word_to_be_appended;
        temp_file_name++;
        new_word_to_be_appended++;
    }

    while(* temp_ONLY_FILE_NAME != '\0'){

        *temp_file_name = *temp_ONLY_FILE_NAME;
        temp_file_name++;
        temp_ONLY_FILE_NAME++;
    }

    *temp_file_name = '\0';
    free(ONLY_FILE_NAME);

    return copy_of_file_name;
}

int get_total_number_of_misspelled(struct misspelled_word *m_list_ptr){

    int count_misspelled = 0;
    while(m_list_ptr != NULL){

        if(m_list_ptr -> misspelled == 1){
            count_misspelled ++;
        }
        m_list_ptr = m_list_ptr -> next;
    }

    return count_misspelled;
}

void find_the_top_3(struct dict_word *array, struct dict_word *word_list)
{
    struct dict_word *fixed_word_list = word_list;
    struct dict_word *highest_1 = fixed_word_list;
    struct dict_word *current = NULL;

    if(word_list -> next != NULL){
        current = word_list -> next;
    }

    while(current != NULL){

        if(current -> misspelled_count > highest_1->misspelled_count){
            highest_1 = current;
        }
        current = current -> next;
    }

    array[0] = *highest_1;


    struct dict_word *highest_2 = fixed_word_list;
    struct dict_word *current2 = NULL;

    if(word_list -> next != NULL){
        current2 = word_list -> next;
    }

    while(current2 != NULL){

        if(current2 -> misspelled_count > highest_2->misspelled_count){
            if(current2 != highest_1)
                highest_2 = current2;
        }
        current2 = current2 -> next;
    }

    struct dict_word *highest_3 = fixed_word_list;
    struct dict_word *current3 = NULL;

    if(word_list -> next != NULL){
        current3 = word_list -> next;
    }

    while(current3 != NULL){

        if(current3 -> misspelled_count > highest_3->misspelled_count){
            if(current3 != highest_1 && current3 != highest_2)
                highest_3 = current3;
        }
        current3 = current3 -> next;
    }

    array[0] = *highest_1;
    array[1] = *highest_2;
    array[2] = *highest_3;
}



