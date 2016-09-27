/*
 * File:   TL_Option.h
 * Author: everpan
 *
 * Created on 2011年5月13日, 下午2:35
 */

#ifndef TL_OPTION_H
#define	TL_OPTION_H
#include <iostream>
#include <string>
#include <map>
#include <vector>
namespace tidp {

    class TL_Option {
    public:

        TL_Option() {
        };


        void decode(int argc, char *argv[]);

        bool hasParam(const std::string &sName);

        std::string getValue(const std::string &sName);

        std::map<std::string, std::string>& getMulti();

        std::vector<std::string>& getSingle();

        void output();

    protected:

        void parse(const std::string &s);

    protected:

        std::map<std::string, std::string> _mParam;

        std::vector<std::string> _vSingle;
    };
}
#endif	/* TL_OPTION_H */

