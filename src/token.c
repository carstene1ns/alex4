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
 
 
 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "token.h"
#include "main.h"


////////////////////////////////////////////////////////////////
// token handling
////////////////////////////////////////////////////////////////

// creates a new token
Ttoken *create_token(char *word) {
    Ttoken *tok = malloc(sizeof(Ttoken));
    if (tok != NULL) {
        tok->word = strdup(word);
        tok->next = NULL;
    }
    return tok;
}


// frees a token
void destroy_token(Ttoken *t) {
    free(t->word);
    free(t);
}


// frees all tokens
void flush_tokens(Ttoken *head) {
	if (head == NULL) return;
    if (head->next != NULL) flush_tokens((Ttoken *)head->next);
    destroy_token(head);
}


// inserts a token last in the list
void insert_token(Ttoken *list, Ttoken *t) {
    if (list->next == NULL)
        list->next = (struct Ttoken *)t;
    else insert_token((Ttoken *)list->next, t);
}


// tokenizes the string str
Ttoken *tokenize(char *str) {
    Ttoken *tok_list, *tok_tmp;
    char word[256];
    int a, b, c;
    int i = 0;

    tok_list = create_token("head");

    while(str[i] != '\0') {
        // skip leading whitespace
		while(str[i] == ' ' || str[i] == '\t') i++;

		if (str[i] == '"') { // find end of string part
			i ++;
			a = i;
			while(str[i] != '"') i++;
			b = i;
			i ++;
		}
		else { 		// find end of word
			a = i;
			while(str[i] != ' ' && str[i] != '\t' && str[i] != '\0') i++;
			b = i;
		}
				
		// copy string to word
        for(c=0;c<b-a;c++) word[c] = str[a+c];
		word[c] = '\0';

        // make token and put it in the list
		tok_tmp = create_token(word);
		insert_token(tok_list, tok_tmp);
    }

	tok_tmp->word[strlen(tok_tmp->word) - 1] = '\0';
    tok_tmp = (Ttoken *)tok_list->next;
    destroy_token(tok_list);
    
    return tok_tmp;
}


// returns the work of the token after token
char *get_next_word(Ttoken *t) {
	Ttoken *next = (Ttoken *)t->next;
	if (next == NULL) {
		char buf[80];
		sprintf(buf, "<%s> has no next", t->word);
		msg_box(buf);
	}
	return next->word;
}



// prints a token list
void print_token(Ttoken *t) {
	Ttoken *ptr = t;

	while(ptr != NULL) {
		printf("%s ", ptr->word);
		ptr = (Ttoken *)ptr->next;
	}
}


