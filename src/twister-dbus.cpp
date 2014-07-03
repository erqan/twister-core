/**
  * DBUS entegration for twister P2P
  *
  * Erkan Yüksel © 2014
  */

#include <dbus/dbus.h>

#include "bitcoinrpc.h"

using namespace json_spirit;
using namespace std;

//registered path IDs
typedef enum TW_DBUS_RPID
{
    TW_DBUS_RPID_ROOT,
    TW_DBUS_RPID_TWISTER,
    TW_DBUS_RPID_USERS,
    TW_DBUS_RPID_USER,
    TW_DBUS_RPID_DHT,
    TW_DBUS_RPID_COUNT
} TW_DBUS_RPID;

typedef enum TW_DBUS_METHODS
{
    TW_DBUS_M_HELP,
    TW_DBUS_M_STOP,
    TW_DBUS_M_GETBLOCKCOUNT,
    TW_DBUS_M_GETBESTBLOCKHASH,
    TW_DBUS_M_GETCONNECTIONCOUNT,
    TW_DBUS_M_GETPEERINFO,
    TW_DBUS_M_ADDNODE,
    TW_DBUS_M_ADDDNSSEED,
    TW_DBUS_M_GETADDEDNODEINFO,
    TW_DBUS_M_GETDIFFICULTY,
    TW_DBUS_M_GETGENERATE,
    TW_DBUS_M_SETGENERATE,
    TW_DBUS_M_GETHASHESPERSEC,
    TW_DBUS_M_GETINFO,
    TW_DBUS_M_GETMININGINFO,
    TW_DBUS_M_CREATEWALLETUSER,
    TW_DBUS_M_LISTWALLETUSERS,
    TW_DBUS_M_BACKUPWALLET,
    TW_DBUS_M_WALLETPASSPHRASE,
    TW_DBUS_M_WALLETPASSPHRASECHANGE,
    TW_DBUS_M_WALLETLOCK,
    TW_DBUS_M_ENCRYPTWALLET,
    TW_DBUS_M_GETRAWMEMPOOL,
    TW_DBUS_M_GETBLOCK,
    TW_DBUS_M_GETBLOCKHASH,
    TW_DBUS_M_GETTRANSACTION,
    TW_DBUS_M_LISTTRANSACTIONS,
    TW_DBUS_M_SIGNMESSAGE,
    TW_DBUS_M_VERIFYMESSAGE,
    TW_DBUS_M_GETWORK,
    TW_DBUS_M_GETBLOCKTEMPLATE,
    TW_DBUS_M_SUBMITBLOCK,
    TW_DBUS_M_LISTSINCEBLOCK,
    TW_DBUS_M_DUMPPRIVKEY,
    TW_DBUS_M_DUMPPUBKEY,
    TW_DBUS_M_DUMPWALLET,
    TW_DBUS_M_IMPORTPRIVKEY,
    TW_DBUS_M_IMPORTWALLET,
    TW_DBUS_M_GETRAWTRANSACTION,
    TW_DBUS_M_CREATERAWTRANSACTION,
    TW_DBUS_M_DECODERAWTRANSACTION,
    TW_DBUS_M_SENDRAWTRANSACTION,
    TW_DBUS_M_SENDNEWUSERTRANSACTION,
    TW_DBUS_M_VERIFYCHAIN,
    TW_DBUS_M_GETLASTSOFTCHECKPOINT,
    TW_DBUS_M_DHTPUT,
    TW_DBUS_M_DHTGET,
    TW_DBUS_M_NEWPOSTMSG,
    TW_DBUS_M_NEWDIRECTMSG,
    TW_DBUS_M_NEWRTMSG,
    TW_DBUS_M_GETPOSTS,
    TW_DBUS_M_GETDIRECTMSGS,
    TW_DBUS_M_SETSPAMMSG,
    TW_DBUS_M_GETSPAMMSG,
    TW_DBUS_M_FOLLOW,
    TW_DBUS_M_UNFOLLOW,
    TW_DBUS_M_GETFOLLOWING,
    TW_DBUS_M_GETLASTHAVE,
    TW_DBUS_M_GETNUMPIECES,
    TW_DBUS_M_LISTUSERNAMESPARTIAL,
    TW_DBUS_M_RESCANDIRECTMSGS,
    TW_DBUS_M_RECHECKUSERTORRENT,
    TW_DBUS_M_GETTRENDINGHASHTAGS,
    TW_DBUS_M_GETSPAMPOSTS,
    TW_DBUS_M_TORRENTSTATUS,
    TW_DBUS_M_USERSEARCH,
    TW_DBUS_M_CNT
} TW_DBUS_METHODS;

#define TW_DBUS_IF_CORE     "twister.core"
#define TW_DBUS_IF_USERS    "twister.users"
#define TW_DBUS_IF_DHT      "twister.dht"

#define TW_DBUS_PATH_ROOT       "/"
#define TW_DBUS_PATH_TWISTER    "/twister"
#define TW_DBUS_PATH_USERS      TW_DBUS_PATH_TWISTER "/users"
#define TW_DBUS_PATH_DHT        TW_DBUS_PATH_TWISTER "/dht"

typedef struct tw_dbus_calls_t
{
    string interface;
    string method;
    bool isRPC;
    //0: /, 1: /twister, 2: users, 3: users/[USERNAME], 4: dht
    bool permissions[TW_DBUS_RPID_COUNT];
} tw_dbus_calls_t;

typedef struct tw_dbus_path_t
{
    TW_DBUS_RPID rpid;
    const void *udata1;
    const void *udata2;
} tw_dbus_path_t;

