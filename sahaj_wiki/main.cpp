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

bool debug = false;
char input_file[256] = "/Volumes/MyMedias/MachineLearning/code/code/sahaj_wiki/sahaj_wiki/comp";

int main(int argc, const char * argv[]) {
    comprehension com;
    int ans_index[COMP_NO_QUES] = {0};
    
    /* create comprehension data structure for processing */
    com.init(input_file);
    
    /* check whether the parsing is right */
    if (debug)
        com.cross_check ();
    
    /* find the best ans */
    com.find_best_answers (ans_index);
    
    /* display the right ans */
    com.display_right_answers(!debug);
    
    return 0;
}

/*
 * sentence - Sentences helper class
 */

/* XXX parse() - Parses the given string and makes individual sentences.
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
 *      Compares given sentence, and returns similarity in the form of weights. Eg 1 means sentence
 * is perfect match, 0.5 means only half of the sentence match.
 */
int sentence::find_best(sentence *p_sentences,  int max_sentences) const
{
    int   index, match;
    float tweight, max_weight;
    
    /* inits */
    max_weight = -1;
    match      = -1;
    
    cout << "\nComparing " << *this << "\n";
    
    for (index = 0; index < max_sentences; index++)
    {
        cout << "\t" << index << " " << p_sentences[index] << "\n";
        tweight = compare(p_sentences[index]);
        
        /* find the best match */
        if (tweight > max_weight)
        {
            match      = index;
            max_weight = tweight;
        }
    }
    
    return match;
}

int sentence::find_weights(sentence *p_sentences,  int max_sentences, float *p_weights) const
{
    int   index, max_index;
    float max_weight, weight;
    
    cout << "\nComparing " << *this << "\n";
    
    /*inits*/
    max_index = 0;
    max_weight = 0;
    
    for (index = 0; index < max_sentences; index++)
    {
        cout << "\t" << index << " " << p_sentences[index] << "\n";
        
        weight           = compare(p_sentences[index]);
        p_weights[index] = weight;
        if (weight > max_weight)
        {
            max_weight = weight;
            max_index  = index;
        }
    }
    
    return max_index;
}


/* parse() - Parses the given string and makes individual sentences.
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
 *      Compares given sentence, and returns similarity in the form of weights. Eg 1 means sentence
 * is perfect match, 0.5 means only half of the sentence match.
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

/* compare() - Compares the given sentence.
 *
 * Input :
 *      target      - Sentence to be compared.
 *
 *
 * Output :
 *      None
 *
 * Returns :
 *      Similarity in the form of Weight.
 *
 * Description :
 *      Compares given sentence, and returns similarity in the form of weights. Eg 1 means sentence
 * is perfect match, 0.5 means only half of the sentence match.
 */
float sentence::compare(const sentence &target) const
{
    int    len_src, len_des;
    string src[SEN_MAX_WORDS], des[SEN_MAX_WORDS];
    float  weight ;
    
    /* get the words from sentences */
    len_src = get_words(src, SEN_MAX_WORDS);
    len_des = target.get_words(des, SEN_MAX_WORDS);

    /* compare words */
    weight = compare_words(src, len_src, des, len_des);
    
    return weight;
}

/* compare_words() - Compares two sets of words.
 *
 * Input :
 *      p_src_words - Source words
 *      src_words   - size of <p_src_words>
 *      p_des_words - destination words
 *      des_words   - sizeof <p_des_words>
 *
 *
 * Output :
 *      None
 *
 * Returns :
 *      Similarity in the form of Weight.
 *
 * Description :
 *      Compares given words, and returns similarity in the form of weights. Eg 1 means all words
 * match perfectly, 0.5 means only half of the words match.
 */
float sentence::compare_words(string *p_src_words, int src_words,
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
            weight = compare_word(p_src_words[src_index], p_des_words[des_index]);
            if (weight > max_weight)
            {
                max_weight = weight;
            }
        }
        matches += max_weight;
    }
    
    //weight = matches / des_words;
    weight = matches;
    
    printf ("Mathes %f src %d des %d Weight %f\n", matches, src_words, des_words, weight);
    
    return  weight;
}

/* compare_word() - Compares two given words.
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
 *      Compares given words, and returns similarity in the form of weights. Eg 1 means the
 * words match perfectly, 0.5 means only half of the word match.
 */
