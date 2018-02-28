if [ $# -ne 3 ]
then
	echo "usage error: serverIP serverPort idx"
	exit -1
fi
idx=$3
test -e c$idx && rm c$idx
mkfifo c$idx
./FTPclient  $1 $2 < c$idx  >client$idx.log&  
eval "exec 8>c$idx"
echo -ne "nonsense\n" >&8
echo -ne "CD sandbox/ftp_server_dirs/client_dir_c$idx\n" >&8
echo -ne "USER nonsense\n" >&8
echo -ne "PASS 123\n" >&8
echo -ne "USER chen\n" >&8
echo -ne "PASS 321\n" >&8
echo -ne "PASS 123\n" >&8
echo -ne "CD nonsense\n" >&8
echo -ne "CD sandbox/ftp_server_dirs/client_dir_c$idx\n" >&8
echo -ne "PWD\n" >&8
echo -ne "LS\n" >&8
echo -ne "LS nonsense\n" >&8

echo -ne "!CD sandbox/ftp_client_dirs/local_dir_c$idx\n" >&8
#echo -ne "!LS\n" >&8
echo -ne "GET nonsense\n" >&8
echo -ne "GET server_file_client$idx.txt\n" >&8
echo -ne "PUT nonsense\n" >&8
echo -ne "PUT local_file_client$idx.txt\n" >&8
echo -ne "QUIT\n" >&8

sleep 10
echo "Basic test completed"
