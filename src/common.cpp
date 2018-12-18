#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"


bool str_to_double(string s, double & val){
    int i;
    string num_str="";
    bool got_dec=false;
    for(i=0;i<s.length();i++){
        if(s[i]>='0' && s[i]<='9'){
            num_str += s[i];
        }
        else if(s[i]=='.' && !got_dec){
                num_str += s[i];
                got_dec = true;
                if(num_str[0]=='.'){ // if the str starts with a .
                    return false;
                }
        }
        else if(s[i]!='\'' && s[i]!='"'){
            break;
        }
    }
    if(num_str=="" || num_str=="."){
        return false;
    }
    else{
        val = strtod(num_str.c_str(),NULL);
        return true;
    }
}

//// This extract all numbers from a given text, we need to limit the kind of text that we need to filter
//bool str_to_double(string s, double & val){
//    int i;
//    string num_str="";
//    bool got_dec;
//    got_dec=0;
//    for(i=0;i<s.length();i++){
//        if(s[i]>='0' && s[i]<='9'){
//            num_str += s[i];
//        }
//        else if(s[i]=='.' && !got_dec){
//            num_str += s[i];
//        }
//    }
//    if(num_str=="" || num_str=="."){
//        return 0;
//    }
//    else{

//        val = strtod(num_str.c_str(),NULL);
//        return 1;
//    }
//}


string get_class_from_line(string line){
    int i;
    for(i=0;i<line.length();i++){
        if(line[i]=='\t' || line[i]=='\n'){
            return line.substr(0,i);
        }
    }
    return line;
    //return "";
}
