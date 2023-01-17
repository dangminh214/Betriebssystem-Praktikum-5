//FAIRER SOLUTION
#include <iostream>			// for std::cin, cerr, cout ...
#include <thread>			// for std::this_thread
#include <chrono>			// for std::chrono...
#include <semaphore>
#include "database.h"
#include "reader-writer-threads.h"


// *** reader & writer threads ***

// The writer thread
std::counting_semaphore in{1};
std::counting_semaphore out{1};
std::counting_semaphore db{0};
int rc_in = 0, rc_out = 0;
int wait = 0;
void writer( int writerID, int numSeconds ) {

    std::cout << "Writer " << writerID << " starting..." << std::endl;

    int	tests = 0;
    auto startTime = std::chrono::steady_clock::now();
    std::chrono::seconds maxTime( numSeconds );
    while ( ( std::chrono::steady_clock::now() - startTime ) < maxTime ) {
        in.acquire();
        out.acquire();
        if(rc_in == rc_out)
            out.release();
            else			//while reading -> need to wait
            {
                wait = 1;
                out.release();
                db.acquire();
                wait = 0;
            }
        bool result = theDatabase.write( writerID );
        in.release();
        ++tests;

        // sleep a while...
        int numSeconds2sleep = randomInt( 3 ); // i.e. either 0, 1 or 2
        std::chrono::seconds randomSeconds( numSeconds2sleep );
        std::cout << "WRITER " << writerID
                  << " Finished test " << tests
                  << ", result = " << result
                  << ", sleeping " << numSeconds2sleep
                  << " seconds " << std::endl;
        if ( numSeconds2sleep ) std::this_thread::sleep_for ( randomSeconds );

    } // repeat until time used is up

    std::cout << "WRITER " << writerID
              << "Finished. Returning after " << tests
              << " tests." << std::endl;

} // end writer function

// The reader thread
void reader( int readerID, int numSeconds ) {

    std::cout << "Reader " << readerID << " starting..." << std::endl;

    int	tests=0;
    auto startTime = std::chrono::steady_clock::now();
    std::chrono::seconds maxTime( numSeconds );
    while ( ( std::chrono::steady_clock::now() - startTime ) < maxTime ) {
        in.acquire();
        rc_in++;
        in.release();
        bool result = theDatabase.read( readerID );
        out.acquire();
        rc_out++;
        if(wait == 1 && rc_in == rc_out)
            db.release();
        out.release();
        ++tests;

        // sleep a while...
        int numSeconds2sleep = randomInt( 3 ); // i.e. either 0, 1 or 2
        std::chrono::seconds randomSeconds( numSeconds2sleep );
        std::cout << "READER " << readerID
                  << " Finished test " << tests
                  << ", result = " << result
                  << ", sleeping " << numSeconds2sleep
                  << " seconds " << std::endl;
        if ( numSeconds2sleep ) std::this_thread::sleep_for ( randomSeconds );

    } // repeat until time is used up

    std::cout << "READER " << readerID
              << "Finished. Returning after " << tests
              << " tests." << std::endl;

} // end reader function
