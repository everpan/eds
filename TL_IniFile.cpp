/* 
 * File:   TL_IniFile.cpp
 * Author: everpan
 * 
 * Created on 2011年5月12日, 下午9:35
 */

#include <fstream>

#include "TL_IniFile.h"

namespace tidp {

    TL_IniFile::TL_IniFile() {
    }

    TL_IniFile::~TL_IniFile() {
    }

    TL_IniFile::TL_IniFile(const std::string& inifile) {
        _conf_file_name = inifile;
        parseIniFile(inifile);
    }

    void TL_IniFile::parseIniFile(const std::string& inifile) {
        std::ifstream ifile;
        ifile.open(inifile.c_str(), std::ios_base::in);
        if(!ifile.is_open()){
            throw TL_Exp(std::string("TL_IniFile::parseIniFile:Can't open ") + inifile);
        }
        std::string sline;
        int i = 0;
        size_t pos = 0;
        std::map<std::string, std::string>* mp = NULL;
        while (!ifile.eof()) {
            ++i;
            //ifile.getline(sline,0,)
            getline(ifile, sline);
            //std::cout << i << ":" << sline << std::endl;
            sline = TL_Common::trim(sline);
            if (sline.size() > 0) {
                switch (sline[0]) {
                    case '[':
                    {
                        pos = sline.find("]");
                        if (pos == std::string::npos) {
                            throw TL_Exp("[TL_IniFile::parseIniFile] err session:" + sline);
                            break;
                        }
                        mp = &(_iniconf[TL_Common::trim(sline.substr(1, pos - 1))]);
                        break;
                    }
                    case ';':
                    case '#':
                        break;
                    
                    case '/':
                        //if(sline.size() > 2)
                        break;
                    
                    default:
                    {
                        pos = sline.find("=");
                        if (pos == std::string::npos) {
                            throw TL_Exp("[TL_IniFile::parseIniFile] err session field:" + sline);
                            break;
                        }
                        if (mp)
                            (*mp)[TL_Common::trim(sline.substr(0, pos))] = TL_Common::trim(sline.substr(pos + 1));
                    }
                }
            }
        }
        if (ifile.is_open()) ifile.close();
    }

    std::string&
    TL_IniFile::operator[](const std::string& session) {
        size_t pos = session.find(".");
        if (pos == std::string::npos) {
            throw TL_Exp("TL_IniFile error session.field:" + session);
        }
        //std::cout << session.substr(0,pos) << "." << session.substr(pos+1) << endl;
        return _iniconf[session.substr(0, pos)][session.substr(pos + 1)];
    }
    const std::string& TL_IniFile::getConfFileName(){
        return this->_conf_file_name;
    }
    void TL_IniFile::print() {
        std::map<std::string, std::map<std::string, std::string> >::const_iterator
        it = _iniconf.begin();
        while (it != _iniconf.end()) {
            std::cout << "session:" << it->first << std::endl;
            std::map<std::string, std::string>::const_iterator it2 = it->second.begin();
            while (it2 != it->second.end()) {
                std::cout << it2->first << ": = :" << it2->second << std::endl;
                ++it2;
            }
            ++it;
        }
    }

}
