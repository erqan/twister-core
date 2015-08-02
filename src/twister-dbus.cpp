/**
  * DBUS entegration for twister P2P
  *
  * Erkan Yüksel © 2014-2015
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
    TW_DBUS_M_DHTPUTRAW,
    TW_DBUS_M_NEWPOSTMSG,
    TW_DBUS_M_NEWPOSTRAW,
    TW_DBUS_M_NEWDIRECTMSG,
    TW_DBUS_M_NEWRTMSG,
    TW_DBUS_M_GETPOSTS,
    TW_DBUS_M_GETMENTIONS,
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
    TW_DBUS_M_SEARCH,
    TW_DBUS_M_LISTPERMISSIONS,
    TW_DBUS_M_CHECKOLDMENTIONS,
    TW_DBUS_M_RESCANMENTIONS,
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
    int permissions[TW_DBUS_RPID_COUNT];
} tw_dbus_calls_t;

typedef struct tw_dbus_path_t
{
    TW_DBUS_RPID rpid;
    const void *udata1;
    const void *udata2;
} tw_dbus_path_t;

string tw_dbus_methodes_g[] = {
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
    "dhtputraw",
    "newpostmsg",
    "newpostraw",
    "newdirectmsg",
    "newrtmsg",
    "getposts",
    "getmentions",
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
    "search",
    "listpermissions",
    "checkoldmentions",
    "rescanmentions"
};

tw_dbus_calls_t tw_dbus_call_permissions_g[] = {
    //interface         method name                                             isRPC   root twister users  users/* dht
    {TW_DBUS_IF_CORE,   tw_dbus_methodes_g[TW_DBUS_M_HELP],                     true,  {FALSE, TRUE, FALSE, FALSE, FALSE}},
    {TW_DBUS_IF_CORE,   tw_dbus_methodes_g[TW_DBUS_M_STOP],                     true,  {FALSE, TRUE, FALSE, FALSE, FALSE}},
    {TW_DBUS_IF_CORE,   tw_dbus_methodes_g[TW_DBUS_M_GETBLOCKCOUNT],            true,  {FALSE, TRUE, FALSE, FALSE, FALSE}},
    {TW_DBUS_IF_CORE,   tw_dbus_methodes_g[TW_DBUS_M_GETBESTBLOCKHASH],         true,  {FALSE, TRUE, FALSE, FALSE, FALSE}},
    {TW_DBUS_IF_CORE,   tw_dbus_methodes_g[TW_DBUS_M_GETCONNECTIONCOUNT],       true,  {FALSE, TRUE, FALSE, FALSE, FALSE}},
    {TW_DBUS_IF_CORE,   tw_dbus_methodes_g[TW_DBUS_M_GETPEERINFO],              true,  {FALSE, TRUE, FALSE, FALSE, FALSE}},
    {TW_DBUS_IF_CORE,   tw_dbus_methodes_g[TW_DBUS_M_ADDNODE],                  true,  {FALSE, TRUE, FALSE, FALSE, FALSE}},
    {TW_DBUS_IF_CORE,   tw_dbus_methodes_g[TW_DBUS_M_ADDDNSSEED],               true,  {FALSE, TRUE, FALSE, FALSE, FALSE}},
    {TW_DBUS_IF_CORE,   tw_dbus_methodes_g[TW_DBUS_M_GETADDEDNODEINFO],         true,  {FALSE, TRUE, FALSE, FALSE, FALSE}},
    {TW_DBUS_IF_CORE,   tw_dbus_methodes_g[TW_DBUS_M_GETDIFFICULTY],            true,  {FALSE, TRUE, FALSE, FALSE, FALSE}},
    {TW_DBUS_IF_CORE,   tw_dbus_methodes_g[TW_DBUS_M_GETGENERATE],              true,  {FALSE, TRUE, FALSE, FALSE, FALSE}},
    {TW_DBUS_IF_CORE,   tw_dbus_methodes_g[TW_DBUS_M_SETGENERATE],              true,  {FALSE, TRUE, FALSE, FALSE, FALSE}},
    {TW_DBUS_IF_CORE,   tw_dbus_methodes_g[TW_DBUS_M_GETHASHESPERSEC],          true,  {FALSE, TRUE, FALSE, FALSE, FALSE}},
    {TW_DBUS_IF_CORE,   tw_dbus_methodes_g[TW_DBUS_M_GETINFO],                  true,  {FALSE, TRUE, FALSE, FALSE, FALSE}},
    {TW_DBUS_IF_CORE,   tw_dbus_methodes_g[TW_DBUS_M_GETMININGINFO],            true,  {FALSE, TRUE, FALSE, FALSE, FALSE}},
    {TW_DBUS_IF_USERS,  tw_dbus_methodes_g[TW_DBUS_M_CREATEWALLETUSER],         true,  {FALSE, TRUE, TRUE,  FALSE, FALSE}},
    {TW_DBUS_IF_USERS,  tw_dbus_methodes_g[TW_DBUS_M_LISTWALLETUSERS],          true,  {FALSE, TRUE, TRUE,  FALSE, FALSE}},
    {TW_DBUS_IF_CORE,   tw_dbus_methodes_g[TW_DBUS_M_BACKUPWALLET],             true,  {FALSE, TRUE, FALSE, FALSE, FALSE}},
    {TW_DBUS_IF_CORE,   tw_dbus_methodes_g[TW_DBUS_M_WALLETPASSPHRASE],         true,  {FALSE, TRUE, FALSE, FALSE, FALSE}},
    {TW_DBUS_IF_CORE,   tw_dbus_methodes_g[TW_DBUS_M_WALLETPASSPHRASECHANGE],   true,  {FALSE, TRUE, FALSE, FALSE, FALSE}},
    {TW_DBUS_IF_CORE,   tw_dbus_methodes_g[TW_DBUS_M_WALLETLOCK],               true,  {FALSE, TRUE, FALSE, FALSE, FALSE}},
    {TW_DBUS_IF_CORE,   tw_dbus_methodes_g[TW_DBUS_M_ENCRYPTWALLET],            true,  {FALSE, TRUE, FALSE, FALSE, FALSE}},
    {TW_DBUS_IF_CORE,   tw_dbus_methodes_g[TW_DBUS_M_GETRAWMEMPOOL],            true,  {FALSE, TRUE, FALSE, FALSE, FALSE}},
    {TW_DBUS_IF_CORE,   tw_dbus_methodes_g[TW_DBUS_M_GETBLOCK],                 true,  {FALSE, TRUE, FALSE, FALSE, FALSE}},
    {TW_DBUS_IF_CORE,   tw_dbus_methodes_g[TW_DBUS_M_GETBLOCKHASH],             true,  {FALSE, TRUE, FALSE, FALSE, FALSE}},
    {TW_DBUS_IF_CORE,   tw_dbus_methodes_g[TW_DBUS_M_GETTRANSACTION],           true,  {FALSE, TRUE, FALSE, FALSE, FALSE}},
    {TW_DBUS_IF_CORE,   tw_dbus_methodes_g[TW_DBUS_M_LISTTRANSACTIONS],         true,  {FALSE, TRUE, FALSE, FALSE, FALSE}},
    {TW_DBUS_IF_CORE,   tw_dbus_methodes_g[TW_DBUS_M_SIGNMESSAGE],              true,  {FALSE, TRUE, FALSE, FALSE, FALSE}},
    {TW_DBUS_IF_CORE,   tw_dbus_methodes_g[TW_DBUS_M_VERIFYMESSAGE],            true,  {FALSE, TRUE, FALSE, FALSE, FALSE}},
    {TW_DBUS_IF_CORE,   tw_dbus_methodes_g[TW_DBUS_M_GETWORK],                  true,  {FALSE, TRUE, FALSE, FALSE, FALSE}},
    {TW_DBUS_IF_CORE,   tw_dbus_methodes_g[TW_DBUS_M_GETBLOCKTEMPLATE],         true,  {FALSE, TRUE, FALSE, FALSE, FALSE}},
    {TW_DBUS_IF_CORE,   tw_dbus_methodes_g[TW_DBUS_M_SUBMITBLOCK],              true,  {FALSE, TRUE, FALSE, FALSE, FALSE}},
    {TW_DBUS_IF_CORE,   tw_dbus_methodes_g[TW_DBUS_M_LISTSINCEBLOCK],           true,  {FALSE, TRUE, FALSE, FALSE, FALSE}},
    {TW_DBUS_IF_USERS,  tw_dbus_methodes_g[TW_DBUS_M_DUMPPRIVKEY],              true,  {FALSE, FALSE,FALSE, TRUE,  FALSE}},
    {TW_DBUS_IF_USERS,  tw_dbus_methodes_g[TW_DBUS_M_DUMPPUBKEY],               true,  {FALSE, FALSE,FALSE, TRUE,  FALSE}},
    {TW_DBUS_IF_CORE,   tw_dbus_methodes_g[TW_DBUS_M_DUMPWALLET],               true,  {FALSE, TRUE, FALSE, FALSE, FALSE}},
    {TW_DBUS_IF_CORE,   tw_dbus_methodes_g[TW_DBUS_M_IMPORTPRIVKEY],            true,  {FALSE, TRUE, FALSE, FALSE, FALSE}},
    {TW_DBUS_IF_CORE,   tw_dbus_methodes_g[TW_DBUS_M_IMPORTWALLET],             true,  {FALSE, TRUE, FALSE, FALSE, FALSE}},
    {TW_DBUS_IF_CORE,   tw_dbus_methodes_g[TW_DBUS_M_GETRAWTRANSACTION],        true,  {FALSE, TRUE, FALSE, FALSE, FALSE}},
    {TW_DBUS_IF_CORE,   tw_dbus_methodes_g[TW_DBUS_M_CREATERAWTRANSACTION],     true,  {FALSE, TRUE, FALSE, FALSE, FALSE}},
    {TW_DBUS_IF_CORE,   tw_dbus_methodes_g[TW_DBUS_M_DECODERAWTRANSACTION],     true,  {FALSE, TRUE, FALSE, FALSE, FALSE}},
    {TW_DBUS_IF_CORE,   tw_dbus_methodes_g[TW_DBUS_M_SENDRAWTRANSACTION],       true,  {FALSE, TRUE, FALSE, FALSE, FALSE}},
    {TW_DBUS_IF_USERS,  tw_dbus_methodes_g[TW_DBUS_M_SENDNEWUSERTRANSACTION],   true,  {FALSE, TRUE, TRUE,  FALSE, FALSE}},
    {TW_DBUS_IF_CORE,   tw_dbus_methodes_g[TW_DBUS_M_VERIFYCHAIN],              true,  {FALSE, TRUE, FALSE, FALSE, FALSE}},
    {TW_DBUS_IF_CORE,   tw_dbus_methodes_g[TW_DBUS_M_GETLASTSOFTCHECKPOINT],    true,  {FALSE, TRUE, FALSE, FALSE, FALSE}},
    {TW_DBUS_IF_DHT,    tw_dbus_methodes_g[TW_DBUS_M_DHTPUT],                   true,  {FALSE, FALSE,FALSE, TRUE,  TRUE}},
    {TW_DBUS_IF_DHT,    tw_dbus_methodes_g[TW_DBUS_M_DHTGET],                   true,  {FALSE, FALSE,FALSE, TRUE,  TRUE}},
    {TW_DBUS_IF_DHT,    tw_dbus_methodes_g[TW_DBUS_M_DHTPUTRAW],                true,  {FALSE, FALSE,FALSE, TRUE,  TRUE}},
    {TW_DBUS_IF_USERS,  tw_dbus_methodes_g[TW_DBUS_M_NEWPOSTMSG],               true,  {FALSE, FALSE,FALSE, TRUE,  FALSE}},
    {TW_DBUS_IF_USERS,  tw_dbus_methodes_g[TW_DBUS_M_NEWPOSTRAW],               true,  {FALSE, FALSE,TRUE,  TRUE,  FALSE}},
    {TW_DBUS_IF_USERS,  tw_dbus_methodes_g[TW_DBUS_M_NEWDIRECTMSG],             true,  {FALSE, FALSE,FALSE, TRUE,  FALSE}},
    {TW_DBUS_IF_USERS,  tw_dbus_methodes_g[TW_DBUS_M_NEWRTMSG],                 true,  {FALSE, FALSE,FALSE, TRUE,  FALSE}},
    {TW_DBUS_IF_USERS,  tw_dbus_methodes_g[TW_DBUS_M_GETPOSTS],                 true,  {FALSE, FALSE,TRUE,  FALSE, FALSE}},
    {TW_DBUS_IF_USERS,  tw_dbus_methodes_g[TW_DBUS_M_GETMENTIONS],              true,  {FALSE, FALSE,FALSE, TRUE,  FALSE}},
    {TW_DBUS_IF_USERS,  tw_dbus_methodes_g[TW_DBUS_M_GETDIRECTMSGS],            true,  {FALSE, FALSE,FALSE, TRUE,  FALSE}},
    {TW_DBUS_IF_CORE,   tw_dbus_methodes_g[TW_DBUS_M_SETSPAMMSG],               true,  {FALSE, TRUE, FALSE, FALSE, FALSE}},
    {TW_DBUS_IF_CORE,   tw_dbus_methodes_g[TW_DBUS_M_GETSPAMMSG],               true,  {FALSE, TRUE, FALSE, FALSE, FALSE}},
    {TW_DBUS_IF_USERS,  tw_dbus_methodes_g[TW_DBUS_M_FOLLOW],                   true,  {FALSE, FALSE,FALSE, TRUE,  FALSE}},
    {TW_DBUS_IF_USERS,  tw_dbus_methodes_g[TW_DBUS_M_UNFOLLOW],                 true,  {FALSE, FALSE,FALSE, TRUE,  FALSE}},
    {TW_DBUS_IF_USERS,  tw_dbus_methodes_g[TW_DBUS_M_GETFOLLOWING],             true,  {FALSE, FALSE,FALSE, TRUE,  FALSE}},
    {TW_DBUS_IF_USERS,  tw_dbus_methodes_g[TW_DBUS_M_GETLASTHAVE],              true,  {FALSE, FALSE,FALSE, TRUE,  FALSE}},
    {TW_DBUS_IF_USERS,  tw_dbus_methodes_g[TW_DBUS_M_GETNUMPIECES],             true,  {FALSE, FALSE,FALSE, TRUE,  FALSE}},
    {TW_DBUS_IF_USERS,  tw_dbus_methodes_g[TW_DBUS_M_LISTUSERNAMESPARTIAL],     true,  {FALSE, FALSE,TRUE,  FALSE, FALSE}},
    {TW_DBUS_IF_USERS,  tw_dbus_methodes_g[TW_DBUS_M_RESCANDIRECTMSGS],         true,  {FALSE, FALSE,FALSE, TRUE,  FALSE}},
    {TW_DBUS_IF_USERS,  tw_dbus_methodes_g[TW_DBUS_M_RECHECKUSERTORRENT],       true,  {FALSE, FALSE,TRUE,  TRUE,  FALSE}},
    {TW_DBUS_IF_DHT,    tw_dbus_methodes_g[TW_DBUS_M_GETTRENDINGHASHTAGS],      true,  {FALSE, FALSE,FALSE, FALSE, TRUE}},
    {TW_DBUS_IF_CORE,   tw_dbus_methodes_g[TW_DBUS_M_GETSPAMPOSTS],             true,  {FALSE, TRUE, FALSE, FALSE, FALSE}},
    {TW_DBUS_IF_USERS,  tw_dbus_methodes_g[TW_DBUS_M_TORRENTSTATUS],            true,  {FALSE, FALSE,TRUE,  TRUE,  FALSE}},
    {TW_DBUS_IF_CORE,   tw_dbus_methodes_g[TW_DBUS_M_SEARCH],                   true,  {FALSE, TRUE, TRUE,  FALSE, FALSE}},
    //interface         method name                                             isRPC    root twister users  users/* dht
    {TW_DBUS_IF_CORE,   tw_dbus_methodes_g[TW_DBUS_M_LISTPERMISSIONS],          false, {TRUE,  TRUE, FALSE, FALSE, FALSE}},
    {TW_DBUS_IF_USERS,  tw_dbus_methodes_g[TW_DBUS_M_CHECKOLDMENTIONS],         true,  {FALSE, FALSE,FALSE, TRUE,  FALSE}},
    {TW_DBUS_IF_USERS,  tw_dbus_methodes_g[TW_DBUS_M_RESCANMENTIONS],           true,  {FALSE, FALSE,FALSE, TRUE,  FALSE}},
};

map<string, tw_dbus_calls_t *> tw_dbus_call_map_g;

const char *tw_dbus_introspect_xml[TW_DBUS_RPID_COUNT] = {
    /** / **/
    "<!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object Introspection 1.0//EN\" "
                          "\"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd\">\n"
    "<node>\n"
    "    <interface name=\"org.freedesktop.DBus.Introspectable\">\n"
    "        <method name=\"Introspect\">\n"
    "            <arg type=\"s\" name=\"xml_data\" direction=\"out\"/>\n"
    "        </method>\n"
    "    </interface>\n"
    "    <interface name=\"org.freedesktop.DBus.Peer\">\n"
    "        <method name=\"Ping\" />\n"
    "    </interface>\n"
    "    <interface name=\"twister.core\">\n"
    "        <method name=\"listpermissions\">\n"
    "            <arg type=\"a{sv}\" name=\"permissions\" direction=\"out\"/>\n"
    "        </method>\n"
    "    </interface>\n"
    "    <node name=\"twister\" />\n"
    "</node>",

    /** /twister **/
    "<!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object Introspection 1.0//EN\" "
                          "\"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd\">\n"
    "<node name=\"twister\">\n"
    "    <interface name=\"org.freedesktop.DBus.Introspectable\">\n"
    "        <method name=\"Introspect\">\n"
    "            <arg type=\"s\" name=\"xml_data\" direction=\"out\"/>\n"
    "        </method>\n"
    "    </interface>\n"
    "    <interface name=\"org.freedesktop.DBus.Peer\">\n"
    "        <method name=\"Ping\" />\n"
    "    </interface>\n"
    "    <interface name=\"twister.core\">\n"
    "        <method name=\"listpermissions\">\n"
    "            <arg type=\"a{sv}\" name=\"permissions\" direction=\"out\"/>\n"
    "        </method>\n"
    "        <method name=\"stop\">\n"
    "            <arg type=\"s\" direction=\"out\"/>\n"
    "        </method>\n"
    "        <method name=\"search\">\n"
    "            <arg type=\"s\" name=\"scope\" dircetion=\"in\"/>\n"
    "            <arg type=\"s\" name=\"text\" dircetion=\"in\"/>\n"
    "            <arg type=\"i\" name=\"count\" direction=\"in\"/>\n"
    "            <arg type=\"a{sv}\" name=\"options\" direction=\"in\"/>\n"
    "            <arg type=\"aa{sv}\" direction=\"out\"/>\n"
    "        </method>\n"
    "        <method name=\"getspamposts\">\n"
    "            <arg type=\"i\" name=\"count\" direction=\"in\"/>\n"
    "            <arg type=\"i\" name=\"maxid\" direction=\"in\"/>\n"
    "            <arg type=\"i\" name=\"sinceid\" direction=\"in\"/>\n"
    "            <arg type=\"aa{sv}\" direction=\"out\"/>\n"
    "        </method>\n"
    "        <method name=\"getspammsg\">\n"
    "            <arg type=\"as\" direction=\"out\"/>\n"
    "        </method>\n"
    "        <method name=\"setspammsg\">\n"
    "            <arg type=\"s\" name=\"spamuser\" direction=\"in\"/>\n"
    "            <arg type=\"s\" name=\"spammsg\" direction=\"in\"/>\n"
    "        </method>\n"
    "        <method name=\"getlastsoftcheckpoint\">\n"
    "            <arg type=\"a{sv}\" direction=\"out\"/>\n"
    "        </method>\n"
    "        <method name=\"verifychain\">\n"
    "            <arg type=\"i\" name=\"level\" direction=\"in\"/>\n"
    "            <arg type=\"i\" name=\"block_count\" direction=\"in\"/>\n"
    "            <arg type=\"b\" direction=\"out\"/>\n"
    "        </method>\n"
    "        <method name=\"sendrawtransaction\">\n"
    "            <arg type=\"s\" name=\"hex_string\" direction=\"in\"/>\n"
    "            <arg type=\"s\" direction=\"out\"/>\n"
    "        </method>\n"
    "        <method name=\"decoderawtransaction\">\n"
    "            <arg type=\"s\" name=\"hex_string\" direction=\"in\"/>\n"
    "            <arg type=\"a{sv}\" direction=\"out\"/>\n"
    "        </method>\n"
    "        <method name=\"createrawtransaction\">\n"
    "            <arg type=\"s\" name=\"username\" direction=\"in\"/>\n"
    "            <arg type=\"s\" name=\"pubkey\" direction=\"in\"/>\n"
    "            <arg type=\"s\" name=\"signedbyoldkey\" direction=\"in\"/>\n"
    "            <arg type=\"s\" direction=\"out\"/>\n"
    "        </method>\n"
    "        <method name=\"getrawtransaction\">\n"
    "            <arg type=\"s\" name=\"username\" direction=\"in\"/>\n"
    "            <arg type=\"b\" name=\"verbose\" direction=\"in\"/>\n"
    "            <arg type=\"v\" direction=\"out\"/>\n"
    "        </method>\n"
    "        <method name=\"importwallet\">\n"
    "            <arg type=\"s\" name=\"username\" direction=\"in\"/>\n"
    "        </method>\n"
    "        <method name=\"dumpwallet\">\n"
    "            <arg type=\"s\" name=\"username\" direction=\"in\"/>\n"
    "        </method>\n"
    "        <method name=\"importprivkey\">\n"
    "            <arg type=\"s\" name=\"privkey\" direction=\"in\"/>\n"
    "            <arg type=\"s\" name=\"username\" direction=\"in\"/>\n"
    "            <arg type=\"b\" name=\"rescan\" direction=\"in\"/>\n"
    "            <arg type=\"b\" name=\"allownewuser\" direction=\"in\"/>\n"
    "        </method>\n"
    "    </interface>\n"
    "    <interface name=\"twister.users\">\n"
    "        <method name=\"createwalletuser\">\n"
    "            <arg type=\"s\" name=\"username\" direction=\"in\"/>\n"
    "            <arg type=\"s\" direction=\"out\"/>\n"
    "        </method>\n"
    "        <method name=\"sendnewusertransaction\">\n"
    "            <arg type=\"s\" name=\"username\" direction=\"in\"/>\n"
    "            <arg type=\"s\" direction=\"out\"/>\n"
    "        </method>\n"
    "        <method name=\"listwalletusers\">\n"
    "            <arg type=\"as\" name=\"walletusers\" direction=\"out\"/>\n"
    "        </method>\n"
    "    </interface>\n"
    "    <node name=\"users\" />\n"
    "    <node name=\"dht\" />\n"
    "</node>",

    /** /twister/users **/
    "<!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object Introspection 1.0//EN\" "
                          "\"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd\">\n"
    "<node name=\"users\">\n"
    "    <interface name=\"org.freedesktop.DBus.Introspectable\">\n"
    "        <method name=\"Introspect\">\n"
    "            <arg type=\"s\" name=\"xml_data\" direction=\"out\"/>\n"
    "        </method>\n"
    "    </interface>\n"
    "    <interface name=\"org.freedesktop.DBus.Peer\">\n"
    "        <method name=\"Ping\" />\n"
    "    </interface>\n"
    "    <interface name=\"twister.core\">\n"
    "        <method name=\"search\">\n"
    "            <arg type=\"s\" name=\"scope\" dircetion=\"in\"/>\n"
    "            <arg type=\"s\" name=\"text\" dircetion=\"in\"/>\n"
    "            <arg type=\"i\" name=\"count\" direction=\"in\"/>\n"
    "            <arg type=\"a{sv}\" name=\"options\" direction=\"in\"/>\n"
    "            <arg type=\"aa{sv}\" direction=\"out\"/>\n"
    "        </method>\n"
    "    </interface>\n"
    "    <interface name=\"twister.users\">\n"
    "        <method name=\"getposts\">\n"
    "            <arg type=\"i\" name=\"count\" direction=\"in\"/>\n"
    "            <arg type=\"a{sv}\" name=\"users\" direction=\"in\"/>\n"
    "            <arg type=\"aa{sv}\" name=\"posts\" direction=\"out\"/>\n"
    "        </method>\n"
    "        <method name=\"torrentstatus\">\n"
    "            <arg type=\"s\" name=\"username\" direction=\"in\"/>\n"
    "            <arg type=\"a{sv}\" direction=\"out\"/>\n"
    "        </method>\n"
    "        <method name=\"recheckusertorrent\">\n"
    "            <arg type=\"s\" name=\"username\" direction=\"in\"/>\n"
    "        </method>\n"
    "        <method name=\"listusernamespartial\">\n"
    "            <arg type=\"s\" name=\"text\" direction=\"in\"/>\n"
    "            <arg type=\"i\" name=\"count\" direction=\"in\"/>\n"
    "            <arg type=\"as\" name=\"usernames\" direction=\"out\"/>\n"
    "        </method>\n"
    "        <method name=\"listwalletusers\">\n"
    "            <arg type=\"as\" name=\"walletusers\" direction=\"out\"/>\n"
    "        </method>\n"
    "        <method name=\"createwalletuser\">\n"
    "            <arg type=\"s\" name=\"username\" direction=\"in\"/>\n"
    "            <arg type=\"s\" direction=\"out\"/>\n"
    "        </method>\n"
    "        <method name=\"sendnewusertransaction\">\n"
    "            <arg type=\"s\" name=\"username\" direction=\"in\"/>\n"
    "            <arg type=\"s\" direction=\"out\"/>\n"
    "        </method>\n"
    "    </interface>\n"
    "%s"
    "</node>",

    /** /twister/users/ * **/
    "<!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object Introspection 1.0//EN\" "
                          "\"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd\">\n"
    "<node name=\"%s\">\n"
    "    <interface name=\"org.freedesktop.DBus.Introspectable\">\n"
    "        <method name=\"Introspect\">\n"
    "            <arg type=\"s\" name=\"xml_data\" direction=\"out\"/>\n"
    "        </method>\n"
    "    </interface>\n"
    "    <interface name=\"org.freedesktop.DBus.Peer\">\n"
    "        <method name=\"Ping\" />\n"
    "    </interface>\n"
    "    <interface name=\"twister.users\">\n"
    "        <method name=\"dumpprivkey\">\n"
    "            <arg type=\"s\" name=\"privkey\" direction=\"out\"/>\n"
    "        </method>\n"
    "        <method name=\"dumppubkey\">\n"
    "            <arg type=\"s\" name=\"pubkey\" direction=\"out\"/>\n"
    "        </method>\n"
    "        <method name=\"torrentstatus\">\n"
    "            <arg type=\"a{sv}\" direction=\"out\"/>\n"
    "        </method>\n"
    "        <method name=\"recheckusertorrent\"/>\n"
    "        <method name=\"rescandirectmsgs\"/>\n"
    "        <method name=\"getnumpieces\">\n"
    "            <arg type=\"a{sv}\" direction=\"out\"/>\n"
    "        </method>\n"
    "        <method name=\"getlasthave\">\n"
    "            <arg type=\"a{sv}\" direction=\"out\"/>\n"
    "        </method>\n"
    "        <method name=\"getfollowing\">\n"
    "            <arg type=\"as\" direction=\"out\"/>\n"
    "        </method>\n"
    "        <method name=\"follow\">\n"
    "            <arg type=\"as\" name=\"usernames\" direction=\"in\"/>\n"
    "        </method>\n"
    "        <method name=\"unfollow\">\n"
    "            <arg type=\"as\" name=\"usernames\" direction=\"in\"/>\n"
    "        </method>\n"
    "        <method name=\"getdirectmsgs\">\n"
    "            <arg type=\"i\" name=\"count\" direction=\"in\"/>\n"
    "            <arg type=\"a{sv}\" name=\"options\" direction=\"in\"/>\n"
    "            <arg type=\"aa{sv}\" direction=\"out\"/>\n"
    "        </method>\n"
    "        <method name=\"newrtmsg\">\n"
    "            <arg type=\"i\" name=\"k\" direction=\"in\"/>\n"
    "            <arg type=\"a{sv}\" name=\"rtmsgobj\" direction=\"in\"/>\n"
    "            <arg type=\"a{sv}\" direction=\"out\"/>\n"
    "        </method>\n"
    "        <method name=\"newpostmsg\">\n"
    "            <arg type=\"i\" name=\"k\" direction=\"in\"/>\n"
    "            <arg type=\"s\" name=\"msg\" direction=\"in\"/>\n"
    "            <arg type=\"s\" name=\"reply_n\" direction=\"in\"/>\n"
    "            <arg type=\"i\" name=\"reply_k\" direction=\"in\"/>\n"
    "            <arg type=\"a{sv}\" direction=\"out\"/>\n"
    "        </method>\n"
    "        <method name=\"newdirectmsg\">\n"
    "            <arg type=\"i\" name=\"k\" direction=\"in\"/>\n"
    "            <arg type=\"s\" name=\"to\" direction=\"in\"/>\n"
    "            <arg type=\"s\" name=\"msg\" direction=\"in\"/>\n"
    "            <arg type=\"b\" name=\"copy_self\" direction=\"in\"/>\n"
    "            <arg type=\"a{sv}\" direction=\"out\"/>\n"
    "        </method>\n"
    "    </interface>\n"
    "    <interface name=\"twister.dht\">\n"
    "        <method name=\"dhtget\">\n"
    "            <arg type=\"s\" name=\"username\" direction=\"in\" />\n"
    "            <arg type=\"s\" name=\"resource\" direction=\"in\" />\n"
    "            <arg type=\"s\" name=\"multi\" direction=\"in\" />\n"
    "            <arg type=\"a{su}\" name=\"timeout\" direction=\"in\" />\n"
    "            <arg type=\"u\" name=\"minmulti\" direction=\"in\" />\n"
    "            <arg type=\"aa{sv}\" name=\"value\" direction=\"out\"/>\n"
    "        </method>\n"
    "        <method name=\"dhtput\">\n"
    "            <arg type=\"s\" name=\"username\" direction=\"in\" />\n"
    "            <arg type=\"s\" name=\"resource\" direction=\"in\" />\n"
    "            <arg type=\"s\" name=\"multi\" direction=\"in\" />\n"
    "            <arg type=\"a{sv}\" name=\"value\" direction=\"in\" />\n"
    "            <arg type=\"s\" name=\"sig_user\" direction=\"in\"/>\n"
    "            <arg type=\"u\" name=\"seq\" direction=\"in\"/>\n"
    "        </method>\n"
    "    </interface>\n"
    "</node>",

    /** /twister/dht **/
    "<!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object Introspection 1.0//EN\" "
                          "\"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd\">\n"
    "<node name=\"dht\">\n"
    "    <interface name=\"org.freedesktop.DBus.Introspectable\">\n"
    "        <method name=\"Introspect\">\n"
    "            <arg type=\"s\" name=\"xml_data\" direction=\"out\"/>\n"
    "        </method>\n"
    "    </interface>\n"
    "    <interface name=\"org.freedesktop.DBus.Peer\">\n"
    "        <method name=\"Ping\" />\n"
    "    </interface>\n"
    "    <interface name=\"twister.dht\">\n"
    "        <method name=\"dhtget\">\n"
    "            <arg type=\"s\" name=\"username\" direction=\"in\" />\n"
    "            <arg type=\"s\" name=\"resource\" direction=\"in\" />\n"
    "            <arg type=\"s\" name=\"multi\" direction=\"in\" />\n"
    "            <arg type=\"a{su}\" name=\"timeout\" direction=\"in\" />\n"
    "            <arg type=\"u\" name=\"minmulti\" direction=\"in\" />\n"
    "            <arg type=\"a{sv}\" name=\"value\" direction=\"out\"/>\n"
    "        </method>\n"
    "        <method name=\"dhtput\">\n"
    "            <arg type=\"s\" name=\"username\" direction=\"in\" />\n"
    "            <arg type=\"s\" name=\"resource\" direction=\"in\" />\n"
    "            <arg type=\"s\" name=\"multi\" direction=\"in\" />\n"
    "            <arg type=\"a{sv}\" name=\"value\" direction=\"in\" />\n"
    "            <arg type=\"s\" name=\"sig_user\" direction=\"in\"/>\n"
    "            <arg type=\"u\" name=\"seq\" direction=\"in\"/>\n"
    "        </method>\n"
    "        <method name=\"gettrendinghashtags\">\n"
    "            <arg type=\"i\" name=\"count\" direction=\"in\" />\n"
    "            <arg type=\"aa{sv}\" name=\"value\" direction=\"out\"/>\n"
    "        </method>\n"
    "    </interface>\n"
    "</node>"
};