string tw_dbus_rpc_methods_g[] = {
    "help",
    "stop",
    "getblockcount",
    "getbestblockhash",
    "getconnectioncount",
    "getpeerinfo",
    "addnode",
    "adddnsseed",
    "getaddednodeinfo",
    "getdifficulty",
    "getgenerate",
    "setgenerate",
    "gethashespersec",
    "getinfo",
    "getmininginfo",
    "createwalletuser",
    "listwalletusers",
    "backupwallet",
    "walletpassphrase",
    "walletpassphrasechange",
    "walletlock",
    "encryptwallet",
    "getrawmempool",
    "getblock",
    "getblockhash",
    "gettransaction",
    "listtransactions",
    "signmessage",
    "verifymessage",
    "getwork",
    "getblocktemplate",
    "submitblock",
    "listsinceblock",
    "dumpprivkey",
    "dumppubkey",
    "dumpwallet",
    "importprivkey",
    "importwallet",
    "getrawtransaction",
    "createrawtransaction",
    "decoderawtransaction",
    "sendrawtransaction",
    "sendnewusertransaction",
    "verifychain",
    "getlastsoftcheckpoint",
    "dhtput",
    "dhtget",
    "newpostmsg",
    "newdirectmsg",
    "newrtmsg",
    "getposts",
    "getdirectmsgs",
    "setspammsg",
    "getspammsg",
    "follow",
    "unfollow",
    "getfollowing",
    "getlasthave",
    "getnumpieces",
    "listusernamespartial",
    "rescandirectmsgs",
    "recheckusertorrent",
    "gettrendinghashtags",
    "getspamposts",
    "torrentstatus",
    "usersearch"
};

