// misc tests



// print to event log
function test1() {
    log("Hello, world!") ;
}

// print to event log
function test2() {
    log("Waiting 5 seconds ... you must be able to abort this.") ;
    wait(5000) ;
    log("Complete, not aborted") ;
}

// several high speed usteps --- trace window must follow
function test3() {
    n = 5 ;
    log("Requesting " + n + " ustep().") ;
    for (i=0 ; i < n ; i++) {
        ustep() ;
    }
    log("Complete") ;
}

// show how to handle octal integers
function test4() {
    s = "0o123" ;
    i = +s+2; // prefix a string with "+" to convert to number, then calc
    log("int val of  +'" + s + "' +2  is " + i) ;
    log("octal string of " + i + " is '" + o(i) + "'") ;
    log("... with 6 digits: '" + o(i,6) + "'") ;
    log("Statevars are integers, print octal like  o(get(\"MPC\"))  => " + o(get("MPC")) ) ;
    log("Complete") ;
}


// several high speed usteps with statevar output
// --- trace window must follow
function test5() {
    n = 5 ;
    log("Requesting " + n + " ustep().") ;
    for (i=0 ; i < n ; i++) {
        ustep() ;
        log(
            i + ". MPC=" + o(get("MPC")) + ", UBADDR= " + o(get("UBADDR"))
        ) ;
    }
    log("Complete") ;
}


function isDATI(addr) {
    if ( get("UBADDR") == addr && get("UBCYCLE") == 0 /*DATI*/ )
        return true ;
    else
        return false ;
}


function test6() {
        log("now stepping to error point") ;
        while ( ! isDATI(0o765524) )
          ustep() ;
        log("defective opcode fetched, step to defective uword") ;
        while (get("MPC") != 0o311 )
          ustep() ;
        log("reached \"tstb @#dl11.rcsr\" failure point") ;
}

// main ... select one test
test6() ;