dbus_uint32_t tw_dbus_serial_g = 0;
bool tw_dbus_is_run_g = false;

dbus_uint32_t *tw_dbus_serial()
{
    tw_dbus_serial_g++;

    return &tw_dbus_serial_g;
}

Value tw_dbus_extract_to_json(DBusMessageIter *args)
{
    int t = dbus_message_iter_get_arg_type(args);
    switch (t)
    {
    case DBUS_TYPE_ARRAY:
    {
        DBusMessageIter p_array;
        dbus_message_iter_recurse(args, &p_array);

        if (dbus_message_iter_get_arg_type(&p_array) == DBUS_TYPE_DICT_ENTRY)
        {
            Object e_object;
            do
            {
                DBusMessageIter p_dict;
                dbus_message_iter_recurse(&p_array, &p_dict);

                string label = tw_dbus_extract_to_json(&p_dict).get_str();
                dbus_message_iter_next(&p_dict);
                e_object.push_back(Pair(label, tw_dbus_extract_to_json(&p_dict)));
            } while (dbus_message_iter_next(&p_array));
            return e_object;
        }
        else
        {
            Array e_array;
            do
            {
                e_array.push_back(tw_dbus_extract_to_json(&p_array));
            } while (dbus_message_iter_next(&p_array));
            return e_array;
        }
    }
    case DBUS_TYPE_DICT_ENTRY:
    {
        Object e_object;
        DBusMessageIter p_dict;
        dbus_message_iter_recurse(args, &p_dict);

        string label = tw_dbus_extract_to_json(&p_dict).get_str();
        dbus_message_iter_next(&p_dict);
        e_object.push_back(Pair(label, tw_dbus_extract_to_json(&p_dict)));
        return e_object;
    }
    case DBUS_TYPE_STRUCT:
    {
        Array e_array;
        DBusMessageIter p_struct;
        dbus_message_iter_recurse(args, &p_struct);

        do
        {
            e_array.push_back(tw_dbus_extract_to_json(&p_struct));
        } while (dbus_message_iter_next(&p_struct));
        return e_array;
    }
    case DBUS_TYPE_VARIANT:
    {
        DBusMessageIter p_variant;
        dbus_message_iter_recurse(args, &p_variant);

        return tw_dbus_extract_to_json(&p_variant);
    }
    case DBUS_TYPE_INT64:
    {
        dbus_int64_t val;
        dbus_message_iter_get_basic(args, &val);

        return Value(val);
    }
    case DBUS_TYPE_INT32:
    {
        dbus_int32_t val;
        dbus_message_iter_get_basic(args, &val);

        return Value(val);
    }
    case DBUS_TYPE_STRING:
    {
        char *val;
        dbus_message_iter_get_basic(args, &val);

        return Value(string(val));
    }
    case DBUS_TYPE_DOUBLE:
    {
        double val;
        dbus_message_iter_get_basic(args, &val);

        return Value(val);
    }
    case DBUS_TYPE_BOOLEAN:
    {
        bool val;
        dbus_message_iter_get_basic(args, &val);

        return Value(val);
    }
    default:
        fprintf(stderr, "unsupported data type: %d\n", t);
        return Value();
    }
}


