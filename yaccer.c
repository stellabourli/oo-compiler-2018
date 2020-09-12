%{
#include <stdio.h>
#include <stdlib.h>
extern int yylex();
extern int yyparse();
extern FILE *yyin;
FILE *c_file;
extern char yytext[];
extern int column;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

	//STRUCTS
typedef struct field_s Field;
typedef struct class_s Class;

struct field_s {
	char type[20];
	char name[40];
	int isArray;
	int *int_value;
	Class *object_value;
};

typedef struct {
	char type[20];
	char name[40];
} Parameter;

typedef struct {
	int isAbstract;
	Parameter parameters[20];
	char methodBody[10000];
	char return_type[30];
} Method;

struct class_s {
	char name[40];
	Field fields[10];
	int fieldsSize;
	Class *parents;
	int countParents;
};

typedef struct {
	Class *object_class;
	char name[40];
} Object;


typedef struct class_node_s{
    Class value;
    struct class_node_s *next;
} class_node;

typedef struct object_node_s{
    Object value;
    struct object_node_s *next;
} object_node;

class_node *class_head = NULL;
class_node *class_tail = NULL;

void class_addNode(Class value) {
	class_node *temp = NULL;
	if (class_head == NULL) {
		class_tail = (class_node *) malloc (sizeof(class_node));
		if(class_tail==NULL) exit(1);
		class_tail->value = value;
		class_tail->next = NULL;
		class_head = class_tail;
	}
	else {
	temp = class_tail;

	class_tail = (class_node *) malloc (sizeof(class_node));
	if(class_tail==NULL) exit(1);
	class_tail->value = value;
	class_tail->next = NULL;

	temp->next = class_tail;
	}
}

object_node *object_head = NULL;
object_node *object_tail = NULL;

void object_addNode(Object value) {
	object_node *temp = NULL;
	if (object_head == NULL) {
		object_tail = (object_node *) malloc (sizeof(object_node));
		if(object_tail==NULL) exit(1);
		object_tail->value = value;
		object_tail->next = NULL;
		object_head = object_tail;
	}
	else {
	temp = object_tail;

	object_tail = (object_node *) malloc (sizeof(object_node));
	if(object_tail==NULL) exit(1);
	object_tail->value = value;
	object_tail->next = NULL;

	temp->next = object_tail;
	}
}

Class *newClass;
Field *newField;
Method *newMethod;
Parameter *newParameter;
int *initializers;
int initializersSize = 0;
int isField = 0;
int isParameter = 0;
int countParameters = 0;
int isMethodBody = 0;
int parameterMode = 0;
int isMethodType = 0;
int isSelf = 0;
int isArgument = 0;
int isString = 0;
int isCondition = 0;

char print_text[1000];
char temp_print_text[1000];
int isPrint = 0;
int printableForms[20];
int countPrintableForms = 0;

int keepParent=-1;
int testCounter = 0;
int i, j;
int labelCounter = 0;
char temp_position[10];
int mainMode = 0;
Object *newObject;
int isConstructor = 0;
char constructor_parameters[200];
char buffer[150];

char argumentTypes[20][50];
int countArgumentTypes = 0;

int findSelfField(char *str) {
	if(strchr(str, '[')){
		while(str[strlen(str)-1] != '[') {
			str[strlen(str)-1] = 0;
		}
		str[strlen(str)-1] = 0;
	}
	for(i=0; i<10; i++) {
		if(strcmp(newClass->fields[i].name, str) == 0) {
			return i;
		}
	}
	printf("ERROR FIND SELF FIELD !!");
	exit(1);
}

int findClass(char *name) {
	class_node *temp = class_head;
	i=0;
	while(temp) {
		if(strcmp(temp->value.name, name)==0) {
			return i;
		}
		i++;
		temp = temp->next;
	}
}

char *typeOfCallerVariable(char *variable) {
	if(strchr(variable, '[')){
		while(variable[strlen(variable)-1] != '[') {
			variable[strlen(variable)-1] = 0;
		}
		variable[strlen(variable)-1] = 0;
	}
	for(i=0; i<countParameters; i++) {
		if(strcmp(variable, newMethod->parameters[i].name) == 0) {
			return newMethod->parameters[i].type;
		}
	}

	for(i=0; i<newClass->fieldsSize; i++) {
		if(strcmp(variable, newClass->fields[i].name) == 0) {
			return newClass->fields[i].type;
		}
	}

	printf("\nID %s is not a variable nor a parent Class!!!!\n", variable);
	exit(1);
}

int checkParentClass(char * classType) {
	for(i=0; i<newClass->countParents; i++) {
		if(strcmp(classType, newClass->parents[i].name) == 0) {
			return 1;
		}
	}
	return 0;
}

void checkIfParentExists(char *name) {
	int found = 0;
	class_node *temp = class_head;
	while(temp) {
		if(strcmp(temp->value.name, name)==0) {
			found = 1;
		}
		temp = temp->next;
	}
	if(found == 0) {
		printf("\nERROR: Class '%s' was not declared!\n", name);
		exit(1);
	}
}

void createParent(char *name) {

	class_node *temp = class_head;

	while(temp) {
		if(strcmp(temp->value.name, name)==0) {
			newClass->parents[newClass->countParents] = temp->value;
		}
		temp = temp->next;
	}

	newClass->countParents++;
}

int findParent(char *name) {
	for(i=0; i<newClass->countParents; i++) {
		if(strcmp(name, newClass->parents[i].name) == 0) {
			return i;
		}
	}
	printf("\nERROR!!, cannot find this parent!\n");
	exit(1);
}

void registerGrandparents(char *parentName) {
	class_node *temp = class_head;
	while(temp) {
		if(strcmp(parentName, temp->value.name) == 0) {
			for(i=0; i<temp->value.countParents; i++) {
				newClass->parents[newClass->countParents] = temp->value.parents[i];
				newClass->countParents++;
			}
		}
		temp = temp->next;
	}
}

char *findObject(char *name) {
	object_node *temp = object_head;
	strcpy(buffer, "object_head");
	while(temp) {
		if(strcmp(temp->value.name, name) == 0) {
			return buffer;
		}
		strcat(buffer, "->next");
		temp = temp->next;
	}
	printf("\nERROR: there is no object with name: %s\n", name);
	exit(1);
}

int searchObject(char *name) {
	object_node *temp = object_head;

	while(temp) {
		if(strcmp(temp->value.name, name) == 0) {
			return 1;
		}
		temp = temp->next;
	}
	return 0;
}

char *searchObjectClass(char *name) {
	object_node *temp = object_head;
	while(temp) {
		if(strcmp(temp->value.name, name) == 0) {
			return temp->value.object_class->name;
		}
		temp = temp->next;
	}
	printf("\nERROR, cannot find class name!\n");
	exit(1);
}

void makeObjectClassCopy(char *name) {
	class_node *temp = class_head;
	while(temp) {
		if(strcmp(temp->value.name, name) == 0) {
			strcpy(newObject->object_class->name, name);
			for(int i=0; i<temp->value.fieldsSize; i++) {
				newObject->object_class->fields[i] = temp->value.fields[i];
			}
			newObject->object_class->fieldsSize = temp->value.fieldsSize;

			for(int i=0; i<temp->value.countParents; i++) {
				newObject->object_class->fields[i] = temp->value.fields[i];
			}
			newObject->object_class->countParents = temp->value.countParents;
			return;
		}
		temp = temp->next;
	}
	printf("\nERROR I cannot copy this class!\n");
	exit(1);
}

/*int isNumber(char *input) {
    for (i = 0; input[i] != '\0'; i++)
        if (isalpha(input[i]))
            return 0;
    return 1;
}*/

char *searchArgType(char *arg) {
	for(i=0; i<countParameters; i++) {
		if(strcmp(newMethod->parameters[i].name, arg) == 0) {
			return newMethod->parameters[i].type;
		}
	}
	return "int";
}

char *printParamTypes() {
	buffer[0] = '\0';
	strcpy(buffer, "");
	for(i=0; i<countArgumentTypes; i++) {
		strcat(buffer, "$");
		strcat(buffer, argumentTypes[i]);
	}
	return buffer;
}

void printfFunctionsToFile() {
}

void printClassesToFile() {
	class_node *temp = class_head;

	fprintf(c_file, "printClassesToFile() {\n");
	strcpy(buffer, "class_head");
	while(temp) {
		fprintf(c_file, "\t%s = (Class *) malloc(sizeof(Class));\n", buffer);
		fprintf(c_file, "\tif(%s == NULL) { printf(\"\\n class malloc problem!!\\n\"); exit(1); }\n", buffer);

		fprintf(c_file, "\t%s->value.name = \"%s\";\n", buffer, temp->value.name);
		for(i=0; i<temp->value.fieldsSize; i++) {
			fprintf(c_file, "\t%s->value.fields[%d] = (Field *) malloc(sizeof(Field));\n", buffer, i);
			fprintf(c_file, "\tif(%s->value.fields[%d] == NULL) { printf(\"\\n field malloc problem!!\\n\"); exit(1); }\n", buffer, i);

			fprintf(c_file, "\t%s->value.fields[%d].type = \"%s\";\n", buffer, i, temp->value.fields[i].type);
			fprintf(c_file, "\t%s->value.fields[%d].name = \"%s\";\n", buffer, i, temp->value.fields[i].name);
			fprintf(c_file, "\t%s->value.fields[%d].isArray = %d;\n", buffer, i, temp->value.fields[i].isArray);
			if(temp->value.fields[i].isArray == 0) {
				if(temp->value.fields[i].type == "int") {
					fprintf(c_file, "\t%s->value.fields[%d].int_value = (int *) malloc(sizeof(int));\n", buffer, i);
					fprintf(c_file, "\tif(%s->value.fields[%d].int_value == NULL) { printf(\"\\n int_value malloc problem!!\\n\"); exit(1); }\n", buffer, i);
				}
				else {
					fprintf(c_file, "\t%s->value.fields[%d].object_value = (Class *) malloc(sizeof(Class));\n", buffer, i);
					fprintf(c_file, "\tif(%s->value.fields[%d].object_value == NULL) { printf(\"\\n object_value malloc problem!!\\n\"); exit(1); }\n", buffer, i);
				}
			}
			else {
				if(temp->value.fields[i].type == "int") {
					fprintf(c_file, "\t%s->value.fields[%d].int_value = (int *) malloc(%d*sizeof(int));\n", buffer, i, temp->value.fields[i].isArray);
					fprintf(c_file, "\tif(%s->value.fields[%d].int_value == NULL) { printf(\"\\n int_value malloc problem!!\\n\"); exit(1); }\n", buffer, i);
				}
				else {
					fprintf(c_file, "\t%s->value.fields[%d].object_value = (Class *) malloc(%d*sizeof(Class));\n", buffer, i, temp->value.fields[i].isArray);
					fprintf(c_file, "\tif(%s->value.fields[%d].object_value == NULL) { printf(\"\\n object_value malloc problem!!\\n\"); exit(1); }\n", buffer, i);
				}
			}
		}
		fprintf(c_file, "\t%s->value.fieldsSize = %d;\n", buffer, temp->value.fieldsSize);

		if(temp->value.countParents>0) {
			fprintf(c_file, "\t%s->value.parents = (Class *) malloc(10*sizeof(Class));\n", buffer);
			fprintf(c_file, "\tif(%s->value.parents == NULL) { printf(\"\\n class//parents malloc problem!!\\n\"); exit(1); }\n", buffer);

			fprintf(c_file, "\tclass_node *temp;\n");
			for(i=0; i<temp->value.countParents; i++) {
				fprintf(c_file, "\ttemp = class_head;\n");
				fprintf(c_file, "\twhile(temp) {\n");
				fprintf(c_file, "\t\tif(strcmp(temp->value.name, \"%s\")==0)\n", temp->value.parents[i].name);
				fprintf(c_file, "\t\t\t%s->value.parents[%d] = temp->value;\n", buffer, i);
				fprintf(c_file, "\t\ttemp = temp->next;\n");
				fprintf(c_file, "\t}\n");
			}
		}

		fprintf(c_file, "\t%s->value.countParents = %d;\n", buffer, temp->value.countParents);

		strcat(buffer, "->next");
		temp = temp->next;
	}
	fprintf(c_file, "}\n");
}

%}

