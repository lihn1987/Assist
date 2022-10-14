#define BOOST_TEST_MODULE My Test

#include <boost/test/included/unit_test.hpp>
#include <memory>
#include <stdio.h>
#include <boost/thread.hpp>
#include <boost/format.hpp>

#include "../../crypto_tools/key_tools.h"
#include "../../net_lib/net_message.h"
#include "../../log/log.h"
#include "../../asio_manager/asio_manager.h"
#include "../../net_lib/chat_server.h"
#include "../../net_lib/chat_client.h"
BOOST_AUTO_TEST_CASE(net_test1) {
    GetChatLog()->LogDebug("===net_test1 测试开始");
    GetAsioManager()->StartThreads();
    std::shared_ptr<ChatServer> server = std::make_shared<ChatServer>();
    server->StartServer(GetAsioManager()->GetIoService(), 8801);
    server.reset();
    GetAsioManager()->StopThreads();
    BOOST_CHECK(GetAsioManager()->GetThreadCount() == 0);
    GetChatLog()->LogDebug("net_test1 测试完成");
}

BOOST_AUTO_TEST_CASE(net_test2) {
    GetChatLog()->LogDebug("===net_test2 测试开始");
    GetAsioManager()->StartThreads();
    uint32_t client_count = 1;
    std::vector<std::shared_ptr<ChatClient>> client_list;
    for(int i = 0; i < client_count; i++) {
        auto client = std::make_shared<ChatClient>(GetAsioManager()->GetIoService());
        client->Connect("127.0.0.1", 8801);
        client_list.push_back(client);
    }
    for(int i = 0; i < client_count; i++) {
        client_list[i]->Clear();
    }
    client_list.clear();
    GetAsioManager()->StopThreads();
    
    BOOST_CHECK(GetAsioManager()->GetThreadCount() == 0);
    GetChatLog()->LogDebug("===net_test2 测试完成");
}

BOOST_AUTO_TEST_CASE(net_test3) {
    GetChatLog()->LogDebug("===net_test3 测试开始");
    boost::this_thread::sleep(boost::posix_time::seconds(2));
    GetAsioManager()->StartThreads();
    uint32_t client_count = 100;
    std::vector<std::shared_ptr<ChatClient>> client_list;
    std::shared_ptr<ChatServer> server = std::make_shared<ChatServer>();
    server->StartServer(GetAsioManager()->GetIoService(), 8801);

    for(int i = 0; i < client_count; i++) {
        auto client = std::make_shared<ChatClient>(GetAsioManager()->GetIoService());
        client->Connect("127.0.0.1", 8801);
        client_list.push_back(client);
    }

    boost::this_thread::sleep(boost::posix_time::seconds(5));
    GetChatLog()->LogDebug((boost::format("当前服务器内的总客户端数量为: %d")%server->GetAllUnknowSocketItem().size()).str());
    BOOST_CHECK(server->GetAllUnknowSocketItem().size() == client_count);

    for(int i = 0; i < client_count; i++) {
        client_list[i]->Clear();
    }
    client_list.clear();
    server.reset();
    GetAsioManager()->StopThreads();
    
    BOOST_CHECK(GetAsioManager()->GetThreadCount() == 0);
    GetChatLog()->LogDebug("===net_test3 测试完成");
}

BOOST_AUTO_TEST_CASE(net_test4) {
    GetChatLog()->LogDebug("===net_test4 测试开始");
    boost::this_thread::sleep(boost::posix_time::seconds(2));
    GetAsioManager()->StartThreads();
    uint32_t client_count = 10;
    std::vector<std::shared_ptr<ChatClient>> client_list;
    std::shared_ptr<ChatServer> server = std::make_shared<ChatServer>();
    server->StartServer(GetAsioManager()->GetIoService(), 8801);

    for(int i = 0; i < client_count; i++) {
        auto client = std::make_shared<ChatClient>(GetAsioManager()->GetIoService());
        client->Connect("127.0.0.1", 8801);
        client_list.push_back(client);
    }

    boost::this_thread::sleep(boost::posix_time::seconds(5));
    BOOST_CHECK(server->GetAllUnknowSocketItem().size() == client_count);
    
    // 开始发送注册信息
    uint32_t user_count = 3;
    std::vector<std::string> pri_key_list;
    std::vector<std::string> pub_key_list;
    for (uint32_t i = 0; i < user_count; i++) {
        std::string pri_key = CreatePrivateKey();
        std::string pub_key = GetPublicKeyByPrivateKey(pri_key);
        pri_key_list.push_back(pri_key);
        pub_key_list.push_back(pub_key);
        std::shared_ptr<NetMessage> msg = NetMessage::CreateRegistMsg(pub_key, pri_key);
        client_list[i]->GetSocketItem()->Write(msg);
    }
    
    

    boost::this_thread::sleep(boost::posix_time::seconds(2));
    BOOST_CHECK(server->GetAllUnknowSocketItem().size() == client_count-user_count);
    BOOST_CHECK(server->GetAllRegistorSocketItem().size() == user_count);

    for(int i = 0; i < client_count; i++) {
        client_list[i]->Clear();
    }
    client_list.clear();
    server.reset();
    GetAsioManager()->StopThreads();
    
    BOOST_CHECK(GetAsioManager()->GetThreadCount() == 0);
    GetChatLog()->LogDebug("===net_test4 测试完成");
}

// BOOST_AUTO_TEST_CASE(net_test1) {
//     uint32_t client_count = 1;

//     GetAsioManager()->StartThreads();
//     std::shared_ptr<ChatServer> server = std::make_shared<ChatServer>();
//     std::vector<std::shared_ptr<ChatClient>> client_list;

//     server->StartServer(GetAsioManager()->GetIoService(), 8801);
//     for(int i = 0; i < client_count; i++) {
//         auto client = std::make_shared<ChatClient>(GetAsioManager()->GetIoService());
//         client->Connect("127.0.0.1", 8801);
//         client_list.push_back(client);
//     }

//     boost::this_thread::sleep(boost::posix_time::seconds(2));
//     GetChatLog()->LogDebug((boost::format("当前服务器内的总客户端数量为: %d")%server->GetAllUnknowSocketItem().size()).str());
//     BOOST_CHECK(server->GetAllUnknowSocketItem().size() == client_count);
//     getchar();
//     server.reset();
//     for(int i = 0; i < client_count; i++) {
//         client_list[i]->Clear();
//     }
//     boost::this_thread::sleep(boost::posix_time::seconds(5));
//     GetAsioManager()->StopThreads();

// }