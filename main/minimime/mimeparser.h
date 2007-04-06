#ifndef _MIMEPARSER_H_INCLUDED
#define _MIMEPARSER_H_INCLUDED

#include "mm.h"

struct s_position
{
	size_t opaque_start;
	size_t start;
	size_t end;
};

struct lexer_state
{
	int header_state;
	int lineno;
	size_t current_pos;
	int condition;

	int is_envelope;

	size_t message_len;
	size_t buffer_length;

	/* temporary marker variables */
	size_t body_opaque_start;
	size_t body_start;
	size_t body_end;
	size_t preamble_start;
	size_t preamble_end;
	size_t postamble_start;
	size_t postamble_end;

	char *boundary_string;
	char *endboundary_string;
	char *message_buffer;
};


struct parser_state
{
	MM_CTX *ctx;
	struct mm_mimepart *envelope;
	struct mm_mimepart *temppart;
	struct mm_mimepart *current_mimepart;
	struct mm_content *ctype;
	int parsemode;
	int have_contenttype;
	int debug;
	int mime_parts;
	struct lexer_state lstate;
};


#include "mimeparser.tab.h"

/**
 * Prototypes for functions used by the parser routines
 */
int 	count_lines(char *);
int 	dprintf2(struct parser_state *, const char *, ...);
int 	mimeparser_yyparse(struct parser_state *, void *);
int 	mimeparser_yylex(YYSTYPE *, void *);
int	mimeparser_yyerror(struct parser_state *, void *, const char *);


#endif /* ! _MIMEPARSER_H_INCLUDED */
