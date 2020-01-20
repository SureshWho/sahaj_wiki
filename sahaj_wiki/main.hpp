//
//  main.h
//  sahaj_wiki
//
//  Created by Ezhil Nithya Suresh on 20/01/20.
//  Copyright © 2020 Ezhil Nithya Suresh. All rights reserved.
//

#ifndef main_h
#define main_h

class sentence : public std::string
{
    public :
    
    //sentence& operator= (const std::string& str){return *this;};
    
    /* get all the base class constructers */
    using std::string::string;
    using std::string::operator=;
    
    /* prase the given lines into mltiple sentences */
    static int parse(const std::string &str, sentence *psentences,  int max_sentences, char delimeter);
};

/*
 *
 */
#define COMP_STORY_LINES  100
#define COMP_NO_QUES      5

class comprehension
{
public:
    sentence  story[COMP_STORY_LINES];
    sentence  questions[COMP_NO_QUES];
    sentence  answers[COMP_NO_QUES];
    
    bool init (const char *pFileName);
    int story_sz() const;
    
private:
    int init_sentences(std::ifstream &fin, sentence *psentences, char delimeter);
    int init_n_sentences(std::ifstream &fin, sentence *psentences, int n);
    
    int sz_story, sz_questions, sz_answers;
    
};


#endif /* main_h */
