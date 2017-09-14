# setproxy
Tool to set proxy configuration on Windows

SetProxy.exe Version 1.1
SetProxy.exe usage|reset|autoconfigURL|auto|direct|manual|manual HOST:PORT[;https=HOST:PORT][;ftp=HOST:PORT]|bypass <bypass ports>

Running setproxy with no parameters or usage displays current proxy configuration and help

usage    --  Shows current proxy settings and help.

reset    --  Resets proxy settings (DIRECT included).

autoconfigURL http://proxy/autoconfig.pac

auto    --  Auto detect proxy settings.

direct  --  Direct Internet Access, proxy disabled.

manual  --  Manually Set proxy settings (use existing settings).

bypass  --  Set the Proxy Bypass addresses.

manual http=HOST:PORT[;ftp=HOST:PORT]  --  (configure server)

manual http=http://proxy:80;https=https://proxy:80bypass "172.*;157.*;10.*;127.*;<local>"

References
**********
Setting and Retrieving Internet Options https://msdn.microsoft.com/en-us/library/aa385384(v=vs.85).aspx
How to programmatically query and set proxy settings under WinINet https://support.microsoft.com/en-us/help/226473/how-to-programmatically-query-and-set-proxy-settings-under-internet-ex
InternetQueryOption function https://msdn.microsoft.com/en-us/library/aa385101(v=vs.85).aspx
