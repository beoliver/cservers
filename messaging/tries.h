//
//  tries.h
//  chitchat
//
//  Created by Ben Oliver on 06/11/15.
//  Copyright © 2015 Ben Oliver. All rights reserved.
//

#ifndef tries_h
#define tries_h

typedef struct ascii_trie_s {
    char key;
    int val;
    unsigned short weight;
    struct ascii_trie_s* child;
    struct ascii_trie_s* next;
} ascii_trie ;

ascii_trie* ascii_trie_init();

void free_ascii_trie(ascii_trie*);

ascii_trie* ascii_trie_lookup(char*, int*, ascii_trie*);

int ascii_trie_insert(char*, int, ascii_trie*);

void ascii_trie_delete(char*, ascii_trie*);

#endif /* tries_h */