%union {
	char strV[10000];
	int intV;
};

%token <strV>IDENTIFIER <strV>CONSTANT STRING_LITERAL VOID
%token INC_OP DEC_OP LE_OP GE_OP EQ_OP NE_OP AND_OP OR_OP
%token MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN SUB_ASSIGN
%token <strV>INT
%token CASE DEFAULT IF ELSE SWITCH WHILE DO FOR CONTINUE BREAK RETURN
%token DEF CLASS ABSTRACT INHERITS PRINT

%type<strV> type_specifier argument_expression_list compound_statement statement no_semicolon_statement one_or_more_statements
%type<strV> signed_constant primary_expression postfix_expression statement_list declaration_list declaration
%type<strV> multiplicative_expression additive_expression direct_declarator initializer constant_expression iteration_statement
%type<strV> relational_expression equality_expression logical_and_expression logical_or_expression assignment_expression
%type<strV> jump_statement print_statement selection_statement parameter_declaration print_elements initializer_list
%type<strV> else_follow elseif_follow

%start translation_unit
%%

signed_constant
  : CONSTANT																																			  	{ strcpy($$, $1); }
  | '+' CONSTANT																																			{ strcpy($$, $2); }
  | '-' CONSTANT																																			{ strcpy($$, strcat("-", $2)); }
  ;

