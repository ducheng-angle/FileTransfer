#ifndef __COMMON_HPP
#define __COMMON_HPP
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
#include <vector>
#define BUFSIZE 4096*10
#define BLOCKSIZE 4096
#define PORT 6060
#define SERVERIP "192.168.37.131"
#define PATH "test"
#define SIZE 30
 
static void Usage()
{
	std::cout << "Usage: io=read|write start=N end=N [context=]" << std::endl;
}

static std::string& StrTrim(std::string &s)   
{  
    if (s.empty()) {  
        return s;  
    }  
  
    s.erase(0,s.find_first_not_of(" "));  
    s.erase(s.find_last_not_of(" ") + 1);  
    return s;  
} 

static std::vector<std::string> StrSplit(std::string str,std::string pattern) { 
	int pos; 
	std::vector<std::string> result; 
	str += pattern;
	int size = str.size(); 
	for(int i = 0; i < size; i++) {
		pos = str.find(pattern,i); 
		if(pos < size) { 
			std::string s = str.substr(i, pos - i); 
			result.push_back(s);
			i = pos + pattern.size() - 1;
		} 
	} 
	return result;
}

static inline int index_in_strings(const char *strings, const char *key)
{
	int idx = 0;

	while (*strings) {
		if (strcmp(strings, key) == 0) {
			return idx;
		}
		strings += strlen(strings) + 1; /* skip NUL */
		idx++;
	}
	return -1;
}
static int isalldigit(char *s)
{
    char c;
    while (c = *s)
    {
        if (!isdigit(c))
            return 0;
        s++;
    }
    return 1;
}

static int Parse(std::string s,int &flags,int &start,int &end,std::string &str)
{
	int ret = 0;
	static const char keywords[] =
		"io\0""start\0""end\0""context\0";
	enum {
		OP_io = 0,
		OP_start,
		OP_end,
		OP_context
	};
	std::vector<std::string> a;
	s=StrTrim(s);
	a=StrSplit(s," ");
	for(int i=0;i<a.size();i++)
	{
	    int what;
	    std::string temp;
	    temp=a[i];
		char *arg=const_cast<char*>(temp.c_str());
		char *val = strchr(arg, '=');
		if (NULL == val){
			Usage();
		    return 1;
		}
		*val = '\0';
		what = index_in_strings(keywords, arg);
		if (what < 0){
			Usage();
		    return 1;
		}
		val++;
		std::string ptr;
		switch (what) {
			case OP_io:
		         ptr=val;
			     if(ptr=="read"){
		         	flags=0;
		         }
		         else if(ptr=="write"){
		            flags=1;
		         }
		         else{
		            Usage();
		            return 1;
		         }
		         break;
		    case OP_start:
		         if(isalldigit(val)){
		            start=atoi(val);
		         }
		         else{
		            Usage();
		            return 1;
		         }
		         break;
		    case OP_end:
		         if(isalldigit(val)){
		            end=atoi(val);
		         }
		         else{
		            Usage();
		            return 1;
		         }
		         break;
		    case OP_context:
		         str=val;
		         break;
		    default:
		         Usage();
		         return 1;
		    } 
	}
	if(start>=end || start < 0){
		std::cout << "start no more than end,or start no less than 0" << std::endl;
		return 1;
	}

	return 0;
}

#endif
