/* 
 * File:   TL_IniFile.h
 * Author: everpan
 *
 * Created on 2011年5月12日, 下午9:35
 */

#ifndef TL_IniFile_H
#define	TL_IniFile_H
#include <iostream>
#include <algorithm>
#include <iterator>
#include <fstream>
#include <string>
#include <map>
#include "TL_Common.h"
#include "TL_Exp.h"
namespace tidp {

    class TL_IniFile {
    public:
        TL_IniFile();
        TL_IniFile(const std::string& inifile);


        void parseIniFile(const std::string& inifile);
        virtual ~TL_IniFile();
        void print();
        //get value by "session.field"
        std::string& operator[](const std::string& session);
        const std::string& getConfFileName();
    private:
        std::map<std::string, std::map<std::string, std::string> > _iniconf;
        std::string _conf_file_name;
    };
}
#endif	/* TL_IniFile_H */

