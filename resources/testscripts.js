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

// main ... select one test
test1() ;