primary_expression
  : IDENTIFIER																																				{ strcpy($$, $1);	}
	| signed_constant																																		{ strcpy($$, $1); }
  | STRING_LITERAL																																		{ isString = 1;
																																												if(isPrint==1) {
																																													strcpy($$,"\"");
																																													strcat($$,$1);
																																													strcat($$,"\"");
																																												}
																																											}
  | '(' assignment_expression ')'																										  { strcpy($$, $2); }
	;

postfix_expression
	: primary_expression																																{ strcpy($$,$1); }
	| postfix_expression '[' assignment_expression ']'																							{ snprintf($$, sizeof($$), "%s[%s]", $1, $3); }
	| postfix_expression '(' ')'																												{ if(mainMode == 1) {
																																													if(findClass($1) > 0) {
																																														strcpy(constructor_parameters, "");
																																													}
																																													snprintf($$, sizeof($$), "%s", $1);
																																												}
																																												else {
																																													if(isSelf>0) {
																																														if(isPrint==1 || isArgument==1 || isCondition==1) {
																																															snprintf($$, sizeof($$), "%s$%s(%s)", $1, newClass->name, newMethod->parameters[0].name);
																																														}
																																														else {
																																															snprintf($$, sizeof($$), "%s$%s(%s);", $1, newClass->name, newMethod->parameters[0].name);
																																														}
																																														isSelf--;
																																													}
																																													else {
																																														if(keepParent>=0) {
																																															if(isPrint==1 || isArgument==1 || isCondition==1) {
																																																snprintf($$, sizeof($$), "%s(self->parents[%d])", $1, keepParent);
																																															}
																																															else {
																																																snprintf($$, sizeof($$), "%s(self->parents[%d]);", $1, keepParent);
																																															}
																																														}
																																														else {
																																															if(isPrint==1 || isArgument==1 || isCondition==1) {
																																																snprintf($$, sizeof($$), "%s(%s)", $1, newMethod->parameters[0].name);
																																															}
																																															else {
																																																snprintf($$, sizeof($$), "%s(%s);", $1, newMethod->parameters[0].name);
																																															}
																																														}
																																													}
																																												}
																																											}
	| postfix_expression '(' { isArgument = 1; countArgumentTypes = 0; } argument_expression_list {isArgument = 0;} ')'
																																											{ if(mainMode == 1) {
																																													if(findClass($1) > 0) {
																																														strcpy(constructor_parameters, $4);
																																													}
																																													printf("HEREEEEEEEEEEEEEE%s", printParamTypes());
																																													snprintf($$, sizeof($$), "%s", $1);
																																												}
																																												else {
																																													if(isSelf>0) {
																																														if(isPrint==1 || isArgument==1 || isCondition==1) {
																																															snprintf($$, sizeof($$), "%s$%s%s(%s, %s)", $1, newClass->name, printParamTypes(), newMethod->parameters[0].name, $4);
																																														}
																																														else {
																																															snprintf($$, sizeof($$), "%s$%s%s(%s, %s);", $1, newClass->name, printParamTypes(), newMethod->parameters[0].name, $4);
																																														}
																																														isSelf--;
																																													}
																																													else {
																																														if(isPrint==1 || isArgument==1 || isCondition==1) {
																																															snprintf($$, sizeof($$), "%s(self->parents[%d], %s)", $1, keepParent, $4);
																																														}
																																														else {
																																															snprintf($$, sizeof($$), "%s(self->parents[%d], %s);", $1, keepParent, $4);
																																														}
																																												}
																																											}
																																										}
	| postfix_expression { if(checkParentClass($1) == 1)
		keepParent = findParent($1); if(strcmp($1,"self") == 0) isSelf++; } '.' postfix_expression
																																				{ if (mainMode == 1) {
																																						if(strcmp(constructor_parameters,"") == 0) {
																																							snprintf($$, sizeof($$), "\t%s$%s(%s);",searchObjectClass($1), $4, $1);
																																						}
																																						else {
																																							snprintf($$, sizeof($$), "\t%s$%s(%s,%s);",searchObjectClass($1), $4, $1,constructor_parameters);
																																						}
																																					}
																																					else {
																																						if(isSelf>0) {
																																							if(strchr($4, '(')) {
																																								if(strchr($4, '$')){
																																									strcpy($$, $4);
																																								}
																																								else {
																																									if(isPrint==1 || isArgument==1 || isCondition==1) {
																																										snprintf($$, sizeof($$), "%s$%s", newClass->name, $4);
																																									}
																																									else {
																																										snprintf($$, sizeof($$), "\n\t%s$%s", newClass->name, $4);
																																									}
																																								}
																																							}
																																							else {
																																								if(strchr($4, '[')) {
																																									strcpy(temp_position, $4);
																																									while(temp_position[0] != '[') {
																																										memmove(&temp_position[0], &temp_position[0 + 1], strlen(temp_position) - 0);
																																									}
																																									snprintf($$, sizeof($$), "%s->fields[%d]%s", $1, findSelfField($4),temp_position);
																																								}
																																								else {
																																									snprintf($$, sizeof($$), "%s->fields[%d]", $1, findSelfField($4));
																																								}
																																								isSelf--;
																																							}
																																						}
																																						else if(isString==0){
																																							if(searchObject($1) > 0) {
																																								snprintf($$, sizeof($$), " %s$%s", searchObjectClass($1), $4);
																																							}
																																							else {
																																								if(checkParentClass($1) == 1) {
																																									if(isPrint==1 || isArgument==1 || isCondition==1) {
																																										snprintf($$, sizeof($$), " %s$%s", $1, $4);
																																									}
																																									else {
																																										snprintf($$, sizeof($$), "\n\t%s$%s%s%s%s", $1, "__init__", $1, strstr(printParamTypes() + 1, "$"), strstr($4, "("));
																																									}
																																									keepParent = -1;
																																								}
																																								else {
																																									if(isPrint==1 || isArgument==1 || isCondition==1) {
																																										snprintf($$, sizeof($$), "%s$%s", typeOfCallerVariable($1), $4);
																																									}
																																									else {
																																										snprintf($$, sizeof($$), "\n\t%s$%s", typeOfCallerVariable($1), $4);
																																									}
																																								}
																																							}
																																						}
																																					}
																																				}
	| postfix_expression INC_OP
	| postfix_expression DEC_OP
	;

