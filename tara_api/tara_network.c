#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/file.h>

#include <net/if.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/un.h> 
#include <net/route.h>
#include <unistd.h>
#include <signal.h>
#include <asm/types.h>
#include <linux/sockios.h>

#include "shmkey.h"
#include "mutex.h"
#include "tara_log.h"
#include "tara_network.h"

#define PROC_ROUTE_PATH		"/proc/net/route"
#define RESOLV_CONF				"/etc/resolv.conf"
#define CONTROL_LOCK      "/tmp/.net_control"

static struct ifreq hwaddr;
static u8 ethaddr[6];
static s8 hostname[MAX_HOSTNAME_LEN];

static int try_lock()
{
	int fd = open(CONTROL_LOCK, O_CREAT, 0666);
	int locked=0;
	if (fd != -1) {
		int cnt=0;
		do {
			if (flock(fd, LOCK_EX) == 0) {
				locked = 1;
				break;
			}
			
			usleep(100000);
		} while (cnt++ < 10);
		
		if (locked == 0) {
			close(fd);
			fd = -1;
		}		
	}
	
	return fd;
}

static void unlock(int fd)
{
	if (fd != -1) {
		flock(fd, LOCK_UN);
		close(fd);
	}
}

static struct sockaddr_in saddr = {
	sin_family:	PF_INET,
	sin_port:	0
};

s32 net_control(s32 cmd, void *data)
{
	int ret=0;
	int fd=-1;
	char *sockname = NET_CMD_SOCKET;
	struct stat sb;
	struct sockaddr_un addr;
	u32 buf[7];
	fd_set rfds;
	struct timeval tv;
	int cnt, size_to_read=0;
	tv.tv_sec = 2;
	tv.tv_usec = 0;
	
	u32 *pu32 = buf;
	
	if (cmd >= NET_CMD_MAX) return ERROR_INVALARG;
	if (stat(sockname, &sb) != 0) {
		Tara_log(TARA_LOG_ERROR, "%s:%d sockname:%s not exist\n", __FUNCTION__, __LINE__, sockname);
		ret = -1;
		goto CLEANUP;
	}

	// connect to server
	fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (fd == -1) goto CLEANUP;

	
	memset(&addr, 0, sizeof(struct sockaddr_un));
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, sockname);
	if (connect(fd, (struct sockaddr*)&addr, sizeof(struct sockaddr_un)) == -1) {
		ret = -1;
		goto CLEANUP;
	}
	fcntl(fd, F_SETFL, O_NONBLOCK);
	memset(buf, 0, sizeof(buf));
	pu32[0] = cmd;
	if (cmd == NET_CMD_SET_ETHADDR) {
		memcpy(&pu32[1], (u32 *)data, sizeof(u32)*6);
	} else if (cmd == NET_CMD_SET_DHCP) {
		memcpy(&pu32[1], (u32 *)data, sizeof(u32));
	}

	// send command
	if (write(fd, buf, sizeof(u32)*7) == -1) {
		Tara_log(TARA_LOG_ERROR, "%s:%d write error: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		goto CLEANUP;
	}

	ret = 0;
	if (pu32[0] > NET_CMD_GET_MAX) goto CLEANUP;
	// wait ack ( optional )
	FD_ZERO(&rfds);
	FD_SET(fd, &rfds);
	cnt = 0;
	if (cmd == NET_CMD_GET_ETHADDR) {
		size_to_read = sizeof(u32) * 6;
		pu32 = (u32 *)data;
	} else if (cmd == NET_CMD_GET_DHCP) {
		size_to_read = sizeof(TaraNetDhcpConfig);
		pu32 = (u32* )data;
	}

	while(cnt++ < 3) {
		ret = select(fd+1, &rfds, NULL, NULL, &tv);
		if (ret > 0) {
			int retry = 3;
			while(size_to_read > 0) {
				ret = read(fd, pu32, size_to_read);
				if (ret > 0) {
					size_to_read -= ret;
					pu32 += ret;
				} else if (ret < 0) break;
				else {
					if (retry-- == 0) break;
				}
			}
			break;
		}
	}
	if (size_to_read == 0) ret = 0;
	else ret = -1;

CLEANUP:
	if (fd != -1) close(fd);
	return ret;
}

