#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define OPTIONAL(R) \
__extension__({     \
    R;              \
    1;              \
})

#define ZERO_OR_MANY(R)\
__extension__({        \
    while(R);          \
    1;                 \
})

#define ONE_OR_MANY(R)    \
__extension__({     \
    int res = 0;\
    if (R)          \
    {               \
        while (R);  \
        res = 1; \
    }               \
    res;	        \
})

struct parser
{
    char *input;
    int cursor;
    struct list_capt_s *capture;
    struct ast_node *ast;
    struct error_s *error;
};

enum error_type_e
{
    ON_CHAR,
    ON_TEXT,
    ON_RANGE,
    ON_INSET,
    ON_OUTSET
};

struct error_s
{
    enum error_type_e type;
    union
    {
        char c;
        char *text;
        char *inset;
        char *outset;
        struct
        {
            char begin;
            char end;
        } range;
    } u;
};

enum ast_node_type
{
    AST_NODE_EMPTY,
    AST_NODE_INI_FILE,
    AST_NODE_SECTION,
    AST_NODE_KEY_VALUE,
    AST_NODE_ASSIGN,
    AST_INPUT,
    AST_LIST,
    AST_AND_OR,
    AST_PIPELINE,
    AST_COMMAND,
    AST_SIMPLE_COMMAND,
    AST_SHELL_COMMAND,
    AST_FUNCDEC,
    AST_REDIRECTION,
    AST_PREFIX,
    AST_ELEMENT,
    AST_COMPOUND_LIST,
    AST_RULE_FOR,
    AST_RULE_WHILE,
    AST_RULE_UNTIL,
    AST_RULE_CASE,
    AST_RULE_IF,
    AST_ELSE_CLAUSE,
    AST_DO_GROUP,
    AST_CASE_CLAUSE,
    AST_CASE_ITEM,
    AST_NODE_HEREDOC
};

// ast inifile
struct ast_node
{
    enum ast_node_type type;
    void *data;
    size_t nb_children;
    size_t capacity;
    struct ast_node **children; // array of children
};

struct ast_section
{
    char *identifier;
};

struct ast_key_value
{
    char *id;
    char *value;
};

struct ast_assign
{
    char *id;
    char *num;
};

struct capture_s
{
    int begin;
    int end;
};

struct list_capt_s
{
    char *tag;
    struct capture_s capt;
    struct list_capt_s *next;
};

//ast functional
struct ast_input{char *elt;};

struct ast_list{char *elt;};

struct ast_and_or{char *elt;};

struct ast_pipeline{char *elt;};

struct ast_command{char *elt;};

struct ast_simple_command{char *elt;};

struct ast_shell_command{char *elt;};

struct ast_funcdec{char *elt;};

struct ast_redirection{char *elt;};

struct ast_prefix{char *elt;};

struct ast_element{char *elt;};

struct ast_compound_list{char *elt;};

struct ast_rule_for{char *elt;};

struct ast_rule_while{char *elt;};

struct ast_untile{char *elt;};

struct ast_case{char *elt;};

struct ast_if{char *elt;};

struct ast_else_clause{char *elt;};

struct ast_do_group{char *elt;};

struct ast_case_clause{char *elt;};

struct ast_case_item{char *elt;};

//parser_init
struct parser *parser_new_from_string(const char *text);
void parser_free(struct parser *p);

//parser_character
bool parser_eof(struct parser *p);
char parser_getchar(struct parser *p);
bool parser_peekchar(struct parser * p, char c);
bool parser_readchar(struct parser *p, char c);
bool parser_readrange(struct parser *p, char begin, char end);

//parser_string_includes
bool parser_readtext(struct parser *p, char *text);
bool parser_readinset(struct parser *p, char *set);
bool parser_readoutset(struct parser *p, char *set);

//parser_read_alphanum
bool parser_readalpha(struct parser *p);
bool parser_readnum(struct parser *p);
bool parser_readletter(struct parser *p);
bool parser_readalphanum(struct parser *p);

//parser_read_interval
bool parser_readeol(struct parser *p);
bool parser_readidentifier(struct parser *p);
bool parser_readinteger(struct parser *p);

//parser_capture_init
void free_list_capt_s(struct list_capt_s *capture);
struct list_capt_s *list_capt_init(void);

//parser_capture
char *extract_string(char *s, int begin, int end);
void print_capture(struct parser *p, struct list_capt_s *capture);
void list_capt_store(struct list_capt_s *, const char *, struct capture_s *);
struct capture_s *list_capt_lookup(struct list_capt_s *, const char *);
void remove_capture_by_tag(struct parser *p, const char *tag);
bool parser_readassign(struct parser *p);

static inline bool parser_begin_capture(struct parser *p, const char *tag)
{
    struct capture_s capt = { p->cursor, 0 };
    list_capt_store(p->capture, tag, &capt);
    return true;
}

static inline char *parser_get_capture(struct parser *p, const char *tag)
{
    struct capture_s *pcapt = list_capt_lookup(p->capture, tag);
    if (!pcapt)
        return false;
    return strndup(&p->input[pcapt->begin], pcapt->end - pcapt->begin);
}

static inline bool parser_end_capture(struct parser *p, const char *tag)
{
    struct capture_s *pcapt = list_capt_lookup(p->capture, tag);
    if (!pcapt)
        return false;
    pcapt->end = p->cursor;
    return true;
}

//ast_init
struct ast_node *ast_init(enum ast_node_type, void *data);
void ast_free(struct ast_node *ast);

//ast_data
void ast_assign_free(struct ast_assign *ast_assign);
void ast_key_value_free(struct ast_key_value *ast_key_value);
void ast_section_free(struct ast_section *ast_sec);

//ast
void ast_store(struct parser *p, enum ast_node_type type, struct ast_assign
*ast_assign);
// coder ast_lookup : renvoie un ast_node en cherchant le premier tag qui match
struct ast_node *ast_lookup(struct parser *p, const char *tag);
void ast_set_in_parent(struct ast_node *parent, struct ast_node *ast);
struct ast_node *ast_get_from_parent(struct ast_node *parent, enum ast_node_type type_ast_search);
void ast_set_in_parser(struct parser *p, struct ast_node *ast);
struct ast_node *ast_get_from_parser(struct parser *p, enum ast_node_type type_ast_search);