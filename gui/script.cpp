/* script - execute Jscript code to control uStepping

 Embedded engine is duktape.org

 Exposes to JScript:
 - statevars[] a,d tests (equal(varname value))
 - commands to enable/disable manual clock
 - comannd to ustep
 
*/

#include <wx/log.h>
#include "script.h"

Application *app = nullptr;


// config Jscript engine

Script::Script() {
	app = nullptr ;
	ctx = nullptr ;
}

Script::~Script() {
    // Clean up and destroy the Duktape context
	if (ctx)
	    duk_destroy_heap(ctx);
	ctx = nullptr ;
	app = nullptr;
}

void Script::init(Application *_app) {
	app = _app ;		
	// Create a Duktape heap and context
	duk_context* ctx = duk_create_heap_default();
	if (!ctx) 
		wxLogError("Failed to create a Duktape heap.");
	return ;
}