s32 net_setenv(char *name, char *value)
{
	int lockfd;
	char cmd[256] = {'\0'};
	Tara_log(TARA_LOG_INFO, "%s:%d %s=%s\n", __FUNCTION__, __LINE__, name, value);
	if (name == NULL || value == NULL) return ERROR_INVALARG;
	if ((strlen(name) + strlen(value)) > 240) return ERROR_INVALARG;
	snprintf(cmd, sizeof(cmd), "/srv/sbin/fw_setenv %s %s", name, value);
	lockfd=try_lock();
	if (lockfd) {
		int rc = system(cmd);
		if (WIFSIGNALED(rc)) {
			Tara_log(TARA_LOG_ERROR, "%s:%d system call interrupted by signal %d (%s)\n", __FUNCTION__, __LINE__, WTERMSIG(rc), strsignal(WTERMSIG(rc)));
		}
	}
	unlock(lockfd);
	return 0;
}

in_addr_t Tara_net_getIPv4Addr(char *ifname)
{
	struct ifreq ifr;
	int skfd;
	struct sockaddr_in *sa;

	if ((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		Tara_log(TARA_LOG_ERROR, "%s:%d not exist\n", __FUNCTION__, __LINE__);
		return ERROR_FAIL;
	}

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if (ioctl(skfd, SIOCGIFADDR, &ifr) < 0) {
		Tara_log(TARA_LOG_ERROR, "%s:%d ioctl SIOCGIFADDR\n", __FUNCTION__, __LINE__);
		close(skfd);
		return ERROR_FAIL;
	}
	close(skfd);
	
	sa = (struct sockaddr_in *) &ifr.ifr_addr;
	return sa->sin_addr.s_addr;
}

s32 Tara_net_setIPv4Addr(char *ifname, in_addr_t addr)
{
	struct ifreq ifr;
	int skfd;

	if (!ifname) return ERROR_INVALARG;
	if ( (skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		Tara_log(TARA_LOG_ERROR, "%s:%d not exist\n", __FUNCTION__, __LINE__);
		return ERROR_FAIL;
	}
	saddr.sin_addr.s_addr = addr;
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	memcpy((char *) &ifr.ifr_addr, (char *) &saddr, sizeof(struct sockaddr));
	if (ioctl(skfd, SIOCSIFADDR, &ifr) < 0) {
		Tara_log(TARA_LOG_ERROR, "%s:%d ioctl SIOCSIFADDR\n", __FUNCTION__, __LINE__);
		close(skfd);
		return ERROR_FAIL;
	}
	close(skfd);
	
	net_setenv("ipaddr", inet_ntoa(saddr.sin_addr));
	net_control(NET_CMD_SET_IPV4ADDR, &saddr.sin_addr);
	return 0;	
}

in_addr_t Tara_net_getSubnetMask(char *ifname)
{
	struct ifreq ifr;
	int skfd;
	struct sockaddr_in *sa;

	if ((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		Tara_log(TARA_LOG_ERROR, "%s:%d not exist\n", __FUNCTION__, __LINE__);
		return ERROR_FAIL;
	}

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if (ioctl(skfd, SIOCGIFNETMASK, &ifr) < 0) {
		Tara_log(TARA_LOG_ERROR, "%s:%d ioctl SIOCGIFNETMASK\n", __FUNCTION__, __LINE__);
		close(skfd);
		return ERROR_FAIL;
	}
	close(skfd);

	sa = (struct sockaddr_in *) &ifr.ifr_addr;
	return sa->sin_addr.s_addr;
}

s32 Tara_net_setSubnetMask(char *ifname, in_addr_t addr)
{
	struct ifreq ifr;
	int skfd;

	if (!ifname) return ERROR_INVALARG;
	if ((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		Tara_log(TARA_LOG_ERROR, "%s:%d not exist\n", __FUNCTION__, __LINE__);
		return ERROR_FAIL;
	}
	saddr.sin_addr.s_addr = addr;
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	memcpy((char *) &ifr.ifr_addr, (char *) &saddr, sizeof(struct sockaddr));
	if (ioctl(skfd, SIOCSIFNETMASK, &ifr) < 0) {
		Tara_log(TARA_LOG_ERROR, "%s:%d ioctl SIOCSIFNETMASK\n", __FUNCTION__, __LINE__);
		close(skfd);
		return ERROR_FAIL;
	}
	close(skfd);
	net_setenv("netmask", inet_ntoa(saddr.sin_addr));
	return 0;
}

s32 net_add_gateway(in_addr_t addr)
{
	struct rtentry rt;
	int skfd;

	memset((char *) &rt, 0, sizeof(struct rtentry));
	rt.rt_flags = (RTF_UP | RTF_GATEWAY);
	rt.rt_dst.sa_family = PF_INET;
	rt.rt_genmask.sa_family = PF_INET;

	saddr.sin_addr.s_addr = addr;
	memcpy((char *) &rt.rt_gateway, (char *) &saddr, sizeof(struct sockaddr));

	if ((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		Tara_log(TARA_LOG_ERROR, "%s:%d not exist\n", __FUNCTION__, __LINE__);
		return ERROR_FAIL;
	}

	if (ioctl(skfd, SIOCADDRT, &rt) < 0) {
		Tara_log(TARA_LOG_ERROR, "%s:%d ioctl SIOCADDRT\n", __FUNCTION__, __LINE__);
		close(skfd);
		return ERROR_FAIL;
	}
	close(skfd);
	net_setenv("gatewayip", inet_ntoa(saddr.sin_addr));
	return 0;
}

s32 net_del_gateway(in_addr_t addr)
{
	struct rtentry rt;
	int skfd;

	memset((char *) &rt, 0, sizeof(struct rtentry));
	rt.rt_flags = (RTF_UP | RTF_GATEWAY);
	rt.rt_dst.sa_family = PF_INET;
	rt.rt_genmask.sa_family = PF_INET;

	saddr.sin_addr.s_addr = addr;
	memcpy((char *) &rt.rt_gateway, (char *) &saddr, sizeof(struct sockaddr));

	if ((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		Tara_log(TARA_LOG_ERROR, "%s:%d not exist\n", __FUNCTION__, __LINE__);
		return ERROR_FAIL;
	}

	if (ioctl(skfd, SIOCDELRT, &rt) < 0) {
		Tara_log(TARA_LOG_ERROR, "%s:%d ioctl SIOCDELRT\n", __FUNCTION__, __LINE__);
		close(skfd);
		return ERROR_FAIL;
	}
	close(skfd);
	return 0;
}

in_addr_t Tara_net_getGateway()
{
	char buf[132];
	char iface[16];
	unsigned long dest, gate;
	int iflags;
	FILE *fp = fopen(PROC_ROUTE_PATH, "r");

	if (!fp) {
		Tara_log(TARA_LOG_ERROR, "%s:%d %s INET (IPv4) not configured in this system.\n", __FUNCTION__, __LINE__, PROC_ROUTE_PATH);
		return (INADDR_ANY);
	}
	if (! fgets(buf, 130, fp)) {
		Tara_log(TARA_LOG_ERROR, "%s:%d fgets failed: %s (ignored)\n", __FUNCTION__, __LINE__, strerror(errno));
	}
	while (fgets(buf, 130, fp) != NULL) {
		sscanf(buf, "%s\t%08lX\t%08lX\t%8X\t", iface, &dest, &gate, &iflags);
		if ((iflags & (RTF_UP | RTF_GATEWAY)) == (RTF_UP | RTF_GATEWAY)) {
			fclose(fp);
			return (in_addr_t)gate;
		}
	}
	fclose(fp);
	return (INADDR_ANY);
}

s32 Tara_net_setGateway(char *ifname, in_addr_t addr)
{
	s32 ret;
	char buf[132];
	char iface[16];
	unsigned long dest, gate;
	int iflags;
	
	in_addr_t ip, mask;
	char ipstr[4], maskstr[4], gwstr[4];
	if (!ifname) return ERROR_INVALARG;
	ip = Tara_net_getIPv4Addr(ifname);
	ipstr[3] = (char)(ip & 0x000000ff);
	ipstr[2] = (char)((ip & 0x0000ff00) >> 8);
	ipstr[1] = (char)((ip & 0x00ff0000) >> 16);
	ipstr[0] = (char)((ip & 0xff000000) >> 24);
	mask = Tara_net_getSubnetMask(ifname);
	maskstr[3] = (char)(mask & 0x000000ff);
	maskstr[2] = (char)((mask & 0x0000ff00) >> 8);
	maskstr[1] = (char)((mask & 0x00ff0000) >> 16);
	maskstr[0] = (char)((mask & 0xff000000) >> 24);
	gwstr[3] = (char)(addr & 0x000000ff);
	gwstr[2] = (char)((addr & 0x0000ff00) >> 8);
	gwstr[1] = (char)((addr & 0x00ff0000) >> 16);
	gwstr[0] = (char)((addr & 0xff000000) >> 24);

	ret = (gwstr[3] & maskstr[3]) ^ (ipstr[3] & maskstr[3]);
	ret |= (gwstr[2] & maskstr[2]) ^ (ipstr[2] & maskstr[2]);
	ret |= (gwstr[1] & maskstr[1]) ^ (ipstr[1] & maskstr[1]);
	ret |= (gwstr[0] & maskstr[0]) ^ (ipstr[0] & maskstr[0]);;
	if (ret > 0) return ERROR_INVALARG;
	
	FILE *fp = fopen(PROC_ROUTE_PATH, "r");
	if (!fp) {
		Tara_log(TARA_LOG_ERROR, "%s:%d %s INET (IPv4) not configured in this system.\n", __FUNCTION__, __LINE__, PROC_ROUTE_PATH);
		return ERROR_FAIL;
	}
	if (! fgets(buf, 130, fp)) {
		Tara_log(TARA_LOG_ERROR, "%s:%d fgets failed: %s (ignored)\n", __FUNCTION__, __LINE__, strerror(errno));
	}
	while (fgets(buf, 130, fp) != NULL) {
		sscanf(buf, "%s\t%08lX\t%08lX\t%8X\t", iface, &dest, &gate, &iflags);
		if ((iflags & (RTF_UP | RTF_GATEWAY)) == (RTF_UP | RTF_GATEWAY)) {
			net_del_gateway((in_addr_t)gate);
		}
	}
	fclose(fp);	
	return net_add_gateway(addr);
}

in_addr_t Tara_net_getDNS()
{
	FILE *fp;
	char dnsname[80];

	fp = fopen(RESOLV_CONF, "r");
	if (fp) {
		if (fscanf(fp, "nameserver %s\n", dnsname) != EOF) {
			fclose(fp);
			Tara_log(TARA_LOG_INFO, "%s:%d dns : %s\n", __FUNCTION__, __LINE__, dnsname);
			return inet_addr(dnsname);
		}
		Tara_log(TARA_LOG_ERROR, "%s:%d scan fail!!\n", __FUNCTION__, __LINE__);
	}
	Tara_log(TARA_LOG_ERROR, "%s:%d open %s error!\n", __FUNCTION__, __LINE__, RESOLV_CONF);
	return INADDR_ANY;
}

s32 Tara_net_setDNS(char *dnsname)
{
	FILE *fp;
	
	if (!dnsname) return ERROR_INVALARG;
	Tara_log(TARA_LOG_INFO, "%s:%d %s\n", __FUNCTION__, __LINE__, dnsname);
	fp = fopen(RESOLV_CONF, "w");
	if ( fp ) {
		fprintf(fp, "nameserver %s\n", dnsname);
		fclose(fp);
		net_setenv("dns", dnsname);
//		net_setenv("dns", inet_ntoa(saddr.sin_addr));
		return 0;
	}
	Tara_log(TARA_LOG_ERROR, "%s:%d open %s error!\n", __FUNCTION__, __LINE__, RESOLV_CONF);
	return ERROR_FAIL;
}

s32 Tara_net_getDHCP()
{
	TaraNetDhcpConfig dhcp;
	net_control(NET_CMD_GET_DHCP, &dhcp);
	return dhcp.on;
}

s32 Tara_net_setDHCP(s32 on)
{
	if (on != NET_DHCP_ON && on != NET_DHCP_OFF) return ERROR_INVALARG;
	net_control(NET_CMD_SET_DHCP, &on);
	if (on == NET_DHCP_ON) net_setenv("ip", "dhcp");
	else net_setenv("ip", "static");
	return 0;
}

u8* Tara_net_getHWaddr(char *ifname)
{
	int skfd;

	if (!ifname) return NULL;
	if ((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		Tara_log(TARA_LOG_ERROR, "%s:%d not exist\n", __FUNCTION__, __LINE__);
		return NULL;
	}

	strncpy(hwaddr.ifr_name, ifname, IFNAMSIZ);
	if (ioctl(skfd, SIOCGIFHWADDR, &hwaddr) < 0) {
		Tara_log(TARA_LOG_ERROR, "%s:%d ioctl SIOCGIFHWADDR\n", __FUNCTION__, __LINE__);
		close(skfd);
		return NULL;
	}
	close(skfd);

	return (u8 *)hwaddr.ifr_ifru.ifru_hwaddr.sa_data;
}

u8* Tara_net_getEthaddr()
{
	s32 ret, i;
	u32 tmp[6];
	ret = net_control(NET_CMD_GET_ETHADDR, tmp);
	if(ret == -1) return NULL; 
	
	if ((tmp[0]^tmp[1]^tmp[2]^tmp[3]^tmp[4]^tmp[5]^0x0) == 0) return NULL;
	for (i=0; i<6; i++) *(ethaddr+i) = (u8)tmp[i];
	Tara_log(TARA_LOG_DEBUG, "%s:%2x, %2x, %2x, %2x, %2x, %2x\n", __func__, tmp[0], tmp[1], tmp[2], tmp[3], tmp[4], tmp[5]);
	return (u8 *)ethaddr;
}

s32 Tara_net_setEthaddr(u8 *mac)
{
	u32 buf[6], i;
	memset(&buf, 0, sizeof(buf));
	if (!mac) return ERROR_INVALARG;
	for (i=0; i<6; i++) buf[i] = *(mac+i);
	Tara_log(TARA_LOG_DEBUG, "=== %s:%2x %2x %2x %2x %2x %2x ===\n", __func__, buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);
	net_control(NET_CMD_SET_ETHADDR, buf);
	return 0;
}

s32 Tara_net_delEthaddr()
{
	u32 buf[6];
	memset(&buf, 0, sizeof(buf));
	Tara_log(TARA_LOG_DEBUG, "=== %s:%2x %2x %2x %2x %2x %2x ===\n", __func__, buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);
	net_control(NET_CMD_SET_ETHADDR, buf);
	return 0;
}

s32 Tara_net_setHostname(s8 *name, size_t len)
{
	if (!name) return ERROR_INVALARG;
	gethostname(hostname, MAX_HOSTNAME_LEN);
	if (strcmp(hostname, name) == 0) return 0;
	
	Tara_log(TARA_LOG_INFO, "%s:%d %s, %d\n", __FUNCTION__, __LINE__, name, (s32)len);
	if (sethostname(name, len) < 0) {
		perror("sethostname ");
		return ERROR_FAIL;
	}
	net_setenv("Hostname", name);
	return 0;
}

s8* Tara_net_getHostname()
{
	if (gethostname(hostname, MAX_HOSTNAME_LEN) < 0) {
		perror("gethostname ");
		return NULL;
	}
	Tara_log(TARA_LOG_INFO, "%s:%d %s\n", __FUNCTION__, __LINE__, hostname);
	return hostname;
}

