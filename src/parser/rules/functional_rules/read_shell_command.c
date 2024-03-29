#include <parser/rules/rules.h>
/**
** \file read_shell_command.c
** \brief reads shell_command grammar as specified by the subject.
**
** shell_command: '{' compound_list '}'
** | '(' compound_list ')'
** | rule_for
** | rule_while
** | rule_until
** | rule_case
** | rule_if
** \author Thibault
** \version 0.3
** \date February 2019
*/
static bool read_first(struct parser *p)
{
    unsigned int tmp = p->cursor;
    if (parser_readchar(p, '{')
        && ZERO_OR_MANY(read_end_of_line(p))
        && read_compound_list(p)
        && ZERO_OR_MANY(read_end_of_line(p))
        && parser_readchar(p, '}'))
    {
        return true;
    }

    p->cursor = tmp;

    return false;
}

static bool read_second(struct parser *p)
{
    unsigned int tmp = p->cursor;
    if (parser_readchar(p, '(')
        && read_compound_list(p)
        && parser_readchar(p, ')'))
    {
        return true;
    }

    p->cursor = tmp;

    return false;
}

bool read_shell_command(struct parser *p)
{
    unsigned int tmp = p->cursor;

    bool compound_list = false;
    bool rule_for = false;
    bool rule_while = false;
    bool rule_until = false;
    bool rule_case = false;
    bool rule_if = false;

    if ((compound_list = read_first(p))
       ||(compound_list = read_second(p))
       ||(rule_for = read_rule_for(p))
       || (rule_while = read_rule_while(p))
       || (rule_until = read_rule_until(p))
       || (rule_case = read_rule_case(p))
       || (rule_if = read_rule_if(p)))
    {
        struct ast_node *ast = ast_shell_command_init();
        struct ast_node *ast_child = NULL;

        if (compound_list)
            ast_child = ast_get_from_parser(p, AST_COMPOUND_LIST);
        else if (rule_for)
            ast_child = ast_get_from_parser(p, AST_RULE_FOR);
        else if (rule_while)
            ast_child = ast_get_from_parser(p, AST_RULE_WHILE);
        else if (rule_until)
            ast_child = ast_get_from_parser(p, AST_RULE_UNTIL);
        else if (rule_case)
            ast_child = ast_get_from_parser(p, AST_RULE_CASE);
        else if (rule_if)
            ast_child = ast_get_from_parser(p, AST_RULE_IF);

        ast_set_in_parent(ast, ast_child);
        ast_set_in_parser(p, ast);

        return true;
    }

    p->cursor = tmp;

    return false;
}

int ast_shell_command_exec(struct ast_node *ast)
{
    if (ast->type != AST_SHELL_COMMAND)
        error_ast_exec("ast_shell_command_exec");
    struct ast_node *first_child = ast->children[0];
    return first_child->exec(first_child);
}

struct ast_node *ast_shell_command_init(void)
{
    struct ast_node *ast = ast_init(AST_SHELL_COMMAND, NULL);
    ast->exec = ast_shell_command_exec;
    return ast;
}