tw_dbus_calls_t tw_dbus_call_permissions_g[] = {
    //interface         method name                                                isRPC   root twister users  users/* dht
    {TW_DBUS_IF_CORE,   tw_dbus_rpc_methods_g[TW_DBUS_M_HELP],                     true,  {false, true, false, false, false}},
    {TW_DBUS_IF_CORE,   tw_dbus_rpc_methods_g[TW_DBUS_M_STOP],                     true,  {false, true, false, false, false}},
    {TW_DBUS_IF_CORE,   tw_dbus_rpc_methods_g[TW_DBUS_M_GETBLOCKCOUNT],            true,  {false, true, false, false, false}},
    {TW_DBUS_IF_CORE,   tw_dbus_rpc_methods_g[TW_DBUS_M_GETBESTBLOCKHASH],         true,  {false, true, false, false, false}},
    {TW_DBUS_IF_CORE,   tw_dbus_rpc_methods_g[TW_DBUS_M_GETCONNECTIONCOUNT],       true,  {false, true, false, false, false}},
    {TW_DBUS_IF_CORE,   tw_dbus_rpc_methods_g[TW_DBUS_M_GETPEERINFO],              true,  {false, true, false, false, false}},
    {TW_DBUS_IF_CORE,   tw_dbus_rpc_methods_g[TW_DBUS_M_ADDNODE],                  true,  {false, true, false, false, false}},
    {TW_DBUS_IF_CORE,   tw_dbus_rpc_methods_g[TW_DBUS_M_ADDDNSSEED],               true,  {false, true, false, false, false}},
    {TW_DBUS_IF_CORE,   tw_dbus_rpc_methods_g[TW_DBUS_M_GETADDEDNODEINFO],         true,  {false, true, false, false, false}},
    {TW_DBUS_IF_CORE,   tw_dbus_rpc_methods_g[TW_DBUS_M_GETDIFFICULTY],            true,  {false, true, false, false, false}},
    {TW_DBUS_IF_CORE,   tw_dbus_rpc_methods_g[TW_DBUS_M_GETGENERATE],              true,  {false, true, false, false, false}},
    {TW_DBUS_IF_CORE,   tw_dbus_rpc_methods_g[TW_DBUS_M_SETGENERATE],              true,  {false, true, false, false, false}},
    {TW_DBUS_IF_CORE,   tw_dbus_rpc_methods_g[TW_DBUS_M_GETHASHESPERSEC],          true,  {false, true, false, false, false}},
    {TW_DBUS_IF_CORE,   tw_dbus_rpc_methods_g[TW_DBUS_M_GETINFO],                  true,  {false, true, false, false, false}},
    {TW_DBUS_IF_CORE,   tw_dbus_rpc_methods_g[TW_DBUS_M_GETMININGINFO],            true,  {false, true, false, false, false}},
    {TW_DBUS_IF_CORE,   tw_dbus_rpc_methods_g[TW_DBUS_M_CREATEWALLETUSER],         true,  {false, true, false, false, false}},
    {TW_DBUS_IF_CORE,   tw_dbus_rpc_methods_g[TW_DBUS_M_LISTWALLETUSERS],          true,  {false, true, false, false, false}},
    {TW_DBUS_IF_CORE,   tw_dbus_rpc_methods_g[TW_DBUS_M_BACKUPWALLET],             true,  {false, true, false, false, false}},
    {TW_DBUS_IF_CORE,   tw_dbus_rpc_methods_g[TW_DBUS_M_WALLETPASSPHRASE],         true,  {false, true, false, false, false}},
    {TW_DBUS_IF_CORE,   tw_dbus_rpc_methods_g[TW_DBUS_M_WALLETPASSPHRASECHANGE],   true,  {false, true, false, false, false}},
    {TW_DBUS_IF_CORE,   tw_dbus_rpc_methods_g[TW_DBUS_M_WALLETLOCK],               true,  {false, true, false, false, false}},
    {TW_DBUS_IF_CORE,   tw_dbus_rpc_methods_g[TW_DBUS_M_ENCRYPTWALLET],            true,  {false, true, false, false, false}},
    {TW_DBUS_IF_CORE,   tw_dbus_rpc_methods_g[TW_DBUS_M_GETRAWMEMPOOL],            true,  {false, true, false, false, false}},
    {TW_DBUS_IF_CORE,   tw_dbus_rpc_methods_g[TW_DBUS_M_GETBLOCK],                 true,  {false, true, false, false, false}},
    {TW_DBUS_IF_CORE,   tw_dbus_rpc_methods_g[TW_DBUS_M_GETBLOCKHASH],             true,  {false, true, false, false, false}},
    {TW_DBUS_IF_CORE,   tw_dbus_rpc_methods_g[TW_DBUS_M_GETTRANSACTION],           true,  {false, true, false, false, false}},
    {TW_DBUS_IF_CORE,   tw_dbus_rpc_methods_g[TW_DBUS_M_LISTTRANSACTIONS],         true,  {false, true, false, false, false}},
    {TW_DBUS_IF_CORE,   tw_dbus_rpc_methods_g[TW_DBUS_M_SIGNMESSAGE],              true,  {false, true, false, false, false}},
    {TW_DBUS_IF_CORE,   tw_dbus_rpc_methods_g[TW_DBUS_M_VERIFYMESSAGE],            true,  {false, true, false, false, false}},
    {TW_DBUS_IF_CORE,   tw_dbus_rpc_methods_g[TW_DBUS_M_GETWORK],                  true,  {false, true, false, false, false}},
    {TW_DBUS_IF_CORE,   tw_dbus_rpc_methods_g[TW_DBUS_M_GETBLOCKTEMPLATE],         true,  {false, true, false, false, false}},
    {TW_DBUS_IF_CORE,   tw_dbus_rpc_methods_g[TW_DBUS_M_SUBMITBLOCK],              true,  {false, true, false, false, false}},
    {TW_DBUS_IF_CORE,   tw_dbus_rpc_methods_g[TW_DBUS_M_LISTSINCEBLOCK],           true,  {false, true, false, false, false}},
    {TW_DBUS_IF_USERS,  tw_dbus_rpc_methods_g[TW_DBUS_M_DUMPPRIVKEY],              true,  {false, false,false, true,  false}},
    {TW_DBUS_IF_USERS,  tw_dbus_rpc_methods_g[TW_DBUS_M_DUMPPUBKEY],               true,  {false, false,false, true,  false}},
    {TW_DBUS_IF_CORE,   tw_dbus_rpc_methods_g[TW_DBUS_M_DUMPWALLET],               true,  {false, true, false, false, false}},
    {TW_DBUS_IF_CORE,   tw_dbus_rpc_methods_g[TW_DBUS_M_IMPORTPRIVKEY],            true,  {false, true, false, false, false}},
    {TW_DBUS_IF_CORE,   tw_dbus_rpc_methods_g[TW_DBUS_M_IMPORTWALLET],             true,  {false, true, false, false, false}},
    {TW_DBUS_IF_CORE,   tw_dbus_rpc_methods_g[TW_DBUS_M_GETRAWTRANSACTION],        true,  {false, true, false, false, false}},
    {TW_DBUS_IF_CORE,   tw_dbus_rpc_methods_g[TW_DBUS_M_CREATERAWTRANSACTION],     true,  {false, true, false, false, false}},
    {TW_DBUS_IF_CORE,   tw_dbus_rpc_methods_g[TW_DBUS_M_DECODERAWTRANSACTION],     true,  {false, true, false, false, false}},
    {TW_DBUS_IF_CORE,   tw_dbus_rpc_methods_g[TW_DBUS_M_SENDRAWTRANSACTION],       true,  {false, true, false, false, false}},
    {TW_DBUS_IF_CORE,   tw_dbus_rpc_methods_g[TW_DBUS_M_SENDNEWUSERTRANSACTION],   true,  {false, true, false, false, false}},
    {TW_DBUS_IF_CORE,   tw_dbus_rpc_methods_g[TW_DBUS_M_VERIFYCHAIN],              true,  {false, true, false, false, false}},
    {TW_DBUS_IF_CORE,   tw_dbus_rpc_methods_g[TW_DBUS_M_GETLASTSOFTCHECKPOINT],    true,  {false, true, false, false, false}},
    {TW_DBUS_IF_DHT,    tw_dbus_rpc_methods_g[TW_DBUS_M_DHTPUT],                   true,  {false, false,false, true,  true}},
    {TW_DBUS_IF_DHT,    tw_dbus_rpc_methods_g[TW_DBUS_M_DHTGET],                   true,  {false, false,false, true,  true}},
    {TW_DBUS_IF_USERS,  tw_dbus_rpc_methods_g[TW_DBUS_M_NEWPOSTMSG],               true,  {false, false,false, true,  false}},
    {TW_DBUS_IF_USERS,  tw_dbus_rpc_methods_g[TW_DBUS_M_NEWDIRECTMSG],             true,  {false, false,false, true,  false}},
    {TW_DBUS_IF_USERS,  tw_dbus_rpc_methods_g[TW_DBUS_M_NEWRTMSG],                 true,  {false, false,false, true,  false}},
    {TW_DBUS_IF_USERS,  tw_dbus_rpc_methods_g[TW_DBUS_M_GETPOSTS],                 true,  {false, false,true,  false, false}},
    {TW_DBUS_IF_USERS,  tw_dbus_rpc_methods_g[TW_DBUS_M_GETDIRECTMSGS],            true,  {false, false,false, true,  false}},
    {TW_DBUS_IF_CORE,   tw_dbus_rpc_methods_g[TW_DBUS_M_SETSPAMMSG],               true,  {false, true, false, false, false}},
    {TW_DBUS_IF_CORE,   tw_dbus_rpc_methods_g[TW_DBUS_M_GETSPAMMSG],               true,  {false, true, false, false, false}},
    {TW_DBUS_IF_USERS,  tw_dbus_rpc_methods_g[TW_DBUS_M_FOLLOW],                   true,  {false, false,false, true,  false}},
    {TW_DBUS_IF_USERS,  tw_dbus_rpc_methods_g[TW_DBUS_M_UNFOLLOW],                 true,  {false, false,false, true,  false}},
    {TW_DBUS_IF_USERS,  tw_dbus_rpc_methods_g[TW_DBUS_M_GETFOLLOWING],             true,  {false, false,false, true,  false}},
    {TW_DBUS_IF_USERS,  tw_dbus_rpc_methods_g[TW_DBUS_M_GETLASTHAVE],              true,  {false, false,false, true,  false}},
    {TW_DBUS_IF_USERS,  tw_dbus_rpc_methods_g[TW_DBUS_M_GETNUMPIECES],             true,  {false, false,false, true,  false}},
    {TW_DBUS_IF_USERS,  tw_dbus_rpc_methods_g[TW_DBUS_M_LISTUSERNAMESPARTIAL],     true,  {false, false,false, true,  false}},
    {TW_DBUS_IF_USERS,  tw_dbus_rpc_methods_g[TW_DBUS_M_RESCANDIRECTMSGS],         true,  {false, false,false, true,  false}},
    {TW_DBUS_IF_USERS,  tw_dbus_rpc_methods_g[TW_DBUS_M_RECHECKUSERTORRENT],       true,  {false, false,false, true,  false}},
    {TW_DBUS_IF_DHT,    tw_dbus_rpc_methods_g[TW_DBUS_M_GETTRENDINGHASHTAGS],      true,  {false, false,false, false, true}},
    {TW_DBUS_IF_CORE,   tw_dbus_rpc_methods_g[TW_DBUS_M_GETSPAMPOSTS],             true,  {false, true, false, false, false}},
    {TW_DBUS_IF_USERS,  tw_dbus_rpc_methods_g[TW_DBUS_M_TORRENTSTATUS],            true,  {false, false,false, true,  false}},
    {TW_DBUS_IF_USERS,  tw_dbus_rpc_methods_g[TW_DBUS_M_USERSEARCH],               true,  {false, false,true,  false, true}}
    //interface         method name                                                isRPC    root twister users  users/* dht
};

