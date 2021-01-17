// Copyright 2021 Artamonov Mark <a.mark.2001@mail.ru>

#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>
#include <boost/thread.hpp>
#include <chrono>
#include <iostream>
#include <string>


using boost::asio::buffer;
using boost::asio::ip::tcp;
boost::asio::io_service the_service;

class talk_to_server {
public:
    talk_to_server(
            const std::string& a_username)
            : the_socket(the_service),
            is_started(true),
            the_username(a_username) {}
    void to_write_the_message(
            const std::string& a_message) {
        the_socket.write_some(
                buffer(a_message)); }
    void to_ask_clients() {
        to_write_the_message(
                "ask_clients\n");
        to_read_the_answer();
    }
    void to_login() {
        to_ask_clients();
    }
    void to_ping(
            const std::string& an_message) {
        std::istringstream istream(
                an_message);
        std::string an_answer;
        istream >> an_answer >> an_answer;
        if (an_answer =="client_list_changed")
            to_ask_clients();
    }
    void to_clients(
            const std::string& a_message) {
        std::string the_clients =
                a_message.substr(8);
        std::cout << the_username
        << ", new client list:"
        << the_clients;
    }
    void to_message_the_process() {
        std::string the_message(
                the_buffer, is_read);
        if (the_message.find(
                "login ") == 0)
            to_login();
        else if (the_message.find(
                "ping") == 0)
            to_ping(the_message);
        else if (the_message.find(
                "clients ") == 0)
            to_clients(the_message);
        else
            std::cerr << "invalid msg "
            << the_message << std::endl;
    }

    void to_make_connection(
            tcp::endpoint an_endpoint) {
        the_socket.connect(an_endpoint); }

    size_t is_reading_completed(
            const boost::system
            ::error_code& an_error,
            size_t the_bytes) {
        if (an_error) return 0;
        bool is_found = std::find(
                the_buffer, the_buffer
                + the_bytes, '\n')
                        < the_buffer + the_bytes;
        return is_found ? 0 : 1;
    }

    void to_read_the_answer() {
        is_read = 0;
        read(the_socket,buffer(
                the_buffer),
             boost::bind(
                     &talk_to_server::
                     is_reading_completed,
                     this, _1, _2));
        to_message_the_process();
    }

    void to_write_the_request() {
        to_write_the_message(
                "ping\n");
    }

    void completed_functional_cycle() {
        to_write_the_message(
                "login "
                + the_username + "\n");
        to_read_the_answer();
        while (is_started) {
            to_write_the_request();
            to_read_the_answer();
            boost::this_thread::
            sleep(boost::
            posix_time::millisec(
                    rand_r(0) %
                    the_sleeping_time));
        }
    }
private:
    tcp::socket the_socket;
    int is_read;
    enum { max_message_length = 1024 };
    char the_buffer[max_message_length];
    bool is_started;
    const int the_sleeping_time = 7000;
    std::string the_username;
};

int main() {
    return 0;
}