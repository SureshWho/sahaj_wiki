//
//  main.cpp
//  sahaj_wiki
//
//  Created by Suresh Thiyagarajan on 19/01/20.
//  Copyright © 2020 Suresh Thiyagarajan. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <math.h>

#include "main.hpp"

using namespace std;

/* app level default */
bool debug = false;
char input_file[256] = "/Volumes/MyMedias/MachineLearning/code/code/sahaj_wiki/sahaj_wiki/input.txt";
char test_file[256]  = "/Volumes/MyMedias/MachineLearning/code/code/sahaj_wiki/sahaj_wiki/test.txt";

/*
 * Usage:   sahaj_wiki [debug(0|1)] [inputfile]
 * Example: sahaj_wiki 0 story.txt
 */
int main(int argc, const char * argv[]) {
    bool debug = false;
    comprehension com, test_com;
    
    /* process command line */
    printf ("Usage: sahaj_wiki [debug(0|1)] [inputfile] [test_file]\n");
    if (argc >= 2) debug = (strcmp(argv[1], "1") == 0) ;
    if (argc >= 3) strcpy(input_file, argv[2]) ;
    if (argc >= 4) strcpy(test_file,  argv[3]) ;
    
    /* init debug change it to true to get more debug outs */
    comprehension::debug = debug;
    sentence::debug      = debug;
    
    /* if test file is supplied run the test */
    if ( strlen(test_file) != 0)
        if (!test_com.run_test (test_file))
            return -1 ;
    
    /* create comprehension data structure for processing */
    if(!com.init (input_file))
        return -1;
    
    /* check whether the parsing was right */
    com.display_org_content ();
    
    /* find the best ans */
    com.find_best_answers ();
    
    /* display the right ans */
    com.display_right_answers ();
    
    return 0;
}

/*
 * sentence - Sentences helper class
 */

/* find_weight() - Compares two sentenses and find similarity weight.
 *
 * Input :
 *      target      - Sentence to be compared.
 *
 * Output :
 *      None
 *
 * Returns :
 *      Similarity in the form of Weight.
 *
 * Description :
 *      Compares given sentence with this object sentece, and returns similarity in the
 *      form of weights.
 */
float sentence::find_weight(const sentence &target) const
{
    int    len_src, len_des;
    string src[SEN_MAX_WORDS], des[SEN_MAX_WORDS];
    float  weight ;
    
    /* get the words from sentences */
    len_src = get_words(src, SEN_MAX_WORDS);
    len_des = target.get_words(des, SEN_MAX_WORDS);

    /* compare words */
    weight = find_weight_words(src, len_src, des, len_des);
    
    return weight;
}

/* find_weights - Compares the class sentence with given sentences and return weights.
 *
 * Input :
 *      p_sentences   - List of sentences to be compared.
 *      max_sentences - Size of <p_sentences> can hold
 *      p_weights     - Pointer for returning weights should able to hold
 *                      <max_sentences> number of weights.
 * Output :
 *      p_weights - Weights
 *
 * Returns :
 *      Index for the sentence which matches closly.
 *
 * Description :
 *      Compares the object's sentence with given sentences, and returns the similarity
 * all given sentences in the form of weights. Also return the index of the sentence which matchs
 * closly with the given sentence.
 */
int sentence::find_weights(sentence *p_sentences,  int max_sentences, float *p_weights) const
{
    int   index, max_index;
    float max_weight, weight;
    
    DEBUG_LOG cout << "\nComparing " << *this << "\n";
    
    /*inits*/
    max_index  = 0;
    max_weight = 0;
    
    /* for each sentence find the comparing weight */
    for (index = 0; index < max_sentences; index++)
    {
        DEBUG_LOG cout << "\t" << index << " " << p_sentences[index] << "\n";
        
        weight           = find_weight(p_sentences[index]);
        p_weights[index] = weight;
        
        /* find the best matching sentence */
        if (weight > max_weight)
        {
            max_weight = weight;
            max_index  = index;
        }
    }
    
    /* return best matching sentence */
    return max_index;
}

