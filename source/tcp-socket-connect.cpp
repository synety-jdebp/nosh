/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <csignal>
#include <cerrno>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include "popt.h"
#include "utils.h"
#include "fdutils.h"
#include "ProcessEnvironment.h"
#include "FileDescriptorOwner.h"

/* Helper functions *********************************************************
// **************************************************************************
*/

static inline
const char *
q (
	const ProcessEnvironment & envs,
	const char * name
) {
	const char * value(envs.query(name));
	return value ? value : "";
}

/* Main function ************************************************************
// **************************************************************************
*/

void
tcp_socket_connect ( 
	const char * & next_prog,
	std::vector<const char *> & args,
	ProcessEnvironment & envs
) {
	const char * prog(basename_of(args[0]));
	const char * localhost(0);
	const char * localaddress(0);
	const char * localport(0);
	bool verbose(false);
	bool keepalives(false);
	bool numeric_host(false);
	bool numeric_service(false);
	bool check_interfaces(false);
#if defined(IP_OPTIONS)
	bool no_kill_IP_options(false);
#endif
	bool no_delay(false);
	try {
		popt::bool_definition verbose_option('v', "verbose", "Print status information.", verbose);
		popt::bool_definition keepalives_option('k', "keepalives", "Enable TCP keepalive processing.", keepalives);
		popt::bool_definition check_interfaces_option('\0', "check-interfaces", "Disallow IPv4 or IPv6 if no interface supports them.", check_interfaces);
#if defined(IP_OPTIONS)
		popt::bool_definition no_kill_IP_options_option('o', "no-kill-IP-options", "Allow a client to set source routes.", no_kill_IP_options);
#endif
		popt::bool_definition no_delay_option('D', "no-delay", "Disable the TCP delay algorithm.", no_delay);
		popt::bool_definition numeric_host_option('H', "numeric-host", "Assume that the host is an IP address.", numeric_host);
		popt::bool_definition numeric_service_option('\0', "numeric-service", "Assume that the service is a port number.", numeric_service);
		popt::string_definition localhost_option('l', "local-host", "name", "Override the local host.", localhost);
		popt::string_definition localaddress_option('i', "local-address", "address", "Override the local IP address.", localaddress);
		popt::string_definition localport_option('p', "local-port", "port", "Override the local port.", localport);
		popt::definition * top_table[] = {
			&verbose_option,
			&keepalives_option,
			&check_interfaces_option,
#if defined(IP_OPTIONS)
			&no_kill_IP_options_option,
#endif
			&no_delay_option,
			&numeric_host_option,
			&numeric_service_option,
			&localhost_option,
			&localaddress_option,
			&localport_option
		};
		popt::top_table_definition main_option(sizeof top_table/sizeof *top_table, top_table, "Main options", "host port prog");

		std::vector<const char *> new_args;
		popt::arg_processor<const char **> p(args.data() + 1, args.data() + args.size(), prog, main_option, new_args);
		p.process(true /* strictly options before arguments */);
		args = new_args;
		next_prog = arg0_of(args);
		if (p.stopped()) throw EXIT_SUCCESS;
	} catch (const popt::error & e) {
		std::fprintf(stderr, "%s: FATAL: %s: %s\n", prog, e.arg, e.msg);
		throw EXIT_FAILURE;
	}

	if (args.empty()) {
		std::fprintf(stderr, "%s: FATAL: %s\n", prog, "Missing connect host name.");
		throw static_cast<int>(EXIT_USAGE);
	}
	const char * connecthost(args.front());
	args.erase(args.begin());
	if (args.empty()) {
		std::fprintf(stderr, "%s: FATAL: %s\n", prog, "Missing connect port number.");
		throw static_cast<int>(EXIT_USAGE);
	}
	const char * connectport(args.front());
	args.erase(args.begin());
	if (args.empty()) {
		std::fprintf(stderr, "%s: FATAL: %s\n", prog, "Missing next program.");
		throw static_cast<int>(EXIT_USAGE);
	}
	next_prog = arg0_of(args);

	addrinfo * remote_info_list(0), remote_hints = {0,0,0,0,0,0,0,0};
	remote_hints.ai_family = AF_UNSPEC;
	remote_hints.ai_socktype = SOCK_STREAM;
	remote_hints.ai_protocol = 0;
	remote_hints.ai_flags = 0;
	if (numeric_host) remote_hints.ai_flags |= AI_NUMERICHOST;
	if (numeric_service) remote_hints.ai_flags |= AI_NUMERICSERV;
	if (check_interfaces) remote_hints.ai_flags |= AI_ADDRCONFIG;
	const int rrc(getaddrinfo(connecthost, connectport, &remote_hints, &remote_info_list));
	if (0 != rrc) {
		const int error(errno);
		std::fprintf(stderr, "%s: FATAL: %s %s: %s\n", prog, connecthost, connectport, EAI_SYSTEM == rrc ? std::strerror(error) : gai_strerror(rrc));
		throw EXIT_FAILURE;
	}

	for (addrinfo * remote_info(remote_info_list); ; remote_info = remote_info->ai_next) {
		if (!remote_info) {
			std::fprintf(stderr, "%s: FATAL: %s\n", prog, "No addresses (remaining) to connect to.");
		free_fail:
			if (remote_info_list) freeaddrinfo(remote_info);
			throw EXIT_FAILURE;
		}

		FileDescriptorOwner s(socket(remote_info->ai_family, remote_info->ai_socktype, remote_info->ai_protocol));
		if (0 > s.get()) {
		exit_error:
			const int error(errno);
			std::fprintf(stderr, "%s: FATAL: %s\n", prog, std::strerror(error));
			goto free_fail;
		}

		if (localaddress || localport) {
			addrinfo * local_info(0), local_hints = {0,0,0,0,0,0,0,0};
			local_hints.ai_family = remote_info->ai_family;
			local_hints.ai_socktype = remote_info->ai_socktype;
			local_hints.ai_protocol = remote_info->ai_protocol;
			local_hints.ai_flags = AI_NUMERICHOST|AI_NUMERICSERV|AI_PASSIVE;
			if (check_interfaces) local_hints.ai_flags |= AI_ADDRCONFIG;
			const int lrc(getaddrinfo(localaddress, localport, &local_hints, &local_info));
			if (0 != lrc) {
				const int error(errno);
				std::fprintf(stderr, "%s: FATAL: %s %s: %s\n", prog, localaddress, localport, EAI_SYSTEM == lrc ? std::strerror(error) : gai_strerror(lrc));
				goto free_fail;
			}
			if (0 > bind(s.get(), local_info->ai_addr, local_info->ai_addrlen)) goto exit_error;
			freeaddrinfo(local_info);
		}

		if (keepalives) {
			if (0 > socket_set_boolean_option(s.get(), SOL_SOCKET, SO_KEEPALIVE, true)) goto exit_error ;
		}
		if (no_delay) {
			if (0 > socket_set_boolean_option(s.get(), IPPROTO_TCP, TCP_NODELAY, true)) goto exit_error ;
		}
#if defined(IP_OPTIONS)
		if (!no_kill_IP_options) {
			switch (remote_info->ai_family) {
				case AF_INET:
					if (0 > setsockopt(s.get(), IPPROTO_IP, IP_OPTIONS, 0, 0)) goto exit_error ;
					break;
				default:
					break;
			}
		}
#endif

		if (0 > socket_connect(s.get(), remote_info->ai_addr, remote_info->ai_addrlen)) continue;

		sockaddr_storage localaddr;
		socklen_t localaddrsz = sizeof localaddr;
		if (0 > getsockname(s.get(), reinterpret_cast<sockaddr *>(&localaddr), &localaddrsz)) goto exit_error;

		if (0 > dup2(s.get(), 6)) goto exit_error;
		if (0 > dup2(s.get(), 7)) goto exit_error;
		if (6 == s.get() || 7 == s.get())
			s.release();

		envs.set("PROTO", "TCP");
		switch (localaddr.ss_family) {
			case AF_INET:
			{
				const struct sockaddr_in & localaddr4(*reinterpret_cast<const struct sockaddr_in *>(&localaddr));
				char port[64], ip[INET_ADDRSTRLEN];
				if (0 == inet_ntop(localaddr4.sin_family, &localaddr4.sin_addr, ip, sizeof ip)) goto exit_error;
				snprintf(port, sizeof port, "%u", ntohs(localaddr4.sin_port));
				envs.set("TCPLOCALIP", ip);
				envs.set("TCPLOCALPORT", port);
				break;
			}
			case AF_INET6:
			{
				const struct sockaddr_in6 & localaddr6(*reinterpret_cast<const struct sockaddr_in6 *>(&localaddr));
				char port[64], ip[INET6_ADDRSTRLEN];
				if (0 == inet_ntop(localaddr6.sin6_family, &localaddr6.sin6_addr, ip, sizeof ip)) goto exit_error;
				snprintf(port, sizeof port, "%u", ntohs(localaddr6.sin6_port));
				envs.set("TCPLOCALIP", ip);
				envs.set("TCPLOCALPORT", port);
				break;
			}
			default:
				envs.set("TCPLOCALIP", 0);
				envs.set("TCPLOCALPORT", 0);
				break;
		}
		switch (remote_info->ai_family) {
			case AF_INET:
			{
				const struct sockaddr_in & remoteaddr4(*reinterpret_cast<const struct sockaddr_in *>(remote_info->ai_addr));
				char port[64], ip[INET_ADDRSTRLEN];
				if (0 == inet_ntop(remoteaddr4.sin_family, &remoteaddr4.sin_addr, ip, sizeof ip)) goto exit_error;
				snprintf(port, sizeof port, "%u", ntohs(remoteaddr4.sin_port));
				envs.set("TCPREMOTEIP", ip);
				envs.set("TCPREMOTEPORT", port);
				break;
			}
			case AF_INET6:
			{
				const struct sockaddr_in6 & remoteaddr6(*reinterpret_cast<const struct sockaddr_in6 *>(remote_info->ai_addr));
				char port[64], ip[INET6_ADDRSTRLEN];
				if (0 == inet_ntop(remoteaddr6.sin6_family, &remoteaddr6.sin6_addr, ip, sizeof ip)) goto exit_error;
				snprintf(port, sizeof port, "%u", ntohs(remoteaddr6.sin6_port));
				envs.set("TCPREMOTEIP", ip);
				envs.set("TCPREMOTEPORT", port);
				break;
			}
			default:
				envs.set("TCPREMOTEIP", 0);
				envs.set("TCPREMOTEPORT", 0);
				break;
		}
		envs.set("TCPLOCALHOST", localhost);
		envs.set("TCPLOCALINFO", 0);
		envs.set("TCPREMOTEHOST", 0);
		envs.set("TCPREMOTEINFO", 0);

		if (verbose)
			std::fprintf(stderr, "%s: %u %s %s %s %s\n", prog, getpid(), q(envs, "TCPLOCALIP"), q(envs, "TCPLOCALPORT"), q(envs, "TCPREMOTEIP"), q(envs, "TCPREMOTEPORT"));

		return;
	}
}
