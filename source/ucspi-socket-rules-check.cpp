/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include "popt.h"
#include "utils.h"
#include "fdutils.h"
#include "ProcessEnvironment.h"

/* Main function ************************************************************
// **************************************************************************
*/

static bool verbose(false);

static
bool
allowed (
	const char * prog,
	const char * name,
	const std::string & subdir
) {
	const int dir_fd(open_dir_at(AT_FDCWD, subdir.c_str()));
	if (0 > dir_fd) return false;
	const int allowed(faccessat(dir_fd, "allow", F_OK, AT_EACCESS));
	const int denied(faccessat(dir_fd, "deny", F_OK, AT_EACCESS));
	close(dir_fd);
	if (0 <= allowed) return true;
	if (0 <= denied) {
		if (verbose)
			std::fprintf(stderr, "%s: FATAL: %s: %s\n", prog, name, "Access denied.");
		throw EXIT_FAILURE;
	}
	return false;
}

static
struct in_addr 
make_mask4 (
	unsigned prefix_length
) {
	struct in_addr r;
	unsigned char * b(reinterpret_cast<unsigned char *>(&r));
	for (size_t j(0); j < sizeof r; ++j) {
		if (prefix_length <= 0)
			b[j] = 0;
		else if (prefix_length > 7) {
			b[j] = 0xFF;
			prefix_length -= 8;
		} else {
			b[j] = 0xFF << (8U - prefix_length);
			prefix_length = 0;
		}
	}
	return r;
}

static
struct in6_addr 
make_mask6 (
	unsigned prefix_length
) {
	struct in6_addr r;
	unsigned char * b(reinterpret_cast<unsigned char *>(&r));
	for (size_t j(0); j < sizeof r; ++j) {
		if (prefix_length <= 0)
			b[j] = 0;
		else if (prefix_length > 7) {
			b[j] = 0xFF;
			prefix_length -= 8;
		} else {
			b[j] = 0xFF << (8U - prefix_length);
			prefix_length = 0;
		}
	}
	return r;
}

struct in_addr
operator & (
	const struct in_addr & lhs,
	const struct in_addr & rhs
) {
	struct in_addr r;
	const unsigned char * lb(reinterpret_cast<const unsigned char *>(&lhs));
	const unsigned char * rb(reinterpret_cast<const unsigned char *>(&rhs));
	unsigned char * b(reinterpret_cast<unsigned char *>(&r));
	for (size_t j(0); j < sizeof r; ++j) 
		b[j] = lb[j] & rb[j];
	return r;
}

struct in6_addr
operator & (
	const struct in6_addr & lhs,
	const struct in6_addr & rhs
) {
	struct in6_addr r;
	const unsigned char * lb(reinterpret_cast<const unsigned char *>(&lhs));
	const unsigned char * rb(reinterpret_cast<const unsigned char *>(&rhs));
	unsigned char * b(reinterpret_cast<unsigned char *>(&r));
	for (size_t j(0); j < sizeof r; ++j) 
		b[j] = lb[j] & rb[j];
	return r;
}