map<string, tw_dbus_calls_t *> tw_dbus_call_map_g;

const char *tw_dbus_introspect_xml[TW_DBUS_RPID_COUNT] = {
    /** / **/
    "<!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object Introspection 1.0//EN" "http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd\"> "
    "<node name=\"/twister\"> "
    "</node>",

    /** /twister **/
    "<!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object Introspection 1.0//EN" "http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd\"> "
    "<node name=\"/twister\"> "
        "<interface name=\"org.freedesktop.DBus.Introspectable\"> "
            "<method name=\"Introspect\"> "
                "<arg type=\"s\" name=\"xml_data\" direction=\"out\"/> "
            "</method> "
        "</interface> "
        "<interface name=\"org.freedesktop.DBus.Peer\"> "
            "<method name=\"Ping\" /> "
        "</interface> "
        "<interface name=\"twister.core\"> "
            "<method name=\"listwalletusers\"> "
                "<arg type=\"as\" name=\"walletusers\" direction=\"out\"/> "
            "</method> "
            "<method name=\"stop\"/> "
        "</interface> "
    "</node>",

    /** /twister/users **/
    "<!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object Introspection 1.0//EN" "http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd\"> "
    "<node name=\"/twister/users\"> "
        "<interface name=\"org.freedesktop.DBus.Introspectable\"> "
            "<method name=\"Introspect\"> "
                "<arg type=\"s\" name=\"xml_data\" direction=\"out\"/> "
            "</method> "
        "</interface> "
        "<interface name=\"org.freedesktop.DBus.Peer\"> "
            "<method name=\"Ping\" /> "
        "</interface> "
        "<interface name=\"twister.users\"> "
            "<method name=\"getposts\"> "
                "<arg type=\"i\" name=\"count\" direction=\"in\"/> "
                "<arg type=\"r\" name=\"users\" direction=\"in\"/> "
                "<arg type=\"aa{sv}\" name=\"posts\" direction=\"out\"/> "
            "</method> "
        "</interface> "
    "</node>",

    /** /twister/users/ * **/
    "<!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object Introspection 1.0//EN" "http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd\"> "
    "<node name=\"/twister/users/%s\"> "
        "<interface name=\"org.freedesktop.DBus.Introspectable\"> "
            "<method name=\"Introspect\"> "
                "<arg type=\"s\" name=\"xml_data\" direction=\"out\"/> "
            "</method> "
        "</interface> "
        "<interface name=\"org.freedesktop.DBus.Peer\"> "
            "<method name=\"Ping\" /> "
        "</interface> "
        "<interface name=\"twister.users\"> "
            "<method name=\"dumpprivkey\"> "
                "<arg type=\"s\" name=\"privkey\" direction=\"out\"/> "
            "</method> "
            "<method name=\"dumppubkey\"> "
                "<arg type=\"s\" name=\"pubkey\" direction=\"out\"/> "
            "</method> "
        "</interface> "
        "<interface name=\"twister.dht\"> "
            "<method name=\"dhtget\"> "
                "<arg type=\"s\" name=\"username\" direction=\"in\" /> "
                "<arg type=\"s\" name=\"resource\" direction=\"in\" /> "
                "<arg type=\"s\" name=\"multi\" direction=\"in\" /> "
                "<arg type=\"a{su}\" name=\"timeout\" direction=\"in\" /> "
                "<arg type=\"u\" name=\"minmulti\" direction=\"in\" /> "
                "<arg type=\"aa{sv}\" name=\"value\" direction=\"out\"/> "
            "</method> "
            "<method name=\"dhtput\"> "
                "<arg type=\"s\" name=\"username\" direction=\"in\" /> "
                "<arg type=\"s\" name=\"resource\" direction=\"in\" /> "
                "<arg type=\"s\" name=\"multi\" direction=\"in\" /> "
                "<arg type=\"a{sv}\" name=\"value\" direction=\"in\" /> "
                "<arg type=\"s\" name=\"sig_user\" direction=\"in\"/> "
                "<arg type=\"u\" name=\"seq\" direction=\"in\"/> "
            "</method> "
        "</interface> "
    "</node>",

    /** /twister/dht **/
    "<!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object Introspection 1.0//EN" "http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd\"> "
    "<node name=\"/twister/dht\"> "
        "<interface name=\"org.freedesktop.DBus.Introspectable\"> "
            "<method name=\"Introspect\"> "
                "<arg type=\"s\" name=\"xml_data\" direction=\"out\"/> "
            "</method> "
        "</interface> "
        "<interface name=\"org.freedesktop.DBus.Peer\"> "
            "<method name=\"Ping\" /> "
        "</interface> "
        "<interface name=\"twister.dht\"> "
            "<method name=\"dhtget\"> "
                "<arg type=\"s\" name=\"username\" direction=\"in\" /> "
                "<arg type=\"s\" name=\"resource\" direction=\"in\" /> "
                "<arg type=\"s\" name=\"multi\" direction=\"in\" /> "
                "<arg type=\"a{su}\" name=\"timeout\" direction=\"in\" /> "
                "<arg type=\"u\" name=\"minmulti\" direction=\"in\" /> "
                "<arg type=\"r\" name=\"value\" direction=\"out\"/> "
            "</method> "
            "<method name=\"dhtput\"> "
                "<arg type=\"s\" name=\"username\" direction=\"in\" /> "
                "<arg type=\"s\" name=\"resource\" direction=\"in\" /> "
                "<arg type=\"s\" name=\"multi\" direction=\"in\" /> "
                "<arg type=\"a{sv}\" name=\"value\" direction=\"in\" /> "
                "<arg type=\"s\" name=\"sig_user\" direction=\"in\"/> "
                "<arg type=\"u\" name=\"seq\" direction=\"in\"/> "
            "</method> "
        "</interface> "
    "</node>"
};