/* find_weight_words() - Finds similarity between two sets words.
 *
 * Input :
 *      p_src_words - Source words
 *      src_words   - size of <p_src_words>
 *      p_des_words - destination words
 *      des_words   - sizeof <p_des_words>
 *
 * Output :
 *      None
 *
 * Returns :
 *      Similarity between word sets (in the form of weights).
 *
 * Description :
 *      Compares given words, and returns similarity in the form of weights.
 */
float sentence::find_weight_words(string *p_src_words, int src_words,
                              string *p_des_words, int des_words)
{
    int    src_index, des_index;
    float  max_weight, weight, matches;
    size_t pos;

    /* inits */
    matches    = 0;
    
    /* compare each source word with all destination words */
    des_index = 0;
    for (src_index = 0; src_index < src_words; src_index++)
    {
        max_weight = 0.0;
        
        /* if the src word is a common word skip it */
        pos = COMMON_WORDS.find(p_src_words[src_index]);
        if (pos != npos)
            continue;
        
        for (des_index = 0; des_index < des_words; des_index++)
        {
            weight = find_weight_word(p_src_words[src_index], p_des_words[des_index]);
            if (weight > max_weight)
            {
                max_weight = weight;
            }
        }
        matches += max_weight;
    }
    
    //weight = matches / des_words;
    weight = matches;
    
    DEBUG_LOG printf ("Mathes %f src %d des %d Weight %f\n", matches, src_words, des_words, weight);
    
    return  weight;
}

/* find_weight_word() - Compares two given words.
 *
 * Input :
 *      src - Source word
 *      des - Word to be compared with
 *
 * Output :
 *      None
 *
 * Returns :
 *      Similarity in the form of Weight.
 *
 * Description :
 *      Compares given words, and returns similarity in the form of weights.
 */
float sentence::find_weight_word(string src, string des)
{
    size_t src_len, des_len, pos, len;
    int    matches = 0;
    float  weight;
    
    /* compare only smallest chars of two words */
    src_len = src.length();
    des_len = des.length();
    len     = min(src_len, des_len);

    /* compare each char */
    for (pos = 0; pos < len; pos++)
        if (tolower(src.at(pos)) == tolower(des.at(pos)))
            matches++;
    
    /* find the weight */
     weight = (float) matches / src_len;
    //weight = round(weight - 0.2);
    
    //cout << "weight " << src << " " << des << " " << weight << "\n";
    
    return weight;
}

/* parse() - Parses the given string and breaks them into individual sentences.
 *
 * Input :
 *      str           - String to be parsed.
 *      p_sentences   - Where to store the parsed sentences.
 *      max_sentences - How much <p_sentences> can hold
 *      delimeter     - delimeter for parsing.
 *
 *
 * Output :
 *      p_sentences - contains pasrsed sentences
 *
 * Returns :
 *      Number of sentences created.
 *
 * Description :
 *      Parses the given string and breaks them into individual sentences.
 */
int sentence::parse(const string &str, sentence *p_sentences,  int max_sentences, char delimeter)
{
    int    i = 0;
    size_t last_pos, cur_pos;
    
    /* start the search */
    last_pos = 0;
    cur_pos  = str.find (delimeter);
    
    /* while delimeter exists parse words */
    while ((cur_pos != string::npos) && (i < max_sentences))
    {
        p_sentences[i++] = str.substr(last_pos, cur_pos - last_pos);
        last_pos = cur_pos + 1;
        cur_pos  = str.find (delimeter, last_pos);
    }
    
    /* make word from remaing string */
    cur_pos = str.size();
    if ((cur_pos - last_pos) && (i < max_sentences))
        p_sentences[i++] = str.substr(last_pos, cur_pos - last_pos);
    
    return i;
}

