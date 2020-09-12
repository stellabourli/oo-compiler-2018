D			[0-9]
L			[a-zA-Z_]
H			[a-fA-F0-9]
E			[Ee][+-]?{D}+

%{
#include <stdio.h>
#include "y.tab.h"

void count();
%}

%%
"break"			{ count(); return(BREAK); }
"case"			{ count(); return(CASE); }
"continue"	{ count(); return(CONTINUE); }
"default"		{ count(); return(DEFAULT); }
"do"			  { count(); return(DO); }
"else"			{ count(); return(ELSE); }
"for"			  { count(); return(FOR); }
"if"			  { count(); return(IF); }
"int"			  { count(); return(INT); }
"return"		{ count(); return(RETURN); }
"switch"		{ count(); return(SWITCH); }
"while"			{ count(); return(WHILE); }
"def"				{ count(); return(DEF); }
"class"			{ count(); return(CLASS); }
"abstract"	{ count(); return(ABSTRACT); }
"inherits"	{ count(); return(INHERITS); }
"print"			{ count(); return(PRINT); }
"void"			{ count(); return(VOID); }

{L}({L}|{D})*		{ count(); strcpy(yylval.strV, yytext); return(IDENTIFIER); }
{D}+						{ count(); strcpy(yylval.strV, yytext); return(CONSTANT); }
\'(.)*\'				{ count(); memmove(&yytext[0], &yytext[0 + 1], strlen(yytext) - 0);
									yytext[strlen(yytext)-1] = 0;
									strcpy(yylval.strV, yytext); return(STRING_LITERAL); }


"+="		{ count(); return(ADD_ASSIGN); }
"-="		{ count(); return(SUB_ASSIGN); }
"*="		{ count(); return(MUL_ASSIGN); }
"/="		{ count(); return(DIV_ASSIGN); }
"%="		{ count(); return(MOD_ASSIGN); }
"++"		{ count(); return(INC_OP); }
"--"		{ count(); return(DEC_OP); }
"&&"		{ count(); return(AND_OP); }
"||"		{ count(); return(OR_OP); }
"<="		{ count(); return(LE_OP); }
">="		{ count(); return(GE_OP); }
"=="		{ count(); return(EQ_OP); }
"!="		{ count(); return(NE_OP); }
";"			{ count(); return(';'); }
"{"		  { count(); return('{'); }
"}"		  { count(); return('}'); }
","			{ count(); return(','); }
":"			{ count(); return(':'); }
"="			{ count(); return('='); }
"("			{ count(); return('('); }
")"			{ count(); return(')'); }
"["		  { count(); return('['); }
"]"		  { count(); return(']'); }
"."			{ count(); return('.'); }
"-"			{ count(); return('-'); }
"+"			{ count(); return('+'); }
"*"			{ count(); return('*'); }
"/"			{ count(); return('/'); }
"%"			{ count(); return('%'); }
"<"			{ count(); return('<'); }
">"			{ count(); return('>'); }

[ \t\v\n\f]		{ count(); }
.			{  }

%%

int yywrap()
{
	//TODO edw lew ti na kanei otan teleiwsei
	return(1);
}


/*comment()
{
	char c, c1;

loop:
	while ((c = input()) != '*' && c != 0)
		putchar(c);

	if ((c1 = input()) != '/' && c != 0)
	{
		unput(c1);
		goto loop;
	}

	if (c != 0)
		putchar(c1);
}
*/

int column = 0;

void count()
{
	int i;

	for (i = 0; yytext[i] != '\0'; i++)
		if (yytext[i] == '\n')
			column = 0;
		else if (yytext[i] == '\t')
			column += 8 - (column % 8);
		else
			column++;
	ECHO;
}
