#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <uchar.h>
#include <locale.h>

#include <uchar.h>
#include <unicode/utypes.h>
#include <unicode/uchar.h>
#include <unicode/umachine.h>
#include <unicode/ucnv.h>
#include <unicode/ustdio.h>

#include "c-calipto/sexpr.h"
#include "c-calipto/stream.h"
#include "c-calipto/scanner.h"
#include "c-calipto/reader.h"
#include "c-calipto/interpreter.h"

sexpr* system_exit(int32_t arg_count, term* args) {
	return NULL;
}

sexpr* data_cons(int32_t arg_count, term* args) {
	return NULL;
}

sexpr* data_des(int32_t arg_count, term* args) {
	return NULL;
}

sexpr* data_eq(int32_t arg_count, term* args) {
	return NULL;
}

binding builtin_binding(UChar* ns, UChar* n, sexpr* (*f)(int32_t arg_count, term* args)) {
	builtin* b = malloc(sizeof(builtin));
	b->symbol = sexpr_usymbol(u"builtin", n);
	b->apply = f;
	return (binding){ sexpr_usymbol(ns, n), { BUILTIN, b }};
}

bindings* builtin_bindings() {
	binding bindings[] = {
		builtin_binding(u"system", u"exit", *system_exit),
		builtin_binding(u"data", u"cons", *data_cons),
		builtin_binding(u"data", u"des", *data_des),
		builtin_binding(u"data", u"eq", *data_eq),
	};

	size_t c = sizeof(bindings)/sizeof(binding);
	return make_bindings(NULL, c, bindings);
}

int main(int argc, char** argv) {
	setlocale(LC_ALL, "");
	UErrorCode error = 0;
	UConverter* char_conv = ucnv_open(NULL, &error);

	sexpr* args = sexpr_nil();
	for (int i = argc - 1; i >= 0; i--) {
		sexpr* arg = sexpr_string(char_conv, argv[i]);
		sexpr* rest = args;

		args = sexpr_cons(arg, rest);

		sexpr_free(arg);
		sexpr_free(rest);
	}

	sexpr_dump(args);

	UFILE* f = u_fopen("./bootstrap.cal", "r", NULL, NULL);
	stream* st = open_file_stream(f);
	scanner* sc = open_scanner(st);
	reader* r = open_reader(sc);

	sexpr* e = read(r);
	sexpr_dump(e);
	
	bindings* b = builtin_bindings();
	eval(e, b);
	free(b);

	sexpr_free(e);

	close_reader(r);
	close_scanner(sc);
	close_stream(st);
	u_fclose(f);

	sexpr_free(args);

	ucnv_close(char_conv);
	return 0;
}