void
ucspi_socket_rules_check ( 
	const char * & next_prog,
	std::vector<const char *> & args,
	ProcessEnvironment & envs
) {
	const char * prog(basename_of(args[0]));
	try {
		popt::bool_definition verbose_option('v', "verbose", "Print status information.", verbose);
		popt::definition * top_table[] = {
			&verbose_option
		};
		popt::top_table_definition main_option(sizeof top_table/sizeof *top_table, top_table, "Main options", "prog");

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
		std::fprintf(stderr, "%s: FATAL: %s\n", prog, "Missing next program.");
		throw static_cast<int>(EXIT_USAGE);
	}

	const char * proto(envs.query("PROTO"));
	if (!proto) {
		std::fprintf(stderr, "%s: FATAL: %s: %s\n", prog, "PROTO", "Missing environment variable.");
		throw EXIT_FAILURE;
	}
	if (0 == std::strcmp(proto, "UNIX")) {
		const char * uid(envs.query("UNIXREMOTEUID"));
		if (!uid) {
			std::fprintf(stderr, "%s: FATAL: %s: %s\n", prog, "UNIXREMOTEUID", "Missing environment variable.");
			throw EXIT_FAILURE;
		}
		const char * gid(envs.query("UNIXREMOTEGID"));
		if (!gid) {
			std::fprintf(stderr, "%s: FATAL: %s: %s\n", prog, "UNIXREMOTEGID", "Missing environment variable.");
			throw EXIT_FAILURE;
		}
		if (allowed(prog, uid, "uid/" + std::string(uid) + "/")) return;
		if (allowed(prog, gid, "gid/" + std::string(gid) + "/")) return;
		if (allowed(prog, "default", "uid/default/")) return;
		if (verbose)
			std::fprintf(stderr, "%s: FATAL: %s: %s\n", prog, proto, "Access denied.");
		throw EXIT_FAILURE;
	} else
	if (0 == std::strcmp(proto, "TCP")) {
		const char * ip(envs.query("TCPREMOTEIP"));
		if (!ip) {
			std::fprintf(stderr, "%s: FATAL: %s: %s\n", prog, "TCPREMOTEIP", "Missing environment variable.");
			throw EXIT_FAILURE;
		}
		struct in_addr addr4;
		struct in6_addr addr6;
		if (0 < inet_pton(AF_INET, ip, &addr4)) {
			const std::string dir("ip4/");
			for (unsigned prefix_length(33); prefix_length > 0; ) {
				--prefix_length;
				const struct in_addr net4(make_mask4(prefix_length) & addr4);
				char buf[INET_ADDRSTRLEN], suffix[32];
				inet_ntop(AF_INET, &net4, buf, sizeof buf);
				snprintf(suffix, sizeof suffix, "_%u", prefix_length);
				if (allowed(prog, ip, (dir + buf) + suffix)) return;
			}
		} else 
		if (0 < inet_pton(AF_INET6, ip, &addr6)) {
			const std::string dir("ip6/");
			for (unsigned prefix_length(129); prefix_length > 0; ) {
				--prefix_length;
				const struct in6_addr net6(make_mask6(prefix_length) & addr6);
				char buf[INET6_ADDRSTRLEN], suffix[32];
				inet_ntop(AF_INET6, &net6, buf, sizeof buf);
				snprintf(suffix, sizeof suffix, "_%u", prefix_length);
				if (allowed(prog, ip, (dir + buf) + suffix)) return;
			}
		} else 
		{
			std::fprintf(stderr, "%s: FATAL: %s: %s\n", prog, ip, "Invalid IP address.");
			throw EXIT_FAILURE;
		}
		if (verbose)
			std::fprintf(stderr, "%s: FATAL: %s: %s\n", prog, ip, "Access denied.");
		throw EXIT_FAILURE;
	} else
	if (0 == std::strcmp(proto, "TCP6")) {
		const char * ip(envs.query("TCP6REMOTEIP"));
		if (!ip) {
			std::fprintf(stderr, "%s: FATAL: %s: %s\n", prog, "TCP6REMOTEIP", "Missing environment variable.");
			throw EXIT_FAILURE;
		}
		struct in_addr addr4;
		struct in6_addr addr6;
		if (0 < inet_pton(AF_INET, ip, &addr4)) {
			const std::string dir("ip4/");
			for (unsigned prefix_length(33); prefix_length > 0; ) {
				--prefix_length;
				const struct in_addr net4(make_mask4(prefix_length) & addr4);
				char buf[INET_ADDRSTRLEN], suffix[32];
				inet_ntop(AF_INET, &net4, buf, sizeof buf);
				snprintf(suffix, sizeof suffix, "_%u", prefix_length);
				if (allowed(prog, ip, (dir + buf) + suffix)) return;
			}
		} else 
		if (0 < inet_pton(AF_INET6, ip, &addr6)) {
			const std::string dir("ip6/");
			for (unsigned prefix_length(129); prefix_length > 0; ) {
				--prefix_length;
				const struct in6_addr net6(make_mask6(prefix_length) & addr6);
				char buf[INET6_ADDRSTRLEN], suffix[32];
				inet_ntop(AF_INET6, &net6, buf, sizeof buf);
				snprintf(suffix, sizeof suffix, "_%u", prefix_length);
				if (allowed(prog, ip, (dir + buf) + suffix)) return;
			}
		} else 
		{
			std::fprintf(stderr, "%s: FATAL: %s: %s\n", prog, ip, "Invalid IP address.");
			throw EXIT_FAILURE;
		}
		if (verbose)
			std::fprintf(stderr, "%s: FATAL: %s: %s\n", prog, ip, "Access denied.");
		throw EXIT_FAILURE;
	} else
	{
		std::fprintf(stderr, "%s: FATAL: %s: %s\n", prog, proto, "Valid values are \"UNIX\", \"TCP\", and \"TCP6\".");
		throw EXIT_FAILURE;
	}
}
