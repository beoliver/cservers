//
//  tries.c
//  chitchat
//
//  Created by Ben Oliver on 06/11/15.
//  Copyright © 2015 Ben Oliver. All rights reserved.
//

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "tries.h"





void free_ascii_trie(ascii_trie* node) {
    if (node != NULL) {
        free_ascii_trie(node->next);
        free_ascii_trie(node->child);
        free(node);
    }
}


ascii_trie* ascii_trie_init() {
    ascii_trie* root = malloc(sizeof(ascii_trie));
    root->key        = 0x00;
    root->child      = NULL;
    root->next       = NULL;
    return root;
}

ascii_trie* ascii_trie_lookup(char* key, int* valp, ascii_trie* node) {
    
    while (1) {
        
        while ((*key != node->key) && (node->next != NULL)) {
            node = node->next;
        }
        
        if (*key != node->key) {
            printf("name does not exist\n");
            return NULL;
        }
        
        if (*key == 0x00) {
            *valp = node->val;
            printf("name exists\n");
            return node;
        }
        
        key  = key+1;
        node = node->child;
        
    }
}



void print_weights(ascii_trie* node) {
    if (node != NULL) {
        printf("node: %c weight: %d\n", node->key, node->weight);
        print_weights(node->next);
        print_weights(node->child);
    }
}



static ascii_trie* unsafe_trie_insert(char* key, int val, ascii_trie* node) {
    
    // element must not allready exist in trie
    
    while (1) {
        
        // move right untill we find a match OR reach the end of the row
        
        while ((*key != node->key) && (node->next != NULL)) {
            node = node->next;
        }
        
        if (*key == node->key) {
            
            node->weight += 1 ;
            node = node->child ;
            key  = key+1 ;
            
        } else {
            
            node->next   = malloc(sizeof(ascii_trie));
            node         = node->next;
            node->weight = 1;
            node->key    = *key;
            node->next   = NULL;
            node->child  = NULL;
            
            while (*key != '\0') {
                key = key+1;
                node->child = malloc(sizeof(ascii_trie));
                node = node->child;
                node->weight = 1;
                node->key   = *key;
                node->next  = NULL;
                node->child = NULL;
            }
         
            node->weight = 0;
            node->val = val;
            return node;
        }
    }
}


int ascii_trie_insert(char* key, int val, ascii_trie* root) {
    int valp;
    if (ascii_trie_lookup(key, &valp, root) == NULL) {
        if (unsafe_trie_insert(key, val, root) != NULL) {
            return 1; // ok
        } else {
            return -1; // actual error
        }
    }
    return 0; // key already exists
}


static void unsafe_trie_delete(char* key, ascii_trie* node, ascii_trie* parent) {
    
    ascii_trie* prev = NULL;
    
    while ((node->key != *key) && (node->next != NULL)) {
        prev = node;
        node = node->next;
    }
    
    // we have now found the node that we need to use
    
    if (node->weight < 2) {
        if (prev == NULL) {
            assert(parent != NULL);
            parent->child = node->next;
        } else {
            prev->next = node->next;
        }
        // only one string uses this node
        // thus only one string uses the children of the node
        // use < 2 as we might allready be at the terminal node
        // which has weight 0
        // remember that we might not be at the end of a row
        // so we use prev->next to join the previous node with node->next
        // which is either a node or NULL
        free_ascii_trie(node->child);
        free(node);
    } else {
        // node weight is > 1
        node->weight--;
        unsafe_trie_delete(key+1, node->child, node);
    }
    
}


void ascii_trie_delete(char* key, ascii_trie* root) {
    int valp;
    if (ascii_trie_lookup(key,&valp,root) != NULL) {
        unsafe_trie_delete(key, root, NULL);
    }
}