/* get_words() - Returns all words in the given sentence as individual string(s)
 *
 * Input :
 *      p_words   - Where to return the words
 *      max_words - Size of <p_words>
 *
 * Output :
 *      p_words   - Contains words
 *
 * Returns :
 *      Number of words.
 *
 * Description :
 *      Reads all words from the sentence, ad returns as individual string(s).
 */
int sentence::get_words(string *p_words, int max_words) const
{
    int    num_of_words = 0;
    size_t pos = 0;
    
    /* read all words */
    while ((pos < length() ) && (num_of_words < max_words))
        p_words[num_of_words++] = get_word(pos);
    
    return num_of_words;
}

/* get_word() - Returns a word
 *
 * Input :
 *      pos         - Position from where to read a word
 *
 * Output :
 *      pos         - Returns the new position.
 *
 * Returns :
 *      word from the sentence
 *
 * Description :
 *      Reads a word from current <pos> location. This function also modifies the <pos> to the
 * new location, this helps to iterate the sentence for words.
 */
string sentence::get_word(size_t &pos) const
{
    string word;
    int    state = END;
    size_t sz  = length() ;
    
    /* inits */
    state = END;
    sz  = length() ;
    word = "";
    
    /* for each postion */
    while (pos < sz)
    {
       /* if it is not space add the chars into the word */
        if (at(pos) != ' ')
        {
            word  = word + at(pos);
            state = MIDDLE;
        }
        
        /* if white space and previous state was middle of a word */
        if ((at(pos) == ' ') && (state == MIDDLE))
        {
            return word;
        }
        
        /* go to next char */
        pos++;
    }
    return word;
}

/*
 * comprehension - Comprehension helper class
 */

/* init() - Initialize the comprension object with the given file
 *
 * Input :
 *      pFileName          - Pointer to the input file name
 *      test_right_answers - If the input file is a test file, pass this pointer to get right answers
 *                           from test file.
 * Output :
 *      test_right_answers - Right answers from test file is returned here.
 *
 * Returns :
 *      true - if sucess.
 *
 * Description :
 *      From the given file, this function creates story, questions, and original answers data
 * stuctures. Each (story, questions, and answers) will be sepereted as individual sentences
 * and stored.
 *
 * Input File Format
 *      - The first line contains a short paragraph of text from Wikipedia.
 *      - Lines 2 to 6 contain a total of 5 questions.
 *      - Line 7 contains all the five answers, which are jumbled up.
 *      - Line 8 If it is a test file, this line contains the right answers.
 *
 */
bool comprehension::init (const char *pFileName, sentence *test_right_answers)
{
    ifstream fin;
    string   line;
    bool     ret = true;
    
    /* open the given file */
    fin.open(pFileName, ios::in);
    if (fin.fail())
    {
        printf ("ERROR !!! : File open error %s\n", pFileName);
        return false;
    }
    
    /* read the story paragraph and store it as individual sentences */
    sz_story = parse (fin, story, COMP_STORY_LINES, '.');
    
    /* read the questions and store it as individual sentences */
    if (sz_story > 0)
        sz_questions = parse (fin, questions, COMP_NO_QUES);
    
    /* read the answers and store it as individual sentences */
    if (sz_questions == COMP_NO_QUES)
        sz_answers = parse (fin, answers, COMP_NO_QUES, ';');
    
    if (sz_answers != sz_questions)
    {
        printf ("ERROR !!! Questions and Answers does not match\n");
        ret = false;
    }
    
    /* if test file read additional lines (right answers) */
    if ((test_right_answers != NULL) && (ret == true))
    {
        if (sz_answers != parse (fin, test_right_answers, COMP_NO_QUES, ';'))
        {
            printf ("ERROR !!! Questions and Answers does not match\n");
            ret = false;
        }
    }
    
    /* close the file */
    fin.close();
    
    return ret;
}

