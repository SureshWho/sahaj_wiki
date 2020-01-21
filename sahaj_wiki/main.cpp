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

char input_file[256] = "/Volumes/MyMedias/MachineLearning/code/code/sahaj_wiki/sahaj_wiki/comp";

int main(int argc, const char * argv[]) {
    comprehension com;
    int ans, i, story;
    float weight, max_weight = 0;
    
    com.init(input_file);
    std::cout << "0 " << com.story[0] << "\n";
    std::cout << "5 " << com.story[5] << "\n";
    std::cout << "6 " << com.story[6] << "\n";
    std::cout << "Last " << com.story[com.story_sz()-1] << "\n";
    
    std::cout << "Q1 " << com.questions[0] << "\n";
    std::cout << "A1 " << com.answers[0] << "\n";
    
    ans = 0;
    for (i = 0; i < com.story_sz(); i++)
    {
        weight = com.questions[0].compare(com.story[i]);
        if (weight > max_weight)
        {
            ans = i;
            max_weight = weight;
        }
        std::cout << i << " " << weight << "\n";
    }
    
    cout << "ANS : " << ans << " " << com.story[ans];
    story = ans;
    ans = 0;
    max_weight = 0;
    for (i = 0; i < com.answers_sz(); i++)
    {
        weight = com.story[story].compare(com.answers[i]);
        if (weight > max_weight)
        {
            ans = i;
            max_weight = weight;
        }
        std::cout << i << " " << weight << "\n";
    }
    
    cout << "ANS : " << ans << " " << com.answers[ans];
    
    return 0;
}

/*
 *
 */
int sentence::parse(const std::string &str, sentence *psentences,  int max_sentences, char delimeter)
{
    int    i = 0;
    size_t last_pos, cur_pos;
    
    /* start the search */
    last_pos = 0;
    cur_pos  = str.find (delimeter);
    
    /* while delimeter exists */
    while ((cur_pos != string::npos) && (i < max_sentences))
    {
        psentences[i++] = str.substr(last_pos, cur_pos - last_pos);
        last_pos = cur_pos + 1;
        cur_pos  = str.find (delimeter, last_pos);
    }
    
    cur_pos = str.size();
    if ((cur_pos - last_pos) && (i < max_sentences))
        psentences[i++] = str.substr(last_pos, cur_pos - last_pos);
    
    return i;
}

float sentence::compare(const sentence &target) const
{
    int    len_src, len_des;
    string src[SEN_MAX_WORDS], des[SEN_MAX_WORDS];
    
    len_src = get_words(src, SEN_MAX_WORDS);
    len_des = target.get_words(des, SEN_MAX_WORDS);
    
    //printf ("Source %d Des %d", len_src, len_des);
    
    return compare_words(src, len_src, des, len_des);
}

int sentence::sizeinwords() const
{
    int wc = 0;

    for (int i = 0; i < length(); i++)
        if (at(i) == ' ')
            wc++;

    return wc;
}

float sentence::compare_words(string *p_src_words, int src_words, string *p_des_words, int des_words)
{
    int    src_index, des_index;
    float  max_weight, weight, matches = 0.0;
    
    matches    = 0;
    max_weight = 0.0;
    
    for (src_index = 0; src_index < src_words; src_index++)
    {
        max_weight = 0.0;
        for (des_index = 0; des_index < des_words; des_index++)
        {
            weight = compare_word(p_src_words[src_index], p_des_words[des_index]);
            max_weight = max(max_weight, weight);
        }
        matches += max_weight;
    }
    
    printf ("Mathes %f Src Words %d\n", matches, src_words);
    
    return matches / src_words ;
}

int sentence::get_words(string *p_words, int max_words) const
{
    int    num_of_words = 0;
    size_t pos = 0;
    string s;
    
    //cout << "SENTANCE " << *this << ": ";
    
    while ((pos < length() ) && (num_of_words < max_words))
    {
        s = get_word(pos);
        p_words[num_of_words++] = s;
        //cout << s << " " << pos << " ";
    }
    
    //cout << "\n" ;
    
    return num_of_words;
}

float sentence::compare_word(string src, string des)
{
    size_t src_len, des_len, pos, len;
    int    matches = 0;
    float  weight;
    
    /* for all common words return zero as weights */
    pos = COMMON_WORDS.find(src);
    if (pos != npos)
        return 0;
    
    src_len = src.length();
    des_len = des.length();
    len     = min(src_len, des_len);

    for (pos = 0; pos < len; pos++)
        if (tolower(src.at(pos)) == tolower(des.at(pos)))
            matches++;
    weight = (float) matches / src_len;
    weight = round(weight - 0.2);
    
    cout << "weight " << src << " " << des << " " << weight << "\n";
    
    return weight;
}

string sentence::get_word(size_t &pos) const
{
    string word;
    int    state = END;
    size_t last  = length() - 1;
    
    word = "";
    while (pos <= last)
    {
       /* if it is not space add the chars into the word*/
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

/* Read the given comprehension file and make the comprehension object
 *
 */

int comprehension::init_sentences(ifstream &fin, sentence *psentences, char delimeter)
{
    string  line;
    int     lines;
    
    /* read the story */
    getline(fin, line);
    if (fin.fail())
    {
        printf (" ERROR !!! : Missing text \n");
        return -1;
    }
    
    /* convert the story into multiple sentences */
    lines = sentence::parse(line, psentences, COMP_STORY_LINES, delimeter);
    
    return lines;
    
    
}

int comprehension::init_n_sentences(ifstream &fin, sentence *psentences, int N)
{
    string  line;
    int     index;
    
    /* read the questions */
    for (index = 0; index < N; index++)
    {
        getline(fin, line);
        if (!fin.fail())
            questions[index] = line;
        else
        {
            printf (" ERROR !!! : Need N lines \n");
            return -1;
        }
    }
    
    return index;
}

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
    
    /* make setenses for the story */
    sz_story = init_sentences (fin, story, '.');
    
    /* read the questions */
    if (sz_story > 0)
        sz_questions = init_n_sentences(fin, questions, COMP_NO_QUES);
    
    /* make setenses for the answers */
    if (sz_questions == COMP_NO_QUES)
        sz_answers = init_sentences (fin, answers, ';');
    
    if (sz_answers != sz_questions)
        printf ("ERROR !!! Questions and Answers does not match\n");
    
    /* close the file */
    fin.close();
    
    return true;
}

int comprehension::story_sz() const
{
    return sz_story;
}

int comprehension::answers_sz() const
{
    return sz_answers;
}
