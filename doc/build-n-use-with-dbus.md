# BUILDING AND USING WITH DBUS


## To Build

Use `--enable-dbus` parameter to build with DBUS support.

For example;

    ./bootstrap.sh --enable-logging --enable-debug --enable-dht

then

    make

## To Run

Use `-dbus` argument to run with DBUS

For example;

    ./twisterd -rpcuser=user -rpcpassword=pwd -rpcallowip=127.0.0.1 -dbus

## To Test

~~There is no dbus client yet to test properly~~. It can be tested with `twisterd` command line util or with `dbus-send`.
With `twisterd`, all you need is adding `-dbus` parametre to your command.

All rpc command can be used.

For example;

    twisterd -dbus stop
    twisterd -dbus getposts 5 '[{"username":"erqan"}]'

    dbus-send --dest=co.net.twister --print-reply /twister twister.core.stop
    dbus-send --dest=co.net.twister --print-reply /twister/users twister.users.getposts int32:5 dict:string:string:"username","erkanyuksel"
    dbus-send --dest=co.net.twister --print-reply /twister/users/erqan twister.dht.dhtget string:"avatar" string:"s"
    dbus-send --dest=co.net.twister --print-reply /twister/users twister.dht.dhtget string:"erqan" string:"avatar" string:"s"

## DBUS Map

####Paths

    /
    /twister
    /twister/dht
    /twister/users
    /twister/users/[wallet_users_1]
    /twister/users/[wallet_users_2]

####Interfaces

    twister.core
    twister.dht
    twister.users

####Methods

    twister.core.help                   on /twister
    twister.core.stop                   on /twister
    twister.core.getblockscount         on /twister
    twister.core.getbestblockhash       on /twister
    twister.core.getconnectioncount     on /twister
    twister.core.getpeerinfo            on /twister
    twister.core.addnode                on /twister
    twister.core.addnsseed              on /twister
    twister.core.getaddednodeinfo       on /twister
    twister.core.getdifficulty          on /twister
    twister.core.getgenerate            on /twister
    twister.core.setgenerate            on /twister
    twister.core.gethashespersec        on /twister
    twister.core.getinfo                on /twister
    twister.core.getmininginfo          on /twister
    twister.core.createwalletuser       on /twister
    twister.core.listwalletusers        on /twister
    twister.core.backupwallet           on /twister
    twister.core.walletpassphrase       on /twister
    twister.core.walletpassphrasechange on /twister
    twister.core.walletlock             on /twister
    twister.core.encryptwallet          on /twister
    twister.core.getrawmempool          on /twister
    twister.core.getblock               on /twister
    twister.core.getblockhash           on /twister
    twister.core.gettransaction         on /twister
    twister.core.listtransactions       on /twister
    twister.core.signmessage            on /twister
    twister.core.verifymessage          on /twister
    twister.core.getwork                on /twister
    twister.core.getblocktemplate       on /twister
    twister.core.submitblock            on /twister
    twister.core.listsinceblock         on /twister
    twister.users.dumpprivkey           on /twister/users/[WALLET USER NAME]
    twister.users.dumppubkey            on /twister/users/[WALLET USER NAME]
    twister.core.dumpwallet             on /twister
    twister.core.importprivkey          on /twister
    twister.core.importwallet           on /twister
    twister.core.getrawtransaction      on /twister
    twister.core.createrawtransaction   on /twister
    twister.core.decoderawtransaction   on /twister
    twister.core.sendrawtransaction     on /twister
    twister.core.sendnewusertransaction on /twister
    twister.core.verifychain            on /twister
    twister.core.getlastsoftcheckpoint  on /twister
    twister.dht.dhtput                  on /twister/dht and /twister/users/[WALLET USER NAME]
    twister.dht.dhtget                  on /twister/dht and /twister/users/[WALLET USER NAME]
    twister.users.newpostmsg            on /twister/users/[WALLET USER NAME]
    twister.users.newdirectmsg          on /twister/users/[WALLET USER NAME]
    twister.users.newrtmsg              on /twister/users/[WALLET USER NAME]
    twister.users.getposts              on /twister/users
    twister.users.getdirectmsgs         on /twister/users/[WALLET USER NAME]
    twister.core.setspammsg             on /twister
    twister.core.getspammsg             on /twister
    twister.users.follow                on /twister/users/[WALLET USER NAME]
    twister.users.unfollow              on /twister/users/[WALLET USER NAME]
    twister.users.getfollowing          on /twister/users/[WALLET USER NAME]
    twister.users.getlasthave           on /twister/users/[WALLET USER NAME]
    twister.users.getnumpieces          on /twister/users/[WALLET USER NAME]
    twister.users.listusernamespartial  on /twister/users/[WALLET USER NAME]
    twister.users.rescandirectmsgs      on /twister/users/[WALLET USER NAME]
    twister.users.recheckusertorrent    on /twister/users/[WALLET USER NAME]
    twister.dht.gettrendinghashtags     on /twister/dht
    twister.core.getspamposts           on /twister
    twister.users.torrentstatus         on /twister/users/[WALLET USER NAME]
