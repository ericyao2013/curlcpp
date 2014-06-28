/* 
 * File:   curl_multi.cpp
 * Author: Giuseppe
 * 
 * Created on March 25, 2014, 11:02 PM
 */

#include <algorithm>
#include "curl_multi.h"

using std::for_each;
using curl::curl_multi;

// Implementation of constructor.
curl_multi::curl_multi() : curl_interface() {
    this->curl = curl_multi_init();
    if (this->curl == nullptr) {
        throw curl_error("*** Error while initializing multi handler ****",__FUNCTION__);
    }
    this->active_transfers = 0;
    this->message_queued = 0;
}

// Implementation of overloaded constructor.
curl_multi::curl_multi(const long flag) : curl_interface(flag) {
    curl_multi();
}

// Implementation of copy constructor to respect the rule of three.
curl_multi::curl_multi(const curl_multi &multi) : message_queued(multi.message_queued), active_transfers(multi.active_transfers) {
    this->curl = curl_multi_init();
    if (this->curl == nullptr) {
        throw curl_error("*** Error while initializing multi handler ****",__FUNCTION__);
    }
}

// Implementation of assignment operator to perform deep copy.
curl_multi &curl_multi::operator=(const curl_multi &multi) {
    if (this == &multi) {
        return *this;
    }
    curl_multi();
    return *this;
}

// Implementation of destructor.
curl_multi::~curl_multi() noexcept{
    curl_multi_cleanup(this->curl);
}

// Implementation of add method for easy handlers.
void curl_multi::add(const curl_easy &easy) {
    const CURLMcode code = curl_multi_add_handle(this->curl,easy.get_curl());
    if (code != CURLM_OK) {
        throw curl_error(this->to_string(code),__FUNCTION__);
    }
}

// Implementation of overloaded add method for a vector of easy handlers.
void curl_multi::add(const vector<curl_easy> &easy) {
    for_each(easy.begin(),easy.end(),[this](curl_easy handler) {
        this->add(handler);
    });
}

// Implementation of overloaded add method for a list of easy handlers.
void curl_multi::add(const list<curl_easy> &easy) {
    for_each(easy.begin(),easy.end(),[this](curl_easy handler) {
        this->add(handler);
    });
}

// Implementation of remove for easy handlers.
void curl_multi::remove(const curl_easy &easy) {
    const CURLMcode code = curl_multi_remove_handle(this->curl,easy.get_curl());
    if (code != CURLM_OK) {
        throw curl_error(this->to_string(code),__FUNCTION__);
    }
}

// Implementation of read_info method.
void curl_multi::read_info() {
    // Return a vector of infos
}

// Implementation of overloaded read_info method.
void curl_multi::read_info(const curl_easy &easy) {
    CURLMsg *message = nullptr;
    while ((message = curl_multi_info_read(this->curl,&this->message_queued))) {
        if (message->easy_handle == easy.get_curl()) {
            // Find a way to return the info!!
        }
    }
}

// Implementation of perform method.
bool curl_multi::perform() {
    const CURLMcode code = curl_multi_perform(this->curl,&this->active_transfers);
    if (code == CURLM_CALL_MULTI_PERFORM) {
        return false;
    }
    if (code != CURLM_OK) {
        throw curl_error(this->to_string(code),__FUNCTION__);
    }
    return true;
}

// Implementation of socket_action method.
bool curl_multi::socket_action(const curl_socket_t sockfd, const int ev_bitmask) {
    const CURLMcode code = curl_multi_socket_action(this->curl,sockfd,ev_bitmask,&this->active_transfers);
    if (code == CURLM_CALL_MULTI_PERFORM) {
        return false;
    } 
    if (code != CURLM_OK) {
        throw curl_error(this->to_string(code),__FUNCTION__);
    }
    return true;
}

// Implementation of set_fd method.
void curl_multi::set_fd(fd_set *read, fd_set *write, fd_set *exec, int *max_fd) {
    const CURLMcode code = curl_multi_fdset(this->curl,read,write,exec,max_fd);
    if (code != CURLM_OK) {
        throw curl_error(this->to_string(code),__FUNCTION__);
    }
}

// Implementation of wait method.
void curl_multi::wait(struct curl_waitfd extra_fds[], const unsigned int extra_nfds, const int timeout_ms, int *numfds) {
    const CURLMcode code = curl_multi_wait(this->curl,extra_fds,extra_nfds,timeout_ms,numfds);
    if (code != CURLM_OK) {
        throw curl_error(this->to_string(code),__FUNCTION__);
    }
}

// Implementation of assign method.
void curl_multi::assign(const curl_socket_t sockfd, void *sockptr) {
    const CURLMcode code = curl_multi_assign(this->curl,sockfd,sockptr);
    if (code != CURLM_OK) {
        throw curl_error(this->to_string(code),__FUNCTION__);
    }
}

// Implementation of timeout method.
void curl_multi::timeout(long *timeout) {
    const CURLMcode code = curl_multi_timeout(this->curl,timeout);
    if (code != CURLM_OK) {
        throw curl_error(this->to_string(code),__FUNCTION__);
    }
}