Array tw_dbus_extract_to_json(DBusMessage *mess, Array &params)
{
    DBusMessageIter args;

    if (!dbus_message_iter_init(mess, &args))
    {
        fprintf(stdout, "Message has no arguments!\n");
        return params;
    }

    do
    {
        params.push_back(tw_dbus_extract_to_json(&args));
    } while(dbus_message_iter_next(&args));
    return params;
}

DBusMessageIter *tw_dbus_extract_from_json(const Value &ret, DBusMessageIter *args, bool isVariant = false, int level = 0)
{
    switch(ret.type())
    {
    case array_type:
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
        for(unsigned int i = 0; i < arr.size(); i++)
        {
            if (!tw_dbus_extract_from_json(arr[i], &p_array, isVariant || isForced, level+1))
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

        break;
    }
    case obj_type:
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
            if (!tw_dbus_extract_from_json(name, &p_dict, false, level+1))
            {
                fprintf(stderr, "Out Of Memory!\n");
                return 0;
            }
            if (!tw_dbus_extract_from_json(val, &p_dict, true, level+1))
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
        break;
    }
    case str_type:
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
        break;
    }
    case int_type:
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
        break;
    }
    case real_type:
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
        break;
    }
    case bool_type:
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
        break;
    }
    default:
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
    }

    return args;
}

