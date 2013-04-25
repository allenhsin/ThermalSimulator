/* A Bison parser, made by GNU Bison 2.7.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2012 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_VERILOG_YACC_VERILOG_HH_INCLUDED
# define YY_VERILOG_YACC_VERILOG_HH_INCLUDED
/* Enabling traces.  */
#ifndef VERILOGDEBUG
# if defined YYDEBUG
#  if YYDEBUG
#   define VERILOGDEBUG 1
#  else
#   define VERILOGDEBUG 0
#  endif
# else /* ! defined YYDEBUG */
#  define VERILOGDEBUG 0
# endif /* ! defined YYDEBUG */
#endif  /* ! defined VERILOGDEBUG */
#if VERILOGDEBUG
extern int verilogdebug;
#endif

/* Tokens.  */
#ifndef VERILOGTOKENTYPE
# define VERILOGTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum verilogtokentype {
     ASSIGN = 258,
     DEFPARAM = 259,
     ENDGENERATE = 260,
     ENDMODULE = 261,
     GENERATE = 262,
     GENVAR = 263,
     INOUT = 264,
     INPUT = 265,
     MODULE = 266,
     OUTPUT = 267,
     PARAMETER = 268,
     WIRE = 269,
     FLOAT = 270,
     IDENTIFIER = 271,
     UNSIGNED_NUMBER = 272,
     BASE = 273
   };
#endif


#if ! defined VERILOGSTYPE && ! defined VERILOGSTYPE_IS_DECLARED
typedef union VERILOGSTYPE
{
/* Line 2058 of yacc.c  */
#line 59 "verilog.y"

	char *sval;
    char *nval;
    char cval;
    
    Thermal::VerilogNumber* num;    
    Thermal::VerilogExpression* expr;
    Thermal::VerilogRange* range;
    Thermal::VerilogVariables* variables;
    Thermal::VerilogConn* conn;
    
    Thermal::VerilogPortType port_type;
    Thermal::VerilogNetType net_type;
    
    Thermal::VerilogExpressions* exprs;
    Thermal::VerilogConns* conns;
    
    Thermal::VerilogModule* module;
    Thermal::VerilogModules* modules;

    Thermal::VerilogItem* item;
    Thermal::VerilogItems* items;

    Thermal::VerilogNet* net;
    Thermal::VerilogNets* nets;
    Thermal::VerilogInstance* instance;
    Thermal::VerilogInstances* instances;
    Thermal::VerilogParameter* parameter;
    Thermal::VerilogParameters* parameters;
    


/* Line 2058 of yacc.c  */
#line 116 "yacc.verilog.hh"
} VERILOGSTYPE;
# define VERILOGSTYPE_IS_TRIVIAL 1
# define verilogstype VERILOGSTYPE /* obsolescent; will be withdrawn */
# define VERILOGSTYPE_IS_DECLARED 1
#endif

extern VERILOGSTYPE veriloglval;

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int verilogparse (void *YYPARSE_PARAM);
#else
int verilogparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int verilogparse (Thermal::VerilogFile* file_);
#else
int verilogparse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_VERILOG_YACC_VERILOG_HH_INCLUDED  */
