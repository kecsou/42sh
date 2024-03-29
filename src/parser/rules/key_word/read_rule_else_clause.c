#include <parser/rules/rules.h>
/**
** \file read_rule_else_clause.c
** \brief reads case_clause grammar as specified by the subject.
**
** else_clause: 'else' compound_list
** | 'elif' compound_list 'then' compound_list [else_clause]
** \author Daniel
** \version 0.3
** \date March 2019
*/
bool read_else_clause(struct parser *p)
{
    unsigned int tmp = p->cursor;

    if ((parser_readtext(p, "else")
        && read_compound_list(p)
        && read_spaces(p))
        || (parser_readtext(p, "elif")

        && read_compound_list(p)
        && parser_readtext(p, "then")
        && read_compound_list(p)))
    {
        struct ast_node *ast = ast_else_clause_init();

        ast_recover_all_from_parser(ast, p, AST_COMPOUND_LIST);
        read_else_clause(p);
        ast_recover_all_from_parser(ast, p, AST_ELSE_CLAUSE);

        ast_set_in_parser(p, ast);
        return true;
    }

    p->cursor = tmp;

    return false;
}

char *ast_else_clause_to_string(struct ast_node *ast)
{
    char *str = malloc(sizeof(char) * 32);
    sprintf(str, "AST_ELSE_CLAUSE(%ld)", ast->nb_children);
    return str;
}

struct ast_node *ast_else_clause_init(void)
{
    struct ast_node *ast = ast_init(AST_ELSE_CLAUSE, NULL);
    ast->to_string = ast_else_clause_to_string;
    return ast;
}