argument_expression_list
	: assignment_expression																					{ strcpy($$, $1);
																																		strcpy(argumentTypes[countArgumentTypes], searchArgType($1));
																																		countArgumentTypes++;
																																	}
	| argument_expression_list ',' assignment_expression						{ if(keepParent>=0 && strcmp($1, "self")==0) {
																																			strcpy($$, $3);
																																		}
																																		else {
																																			snprintf($$, sizeof($$), "%s, %s", $1, $3);
																																		}
																																		strcpy(argumentTypes[countArgumentTypes], searchArgType($3));
																																		countArgumentTypes++;
																																	}
	;

multiplicative_expression
	: postfix_expression																						{ strcpy($$, $1); }
	| multiplicative_expression '*' postfix_expression							{ snprintf($$, sizeof($$), "%s * %s", $1, $3); }
	| multiplicative_expression '/' postfix_expression							{ snprintf($$, sizeof($$), "%s / %s", $1, $3); }
	| multiplicative_expression '%' postfix_expression							{ snprintf($$, sizeof($$), "%s %% %s", $1, $3); }
	;

additive_expression
	: multiplicative_expression																			{ strcpy($$, $1); }
	| additive_expression '+' multiplicative_expression							{ snprintf($$, sizeof($$), "%s + %s", $1, $3); }
	| additive_expression '-' multiplicative_expression							{ snprintf($$, sizeof($$), "%s - %s", $1, $3); }
	;

