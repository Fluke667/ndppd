// ndppd - NDP Proxy Daemon
// Copyright (C) 2011-2018  Daniel Adolfsson <daniel@priv.nu>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <string>
#include <list>
#include <vector>
#include <map>

#include <sys/poll.h>
#include <net/ethernet.h>
#include <memory>

#include "ndppd.h"
#include "range.h"
#include "address.h"

namespace ndppd
{

    class Session;

    class Proxy;

    class iface
    {
    public:
        // Destructor.
        ~iface();

        static std::shared_ptr<iface> open_ifd(const std::string &name);

        static std::shared_ptr<iface> open_pfd(const std::string &name, bool promiscuous);

        static int poll_all();

        ssize_t read(int fd, struct sockaddr *saddr, ssize_t saddr_size, uint8_t *msg, size_t size);

        ssize_t write(int fd, const Address &daddr, const uint8_t *msg, size_t size);

        // Writes a NB_NEIGHBOR_SOLICIT message to the _ifd socket.
        ssize_t write_solicit(const Address &taddr);

        // Writes a NB_NEIGHBOR_ADVERT message to the _ifd socket;
        ssize_t write_advert(const Address &daddr, const Address &taddr, bool router);

        // Reads a NB_NEIGHBOR_SOLICIT message from the _pfd socket.
        ssize_t read_solicit(Address &saddr, Address &daddr, Address &taddr);

        // Reads a NB_NEIGHBOR_ADVERT message from the _ifd socket;
        ssize_t read_advert(Address &saddr, Address &taddr);

        bool handle_local(const Address &saddr, const Address &taddr);

        bool is_local(const Address &addr);

        void handle_reverse_advert(const Address &saddr, const std::string &ifname);

        // Returns the name of the interface.
        const std::string &name() const;

        void add_serves(const std::shared_ptr<Proxy> &proxy);

        void add_parent(const std::shared_ptr<Proxy> &parent);

        static std::map<std::string, std::weak_ptr<iface> > _map;

        const Range<std::list<std::weak_ptr<Proxy>>::const_iterator> parents() const;

        const Range<std::list<std::weak_ptr<Proxy>>::const_iterator> serves() const;


    private:

        static bool _map_dirty;

        // An array of objects used with ::poll.
        static std::vector<struct pollfd> _pollfds;

        // Updates the array above.
        static void fixup_pollfds();

        static void cleanup();

        // Weak pointer so this object can reference itself.
        std::weak_ptr<iface> _ptr;

        // The "generic" ICMPv6 socket for reading/writing NB_NEIGHBOR_ADVERT
        // messages as well as writing NB_NEIGHBOR_SOLICIT messages.
        int _ifd;

        // This is the PF_PACKET socket we use in order to read
        // NB_NEIGHBOR_SOLICIT messages.
        int _pfd;

        // Previous state of ALLMULTI for the interface.
        int _prev_allmulti;

        // Previous state of PROMISC for the interface
        int _prev_promiscuous;

        // Name of this interface.
        std::string _name;

        std::list<std::weak_ptr<Proxy> > _serves;

        std::list<std::weak_ptr<Proxy> > _parents;

        // The link-layer address of this interface.
        struct ether_addr hwaddr;

        // Turns on/off ALLMULTI for this interface - returns the previous state
        // or -1 if there was an error.
        int allmulti(int state);

        // Turns on/off PROMISC for this interface - returns the previous state
        // or -1 if there was an error
        int promiscuous(int state);

        // Constructor.
        iface();


    };
}
