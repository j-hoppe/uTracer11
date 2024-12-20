/* getopt2.hpp:  advanced commandline parsing

 Copyright (c) 2016, Joerg Hoppe
 j_hoppe@t-online.de, www.retrocmp.com

 Permission is hereby granted, free of charge, to any person obtaining a
 copy of this software and associated documentation files (the "Software"),
 to deal in the Software without restriction, including without limitation
 the rights to use, copy, modify, merge, publish, distribute, sublicense,
 and/or sell copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 JOERG HOPPE BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


 01-Feb-2016  JH      created
 */

#ifndef _GETOPT_H_
#define _GETOPT_H_

#include <stdio.h>
#include <string>
#include <vector>

#define GETOPT_MAX_OPTION_DESCR	100
#define GETOPT_MAX_OPTION_ARGS	100
//#define GETOPT_MAX_ERROR_LEN	256
//#define GETOPT_MAX_LINELEN	256
//#define GETOPT_MAX_DEFAULT_CMDLINE_LEN	1000
#define	GETOPT_MAX_CMDLINE_TOKEN	(2*GETOPT_MAX_OPTION_ARGS)

// status codes of first()/next()
#define GETOPT_STATUS_OK	1
#define GETOPT_STATUS_EOF	0			// cline / argumentlist processed
#define GETOPT_STATUS_ILLEGALOPTION	-1		// undefined -option
#define GETOPT_STATUS_MINARGCOUNT	-2		// not enough args for -option
#define GETOPT_STATUS_MAXARGCOUNT	-3	// too much args for -option
#define GETOPT_STATUS_ILLEGALARG	-4	// argument name not known
#define GETOPT_STATUS_ARGNOTSET	-5	// optional arg not specified
#define GETOPT_STATUS_ARGFORMATINT	-6	// argument has illegal format for decimal integer
#define GETOPT_STATUS_ARGFORMATHEX	-7	// argument has illegal format for hex integer

// functions for formatted print
class getopt_printer_c {
private:
	std::ostream *stream;
	unsigned linelen; // point at wich to break ;
	std::string curline;
public:
	unsigned indent;
	getopt_printer_c(std::ostream& stream, unsigned linelen, unsigned indent); // why no default constructor here?
	void append(std::string s, bool linebreak);
	void append_multilinestring(std::string text);
	void flush();
};

// static description of one Option
class getopt_option_descr_c {
public:
	bool valid; // the single nonoption_descr my not be set
	std::string short_name; // code of option on command line
	std::string long_name;

	std::vector<std::string> fix_args;  // name list of required arguments
	std::vector<std::string> var_args;  // name list of variable arguments
	//	string	fix_args_name_buff;
	//	string	var_args_name_buff;
	unsigned fix_arg_count; //
	unsigned max_arg_count;

	std::string default_args; // string representation of default arguments

	std::string info;
	std::string example_simple_cline_args;
	std::string example_simple_info;
	std::string example_complex_cline_args;
	std::string example_complex_info;

	std::string syntaxhelp; // calculated like "-option arg1 args [optarg]
};

// global record for parser
class getopt_c {
private:
	getopt_option_descr_c nonoption_descr; // cline argument without "-option"

	std::vector<getopt_option_descr_c> option_descrs;
	getopt_option_descr_c *cur_option; // ref to current parsed option
	std::vector<std::string> cur_option_argval; // ptr to parsed args

	std::string curtoken; // ptr to current cline arg, error context

	int curerror;

	std::string default_cmdline_buff;
	int cline_argcount;  // default cmdline + copy of user commandline
	std::vector<std::string> cline_args;
	int cur_cline_arg_idx; // index of next unprocessed argv[]

private:
	int parse_error(int error);
	int arg_error(getopt_option_descr_c& odesc, int error, std::string& argname, std::string argval);
	int optionargidx(getopt_option_descr_c& odesc, std::string& argname);
	std::string getoptionsyntax(getopt_option_descr_c& odesc, int style);
	void help_option_intern(getopt_option_descr_c& odesc, std::ostream& stream, unsigned linelen,
			unsigned indent);

public:
	bool ignore_case; // case sensitivity
	std::string curerrortext;

	getopt_c();
	~getopt_c();

	void init(bool _ignore_case);

	int stringcmp(std::string s1, std::string s2);

	getopt_option_descr_c *define(std::string short_option_name, std::string long_option_name,
			std::string fix_args_csv, std::string opt_args_csv, std::string default_args, std::string info,
			std::string example_simple_cline, std::string example_simple_info,
			std::string example_complex_cline, std::string example_complex_info);

	bool isoption(std::string name);
	int first(int argc, char **argv);
	int next(void);
	int arg_s(std::string argname, std::string& res);
	int arg_i(std::string argname, int *res);
	int arg_u(std::string argname, unsigned *val);
	int arg_o(std::string argname, int *val);
	int arg_h(std::string argname, int *val);

	void help_commandline(std::ostream& stream, unsigned linelen, unsigned indent);
	void help_option(std::ostream& stream, unsigned llen, unsigned indent);
	void help(std::ostream& stream, unsigned linelen, unsigned indent, std::string commandname);

};

#endif