float sentence::compare_word(string src, string des)
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


/* get_words() - Returns all words in the sentence as individual string(s)
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
int comprehension::find_best_answers (int *p_indexs)
{
    int      story_inx, ques, ans, ti;
    float    max_weight;
    sentence right_story_lines[COMP_NO_QUES];
    int      right_ques_indexs[COMP_NO_QUES];
    
    
    //float weights_total[COMP_NO_QUES][COMP_STORY_LINES];
    
    printf ("\nFinding weights for questions\n");
    find_weights(questions, questions_sz(), story, story_sz(), weights_ques,
                 right_stories, right_story_lines);
    find_weights(answers, answers_sz(), right_story_lines, questions_sz(), weights_ans, right_ques_indexs);
    
    printf ("\nQuestion<->Story Weights\n");
    for (ques = 0; ques < questions_sz(); ques++)
    {
        for (story_inx = 0; story_inx < story_sz(); story_inx++)
        {
            printf("%2.2f ", weights_ques[ques][story_inx]);
        }
        printf(" [%d]\n", right_stories[ques]);
    }

    printf ("\nAnswer<->Story Weights\n");
    for (ans = 0; ans < questions_sz(); ans++)
    {
        for (ques = 0; ques < questions_sz(); ques++)
        {
            printf("%2.2f ", weights_ans[ans][ques]);
        }

        ti = right_ques_indexs[ans];
        right_answers[ti]  = ans;
        printf(" [%d]\n", ti);
        
    }
    
    return ques;
}


//XXX
int comprehension::find_best (sentence *p_src,  int sz_src, sentence *p_des,  int sz_des,
                              int* p_indexs, sentence *p_result)
{
    int  index_src = 0;
    int  index;
    
    /* for each src find the best in des */
    for (index_src = 0; index_src < sz_src; index_src++)
    {
        index = p_src[index_src].find_best(p_des, sz_des) ;
        p_indexs[index_src]  = index;
        if (p_result != NULL)
            p_result[index_src]  = p_des[index] ;
    }
    
    return index_src;
}

int comprehension::find_weights (sentence *p_src,  int sz_src, sentence *p_des,  int sz_des,
                                 float p_weights[][COMP_STORY_LINES],
                                 int *p_max_indexs, sentence *p_target)
{
    int   index_src = 0;
    int   best_index;
    
    /* for each src find the best in des */
    for (index_src = 0; index_src < sz_src; index_src++)
    {
        best_index = p_src[index_src].find_weights(p_des, sz_des, p_weights[index_src]) ;
        p_max_indexs[index_src] = best_index;
        if (p_target != NULL)
            p_target[index_src]     = p_des[best_index];
    }
    
    return index_src;
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

/* read_N() - Read N lines from the file
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
int comprehension::read_N (ifstream &fin, sentence *p_sentences, int N)
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

/* init() - Initialize the comprension object with the given file
 *
 * Input :
 *      pFileName - Pointer to the input file name
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
 *
 */
bool comprehension::init (const char *pFileName)
{
    ifstream fin;
    string  line;
    
    /* open the given file */
    fin.open(pFileName, ios::in);
    if (fin.fail())
    {
        printf ("ERROR !!! : File open error \n");
        return false;
    }
    
    /* read the story paragraph and store it as individual sentences */
    sz_story = parse (fin, story, COMP_STORY_LINES, '.');
    
    /* read the questions and store it as individual sentences */
    if (sz_story > 0)
        sz_questions = read_N (fin, questions, COMP_NO_QUES);
    
    /* read the answers and store it as individual sentences */
    if (sz_questions == COMP_NO_QUES)
        sz_answers = parse (fin, answers, COMP_NO_QUES, ';');
    
    if (sz_answers != sz_questions)
        printf ("ERROR !!! Questions and Answers does not match\n");
    
    /* close the file */
    fin.close();
    
    return true;
}

void comprehension::cross_check() const
{
    int index;
    
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

void comprehension::display_right_answers (bool just_ans) const
{
    int i, ans_index, story_index;
    
    for (i = 0; i < questions_sz(); i++)
        {
            ans_index    = right_answers[i];
            story_index  = right_stories[i];
            if (just_ans)
                cout << answers[ans_index] << "\n";
            else
                cout << questions[i] << "->" << story[story_index] << "->" << answers[ans_index] << "\n";
        }
}

