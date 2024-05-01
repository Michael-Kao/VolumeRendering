#include "Util.h"

std::vector<std::string>
strUtil::split(std::string const &target, std::string const &delimiter) {
	int start = 0, end = 0, delimiter_size = delimiter.size();
    if(delimiter_size == 0) return {target};
	std::vector<std::string> res;
	while(target.find(delimiter, start) != std::string::npos) {
		end = target.find(delimiter, start);
		if(end - start > 0) {
			std::string token = target.substr(start, (end - start));
			res.push_back(token);
		}
		start = end + delimiter_size;
	}
	std::string token = target.substr(start);
	res.push_back(token);

	return res;
}


std::string 
strUtil::remove(std::string const &target, std::string const &delimiter, wordCase mode) {
    std::string res = "";
    std::vector<std::string> temp = strUtil::split(target, delimiter);
    for(std::string str : temp) {
        if(mode == LOWER) {
            for(int i = 0; i < str.size(); ++i) {
                if(str[i] >= 'A' && str[i] <= 'Z') {
                    str[i] = str[i] - 'A' + 'a';
                }
            }
        } else if(mode == UPPER) {
            for(int i = 0; i < str.size(); ++i) {
                if(str[i] >= 'a' && str[i] <= 'z') {
                    str[i] = str[i] - 'a' + 'A';
                }
            }
        }
        res += str;
    }
    return res;
}