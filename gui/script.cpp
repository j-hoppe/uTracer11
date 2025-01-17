/* script - execute Jscript code to control uStepping

 Embedded engine is duktape.org

 Exposes to JScript:
 - statevars[] a,d tests (equal(varname value))
 - commands to enable/disable manual clock
 - comannd to ustep
 
*/

#include <wx/log.h>
#include "script.h"
#include "application.h"


// Functions registered for and callable from "duktape" ECMA script interpreter
// Functions are standalone "C", and work on 
// - single global context "Script::app" for GUI operations (log(), showPage())
// - on Script::app->pdp11Adapter fpr PDP11 control (statevars)

Application* Script::app = nullptr ;

// log(): print argument in event log
static duk_ret_t script__log(duk_context* ctx) {
	const char* info = duk_to_string(ctx, 0);
	wxString line("SCRIPT: ");
	line += info;
	wxLogInfo(line); // with timestamp
	/*
		Script::app->mainFrame->eventsTextCtrl->AppendText("SCRIPT: ");
		Script::app->mainFrame->eventsTextCtrl->AppendText(info);
		Script::app->mainFrame->eventsTextCtrl->AppendText("\n");
	*/
		return 0;  // Number of return values
}



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
	ctx = duk_create_heap_default();
	if (!ctx) {
		wxLogFatalError("Failed to create a Duktape heap.");
		return;
	}

	// Register the C functions in the Duktape context
	duk_push_c_function(ctx, script__log, 1 /*nargs*/);
	duk_put_global_string(ctx, "log");


	// selftest: print first "hello"
	eval("log('Hello from JScript!');");
}

// run Jscript code
void Script::eval(std::string js_code) {

	// Evaluate the JavaScript code
	if (duk_peval_string(ctx, js_code.c_str()) != 0) {
		// display error
		const char* error1 = duk_safe_to_string(ctx, -1);
		wxString error2(error1);
		wxLogError("SCRIPT: " + error2);
	}
}

