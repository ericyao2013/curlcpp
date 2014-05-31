/* 
 * File:   CurlEasy.h
 * Author: Giuseppe
 *
 * Created on March 25, 2014, 10:54 PM
 */

#ifndef CURLEASY_H
#define	CURLEASY_H

#include <vector>
#include <list>
#include <memory>
#include <algorithm>
#include <curl/curl.h>

#include "CurlInterface.h"
#include "CurlError.h"
#include "CurlPair.h"

using std::vector;
using std::list;
using std::unique_ptr;
using std::for_each;
using curl::CurlPair;
using curl::CurlInterface;

namespace curl  {
    class CurlEasy : public CurlInterface<CURLcode> {
    public:
        CurlEasy();
        CurlEasy(const long);
        ~CurlEasy();
        template<typename T> CurlEasy &add(const CurlPair<CURLoption,T> &);
        template<typename T> CurlEasy &add(const vector<CurlPair<CURLoption,T>> &);
        template<typename T> CurlEasy &add(const list<CurlPair<CURLoption,T>> &);
        template<typename T> unique_ptr<T> getSessionInfo(const CURLINFO, T *) const;
        void escape(string &);
        void unescape(string &);
        int perform();
        void reset() noexcept;
        CURL *getCurl() const noexcept;
    protected:
        const string toString(const CURLcode) const noexcept;
    private:
        CURL *curl;
    };
    
    // Implementation of addOption method
    template<typename T> CurlEasy &CurlEasy::add(const CurlPair<CURLoption,T> &pair) {
        curl_easy_setopt(this->curl,pair.first(),pair.second());
        return *this;
    }
    // Implementation of overloaded method addOption
    template<typename T> CurlEasy &CurlEasy::add(const vector<CurlPair<CURLoption,T>> &pairs) {
        for_each(pairs.begin(),pairs.end(),[this](CurlPair<CURLoption,T> option) { this->add(option); } );
        return *this;
    }
    // Implementation of overloaded method addOption
    template<typename T> CurlEasy &CurlEasy::add(const list<CurlPair<CURLoption,T> > &pairs) {
        for_each(pairs.begin(),pairs.end(),[this](CurlPair<CURLoption,T> option) { this->add(option); });
        return *this;
    }
    // Implementation of getSessionInfo method
    template<typename T> unique_ptr<T> CurlEasy::getSessionInfo(const CURLINFO info, T *ptr_info) const {
        const CURLcode code = curl_easy_getinfo(this->curl,info,ptr_info);
        if (code != CURLE_OK && ptr_info) {
            throw new CurlError<CURLcode>(this->toString(code),code);
        }
        return unique_ptr<T>{ptr_info};
    }
}

#endif	/* CURLEASY_H */