dbus_uint32_t tw_dbus_serial_g = 0;
bool tw_dbus_is_run_g = false;

dbus_uint32_t *tw_dbus_serial()
{
    tw_dbus_serial_g++;

    return &tw_dbus_serial_g;
}

void tw_dbus_get_params_from_args(Array &params, DBusMessageIter *args, bool uselast = false, char *label = 0)
{
    Array e_array;
    Object e_object;
    int t = dbus_message_iter_get_arg_type(args);
    switch (t)
    {
    case DBUS_TYPE_ARRAY:
        DBusMessageIter p_array;
        dbus_message_iter_recurse(args, &p_array);

        if (dbus_message_iter_get_arg_type(&p_array) == DBUS_TYPE_DICT_ENTRY)
        {
            e_array.push_back(e_object);
            tw_dbus_get_params_from_args(e_array, &p_array, true);
        }
        else
            tw_dbus_get_params_from_args(e_array, &p_array);

        params.push_back(e_array);
        break;
    case DBUS_TYPE_DICT_ENTRY:
        DBusMessageIter p_dict;
        dbus_message_iter_recurse(args, &p_dict);

        tw_dbus_get_params_from_args(params, &p_dict, true);
        break;
    case DBUS_TYPE_STRUCT:
        DBusMessageIter p_struct;
        dbus_message_iter_recurse(args, &p_struct);

        tw_dbus_get_params_from_args(params, &p_struct);
        break;
    case DBUS_TYPE_VARIANT:
        DBusMessageIter p_variant;
        dbus_message_iter_recurse(args, &p_variant);

        tw_dbus_get_params_from_args(params, &p_variant, uselast, label);
        break;
    case DBUS_TYPE_INT64:
        dbus_int64_t val1;
        dbus_message_iter_get_basic(args, &val1);
        if (uselast && params.back().type() == obj_type)
            params.back().get_obj().push_back(Pair(string(label), val1));
        else
            params.push_back(val1);

        break;
    case DBUS_TYPE_INT32:
        dbus_int32_t val2;
        dbus_message_iter_get_basic(args, &val2);
        if (uselast && params.back().type() == obj_type)
            params.back().get_obj().push_back(Pair(string(label), val2));
        else
            params.push_back(val2);

        break;
    case DBUS_TYPE_STRING:
        char *val3;
        dbus_message_iter_get_basic(args, &val3);
        if (uselast && params.back().type() == obj_type)
        {
            if (label)
                params.back().get_obj().push_back(Pair(string(label), string(val3)));
            else if (dbus_message_iter_next(args))
                tw_dbus_get_params_from_args(params, args, true, val3);
        }
        else
            params.push_back(string(val3));

        break;
    case DBUS_TYPE_DOUBLE:
        double val4;
        dbus_message_iter_get_basic(args, &val4);
        if (uselast && params.back().type() == obj_type)
            params.back().get_obj().push_back(Pair(string(label), val4));
        else
            params.push_back(val4);

        break;
    case DBUS_TYPE_BOOLEAN:
        bool val5;
        dbus_message_iter_get_basic(args, &val5);
        if (uselast && params.back().type() == obj_type)
            params.back().get_obj().push_back(Pair(string(label), val5));
        else
            params.push_back(val5);

        break;
    default:
        fprintf(stderr, "unsupported data type\n");
    }

    if (dbus_message_iter_next(args))
        tw_dbus_get_params_from_args(params, args);
}


Array tw_dbus_get_params_from_message(DBusMessage *mess, Array &params)
{
    DBusMessageIter args;

    if (!dbus_message_iter_init(mess, &args))
    {
        fprintf(stdout, "Message has no arguments!\n");
        return params;
    }

    tw_dbus_get_params_from_args(params, &args);
    return params;
}

