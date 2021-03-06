#Nabil Rahiman
#NYU, Abudhabi
#email:nr83@nyu.edu
#18/Feb/2018


from mininet.topo import Topo
from mininet.net import Mininet
from mininet.util import dumpNodeConnections
from mininet.log import setLogLevel
from mininet.cli import CLI
import time

class SingleSwitchTopo(Topo):
    "Single switch connected to n hosts."
    def build(self, n=2):
        switch = self.addSwitch('s1')
        # Python's range(N) generates 0..N-1
        for h in range(n):
            host = self.addHost('h%s' % (h + 1))
            self.addLink(host, switch)

"Create and test a simple network"
topo = SingleSwitchTopo(n=4)
net = Mininet(topo)
net.start()
print "Dumping host connections"
dumpNodeConnections(net.hosts)
print "Testing network connectivity"
h1 = net.get('h1')
h2 = net.get('h2')
h3 = net.get('h3')
print("ping h2")
#CLI(net)
print h1.cmd('ping -c 3 %s' %  h2.IP())
#net.pingAll()
print h1.cmd("./FTPserver &> server_log.txt &")

print "server started"
time.sleep(3)

print h2.cmd("./FTPclient %s 8080 &> client_log.txt &" % (h1.IP()))

print h2.cmd("USER chen")

net.stop()

