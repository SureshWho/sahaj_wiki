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

#include "main.hpp"

using namespace std;

char input_file[256] = "/Volumes/MyMedias/MachineLearning/code/code/sahaj_wiki/sahaj_wiki/comp";

int main(int argc, const char * argv[]) {
    comprehension com;
    
    com.init(input_file);
    std::cout << "0 " << com.story[0] << "\n";
    std::cout << "5 " << com.story[5] << "\n";
    std::cout << "6 " << com.story[6] << "\n";
    std::cout << "Last " << com.story[com.story_sz()-1] << "\n";
    
    std::cout << "Q1 " << com.questions[4] << "\n";
    std::cout << "A1 " << com.answers[4] << "\n";
    
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