DBusMessageIter *tw_dbus_fill_args(Value &ret, DBusMessageIter *args, bool isVariant = false, int level = 0)
{
    if (ret.type() == array_type)
    {
        Array arr = ret.get_array();
        DBusMessageIter p_array, p_variant;
        bool isForced = false;

        if (!isVariant && arr.size() > 0 && arr[0].type() == obj_type)
        {
            if (!dbus_message_iter_open_container(args, DBUS_TYPE_ARRAY, "a{sv}", &p_array))
            {
                fprintf(stderr, "Out Of Memory!\n");
                return 0;
            }
        }
        else if (!isVariant)
        {
            isForced = true;
            if (!dbus_message_iter_open_container(args, DBUS_TYPE_ARRAY, "v", &p_array))
            {
                fprintf(stderr, "Out Of Memory!\n");
                return 0;
            }
        }
        else
        {
            if (!dbus_message_iter_open_container(args, DBUS_TYPE_VARIANT, "av", &p_variant))
            {
                fprintf(stderr, "Out Of Memory!\n");
                return 0;
            }
            if (!dbus_message_iter_open_container(&p_variant, DBUS_TYPE_ARRAY, "v", &p_array))
            {
                fprintf(stderr, "Out Of Memory!\n");
                return 0;
            }
        }

        if (arr.size() == 0)
            arr.push_back("no data!");

        for(unsigned int i = 0; i < arr.size(); i++)
        {
            if (!tw_dbus_fill_args(arr[i], &p_array, isVariant || isForced, level+1))
                return 0;
        }

        if (isVariant)
        {
            if (!dbus_message_iter_close_container(&p_variant, &p_array))
            {
                fprintf(stderr, "Out Of Memory!\n");
                return 0;
            }
            if (!dbus_message_iter_close_container(args, &p_variant))
            {
                fprintf(stderr, "Out Of Memory!\n");
                return 0;
            }
        }
        else if (!dbus_message_iter_close_container(args, &p_array))
        {
            fprintf(stderr, "Out Of Memory!\n");
            return 0;
        }

        fflush(stdout);
    }
    else if (ret.type() == obj_type)
    {
        Object obj = ret.get_obj();
        DBusMessageIter p_struct, p_variant, p_array;

        if (isVariant)
        {
            if (!dbus_message_iter_open_container(args, DBUS_TYPE_VARIANT, "a{sv}", &p_variant))
            {
                fprintf(stderr, "Out Of Memory!\n");
                return 0;
            }
            if (!dbus_message_iter_open_container(&p_variant, DBUS_TYPE_ARRAY, "{sv}", &p_array))
            {
                fprintf(stderr, "Out Of Memory!\n");
                return 0;
            }
        }
        else  if (!dbus_message_iter_open_container(args, DBUS_TYPE_ARRAY, "{sv}", &p_array))
        {
            fprintf(stderr, "Out Of Memory!\n");
            return 0;
        }

        for (Object::const_iterator i = obj.begin(); i != obj.end(); ++i)
        {
            Value name(i->name_);
            Value val(i->value_);

            DBusMessageIter p_dict;

            if (!dbus_message_iter_open_container(&p_array, DBUS_TYPE_DICT_ENTRY, 0, &p_dict))
            {
                fprintf(stderr, "Out Of Memory!\n");
                return 0;
            }
            if (!tw_dbus_fill_args(name, &p_dict, false, level+1))
            {
                fprintf(stderr, "Out Of Memory!\n");
                return 0;
            }
            if (!tw_dbus_fill_args(val, &p_dict, true, level+1))
            {
                fprintf(stderr, "Out Of Memory!\n");
                return 0;
            }
            if (!dbus_message_iter_close_container(&p_array, &p_dict))
            {
                fprintf(stderr, "Out Of Memory!\n");
                return 0;
            }
        }

        if (isVariant)
        {
            if (!dbus_message_iter_close_container(&p_variant, &p_array))
            {
                fprintf(stderr, "Out Of Memory!\n");
                return 0;
            }
            if (!dbus_message_iter_close_container(args, &p_variant))
            {
                fprintf(stderr, "Out Of Memory!\n");
                return 0;
            }
        }
        else if (!dbus_message_iter_close_container(args, &p_array))
        {
            fprintf(stderr, "Out Of Memory!\n");
            return 0;
        }
    }
    else if (ret.type() == str_type)
    {
        DBusMessageIter *args2;
        DBusMessageIter p_variant;
        if (isVariant)
        {
            args2 = args;
            if (!dbus_message_iter_open_container(args2, DBUS_TYPE_VARIANT, "s", &p_variant))
            {
                fprintf(stderr, "Out Of Memory!\n");
                return 0;
            }
            args = &p_variant;
        }

        const char *val = ret.get_str().c_str();
        if (!dbus_message_iter_append_basic(args, DBUS_TYPE_STRING, &val))
        {
            fprintf(stderr, "Out Of Memory!\n");
            return 0;
        }

        if (isVariant)
        {
            if (!dbus_message_iter_close_container(args2, &p_variant))
            {
                fprintf(stderr, "Out Of Memory!\n");
                return 0;
            }
            args = args2;
        }
    }
    else if (ret.type() == int_type)
    {
        DBusMessageIter *args2;
        DBusMessageIter p_variant;

        if (isVariant)
        {
            args2 = args;
            if (!dbus_message_iter_open_container(args2, DBUS_TYPE_VARIANT, "x", &p_variant))
            {
                fprintf(stderr, "Out Of Memory!\n");
                return 0;
            }
            args = &p_variant;
        }

        dbus_int64_t val = ret.get_int64();
        if (!dbus_message_iter_append_basic(args, DBUS_TYPE_INT64, &val))
        {
            fprintf(stderr, "Out Of Memory!\n");
            return 0;
        }

        if (isVariant)
        {
            if (!dbus_message_iter_close_container(args2, &p_variant))
            {
                fprintf(stderr, "Out Of Memory!\n");
                return 0;
            }
            args = args2;
        }
    }
    else if (ret.type() == real_type)
    {
        DBusMessageIter *args2;
        DBusMessageIter p_variant;

        if (isVariant)
        {
            args2 = args;
            if (!dbus_message_iter_open_container(args2, DBUS_TYPE_VARIANT, "d", &p_variant))
            {
                fprintf(stderr, "Out Of Memory!\n");
                return 0;
            }
            args = &p_variant;
        }

        double val = ret.get_real();
        if (!dbus_message_iter_append_basic(args, DBUS_TYPE_DOUBLE, &val))
        {
            fprintf(stderr, "Out Of Memory!\n");
            return 0;
        }

        if (isVariant)
        {
            if (!dbus_message_iter_close_container(args2, &p_variant))
            {
                fprintf(stderr, "Out Of Memory!\n");
                return 0;
            }
            args = args2;
        }
    }
    else if (ret.type() == bool_type)
    {
        DBusMessageIter *args2;
        DBusMessageIter p_variant;

        if (isVariant)
        {
            args2 = args;
            if (!dbus_message_iter_open_container(args2, DBUS_TYPE_VARIANT, "b", &p_variant))
            {
                fprintf(stderr, "Out Of Memory!\n");
                return 0;
            }
            args = &p_variant;
        }

        dbus_bool_t val = ret.get_bool();
        if (!dbus_message_iter_append_basic(args, DBUS_TYPE_BOOLEAN, &val))
        {
            fprintf(stderr, "Out Of Memory!\n");
            return 0;
        }

        if (isVariant)
        {
            if (!dbus_message_iter_close_container(args2, &p_variant))
            {
                fprintf(stderr, "Out Of Memory!\n");
                return 0;
            }
            args = args2;
        }
    }
    else
    {
        DBusMessageIter *args2;
        DBusMessageIter p_variant;
        if (isVariant)
        {
            args2 = args;
            if (!dbus_message_iter_open_container(args2, DBUS_TYPE_VARIANT, "s", &p_variant))
            {
                fprintf(stderr, "Out Of Memory!\n");
                return 0;
            }
            args = &p_variant;
        }

        const char *val = "NO VALUE!";
        if (!dbus_message_iter_append_basic(args, DBUS_TYPE_STRING, &val))
        {
            fprintf(stderr, "Out Of Memory!\n");
            return 0;
        }

        if (isVariant)
        {
            if (!dbus_message_iter_close_container(args2, &p_variant))
            {
                fprintf(stderr, "Out Of Memory!\n");
                return 0;
            }
            args = args2;
        }
    }

    return args;
}

