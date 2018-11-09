
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "logger.h"
#include <ctime>

void log(string file_name, string msg){
    ofstream myfile;
    myfile.open(file_name, ios::app);
    myfile<<get_curr_time()<<"\t"<<msg<<endl;
    myfile.close();
}

string get_curr_time(){
    time_t t = std::time(0);
    tm* now = std::localtime(&t);
    return to_string(now->tm_year + 1900) + "-" + to_string(now->tm_mon + 1) + "-" +
        to_string(now->tm_mday)+ "  " + to_string(now->tm_hour)+":"+to_string(now->tm_min)+":"+to_string(now->tm_sec);
}