relational_expression
	: additive_expression																						{ strcpy($$, $1); }
	| relational_expression '<' additive_expression									{ snprintf($$, sizeof($$), "%s < %s", $1, $3); }
	| relational_expression '>' additive_expression									{ snprintf($$, sizeof($$), "%s > %s", $1, $3); }
	| relational_expression LE_OP additive_expression								{ snprintf($$, sizeof($$), "%s <= %s", $1, $3); }
	| relational_expression GE_OP additive_expression								{ snprintf($$, sizeof($$), "%s >= %s", $1, $3); }
	;

equality_expression
	: relational_expression																					{ strcpy($$, $1); }
	| equality_expression EQ_OP relational_expression								{ snprintf($$, sizeof($$), "%s == %s", $1, $3); }
	| equality_expression NE_OP relational_expression								{ snprintf($$, sizeof($$), "%s != %s", $1, $3); }
	;

logical_and_expression
	: equality_expression																						{ strcpy($$, $1); }
	| logical_and_expression AND_OP equality_expression							{ snprintf($$, sizeof($$), "%s && %s", $1, $3); }
	;

logical_or_expression
	: logical_and_expression																				{ strcpy($$, $1); }
	| logical_or_expression OR_OP logical_and_expression						{ snprintf($$, sizeof($$), "%s || %s", $1, $3); }
	;

assignment_expression
	: logical_or_expression																					{ strcpy($$, $1); }
	| postfix_expression '=' assignment_expression									{ if(isConstructor == 1) {
																																			snprintf($$, sizeof($$), "\n\t%s$__init__(%s, %s);\n", searchObjectClass($1), findObject($1), constructor_parameters);
																																			constructor_parameters[0]='\0';
																																		}
																																		else {
																																			snprintf($$, sizeof($$), "\n\t%s = %s;", $1, $3);
																																		}
																																	}
	| postfix_expression ADD_ASSIGN assignment_expression						{ snprintf($$, sizeof($$), "\n\t%s += %s;", $1, $3); }
	| postfix_expression SUB_ASSIGN assignment_expression						{ snprintf($$, sizeof($$), "\n\t%s -= %s;", $1, $3); }
	;

constant_expression
	: logical_or_expression																					{ strcpy($$, $1); }
	;

declaration
	: type_specifier init_declarator ';'
	;

init_declarator
	: direct_declarator																									{ if(mainMode==1) {
																																					strcpy(newObject->name, $1);
																																					object_addNode(*newObject);
																																					newObject = (Object *) malloc (sizeof(Object));
																																					if(newObject==NULL) exit(1);
																																					newObject->object_class = (Class *) malloc (sizeof(Class));
																																					if(newObject->object_class==NULL) exit(1);
																																					newObject->object_class->fieldsSize = 0;
																																					newObject->object_class->countParents = 0;
																																				}
																																				else {
																																					if(strcmp(newField->type, "int") == 0) {
																																						if(newField->isArray == 0) {
																																							newField->int_value = (int*) malloc (sizeof(int));
																																							if(newField->int_value==NULL) {
																																								puts("ERROR malloc1");
																																								exit(1);
																																							}
																																						}
																																						else {
																																							newField->int_value = (int*) malloc (atoi($1)*sizeof(int));
																																							if(newField->int_value==NULL) {
																																								puts("ERROR malloc2");
																																								exit(1);
																																							}
																																						}
																																					}
																																					else {
																																						if(newField->isArray == 0) {
																																							newField->object_value = (Class*) malloc (sizeof(Class));
																																							if(newField->object_value==NULL) {
																																								puts("ERROR malloc1");
																																								exit(1);
																																							}
																																						}
																																						else {
																																							newField->object_value = (Class*) malloc (atoi($1)*sizeof(Class));
																																							if(newField->object_value==NULL) {
																																								puts("ERROR malloc2");
																																								exit(1);
																																							}
																																						}
																																					}
																																					newClass->fields[newClass->fieldsSize] = *newField;
																																					newClass->fieldsSize++;
																																					newField = (Field *) malloc (sizeof(Field));
																																					if(newField==NULL) {
																																						printf("\nError! newField malloc.\n");
																																						exit(1);
																																					}
																																					newField->isArray = 0;
																																					isField = 0;
																																				}
																																			}

	| direct_declarator '=' initializer																	{ if(strcmp(newField->type, "int") == 0) {
																																					if(newField->isArray == 0) {
																																						newField->int_value = (int*) malloc (sizeof(int));
																																						if(newField->int_value==NULL) {
																																							puts("ERROR malloc3");
																																							exit(1);
																																						}
																																						newField->int_value[0] = atoi($3);
																																					}
																																					else {
																																						newField->int_value = (int*) malloc (atoi($1)*sizeof(int));
																																						if(newField->int_value==NULL) {
																																							puts("ERROR malloc4");
																																							exit(1);
																																						}
																																						for(i=0; i<atoi($1); i++)
																																							newField->int_value[i] = initializers[i];
																																						free(initializers);
																																						initializers = (int *) malloc (50*sizeof(int));
																																						if(initializers==NULL) {
																																							puts("ERROR malloc5");
																																							exit(1);
																																						}
																																						initializersSize = 0;
																																					}
																																				}
																																				else {
																																					printf("\nError: You cannot initialize Object in fields declaration!\n");
																																					exit(1);
																																				}
																																				newClass->fields[newClass->fieldsSize] = *newField;
																																				newClass->fieldsSize++;
																																				newField = (Field *) malloc (sizeof(Field));
																																				if(newField==NULL) {
																																					printf("\nError! newField2 malloc.\n");
																																					exit(1);
																																				}
																																				newField->isArray = 0;
																																				isField = 0;
																																			}
	;