DBusHandlerResult tw_dbus_replyping(DBusConnection* conn, DBusMessage* mess)
{
    DBusMessage* reply;

    reply = dbus_message_new_method_return(mess);

    if (!dbus_connection_send(conn, reply, tw_dbus_serial()))
    {
        fprintf(stderr, "Out Of Memory!\n");
        return DBUS_HANDLER_RESULT_NEED_MEMORY;
    }
    dbus_connection_flush(conn);

    dbus_message_unref(reply);

    return DBUS_HANDLER_RESULT_HANDLED;
}

DBusHandlerResult tw_dbus_introspect(DBusConnection *conn, DBusMessage *mess, tw_dbus_path_t *twdp)
{
    DBusMessage* reply;
    DBusMessageIter args;
    const char *xml;

    if (twdp->rpid == TW_DBUS_RPID_USER && twdp->udata1)
    {
        char xmltmp[10240];

        int l = sprintf(xmltmp, tw_dbus_introspect_xml[twdp->rpid], (char *)twdp->udata1);
        xmltmp[l] = 0;

        xml = xmltmp;
    }
    else
        xml = tw_dbus_introspect_xml[twdp->rpid];

    reply = dbus_message_new_method_return(mess);

    dbus_message_iter_init_append(reply, &args);
    if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &xml))
    {
        fprintf(stderr, "Out Of Memory!\n");
        return DBUS_HANDLER_RESULT_NEED_MEMORY;
    }

    if (!dbus_connection_send(conn, reply, tw_dbus_serial())) {
        fprintf(stderr, "Out Of Memory!\n");
        return DBUS_HANDLER_RESULT_NEED_MEMORY;
    }
    dbus_connection_flush(conn);

    dbus_message_unref(reply);

    return DBUS_HANDLER_RESULT_HANDLED;
}

Array tw_get_wallet_users()
{
    Array params;

    return listwalletusers(params, false).get_array();
}

DBusHandlerResult tw_dbus_runRPC(DBusConnection *conn, DBusMessage *mess, Array &params)
{
    DBusMessage *reply;
    DBusMessageIter args;

    tw_dbus_get_params_from_message(mess, params);

    Value val = tableRPC.execute(string(dbus_message_get_member(mess)), params);

    reply = dbus_message_new_method_return(mess);

    dbus_message_iter_init_append(reply, &args);
    if (!tw_dbus_fill_args(val, &args))
    {
        fprintf(stderr, "Out Of Memory!\n");
        dbus_message_unref(reply);
        return DBUS_HANDLER_RESULT_NEED_MEMORY;
    }

    if (!dbus_connection_send(conn, reply, tw_dbus_serial())) {
        fprintf(stderr, "Out Of Memory!\n");
        dbus_message_unref(reply);
        return DBUS_HANDLER_RESULT_NEED_MEMORY;
    }

    dbus_message_unref(reply);

    return DBUS_HANDLER_RESULT_HANDLED;
}

