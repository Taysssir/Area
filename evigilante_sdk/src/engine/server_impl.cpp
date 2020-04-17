//#include <iostream>
//#include <sstream>
#include <algorithm>
#include <thread>
//#include <chrono>
#include "tcp/logger.hpp"

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include "server_impl.hpp"
//#include "eos/sdk/msgs.pb.h"
#include "message_handler.hpp"
//#include "engine/zlib_utils.h"
#include "server_callsystem.hpp"
#include "logger.hpp"

namespace eos
{
    namespace sdk
    {
        namespace priv
        {

            Server::Server()
                : run4ever_( false )
            {
                consumers_.clear();
                packets_.reserve(10);
                packets_.clear();
            }

            Server::~Server()
            {
                close();
            }

            bool Server::listen(unsigned short port)
            {
                if(run4ever_)
                {
                    return false;
                }

                Socket::Status ready = listener_.listen(port);
                if(ready == Socket::Done)
                {
                    run4ever_ = true;
                    thread_ = std::thread(&Server::run, this);
                    return true;
                }
                return false;
            }


            void Server::run()
            {
                fd_set readfds;
                socket_.reset(new Socket);
                int max_sd;
                int activity;

                while(run4ever_ == true)
                {
                    FD_ZERO(&readfds);
                    int& master_socket = listener_.getHandle();
                    //add master socket to set
                    FD_SET(master_socket, &readfds);
                    max_sd = master_socket;
                    //add child sockets to set
                    for ( auto el : consumers_)
                    {
                        int& sd = el->getHandle();
                        if(sd > 0)      FD_SET( sd , &readfds);
                        if(sd > max_sd) max_sd = sd;
                    }
                    timeval tv = {0, 50000} ; // useconde
                    activity = select( max_sd + 1 , &readfds , NULL , NULL , &tv);
                    if ((activity < 0) && (errno!=EINTR))
                    {
                        continue ;
                    }
                    else if (FD_ISSET(listener_.getHandle() , &readfds)) // accept
                    {
                        if(listener_.accept(*socket_) == Socket::Done)
                        {
                            socket_->setBlocking( false );
                            consumers_.push_back(socket_);

                            Packet connected;
                            if(toPacket(robot_info_, connected))
                                socket_->send( connected);

                            onConnected();

                            socket_.reset(new Socket);
                        }

                    }
                    else // if(consumers_.size() > 0)
                    {
                        // for every consumer, read socket message and threat it
                        for(auto socket : consumers_)
                        {
                            int& sd = socket->getHandle();

                            if (FD_ISSET( sd , &readfds))
                            {
                                Packet packet;
                                Socket::Status status = socket->receive(packet);

                                if(status == Socket::Done)
                                {
                                    if( packet.getType() == RobotDataSubscription::descriptor()->index())
                                    {
                                        LOG_INFO(" RECEIVED DATA Subbscription");
                                    }
                                    else
                                    {
                                        treatMessage(packet);
                                    }
                                }
                                else if( (status == Socket::Disconnected ) || ( status == Socket::Error ) )
                                {
                                    socket->close();
                                }
                            }

                        }
                    }

                    // for every packet have to send, send to everyone connected
                    sendAllPacket();

                    // clean the consumers stack
                    consumers_.erase(std::remove_if(
                                         consumers_.begin(), consumers_.end(),
                                         [](Socket::Ptr socket)
                    {
                                         return !socket->valid();
                                     }), consumers_.end());


                }
                cv_m_.unlock();
                LOG_DEBUG("Server::run - STOPPED");
            }

            void Server::onConnected()
            {
                LOG_INFO("Server::onconnected() -");
                std::lock_guard<std::mutex> lock(protect_backends_);
                for(auto backend : backends_ )
                {
                    backend->onNewClient();
                }
            }

            void Server::sendAllPacket()
            {
                std::lock_guard<std::mutex> lock(mutex_thread_);
                for(auto const& packet : packets_)
                {
                    broadcast( packet );
                }
                packets_.clear();

            }
            bool Server::pub_msg(Packet const &packet)
            {
                std::lock_guard<std::mutex> lock(mutex_thread_);
                if(packets_.size() +1 > packets_.capacity())
                {
                    packets_.erase(packets_.begin());
                }
                packets_.push_back( packet );
                return packets_.size();
            }

            void Server::broadcast(Packet const& packet)
            {
                for(auto socket : consumers_)
                {
                    if(socket->valid())
                    {
                        socket->send(packet);
                    }
                }
            }

            void Server::close()
            {
                // std::lock_guard<std::mutex> lock(mutex_thread_);
                LOG_INFO("Server::close - " );

                listener_.close();

                if(run4ever_)
                {
                    run4ever_ = false;
                    ::shutdown(listener_.getHandle(), SHUT_RDWR);
                    if(thread_.joinable())
                    {
                        LOG_WARN("Server::close - joignable");
                        thread_.join();
                    }
                }
                LOG_WARN("Server::close - [OK]");

            }
            void Server::treatMessage(Packet &packet )
            {
                std::lock_guard<std::mutex> lock(protect_backends_);
                Caller::treatMessage(backends_, packet);

            }

            void Server::subscribe(BackendTreatment::Ptr listener)
            {
                std::lock_guard<std::mutex> lock(protect_backends_);
                if(listener != nullptr)
                {
                    backends_.push_back( listener );
                    listener->setServer( this );
                }
            }

            void Server::setRobotType(std::string const& robot_type)
            {
                robot_info_.set_name( robot_type );
            }

            void Server::setCamera(eos::sdk::Camera::Type const &type, std::string const& ip_address, int & port , int id)
            {
                eos::sdk::Camera * camera = robot_info_.mutable_camera()->Add();
                if(camera)
                {
                    camera->set_type( type );
                    camera->set_ip_address(ip_address );
                    camera->set_port(port);
                    camera->set_id(id);
                }
            }

        } // priv
    } // sdk
} // eos