Array tw_get_wallet_users()
{
    Array params;

    return listwalletusers(params, false).get_array();
}

DBusHandlerResult tw_dbus_listmethodes(DBusConnection *conn, DBusMessage *mess)
{
    DBusMessage *reply;
    DBusMessageIter args, a_array, a_variant, a_element, a_content, a_object, a_perm;
    const char *p_root = TW_DBUS_PATH_ROOT;
    const char *p_twister = TW_DBUS_PATH_TWISTER;
    const char *p_users = TW_DBUS_PATH_USERS;
    const char *p_user = TW_DBUS_PATH_USERS "/*";
    const char *p_dht = TW_DBUS_PATH_DHT;

    reply = dbus_message_new_method_return(mess);

    dbus_message_iter_init_append(reply, &args);
    dbus_message_iter_open_container(&args, DBUS_TYPE_ARRAY, "a{sv}", &a_array);
    for (int i = 0; i < TW_DBUS_M_CNT; i++)
    {
        dbus_message_iter_open_container(&a_array, DBUS_TYPE_ARRAY, "{sv}", &a_element);

        dbus_message_iter_open_container(&a_element, DBUS_TYPE_DICT_ENTRY, 0, &a_content);
        const char *method = tw_dbus_methodes_g[i].c_str();
        dbus_message_iter_append_basic(&a_content, DBUS_TYPE_STRING, &method);
        dbus_message_iter_open_container(&a_content, DBUS_TYPE_VARIANT, "a{sb}", &a_variant);
        dbus_message_iter_open_container(&a_variant, DBUS_TYPE_ARRAY, "{sb}", &a_object);

        dbus_message_iter_open_container(&a_object, DBUS_TYPE_DICT_ENTRY, 0, &a_perm);
        dbus_message_iter_append_basic(&a_perm, DBUS_TYPE_STRING, &p_root);
        dbus_message_iter_append_basic(&a_perm, DBUS_TYPE_BOOLEAN, &(tw_dbus_call_permissions_g[i].permissions[0]));
        dbus_message_iter_close_container(&a_object, &a_perm);

        dbus_message_iter_open_container(&a_object, DBUS_TYPE_DICT_ENTRY, 0, &a_perm);
        dbus_message_iter_append_basic(&a_perm, DBUS_TYPE_STRING, &p_twister);
        dbus_message_iter_append_basic(&a_perm, DBUS_TYPE_BOOLEAN, &(tw_dbus_call_permissions_g[i].permissions[1]));
        dbus_message_iter_close_container(&a_object, &a_perm);

        dbus_message_iter_open_container(&a_object, DBUS_TYPE_DICT_ENTRY, 0, &a_perm);
        dbus_message_iter_append_basic(&a_perm, DBUS_TYPE_STRING, &p_users);
        dbus_message_iter_append_basic(&a_perm, DBUS_TYPE_BOOLEAN, &(tw_dbus_call_permissions_g[i].permissions[2]));
        dbus_message_iter_close_container(&a_object, &a_perm);

        dbus_message_iter_open_container(&a_object, DBUS_TYPE_DICT_ENTRY, 0, &a_perm);
        dbus_message_iter_append_basic(&a_perm, DBUS_TYPE_STRING, &p_user);
        dbus_message_iter_append_basic(&a_perm, DBUS_TYPE_BOOLEAN, &(tw_dbus_call_permissions_g[i].permissions[3]));
        dbus_message_iter_close_container(&a_object, &a_perm);

        dbus_message_iter_open_container(&a_object, DBUS_TYPE_DICT_ENTRY, 0, &a_perm);
        dbus_message_iter_append_basic(&a_perm, DBUS_TYPE_STRING, &p_dht);
        dbus_message_iter_append_basic(&a_perm, DBUS_TYPE_BOOLEAN, &(tw_dbus_call_permissions_g[i].permissions[4]));
        dbus_message_iter_close_container(&a_object, &a_perm);

        dbus_message_iter_close_container(&a_variant, &a_object);
        dbus_message_iter_close_container(&a_content, &a_variant);
        dbus_message_iter_close_container(&a_element, &a_content);
        dbus_message_iter_close_container(&a_array, &a_element);
    }
    dbus_message_iter_close_container(&args, &a_array);

    if (!dbus_connection_send(conn, reply, tw_dbus_serial())) {
        fprintf(stderr, "Out Of Memory!\n");
        dbus_message_unref(reply);
        return DBUS_HANDLER_RESULT_NEED_MEMORY;
    }

    dbus_message_unref(reply);

    return DBUS_HANDLER_RESULT_HANDLED;
}