DBusHandlerResult tw_dbus_dispatch(DBusConnection *conn, DBusMessage *mess, void *udata)
{
    Array params;
    tw_dbus_path_t *twdp = (tw_dbus_path_t *) udata;

    if (twdp->rpid == TW_DBUS_RPID_USER && twdp->udata1)
        params.push_back(string((char *)twdp->udata1));

    if (dbus_message_is_method_call(mess, "org.freedesktop.DBus.Introspectable", "Introspect"))
        return tw_dbus_introspect(conn, mess, twdp);

    if (dbus_message_is_method_call(mess, "org.freedesktop.DBus.Peer", "Ping"))
        return tw_dbus_replyping(conn, mess);

    map<string, tw_dbus_calls_t *>::const_iterator callit = tw_dbus_call_map_g.find(string(dbus_message_get_member(mess)));

    if (callit != tw_dbus_call_map_g.end() &&
        callit->second->interface == dbus_message_get_interface(mess) &&
        callit->second->permissions[twdp->rpid])
            return tw_dbus_runRPC(conn, mess, params);

    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

DBusHandlerResult tw_dbus_dispatch_FB(DBusConnection *conn, DBusMessage *mess, void *udata)
{
    DBusMessage* reply;
    DBusMessageIter args;
    const char *res = "user/command/object not found...";

    fprintf(stdout, ">> line=%d ; path=%s ; if=%s ; m/s=%s \n", __LINE__, dbus_message_get_path(mess), dbus_message_get_interface(mess), dbus_message_get_member(mess));

    reply = dbus_message_new_method_return(mess);

    dbus_message_iter_init_append(reply, &args);
    if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &res))
    {
        fprintf(stderr, "Out Of Memory!\n");
        return DBUS_HANDLER_RESULT_NEED_MEMORY;
    }

    if (!dbus_connection_send(conn, reply, tw_dbus_serial())) {
        fprintf(stderr, "Out Of Memory!\n");
        return DBUS_HANDLER_RESULT_NEED_MEMORY;
    }
    dbus_connection_flush(conn);

    dbus_message_unref(reply);

    return DBUS_HANDLER_RESULT_HANDLED;
}

void StopDBUSThread()
{
    tw_dbus_is_run_g = false;
}

void DBusThread()
{
    DBusError err;
    DBusConnection* conn;
    int ret;
    dbus_error_init(&err);

    conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "Connection Error (%s)\n", err.message);
        dbus_error_free(&err);
    }
    if (NULL == conn) {
        return;
    }

    ret = dbus_bus_request_name(conn, "co.net.twister",
            DBUS_NAME_FLAG_REPLACE_EXISTING
            , &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "Name Error (%s)\n", err.message);
        dbus_error_free(&err);
    }
    if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret) {
        return;
    }

    DBusObjectPathVTable twisterPfb;
    twisterPfb.message_function = tw_dbus_dispatch_FB;

    tw_dbus_path_t twfbp;
    bzero(&twfbp, sizeof(tw_dbus_path_t));

    twfbp.rpid = TW_DBUS_RPID_ROOT;

    if (!dbus_connection_register_fallback(conn, "/", &twisterPfb, &twfbp))
    {
        fprintf(stderr, "registration failed %d\n", __LINE__);
        return;
    }

    DBusObjectPathVTable twisterPath;
    twisterPath.message_function = tw_dbus_dispatch;

    tw_dbus_path_t twp;
    bzero(&twp, sizeof(tw_dbus_path_t));

    twp.rpid = TW_DBUS_RPID_TWISTER;

    if (!dbus_connection_register_object_path(conn, TW_DBUS_PATH_TWISTER, &twisterPath, &twp))
    {
        fprintf(stderr, "registration failed %d\n", __LINE__);
        return;
    }

    DBusObjectPathVTable twisterDHTPath;
    twisterDHTPath.message_function = tw_dbus_dispatch;

    tw_dbus_path_t twdp;
    bzero(&twdp, sizeof(tw_dbus_path_t));

    twdp.rpid = TW_DBUS_RPID_DHT;

    if (!dbus_connection_register_object_path(conn, TW_DBUS_PATH_DHT, &twisterDHTPath, &twdp))
    {
        fprintf(stderr, "registration failed %d\n", __LINE__);
        return;
    }

    DBusObjectPathVTable usersPfb;
    usersPfb.message_function = tw_dbus_dispatch;

    tw_dbus_path_t twup;
    bzero(&twup, sizeof(tw_dbus_path_t));

    twup.rpid = TW_DBUS_RPID_USERS;

    if (!dbus_connection_register_object_path(conn, TW_DBUS_PATH_USERS, &usersPfb, &twup))
    {
        fprintf(stderr, "registration failed %d\n", __LINE__);
        return;
    }

    Array userlist = tw_get_wallet_users();
    DBusObjectPathVTable userPath;
    userPath.message_function = tw_dbus_dispatch;

    tw_dbus_path_t twwup[8];

    for (unsigned int i = 0; i < userlist.size(); i++)
    {
        char upathstr[32] = TW_DBUS_PATH_USERS "/";

        bzero(&twwup[i], sizeof(tw_dbus_path_t));

        twwup[i].rpid = TW_DBUS_RPID_USER;
        twwup[i].udata1 = userlist[i].get_str().c_str();

        if (!dbus_connection_register_object_path(conn, strcat(upathstr, userlist[i].get_str().c_str()), &userPath, &twwup[i]))
            fprintf(stderr, "path registration failed for %s - %d\n", (char *)userlist[i].get_str().c_str(), __LINE__);
    }

    for (int i = 0; i < TW_DBUS_M_CNT; i++)
    {
        tw_dbus_call_map_g[tw_dbus_rpc_methods_g[i]] = &tw_dbus_call_permissions_g[i];
    }

    tw_dbus_is_run_g = true;

    while(tw_dbus_is_run_g)
    {
        if (dbus_connection_read_write(conn, 0))
        {
            dbus_connection_dispatch(conn);
        }
        else
            break;

        sleep(1);
    }

    dbus_connection_unref(conn);
    conn = 0;
}