/* parse() - Read a line from the file, parses it and store it as senetences
 *
 * Input :
 *      fin         - File
 *      p_sentences - Pointer to store read sentences
 *      max_sen     - Max many senetces <p_sentences> can hold
 *     delimeter    - Delimeter for parsing the line.
 *
 * Output :
 *      p_sentences - Holds the sentences
 *
 * Returns :
 *      Sentences read. -1 if error.
 *
 * Description :
 *      Reads N lines from the file and returns it as sentences.
 *
 */
int comprehension::parse (ifstream &fin, sentence *p_sentences, int max_sen, char delimeter)
{
    string  line;
    int     lines;
    
    /* init */
     lines = 0;

    /* read the a line */
    getline (fin, line);
    if (fin.fail())
    {
        printf (" ERROR !!! : Missing text \n");
        return -1;
    }
    
    /* convert the read line into sentences */
    lines = sentence::parse(line, p_sentences, max_sen, delimeter);
    
    return lines;
}

/* parse() - Parse N lines from the file
 *
 * Input :
 *      fin         - File
 *      p_sentences - Pointer to store read lines
 *      N           - How many lines to read
 *
 * Output :
 *      p_sentences - Holds the read lines
 *
 * Returns :
 *      Lines read. -1 if error.
 *
 * Description :
 *      Reads N lines from the file and returns it as sentences.
 *
 */
int comprehension::parse (ifstream &fin, sentence *p_sentences, int N)
{
    string  line;
    int     index;
    
    /* inits */
    index = 0;
    
    /* read the questions */
    for (index = 0; index < N; index++)
    {
        getline(fin, line);
        if (!fin.fail())
            p_sentences[index] = line;
        else
        {
            printf (" ERROR !!! : Need N lines \n");
            return -1;
        }
    }
    
    return index;
}

/* find_best_answers() - Finds the best answer for the comprehension.
 *
 * Input : None
 *
 * Output :
 *      right_stories - Member variable is modified with right stories which machts the questions.
 *      right_answers - Member variable is modified with right answers which matchs the questions.
 *
 * Returns :
 *      Number of sentences processed.
 *
 * Description :
 *      This functions compares each question with story line sentences to find the right story lines
 * which matches the questions. Then each given answers are compared against these story lines to
 * find better answers for corresponding questions.
 */
int comprehension::find_best_answers ()
{
    int      ans, ti;
    sentence right_story_lines[COMP_NO_QUES];
    int      right_ques_indexs[COMP_NO_QUES];
    
    /* First find story lines which machtes given questions */
    find_weights(questions, questions_sz(), story, story_sz(), weights_ques,
                 right_stories, right_story_lines);
    
    /* then compare each answer against these story lines (correponds to questions) to find better
     * answer for each questions.
     */
    find_weights(answers, answers_sz(), right_story_lines, questions_sz(), weights_ans,
                 right_ques_indexs);
    
    /* display the fond weights */
    display_weights (right_stories, right_ques_indexs);
    
    /* convert ans<->story index to ques<->ans index */
    for (ans = 0; ans < questions_sz(); ans++)
    {
        ti = right_ques_indexs[ans];
        right_answers[ti]  = ans;
    }
    
    return ans;
}

/* find_weights() - Finds similarity between two sets sentences.
 *
 * Input :
 *      p_src        - Source sentences
 *      sz_src       - size of <p_src>
 *      p_des        - target sentences
 *      sz_des       - sizeof <p_des>
 *      p_weights    - Pointer for returning weights
 *      p_max_indexs - Pointer for returning set of maximum weights
 *      p_target     - Pointer to return maximumweight strings
 *
 * Output :
 *      p_weights    - weights for all senetences
 *      p_max_indexs - returns index of the target sentences which matchs most with inputs.
 *      p_target     - most match sentence in the form of string.
 *
 * Returns :
 *      Number of sentences processed.
 *
 * Description :
 *      This functions takes two sets of sentences, and finds similarities between each sentence in
 *      source with each sentence in target. Returned weights reflect the simiarities.
 *      Function also return the closest matching sentences with the input sentence.
 */