DBusHandlerResult tw_dbus_runRPC(DBusConnection *conn, DBusMessage *mess, Array &params)
{
    DBusMessage *reply;
    DBusMessageIter args;

    tw_dbus_extract_to_json(mess, params);
    reply = dbus_message_new_method_return(mess);
    dbus_message_iter_init_append(reply, &args);

    try
    {
        Value val = tableRPC.execute(string(dbus_message_get_member(mess)), params);

        if (!tw_dbus_extract_from_json(val, &args))
        {
            fprintf(stderr, "Out Of Memory!\n");
            dbus_message_unref(reply);
            return DBUS_HANDLER_RESULT_NEED_MEMORY;
        }
    }
    catch(Object &err)
    {
        Value val(err);
        if (!tw_dbus_extract_from_json(val, &args))
        {
            fprintf(stderr, "Out Of Memory!\n");
            dbus_message_unref(reply);
            return DBUS_HANDLER_RESULT_NEED_MEMORY;
        }
    }

    if (!dbus_connection_send(conn, reply, tw_dbus_serial())) {
        fprintf(stderr, "Out Of Memory!\n");
        dbus_message_unref(reply);
        return DBUS_HANDLER_RESULT_NEED_MEMORY;
    }

//    dbus_connection_flush(conn);

    dbus_message_unref(reply);

    return DBUS_HANDLER_RESULT_HANDLED;
}

