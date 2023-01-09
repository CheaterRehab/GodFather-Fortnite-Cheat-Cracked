
# GodPaster Private External Fortnite Cheat Cracked 

# We were the first people to leak this source, if anyone acts like they did, send us their p2c on our discord server :)

Crack of a detected Fortnite Cheat by the https://cheater.rehab/ team.

mod aka !mod#9659 (ID: 597485373194764311) is a fraud

This guy is selling a popular fortnite "Private" External called "GodFather". 
The cheat that he is selling is a simple copy + paste and is detected to dear shit on EAC/BE. Loader wasnt even protected...

Credits to: UCFoxi so the kid can paste off of his driver communication method.
 
"Their" driver abuses FlushFileBuffers because it finds PEAuth in kernel, then swaps one majorfunction with his driver (IRP_MJ_FLUSH_BUFFERS)There are public methods on how to do this so there is 0 way this isnt detetcted. Literally all EAC needs to do is just hop on their debugger in kernel and check all running drivers majorfunctions until they end up in your driver. This is most likely why kids are pasting this for BE since Bastian is stupid and cant check for stuff like this.

After a client-side bypass we have in keyauth to bypass the login it then runs a system command that stops Windows Management Instrumentation. It follows that by loading "their" driver using Kdmapper from a byte array (downloaded from keyauth's servers) which is very easy to dump.

Simple anti-pasta has been added to the driver and the source code to avoid further pasting, it should be straightfoward to figure out if you have a half brain.

there are precompiled binaries on the right side of this page, have fun.

enjoy
