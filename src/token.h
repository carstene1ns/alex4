/**************************************************************
 *         _____    __                       _____            *
 *        /  _  \  |  |    ____  ___  ___   /  |  |           *
 *       /  /_\  \ |  |  _/ __ \ \  \/  /  /   |  |_          *
 *      /    |    \|  |__\  ___/  >    <  /    ^   /          *
 *      \____|__  /|____/ \___  >/__/\_ \ \____   |           *
 *              \/            \/       \/      |__|           *
 *                                                            *
 **************************************************************
 *    (c) Free Lunch Design 2003                              *
 *    Written by Johan Peitz                                  *
 *    http://www.freelunchdesign.com                          *
 **************************************************************
 *    This source code is released under the The GNU          *
 *    General Public License (GPL). Please refer to the       *
 *    document license.txt in the source directory or         *
 *    http://www.gnu.org for license information.             *
 **************************************************************/
 
 
 

#ifndef _TOKEN_H_
#define _TOKEN_H_


// the struct for a token
typedef struct {
    char *word;
    struct Ttoken *next;
} Ttoken;


// functions
Ttoken *create_token(char *word);
void destroy_token(Ttoken *t);
void flush_tokens(Ttoken *head);
void insert_token(Ttoken *list, Ttoken *t);
char *get_next_word(Ttoken *t);

Ttoken *tokenize(char *str);


#endif