DBusHandlerResult tw_dbus_replyping(DBusConnection* conn, DBusMessage* mess)
{
    DBusMessage* reply;

    reply = dbus_message_new_method_return(mess);

    if (!dbus_connection_send(conn, reply, tw_dbus_serial())) {
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
    char xmltmp[10240];

    if (twdp->rpid == TW_DBUS_RPID_USERS)
    {
        char tmp2[10000];
        int pos = 0;
        Array userlist = tw_get_wallet_users();

        for (unsigned int i = 0; i < userlist.size(); i++)
        {
            pos += sprintf(tmp2 + pos, "    <node name=\"%s\" />\n", userlist[i].get_str().c_str());
        }
        tmp2[pos] = 0;

        int l = sprintf(xmltmp, tw_dbus_introspect_xml[twdp->rpid], tmp2);
        xmltmp[l] = 0;

        xml = xmltmp;
    } 
    else if (twdp->rpid == TW_DBUS_RPID_USER && twdp->udata1)
    {
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
    {
        if (callit->second->isRPC)
            return tw_dbus_runRPC(conn, mess, params);
        else if (callit->first == tw_dbus_methodes_g[TW_DBUS_M_LISTPERMISSIONS])
            return tw_dbus_listmethodes(conn, mess);
    }

    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

DBusHandlerResult tw_dbus_dispatch_FB(DBusConnection *conn, DBusMessage *mess, void *udata)
{
    DBusMessage* reply;
    DBusMessageIter args;
    const char *res = "user/command/object not found...";
    tw_dbus_path_t *twdp = (tw_dbus_path_t *) udata;

    if (dbus_message_is_method_call(mess, "org.freedesktop.DBus.Introspectable", "Introspect"))
        return tw_dbus_introspect(conn, mess, twdp);

    if (dbus_message_is_method_call(mess, "org.freedesktop.DBus.Peer", "Ping"))
        return tw_dbus_replyping(conn, mess);

    if (dbus_message_is_method_call(mess, "twister.core", tw_dbus_methodes_g[TW_DBUS_M_LISTPERMISSIONS].c_str()))
        return tw_dbus_listmethodes(conn, mess);

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

    conn = dbus_bus_get((DBusBusType)GetArg("-dbus-system", 1), &err);// default: DBUS_BUS_SYSTEM=1
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "Connection Error (%s)\n", err.message);
        dbus_error_free(&err);
    }
    if (NULL == conn) {
        return;
    }

    ret = dbus_bus_request_name(conn, GetArg("-dbus-name", "co.net.twister").c_str(),
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
        tw_dbus_call_map_g[tw_dbus_methodes_g[i]] = &tw_dbus_call_permissions_g[i];
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

Object tw_dbus_call(const char *method, Array& params)
{
    DBusMessage* msg;
    DBusMessageIter args;
    DBusConnection* conn;
    DBusError err;
    DBusPendingCall* pending;

    dbus_error_init(&err);

    conn = dbus_bus_get((DBusBusType)GetArg("-dbus-system", 1), &err);// default: DBUS_BUS_SYSTEM=1
    if (dbus_error_is_set(&err)) { 
        fprintf(stderr, "Connection Error (%s)\n", err.message);

        Object ero;
        ero.push_back(Pair("message", err.message));
        throw ero;

        dbus_error_free(&err);
    }

    for (int i = 0; i < TW_DBUS_M_CNT; i++)
    {
        tw_dbus_call_map_g[tw_dbus_methodes_g[i]] = &tw_dbus_call_permissions_g[i];
    }

    if (!tw_dbus_call_map_g.count(method))
    {
        Object ret;
        Object err;
        err.push_back(Pair("code", -1));
        err.push_back(Pair("message", "unknown method"));
        ret.push_back(Pair("error", err));
        return ret;
    }

    string yol;
    if (tw_dbus_call_map_g[method]->permissions[TW_DBUS_RPID_ROOT])
        yol = TW_DBUS_PATH_ROOT;
    else if (tw_dbus_call_map_g[method]->permissions[TW_DBUS_RPID_TWISTER])
        yol = TW_DBUS_PATH_TWISTER;
    else if (tw_dbus_call_map_g[method]->permissions[TW_DBUS_RPID_USERS])
        yol = TW_DBUS_PATH_USERS;
    else if (tw_dbus_call_map_g[method]->permissions[TW_DBUS_RPID_USER] &&
             !tw_dbus_call_map_g[method]->permissions[TW_DBUS_RPID_DHT])
    {
        yol = string(TW_DBUS_PATH_USERS) + string("/") + params[0].get_str();
        params.erase(params.begin());
    }
    else
        yol = TW_DBUS_PATH_DHT;

    msg = dbus_message_new_method_call(GetArg("-dbus-name", "co.net.twister").c_str(),
                                       yol.c_str(),
                                       tw_dbus_call_map_g[method]->interface.c_str(),
                                       method);
    if (NULL == msg)
    {
        fprintf(stderr, "Message Null\n");
        Object ero;
        ero.push_back(Pair("message", "Message Null"));
        throw ero;
    }

    if (params.size())
    {
        // append arguments
        dbus_message_iter_init_append(msg, &args);
        for(int i = 0; i < params.size(); i++)
        {
            if (!tw_dbus_extract_from_json(params[i], &args))
            {
                fprintf(stderr, "Out Of Memory!\n");
                dbus_message_unref(msg);
                Object ero;
                ero.push_back(Pair("message", "Out Of Memory!"));
                throw ero;
            }
        }
    }

    if (!dbus_connection_send_with_reply (conn, msg, &pending, -1))
    {
        fprintf(stderr, "Out Of Memory!\n"); 
        Object ero;
        ero.push_back(Pair("message", "Out Of Memory!"));
        throw ero;
    }
    if (NULL == pending)
    {
        fprintf(stderr, "Pending Call Null\n"); 
        Object ero;
        ero.push_back(Pair("message", "Pending Call Null!"));
        throw ero;
    }
    dbus_connection_flush(conn);

    dbus_message_unref(msg);

    dbus_pending_call_block(pending);

    msg = dbus_pending_call_steal_reply(pending);
    if (NULL == msg)
    {
        fprintf(stderr, "Reply Null\n"); 
        Object ero;
        ero.push_back(Pair("message", "Reply Null!"));
        throw ero;
    }
    dbus_pending_call_unref(pending);

    if (!dbus_message_iter_init(msg, &args))
    {
        fprintf(stdout, "Message has no arguments!\n");
        return Object();
    }

    Value reply = tw_dbus_extract_to_json(&args);

    dbus_message_unref(msg);

    Object ret;
    Value code;
    if (reply.type() == obj_type &&
        (code = find_value(reply.get_obj(), "code")).type() == int_type &&
        code.get_int())
        ret.push_back(Pair("error", reply));
    else
        ret.push_back(Pair("result", reply));

    return ret;
}
