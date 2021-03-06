/* autoconf header */
#include "config.h"

/* C++ standard headers */
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

/* 3rd party headers */
#include "czmq.h"

/* fncs headers */
#include "fncs.hpp"
#include "fncs_internal.hpp"

using namespace ::std;


int main(int argc, char **argv)
{
    string param_time_stop = "";
    string param_file_name = "";
    fncs::time time_granted = 0;
    fncs::time time_stop = 0;
    vector<string> events;
    ofstream fout;
    ostream out(cout.rdbuf()); /* share cout's stream buffer */

    if (argc < 2) {
        cerr << "Missing stop time parameter." << endl;
        cerr << "Usage: tracer <stop time> [output file]" << endl;
        exit(EXIT_FAILURE);
    }

    if (argc > 3) {
        cerr << "Too many parameters." << endl;
        cerr << "Usage: tracer <stop time> <output file>" << endl;
        exit(EXIT_FAILURE);
    }

    param_time_stop = argv[1];
    if (argc == 3) {
        param_file_name = argv[2];
        fout.open(param_file_name.c_str());
        if (!fout) {
            cerr << "Could not open output file '" << param_file_name << "'." << endl;
            exit(EXIT_FAILURE);
        }
        out.rdbuf(fout.rdbuf()); /* redirect out to use file buffer */
    }

    out << "#time\ttopic\tvalue" << endl;

    fncs::initialize();

    if (!fncs::is_initialized()) {
        cout << "did not connect to broker, exiting" << endl;
        fout.close();
        return EXIT_FAILURE;
    }

    time_stop = fncs::parse_time(param_time_stop);
    cout << "stops at " << time_stop << " nanoseconds" << endl;
    time_stop = fncs::convert_broker_to_sim_time(time_stop);
    cout << "stops at " << time_stop << " in sim time" << endl;

    do {
        time_granted = fncs::time_request(time_stop);
        cout << "time_granted is " << time_granted << endl;
        events = fncs::get_events();
        for (vector<string>::iterator it=events.begin(); it!=events.end(); ++it) {
            out << time_granted
                << "\t" << *it
                << "\t" << fncs::get_value(*it)
                << endl;
        }
    } while (time_granted < time_stop);
    cout << "time_granted was " << time_granted << endl;
    cout << "time_stop was " << time_stop << endl;

    cout << "done" << endl;

    fout.close();

    fncs::finalize();

    return EXIT_SUCCESS;
}