type_specifier
	: INT																																{ if(isMethodBody == 0) {
																																					strcpy(newField->type, "int");
																																					if (parameterMode == 0) {
																																						isField = 1;
																																					}
																																					else {
																																						if(isMethodType == 1) {
																																							strcpy(newMethod->return_type, "int");
																																							isMethodType=0;
																																						}
																																						else {
																																							strcpy(newParameter->type, "int");
																																						}
																																					}
																																				}
	 																																		}
	| IDENTIFIER																												{ if(mainMode==1){
																																					makeObjectClassCopy($1);
																																				}
																																				else {
																																					if(isMethodBody == 0) {
																																						strcpy(newField->type, $1);
																																						if (parameterMode == 0) {
																																							isField = 1;
																																						}
																																						else {
																																							if(isMethodType == 1) {
																																								strcpy(newMethod->return_type, $1);
																																								isMethodType=0;
																																							}
																																							else {
																																								strcpy(newParameter->type, $1);
																																							}
																																						}
																																					}
																																				}
																																			}
	| VOID																															{ if(isMethodType == 1) {
																																					strcpy(newMethod->return_type, "void");
																																					isMethodType=0;
																																				}
																																				else {
																																					printf("ERROR, void is not a valid type!");
																																					exit(1);
																																				}
																																			}
	;

direct_declarator
	: IDENTIFIER																												{ if(strcmp($$, "__main__")==0) {
																																					mainMode = 1;
																																					isConstructor = 1;
																																				}
																																				else {
																																					if(isField==1) {
																																						strcpy(newField->name, $1);
																																					}
																																					else {
																																						isParameter = 1;
																																					}
																																				}
																																			}
	| direct_declarator '[' constant_expression ']'											{ newField->isArray = atoi($3);
																																				strcpy($$,$3);
																																			}
	| direct_declarator '(' parameter_list ')'													{ isParameter = 0;
																																				isMethodBody = 1;
																																			}
	| direct_declarator '(' identifier_list ')'
	| direct_declarator '(' ')'																					{ isParameter = 0;
																																				isMethodBody = 1;
																																			}
	;

parameter_list
	: parameter_declaration
	| parameter_list ',' parameter_declaration
	;

parameter_declaration
	: type_specifier direct_declarator																	{ strcpy(newParameter->name, $2);
																																				newMethod->parameters[countParameters] = *newParameter;
																																				newParameter = (Parameter *) malloc (sizeof(Parameter));
																																				if(newParameter==NULL) {
																																					printf("\nError! newParameter malloc.\n");
																																					exit(1);
																																				}
																																				countParameters++;
																																			}
	;

identifier_list
	: IDENTIFIER
	| identifier_list ',' IDENTIFIER
	;

initializer
	: assignment_expression																						{ strcpy($$, $1); }
	| '{' initializer_list '}'
	;

initializer_list
	: initializer																											{ initializers[initializersSize] = atoi($1);
																																			initializersSize++;
																																		}
	| initializer_list ',' initializer																{ initializers[initializersSize] = atoi($3);
																																			initializersSize++;
																																		}
	;

statement
	: assignment_expression																						{ strcpy($$, $1); }
	| jump_statement																									{ strcpy($$, $1); }
	| print_statement																									{ strcpy($$, $1); }
	;

no_semicolon_statement
	: selection_statement																							{ strcpy($$, $1); }
	| iteration_statement																							{ strcpy($$, $1); }
	;

compound_statement
	: '{' '}'																												{ strcpy($$, ""); }
	| '{' statement_list '}'																				{ strcpy($$, $2); }
	| '{' declaration_list '}'																			{ strcpy($$, $2); }
	| '{' declaration_list statement_list '}'												{ if(mainMode==1) {
																																			strcpy($$, $3);
																																		}
																																		else {
																																			snprintf($$, sizeof($$), "%s\n\n%s", $2, $3);
																																		}
																																	}
	;

declaration_list
	: declaration
	| declaration_list declaration
	;

statement_list
	: statement                       																		{ strcpy($$, $1); }
	| no_semicolon_statement																							{ strcpy($$, $1); }
	| statement_list ';' statement 					                              { snprintf($$, sizeof($$), "%s %s", $1, $3); }
	| statement_list no_semicolon_statement																{ snprintf($$, sizeof($$), "%s %s", $1, $2); }
	;

selection_statement
	: { isCondition =1; } IF assignment_expression {isCondition=0; } one_or_more_statements												{ snprintf($$, sizeof($$), "\n\tif(%s)\t%s", $3, $5); }
	| IF { isCondition =1; } assignment_expression {isCondition=0; } one_or_more_statements elseif_follow					{ snprintf($$, sizeof($$), "\n\tif(%s)\t%s%s", $3, $5, $6); }
	;

