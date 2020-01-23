//
//  main.h
//  sahaj_wiki
//
//  Created by Ezhil Nithya Suresh on 20/01/20.
//  Copyright © 2020 Ezhil Nithya Suresh. All rights reserved.
//

#ifndef main_h
#define main_h

using namespace std;

/* sentence - Driverd from string, support sentence level functions
 *
 */
class sentence : public string
{
public :
    /* get all the base class constructers and operator= functions */
    using string::string;
    using string::operator=;
    
    /* compars this sentence with the given sentence */
    virtual float compare(const sentence &st) const;
    
    /* find a best match from given set of sentences */
    int find_best(sentence *p_sentences,  int max_sentences) const;
    int find_weights(sentence *p_sentences,  int max_sentences, float *p_weights) const;
    
    /* returns a word from the sentence */
    string get_word(size_t &pos) const;
    
    /* returns all the words */
    int get_words(string *p_words, int max_words) const;
    
    /* prase the given lines into multiple sentences */
    static int parse(const string &str, sentence *psentences,  int max_sentences, char delimeter);
    
protected:
    static const int    SEN_MAX_WORDS;                         /* Maximum words in a sentence */
    
private:
    static const int END;
    static const int MIDDLE;
    
    /* compares the given words and return weights */
    static float compare_words(string *p_src_words, int src_words,
                               string *p_des_words, int des_words);
    static float compare_word(string src_word, string des_words);
    
    static const string COMMON_WORDS;
};

/* constants */
const int sentence::SEN_MAX_WORDS   = 64;                       /* Maximum words in a sentence */
const int sentence::MIDDLE          = 1;                        /* state constants internal use */
const int sentence::END             = 0;                        /* state constants internal use */

/* low impact common words which can be ignored while comparing two sentences */
const string sentence::COMMON_WORDS = "are Which What which and the what of by in";

/* comprehension - Support comprehension related funtions
 *
 */
#define COMP_STORY_LINES 100
#define COMP_NO_QUES     5
class comprehension
{
public:
    sentence  story[COMP_STORY_LINES];             /* Story para with each lines seperated */
    sentence  questions[COMP_NO_QUES];             /* questions */
    sentence  answers[COMP_NO_QUES];               /* original answeres with each answer seperated*/
    int  right_answers[COMP_NO_QUES];              /* right answeres */
    int  right_stories[COMP_NO_QUES];              /* correspoding stories  */
    float  weights_ques[COMP_NO_QUES][COMP_STORY_LINES];               /* Ques<->stroy line weight */
    float  weights_ans[COMP_NO_QUES][COMP_STORY_LINES];                /* Ans<->stroy line weight */
    
    /* initialize the internal data structure using given file */
    bool init (const char *pFileName);
    int find_best_answers (int *p_indexs);
    
    /* helpers functions to get size of internal data */
    int story_sz()     const {return sz_story;};
    int questions_sz() const {return sz_questions;};
    int answers_sz()   const {return sz_answers;};
    void cross_check() const;
    void display_right_answers (bool just_ans=true) const;
    
private:
    /* help function to parse the file */
    static int parse(ifstream &fin, sentence *p_sentences, int max_sen, char delimeter);
    static int read_N(ifstream &fin, sentence *p_sentences, int N);
    static int find_best (sentence *p_src,  int sz_src, sentence *p_des,  int sz_des,
                          int* p_indexs, sentence *p_result);
    static int find_weights (sentence *p_src, int sz_src, sentence *p_des,  int sz_des,
                             float p_weight[][COMP_STORY_LINES],
                             int *p_max_indexs, sentence *p_target = NULL);
    int sz_story, sz_questions, sz_answers;

};

#endif /* main_h */