int comprehension::find_weights (sentence *p_src,  int sz_src, sentence *p_des,  int sz_des,
                                 float p_weights[][COMP_STORY_LINES],
                                 int *p_max_indexs, sentence *p_target)
{
    int   index_src = 0;
    int   best_index;
    
    /* for each src find the best in des */
    for (index_src = 0; index_src < sz_src; index_src++)
    {
        /* find the weight similaritis for all the given sentences */
        best_index = p_src[index_src].find_weights(p_des, sz_des, p_weights[index_src]) ;
        
        /* return the best matching sentence string also */
        p_max_indexs[index_src] = best_index;
        if (p_target != NULL)
            p_target[index_src]     = p_des[best_index];
    }
    
    return index_src;
}

/*
 * Diaply helper functions
 */
void comprehension::display_org_content() const
{
    int index;
    
    if (!debug) return ;

    cout << "Story :" << "\n";
    for (index = 0; index < story_sz(); index++)
        cout << index << " " << story[index] << "\n";
    
    cout << "Questions :" << "\n";
    for (index = 0; index < questions_sz(); index++)
        cout << index << " " << questions[index] << "\n";
    
    cout << "Answers :" << "\n";
    for (index = 0; index < answers_sz(); index++)
        cout << index << " " << answers[index] << "\n";

}

void comprehension::display_right_answers () const
{
    int i, ans_index, story_index;
    
    for (i = 0; i < questions_sz(); i++)
        {
            ans_index    = right_answers[i];
            story_index  = right_stories[i];
            if (!debug)
                cout << answers[ans_index] << "\n";
            else {
                cout << i           << "." << questions[i]       << "->" ;
                cout << story_index << "." << story[story_index] << "->";
                cout << ans_index   << "." << answers[ans_index] << "\n";
            }
        }
}


void comprehension::display_weights(int *p_q_s_indexs, int *p_a_s_indexs) const
{
    int   ques, ans, story_inx;
    
    if (!debug) return ;
    
    printf ("\nQuestion<->Story Weights\n");
    for (ques = 0; ques < questions_sz(); ques++)
    {
        for (story_inx = 0; story_inx < story_sz(); story_inx++)
        {
            printf("%2.2f ", weights_ques[ques][story_inx]);
        }
        printf(" [%d]\n", p_q_s_indexs[ques]);
    }

    printf ("\nAnswer<->Story Weights\n");
    for (ans = 0; ans < questions_sz(); ans++)
    {
        for (ques = 0; ques < questions_sz(); ques++)
        {
            printf("%2.2f ", weights_ans[ans][ques]);
        }
        printf(" [%d]\n", p_a_s_indexs[ans]);
    }
}

/* unit test */
bool comprehension::run_test (const char *pFileName)
{
    bool     ret = true;
    int      ans_index,i;
    sentence input_right_answers[COMP_NO_QUES];
    
    /* parse the input test file */
    if (!init (pFileName, input_right_answers))
    {
        printf ("Test Failed !!! Parsing error \n");
        return false;
    }
    
    /* find the best answers using normal method */
    find_best_answers ();

    /* the check right answers from input file agaist the calculated one */
    for (i = 0; i < questions_sz(); i++)
        {
            ans_index = right_answers[i];
            if (input_right_answers[i] != answers[ans_index])
            {
                printf ("Test Failed !!! test answer [%d] does not match with calcuated [%d]\n", i, ans_index);
                cout << "*Test:* " << input_right_answers[i] << " *Calculated:* " << answers [ans_index] << "\n";
                ret = false;
            }
        }
    
    if (ret)
        printf ("Test Passed\n");
    
    return ret;
}