elseif_follow
	: { isCondition =1; } ELSE IF assignment_expression { isCondition=0; } one_or_more_statements										{ snprintf($$, sizeof($$), "\n\telse if(%s)\t%s", $4, $6); }
	| { isCondition =1; } ELSE IF assignment_expression { isCondition=0; } one_or_more_statements elseif_follow			{ snprintf($$, sizeof($$), "\n\telse if(%s)\t%s\n%s", $4, $6, $7); }
	| ELSE IF{ isCondition =1; } assignment_expression { isCondition=0; } one_or_more_statements else_follow			  	{ snprintf($$, sizeof($$), "\n\telse if(%s)\t%s\t%s", $4, $6, $7); }
	;

else_follow
	: ELSE one_or_more_statements																										                    	{ snprintf($$, sizeof($$), "\n\telse\t%s", $2); }
	;


iteration_statement
	: WHILE assignment_expression one_or_more_statements																															{ snprintf($$, sizeof($$), "\n\twhile(%s)\n%s\n", $2, $3); }
	| DO one_or_more_statements WHILE assignment_expression																													{ snprintf($$, sizeof($$), "\n\tdo\n%s\nwhile(%s)", $2, $4); }
	| FOR assignment_expression ';' assignment_expression ';' assignment_expression one_or_more_statements																	{ snprintf($$, sizeof($$), "\n\tfor(%s;%s;%s)\n%s\n", $2, $4, $6, $7); }
	;

jump_statement
	: CONTINUE																											{ strcpy($$, "\n\tcontinue;"); }
	| BREAK																													{ strcpy($$, "\n\tbreak;"); }
	| RETURN																												{ strcpy($$, "\n\treturn;"); }
	| RETURN postfix_expression																			{ snprintf($$, sizeof($$), "\n\treturn %s;", $2); }
	;

print_elements
	: postfix_expression																		 				{ snprintf(temp_print_text, sizeof(temp_print_text), ", %s", $1);
																																		strcat(print_text, temp_print_text);
																																		temp_print_text[0] = '\0';
																																		if (isString == 1) {
																																			printableForms[countPrintableForms] = 1;
																																			countPrintableForms++;
																																			isString = 0;
																																		}
																																		else {
																																			printableForms[countPrintableForms] = 0;
																																			countPrintableForms++;
																																		}
																																	}
	| print_elements ',' postfix_expression													{ snprintf(temp_print_text, sizeof(temp_print_text), ", %s", $3);
																																		strcat(print_text, temp_print_text);
																																		temp_print_text[0] = '\0';
																																		if (isString == 1) {
																																			printableForms[countPrintableForms] = 1;
																																			countPrintableForms++;
																																			isString = 0;
																																		}
																																		else {
																																			printableForms[countPrintableForms] = 0;
																																			countPrintableForms++;
																																		}
																																	}
	;

print_statement
	: PRINT { isPrint = 1; } print_elements														{ strcpy($$, "\n\tprintf(\"");
																																		for(i=0; i<countPrintableForms; i++) {
																																			if(printableForms[i]==1)
																																				strcat($$, "%s");
																																			else
																																				strcat($$, "%d");

																																			if(i!=countPrintableForms-1)
																																				strcat($$, ", ");
																																		}
																																		strcat($$, "\" ");
																																		strcat($$, print_text);
																																		strcat($$, ");");
																																		countPrintableForms = 0;
																																		isPrint = 0;
																																		print_text[0] = '\0';
																																	}
	;

one_or_more_statements
		: statement ';'																								{ snprintf($$, sizeof($$), "%s", $1); }
		| compound_statement																					{ strcat($$, $1); }
		;

translation_unit
	: class_or_main
	| translation_unit class_or_main
	;

class_or_main
	: function_definition
	| CLASS class_definition																				{ class_addNode(*newClass);
																																		newClass = (Class *) malloc (sizeof(Class));
																																		if(newClass==NULL) {
																																			printf("\nError! newClass malloc.\n");
																																			exit(1);
																																		}
																																		newClass->fieldsSize = 0;
																																		newClass->countParents = 0;
																																	}
	;

function_definition
	: DEF { isMethodType=1; } type_specifier direct_declarator ':' compound_statement			{ isMethodBody = 0;
																																													if(mainMode == 1) {
																																														fprintf(c_file, "void printClassesToFile();\n\n");
																																														fprintf(c_file, "int main() {\n");
																																														fprintf(c_file, "\tprintClassesToFile();\n");
																																														fprintf(c_file, "%s\n", $6);
																																														fprintf(c_file, "}\n\n");

																																														printClassesToFile();
																																													}
																																													else {
																																														fprintf(c_file, "%s ", newMethod->return_type);
																																														fprintf(c_file, "%s$", newMethod->parameters[0].type);
																																														fprintf(c_file, "%s", $4);
																																														for(i=0; i<countParameters; i++) {
																																																fprintf(c_file, "$%s", newMethod->parameters[i].type);
																																														}
																																														fprintf(c_file, "(");
																																														for(i=0; i<countParameters; i++) {
																																															fprintf(c_file, "%s %s", newMethod->parameters[i].type, newMethod->parameters[i].name);
																																															if(i!=countParameters-1) {
																																																fprintf(c_file, ", ");
																																															}
																																														}
																																														fprintf(c_file, ") {");
																																														fprintf(c_file, "%s\n", $6);
																																														fprintf(c_file, "}\n\n");
																																													}


																																													newMethod = (Method *) malloc (sizeof(Method));
																																													if(newMethod==NULL) {
																																														printf("\nError! newMethod malloc.\n");
																																														exit(1);
																																													}
																																													countParameters = 0;
																																													newMethod->isAbstract = 0;
																																												}
	| DEF { isMethodType=1; }	ABSTRACT type_specifier direct_declarator										{ newMethod->isAbstract = 1; }
	;

