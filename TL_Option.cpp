/*
 * File:   TL_Option.cpp
 * Author: everpan
 *
 * Created on 2011年5月13日, 下午2:35
 */

#include "TL_Option.h"
namespace tidp {

    void TL_Option::decode(int argc, char *argv[]) {
        _mParam.clear();

        std::vector<std::string> v;
        for (int i = 1; i < argc; ++i) {
            v.push_back(argv[i]);
        }

        for (size_t i = 0; i < v.size(); ++i) {
            const std::string & str = v[i];
            if (str.length() > 2 && str.substr(0, 2) == "--") {
                parse(v[i]);
            } else if (str.length() > 1 && str[0] == '-') {
                if (i < v.size()-1) {
                    ++i;
                    const std::string & str2 = v[i];
                    if (str2.length() > 0 && str2[0] != '-') {
                        _mParam[str.substr(1)] = str2;
                        continue;
                    } else {
                        --i;
                    }
                }
                _mParam[str.substr(1)] = "";
                //_vSingle.push_back(str.substr(1));
            } else {
                _vSingle.push_back(str);
            }
        }
    }

    void TL_Option::parse(const std::string &s) {
        std::string::size_type pos = s.find('=');
        if (pos != std::string::npos) {
            _mParam[s.substr(2, pos - 2)] = s.substr(pos + 1);
        } else {
            _mParam[s.substr(2, pos - 2)] = "";
        }
    }

    std::string TL_Option::getValue(const std::string &sName) {
        if (_mParam.find(sName) != _mParam.end()) {
            return _mParam[sName];
        }
        return "";
    }

    bool TL_Option::hasParam(const std::string &sName) {
        return _mParam.find(sName) != _mParam.end();
    }

    std::vector<std::string>& TL_Option::getSingle() {
        return _vSingle;
    }

    std::map<std::string, std::string>& TL_Option::getMulti() {
        return _mParam;
    }

    void TL_Option::output() {
        std::map<std::string, std::string>::const_iterator it = _mParam.begin();
        for (size_t i = 0; i < _vSingle.size(); ++i) {
            std::cout << _vSingle[i] << std::endl;
        }
        while (it != _mParam.end()) {
            std::cout << it->first << "=" << it->second << std::endl;
            ++it;
        }
    }
}