class_definition
	: IDENTIFIER {strcpy(newClass->name, $1);} ':' class_compound_statement
	| IDENTIFIER {strcpy(newClass->name, $1); } INHERITS parents_list ':' class_compound_statement
	;

parents_list
	: IDENTIFIER																										{ newClass->parents = (Class *) malloc(10*sizeof(Class));
																																		if(newClass->parents == NULL) {
																																			printf("\nError! newClass->parents malloc.\n");
																																			exit(1);
																																		}
																																		checkIfParentExists($1);
																																		createParent($1);
																																		registerGrandparents($1);
																																	}
	| parents_list ',' IDENTIFIER																		{ checkIfParentExists($3);
																																		createParent($3);
																																		registerGrandparents($3);
																																	}

class_compound_statement
	: '{' class_compound_statement_content '}'
	;

class_compound_statement_content
	:
	| declaration_list { parameterMode = 1;} functions_list					{ parameterMode = 0; }
	| declaration_list
	| { parameterMode = 1;} functions_list 													{ parameterMode = 0; }
	;

functions_list
	: function_definition
	| functions_list function_definition
	;


%%

int yyerror(s)
char *s;
{
	fflush(stdout);
	printf("\n%*s\n%*s\n", column, "^", column, s);
}

int main(int argc, char *argv[]) {

	if(argc!=2) {
		printf("Error: wrong type of file!\n");
		exit(1);
	}

	newClass = (Class *) malloc (sizeof(Class));
	if(newClass==NULL) exit(1);
	newClass->fieldsSize = 0;
	newClass->countParents = 0;

	newField = (Field *) malloc (sizeof(Field));
	if(newField==NULL) exit(1);
	newField->isArray = 0;

	newMethod = (Method *) malloc (sizeof(Method));
	if(newMethod==NULL) exit(1);
	newMethod->isAbstract = 0;

	newParameter = (Parameter *) malloc (sizeof(Parameter));
	if(newParameter==NULL) exit(1);

	initializers = (int *) malloc (50*sizeof(int));
	if(initializers==NULL) exit(1);

	newObject = (Object *) malloc (sizeof(Object));
	if(newObject==NULL) exit(1);
	newObject->object_class = (Class *) malloc (sizeof(Class));
	if(newObject->object_class==NULL) exit(1);
	newObject->object_class->fieldsSize = 0;
	newObject->object_class->countParents = 0;

	yyin = fopen(argv[1], "r");
	argv[1][strlen(argv[1])-2] = '\0';
	c_file = fopen(strcat(argv[1], "c"), "w");
	fprintf(c_file, "#include <stdio.h>\n");
	fprintf(c_file, "#include <stdlib.h>\n");
	fprintf(c_file, "#include <string.h>\n");
	fprintf(c_file, "\n");
	fprintf(c_file, "typedef struct field_s Field;\n");
	fprintf(c_file, "typedef struct class_s Class;\n");
	fprintf(c_file, "\n");
	fprintf(c_file, "struct field_s {\n");
	fprintf(c_file, "	char type[20];\n");
	fprintf(c_file, "	char name[40];\n");
	fprintf(c_file, "	int isArray;\n");
	fprintf(c_file, "int *int_value;\n");
	fprintf(c_file, "	Class *object_value;\n");
	fprintf(c_file, "};\n");
	fprintf(c_file, "\n");
	fprintf(c_file, "typedef struct {\n");
	fprintf(c_file, "	char type[20];\n");
	fprintf(c_file, "	char name[40];\n");
	fprintf(c_file, "} Parameter;\n");
	fprintf(c_file, "\n");
	fprintf(c_file, "typedef struct {\n");
	fprintf(c_file, "	int isAbstract;\n");
	fprintf(c_file, "	Parameter parameters[20];\n");
	fprintf(c_file, "	char methodBody[10000];\n");
	fprintf(c_file, "	char return_type[30];\n");
	fprintf(c_file, "} Method;\n");
	fprintf(c_file, "\n");
	fprintf(c_file, "struct class_s {\n");
	fprintf(c_file, "	char name[40];\n");
	fprintf(c_file, "	Field fields[10];\n");
	fprintf(c_file, "	int fieldsSize;\n");
	fprintf(c_file, "	Class *parents;\n");
	fprintf(c_file, "	int countParents;\n");
	fprintf(c_file, "};\n");
	fprintf(c_file, "\n");
	fprintf(c_file, "typedef struct {\n");
	fprintf(c_file, "	Class *object_class;\n");
	fprintf(c_file, "	char name[40];\n");
	fprintf(c_file, "} Object;\n");
	fprintf(c_file, "\n");
	fprintf(c_file, "typedef struct class_node_s{\n");
	fprintf(c_file, "	Class value;\n");
	fprintf(c_file, "	struct class_node_s *next;\n");
	fprintf(c_file, "} class_node;\n");
	fprintf(c_file, "\n");
	fprintf(c_file, "class_node *class_head = NULL;\n");
	fprintf(c_file, "class_node *class_tail = NULL;\n");
	fprintf(c_file, "\n\n");

	yyparse();
	fclose(yyin);
	fclose(c_file);

}
