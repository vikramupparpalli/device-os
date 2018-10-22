/*
 * Copyright (c) 2018 Particle Industries, Inc.  All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

#include "logging.h"
LOG_SOURCE_CATEGORY("system.nm")

#include "system_network_manager.h"
#include "system_error.h"
#include <mutex>
#include "system_led_signal.h"
#include "enumclass.h"
#include "system_commands.h"
#if HAL_PLATFORM_WIFI && HAL_PLATFORM_NCP
#include "network/ncp.h"
#include "wifi_network_manager.h"
#endif // HAL_PLATFORM_WIFI && HAL_PLATFORM_NCP
#if HAL_PLATFORM_NCP && HAL_PLATFORM_CELLULAR
#include "cellular_hal.h"
#endif // HAL_PLATFORM_NCP && HAL_PLATFORM_CELLULAR
#if HAL_PLATFORM_MESH
#include "border_router_manager.h"
#endif // HAL_PLATFORM_MESH
#include "check.h"

#define CHECKV(_expr) \
        ({ \
            const auto _ret = _expr; \
            if (_ret < 0) { \
                return; \
            } \
            _ret; \
        })

#define CHECK_CONT(_expr) \
        ({ \
            const auto _ret = _expr; \
            if (_ret < 0) { \
                continue; \
            } \
            _ret; \
        })

#if HAL_PLATFORM_IFAPI

#if HAL_PLATFORM_OPENTHREAD
#include "system_openthread.h"
#include <openthread/dataset.h>
#include <openthread/thread.h>
#include <openthread/instance.h>
#endif /* HAL_PLATFORM_OPENTHREAD */

using namespace particle::system;
using namespace particle::net;

namespace {

template <typename F>
int for_each_iface(F&& f) {
    if_list* ifs = nullptr;
    CHECK(if_get_list(&ifs));

    for (if_list* iface = ifs; iface != nullptr; iface = iface->next) {
        if (iface->iface) {
            unsigned int flags = 0;
            CHECK_CONT(if_get_flags(iface->iface, &flags));
            if (!(flags & IFF_LOOPBACK)) {
                f(iface->iface, flags);
            }
        }
    }

    if_free_list(ifs);

    return 0;
}

} /* anonymous */

NetworkManager::NetworkManager() {
    state_ = State::NONE;
    ip4State_ = ProtocolState::UNCONFIGURED;
    ip6State_ = ProtocolState::UNCONFIGURED;
    dns4State_ = DnsState::UNCONFIGURED;
    dns6State_ = DnsState::UNCONFIGURED;
}

NetworkManager::~NetworkManager() {
    destroy();
}

NetworkManager* NetworkManager::instance() {
    static NetworkManager man;
    return &man;
}

void NetworkManager::init() {
    if (!ifEventHandlerCookie_) {
        ifEventHandlerCookie_ = if_event_handler_add(&ifEventHandlerCb, this);
    }

    if (!resolvEventHandlerCookie_) {
        resolvEventHandlerCookie_ = resolv_event_handler_add(&resolvEventHandlerCb, this);
    }

    transition(State::DISABLED);
}

void NetworkManager::destroy() {
    if (ifEventHandlerCookie_) {
        if_event_handler_del(ifEventHandlerCookie_);
        ifEventHandlerCookie_ = nullptr;
    }

    if (resolvEventHandlerCookie_) {
        resolv_event_handler_del(resolvEventHandlerCookie_);
        resolvEventHandlerCookie_ = nullptr;
    }
}

int NetworkManager::enableNetworking() {
    /* TODO: this should power on all the network hardware if possible */
    if (state_ == State::DISABLED) {
        transition(State::IFACE_DOWN);
    }
    return SYSTEM_ERROR_INVALID_STATE;
}

int NetworkManager::disableNetworking() {
    /* TODO: this should power off all the network hardware if possible */
    switch (state_) {
        case State::DISABLED: {
            return 0;
        }
        case State::IFACE_DOWN: {
            /* Networking is enabled, but no interfaces were brought up. Nothing to do here */
            transition(State::DISABLED);
            break;
        }
    }
    return SYSTEM_ERROR_INVALID_STATE;
}

bool NetworkManager::isNetworkingEnabled() const {
    return state_ != State::DISABLED;
}

int NetworkManager::activateConnections() {
    if (state_ != State::IFACE_DOWN) {
        return SYSTEM_ERROR_INVALID_STATE;
    }

    /* Get a list of network interfaces */
    transition(State::IFACE_REQUEST_UP);

    int waitingFor = 0;

    /* Bring all the interfaces up */
    CHECK(for_each_iface([&](if_t iface, unsigned int flags) {
        /* Skip interfaces that don't have configuration */
        if (!haveLowerLayerConfiguration(iface)) {
            return;
        }

        // Ignore disabled interfaces
        if (!isInterfaceEnabled(iface)) {
            return;
        }

        if (!(flags & IFF_UP)) {
            CHECKV(if_set_flags(iface, IFF_UP));

            /* FIXME */
            CHECKV(if_set_xflags(iface, IFXF_DHCP));

            /* TODO: establish lower layer connection, e.g. 802.11 */

            ++waitingFor;
        }
    }));

    if (!waitingFor) {
        /* No interfaces needed to be brought up */
        transition(State::IFACE_UP);
    }

    return 0;
}

int NetworkManager::deactivateConnections() {
    switch (state_) {
        case State::DISABLED:
        case State::IFACE_DOWN: {
            return SYSTEM_ERROR_INVALID_STATE;
        }
        default: {
            break;
        }
    }

    transition(State::IFACE_REQUEST_DOWN);

    int waitingFor = 0;

    /* Bring all the interfaces down */
    CHECK(for_each_iface([&](if_t iface, unsigned int flags) {
        /* Skip interfaces that don't have configuration
         * FIXME: Do we need this here as well?
         */
        // if (!haveLowerLayerConfiguration(iface)) {
        //     return;
        // }

        if (flags & IFF_UP) {
            CHECKV(if_clear_flags(iface, IFF_UP));
            ++waitingFor;
        }
    }));

    if (!waitingFor) {
        /* No interfaces needed to be brought down */
        transition(State::IFACE_DOWN);
    }

    return 0;
}

bool NetworkManager::isEstablishingConnections() const {
    switch (state_) {
        case State::IFACE_REQUEST_UP:
        case State::IFACE_UP:
        case State::IFACE_LINK_UP: {
            return true;
        }
    }

    return false;
}

bool NetworkManager::isConnectivityAvailable() const {
    return state_ == State::IP_CONFIGURED;
}

bool NetworkManager::isIp4ConnectivityAvailable() const {
    return ip4State_ == ProtocolState::CONFIGURED;
}

bool NetworkManager::isIp6ConnectivityAvailable() const {
    return ip6State_ == ProtocolState::CONFIGURED;
}

bool NetworkManager::isConfigured(if_t iface) const {
    bool ret = false;
    if (!iface) {
        for_each_iface([&](if_t iface, unsigned int curFlags) {
            if (haveLowerLayerConfiguration(iface)) {
                ret = true;
            }
        });
        return ret;
    } else {
        return haveLowerLayerConfiguration(iface);
    }
}

int NetworkManager::clearConfiguration(if_t oIface) {
    if (state_ != State::IFACE_DOWN && state_ != State::DISABLED) {
        return SYSTEM_ERROR_INVALID_STATE;
    }

    if (!isConfigured()) {
        return SYSTEM_ERROR_NONE;
    }

    int ret = SYSTEM_ERROR_INVALID_ARGUMENT;

    for_each_iface([&](if_t iface, unsigned int flags) {
        char name[IF_NAMESIZE] = {};
        if_get_name(iface, name);

        if (oIface && iface != oIface) {
            return;
        }

#if HAL_PLATFORM_OPENTHREAD
        if (!strncmp(name, "th", 2)) {
            /* OpenThread iface */
            std::lock_guard<ThreadLock> lk(ThreadLock());
            otMasterKey key = {};
            otThreadSetMasterKey(threadInstance(), &key);
            otInstanceErasePersistentInfo(threadInstance());
            system_command_clear();
            ret = SYSTEM_ERROR_NONE;
        }
#endif /* HAL_PLATFORM_OPENTHREAD */
#if HAL_PLATFORM_NCP && HAL_PLATFORM_WIFI
        else if (!strncmp(name, "wl", 2)) {
            auto wifiMan = wifiNetworkManager();
            wifiMan->clearNetworkConfig();
            ret = SYSTEM_ERROR_NONE;
        }
#endif // HAL_PLATFORM_NCP && HAL_PLATFORM_WIFI
#if HAL_PLATFORM_NCP && HAL_PLATFORM_CELLULAR
        else if (!strncmp(name, "pp", 2)) {
            ret = cellular_credentials_clear(nullptr);
        }
#endif // HAL_PLATFORM_NCP && HAL_PLATFORM_CELLULAR
    });

    return ret;
}

NetworkManager::State NetworkManager::getState() const {
    return state_;
}

void NetworkManager::transition(State state) {
    /* From */
    switch (state_) {
        case State::DISABLED: {
            break;
        }
        case State::IFACE_DOWN: {
            break;
        }
        case State::IFACE_REQUEST_DOWN: {
            break;
        }
        case State::IFACE_REQUEST_UP: {
            break;
        }
        case State::IFACE_UP: {
            break;
        }
        case State::IFACE_LINK_UP: {
            break;
        }
        /* Ensure that IPv4/IPv6 protocol state is reset */
        case State::IP_CONFIGURED: {
            if (state != State::IP_CONFIGURED) {
                ip4State_ = ProtocolState::UNCONFIGURED;
                ip6State_ = ProtocolState::UNCONFIGURED;
                dns4State_ = DnsState::UNCONFIGURED;
                dns6State_ = DnsState::UNCONFIGURED;
#if HAL_PLATFORM_MESH
                BorderRouterManager::instance()->stop();
#endif // HAL_PLATFORM_MESH
            }
            break;
        }
    }

    /* To */
    switch (state) {
        case State::DISABLED: {
            LED_SIGNAL_START(NETWORK_OFF, BACKGROUND);
            break;
        }
        case State::IFACE_DOWN: {
            LED_SIGNAL_START(NETWORK_ON, BACKGROUND);
            break;
        }
        case State::IFACE_REQUEST_DOWN: {
            /* TODO: ? */
            break;
        }
        case State::IFACE_REQUEST_UP: {
            LED_SIGNAL_START(NETWORK_CONNECTING, BACKGROUND);
            break;
        }
        case State::IFACE_UP: {
            LED_SIGNAL_START(NETWORK_CONNECTING, BACKGROUND);
            break;
        }
        case State::IFACE_LINK_UP: {
            LED_SIGNAL_START(NETWORK_DHCP, BACKGROUND);
            break;
        }
        case State::IP_CONFIGURED: {
            LED_SIGNAL_START(NETWORK_CONNECTED, BACKGROUND);
            break;
        }
    }

    LOG(INFO, "State changed: %s -> %s", stateToName(state_), stateToName(state));

    state_ = state;
}

void NetworkManager::ifEventHandlerCb(void* arg, if_t iface, const struct if_event* ev) {
    auto self = static_cast<NetworkManager*>(arg);
    self->ifEventHandler(iface, ev);
}

void NetworkManager::ifEventHandler(if_t iface, const struct if_event* ev) {
    switch (ev->ev_type) {
        case IF_EVENT_IF_ADDED: {
            handleIfAdded(iface, ev);
            break;
        }
        case IF_EVENT_IF_REMOVED: {
            handleIfRemoved(iface, ev);
            break;
        }
        case IF_EVENT_STATE: {
            handleIfState(iface, ev);
            break;
        }
        case IF_EVENT_LINK: {
            handleIfLink(iface, ev);
            break;
        }
        case IF_EVENT_ADDR: {
            handleIfAddr(iface, ev);
            break;
        }
        case IF_EVENT_LLADDR: {
            handleIfLinkLayerAddr(iface, ev);
            break;
        }
    }
}

void NetworkManager::handleIfAdded(if_t iface, const struct if_event* ev) {
    /* TODO */
}

void NetworkManager::handleIfRemoved(if_t iface, const struct if_event* ev) {
    /* TODO */
}

void NetworkManager::handleIfState(if_t iface, const struct if_event* ev) {
    if (ev->ev_if_state->state) {
        /* Interface administrative state changed to UP */
        if (state_ == State::IFACE_REQUEST_UP) {
            transition(State::IFACE_UP);

            /* FIXME: is this needed? */
            if (countIfacesWithFlags(IFF_LOWER_UP) > 0) {
                transition(State::IFACE_LINK_UP);
            }
        }
    } else {
        /* Interface administrative state changed to DOWN */
        if (state_ == State::IFACE_REQUEST_DOWN) {
            /* FIXME: LwIP issues netif_set_down callback BEFORE clearing the IFF_UP flag,
             * that's why the count of interfaces with IFF_UP should be 1 here
             */
            if (countIfacesWithFlags(IFF_UP) <= 1) {
                transition(State::IFACE_DOWN);
            }
        }
    }
}

void NetworkManager::handleIfLink(if_t iface, const struct if_event* ev) {
    if (ev->ev_if_link->state) {
        /* Interface link state changed to UP */
        if (state_ == State::IFACE_UP) {
            transition(State::IFACE_LINK_UP);
            refreshIpState();
        } else if (state_ == State::IP_CONFIGURED || state_ == State::IFACE_LINK_UP) {
            refreshIpState();
        }
    } else {
        resetInterfaceProtocolState(iface);
        /* Interface link state changed to DOWN */
        if (state_ == State::IP_CONFIGURED || state_ == State::IFACE_LINK_UP) {
            if (countIfacesWithFlags(IFF_UP | IFF_LOWER_UP) == 0) {
                transition(State::IFACE_UP);
            } else {
                refreshIpState();
            }
        }
    }
}

void NetworkManager::handleIfAddr(if_t iface, const struct if_event* ev) {
    if (state_ == State::IP_CONFIGURED || state_ == State::IFACE_LINK_UP) {
        refreshIpState();
    }
}

void NetworkManager::handleIfLinkLayerAddr(if_t iface, const struct if_event* ev) {
    /* We don't care about this */
}

unsigned int NetworkManager::countIfacesWithFlags(unsigned int flags) const {
    unsigned int count = 0;

    for_each_iface([&](if_t iface, unsigned int curFlags) {
        /* Skip interfaces that don't have configuration */
        if (!haveLowerLayerConfiguration(iface)) {
            return;
        }

        // Ignore disabled interfaces
        if (!isInterfaceEnabled(iface)) {
            return;
        }

        if ((curFlags & flags) == flags) {
            ++count;
        }
    });

    return count;
}

void NetworkManager::refreshIpState() {
    ProtocolState ip4 = ProtocolState::UNCONFIGURED;
    ProtocolState ip6 = ProtocolState::UNCONFIGURED;

    resetInterfaceProtocolState();

    if_addrs* addrs = nullptr;
    CHECKV(if_get_if_addrs(&addrs));

    for (auto addr = addrs; addr != nullptr; addr = addr->next) {
        ProtocolState ifIp4 = ProtocolState::UNCONFIGURED;
        ProtocolState ifIp6 = ProtocolState::UNCONFIGURED;

        /* Skip loopback interface */
        if (addr->ifflags & IFF_LOOPBACK) {
            continue;
        }

        /* Skip non-UP and non-LINK_UP interfaces */
        if ((addr->ifflags & (IFF_UP | IFF_LOWER_UP)) != (IFF_UP | IFF_LOWER_UP)) {
            continue;
        }

        auto a = addr->if_addr;
        if (!a || !a->addr) {
            continue;
        }

        if (a->addr->sa_family == AF_INET) {
            if (a->prefixlen > 0 && /* FIXME */ a->gw) {
                ifIp4 = ProtocolState::CONFIGURED;
            }
        } else if (a->addr->sa_family == AF_INET6) {
            sockaddr_in6* sin6 = (sockaddr_in6*)a->addr;
            auto ip6_addr_data = a->ip6_addr_data;

            /* NOTE: we say that IPv6 is configured if there is at least
             * one IPv6 address on an interface without scope and in a VALID state,
             * which is in fact either PREFERRED or DEPRECATED.
             */
            if (sin6->sin6_scope_id == 0 && a->prefixlen > 0 &&
                    ip6_addr_data && (ip6_addr_data->state & IF_IP6_ADDR_STATE_VALID)) {
                ifIp6 = ProtocolState::CONFIGURED;
            } else if (sin6->sin6_scope_id != 0 && a->prefixlen > 0 &&
                    ip6_addr_data && (ip6_addr_data->state & IF_IP6_ADDR_STATE_VALID)) {
                // Otherwise report link-local
                ifIp6 = ProtocolState::LINKLOCAL;
            }
        } else {
            /* Unknown family */
        }

        if ((int)ifIp4 > (int)ip4) {
            ip4 = ifIp4;
        }
        if ((int)ifIp6 > (int)ip6) {
            ip6 = ifIp6;
        }

        {
            if_t iface;
            if (!if_get_by_index(addr->ifindex, &iface)) {
                auto state = getInterfaceRuntimeState(iface);
                if (state) {
                    if ((int)ifIp4 > (int)state->ip4State.load()) {
                        state->ip4State = ifIp4;
                    }
                    if ((int)ifIp6 > (int)state->ip6State.load()) {
                        state->ip6State = ifIp6;
                    }
                }
            }
        }
    }

    if_free_if_addrs(addrs);

    const auto oldIp4State = ip4State_.load();
    const auto oldIp6State = ip6State_.load();

    refreshDnsState();

    ip4State_ = ip4;
    ip6State_ = ip6;

#if HAL_PLATFORM_MESH
    // FIXME: for now only checking ip4 state
    if (ip4State_ == ProtocolState::CONFIGURED && dns4State_ == DnsState::CONFIGURED) {
        BorderRouterManager::instance()->start();
    } else {
        BorderRouterManager::instance()->stop();
    }
#endif // HAL_PLATFORM_MESH

    const bool ipConfigured = (ip4 == ProtocolState::CONFIGURED && dns4State_ == DnsState::CONFIGURED) ||
            (ip6 == ProtocolState::CONFIGURED && dns6State_ == DnsState::CONFIGURED);

    if (state_ == State::IP_CONFIGURED && !ipConfigured) {
        transition(State::IFACE_LINK_UP);
    } else if (state_ == State::IFACE_LINK_UP && ipConfigured) {
        transition(State::IP_CONFIGURED);
    } else if (state_ == State::IP_CONFIGURED && ipConfigured) {
        /* Transition to the same state here. Once we implement events this will be relevant */
        if (ip4 != oldIp4State || ip6 != oldIp6State) {
            transition(State::IP_CONFIGURED);
        }
    }
}

void NetworkManager::refreshDnsState() {
    resolv_dns_servers* servers = nullptr;
    CHECKV(resolv_get_dns_servers(&servers));

    DnsState ip4 = DnsState::UNCONFIGURED;
    DnsState ip6 = DnsState::UNCONFIGURED;

    for (auto server = servers; server != nullptr; server = server->next) {
        if (!server || !server->server) {
            continue;
        }

        if (server->server->sa_family == AF_INET) {
            ip4 = DnsState::CONFIGURED;
        } else if (server->server->sa_family == AF_INET6) {
            ip6 = DnsState::CONFIGURED;
        }
    }

    resolv_free_dns_servers(servers);

    dns4State_ = ip4;
    dns6State_ = ip6;
}

bool NetworkManager::haveLowerLayerConfiguration(if_t iface) const {
    char name[IF_NAMESIZE] = {};
    if_get_name(iface, name);

#if HAL_PLATFORM_OPENTHREAD
    if (!strncmp(name, "th", 2)) {
        /* OpenThread iface */
        std::lock_guard<ThreadLock> lk(ThreadLock());
        return otDatasetIsCommissioned(threadInstance());
    }
#endif /* HAL_PLATFORM_OPENTHREAD */

#if HAL_PLATFORM_WIFI && HAL_PLATFORM_NCP
    if (!strncmp(name, "wl", 2)) {
        auto wifiMan = wifiNetworkManager();
        return wifiMan->hasNetworkConfig();
    }
#endif // HAL_PLATFORM_WIFI

    return true;
}

void NetworkManager::resolvEventHandlerCb(void* arg, const void* data) {
    auto self = static_cast<NetworkManager*>(arg);
    self->resolvEventHandler(data);
}

void NetworkManager::resolvEventHandler(const void* data) {
    /* FIXME */
    refreshIpState();
}

const char* NetworkManager::stateToName(State state) const {
    static const char* const stateNames[] = {
        "NONE",
        "DISABLED",
        "IFACE_DOWN",
        "IFACE_REQUEST_DOWN",
        "IFACE_REQUEST_UP",
        "IFACE_UP",
        "IFACE_LINK_UP",
        "IP_CONFIGURED"
    };

    return stateNames[::particle::to_underlying(state)];
}

void NetworkManager::populateInterfaceRuntimeState(bool st) {
    for_each_iface([&](if_t iface, unsigned int flags) {
        auto state = getInterfaceRuntimeState(iface);
        if (!state) {
            state = new (std::nothrow) InterfaceRuntimeState;
            if (state) {
                state->iface = iface;
                runState_.pushFront(state);
            }
        }
        if (state) {
            state->enabled = st;
        }
    });
}

int NetworkManager::enableInterface(if_t iface) {
    // Special case - enable all
    if (iface == nullptr) {
        populateInterfaceRuntimeState(true);
    } else {
        auto state = getInterfaceRuntimeState(iface);
        CHECK_TRUE(state, SYSTEM_ERROR_NOT_FOUND);
        state->enabled = true;
    }
    return 0;
}

int NetworkManager::disableInterface(if_t iface) {
    // Special case - disable all
    if (iface == nullptr) {
        populateInterfaceRuntimeState(false);
    } else {
        auto state = getInterfaceRuntimeState(iface);
        CHECK_TRUE(state, SYSTEM_ERROR_NOT_FOUND);
        state->enabled = false;
    }
    return 0;
}

int NetworkManager::syncInterfaceStates() {
    if (isEstablishingConnections()) {
        CHECK(for_each_iface([&](if_t iface, unsigned int flags) {
            if (!haveLowerLayerConfiguration(iface)) {
                return;
            }

            auto state = getInterfaceRuntimeState(iface);
            if (state) {
                if (state->enabled && !(flags & IFF_UP)) {
                    CHECKV(if_set_flags(iface, IFF_UP));
                    CHECKV(if_set_xflags(iface, IFXF_DHCP));
                } else if (!state->enabled && (flags & IFF_UP)) {
                    CHECKV(if_clear_flags(iface, IFF_UP));
                }
            }
        }));
    }

    return 0;
}

NetworkManager::InterfaceRuntimeState* NetworkManager::getInterfaceRuntimeState(if_t iface) const {
    for (auto item = runState_.front(); item != nullptr; item = item->next) {
        if (item->iface == iface) {
            return item;
        }
    }
    return nullptr;
}

bool NetworkManager::isInterfaceEnabled(if_t iface) const {
    auto state = getInterfaceRuntimeState(iface);
    if (state) {
        return state->enabled;
    }
    return false;
}

int NetworkManager::countEnabledInterfaces() {
    int count = 0;
    for (auto item = runState_.front(); item != nullptr; item = item->next) {
        if (item->enabled && haveLowerLayerConfiguration(item->iface)) {
            ++count;
        }
    }

    return count;
}

NetworkManager::ProtocolState NetworkManager::getInterfaceIp4State(if_t iface) const {
    auto state = getInterfaceRuntimeState(iface);
    if (state) {
        return state->ip4State;
    }

    return ProtocolState::UNCONFIGURED;
}

NetworkManager::ProtocolState NetworkManager::getInterfaceIp6State(if_t iface) const {
    auto state = getInterfaceRuntimeState(iface);
    if (state) {
        return state->ip6State;
    }

    return ProtocolState::UNCONFIGURED;
}

void NetworkManager::resetInterfaceProtocolState(if_t iface) {
    for (auto item = runState_.front(); item != nullptr; item = item->next) {
        if (!iface || item->iface == iface) {
            item->ip4State = ProtocolState::UNCONFIGURED;
            item->ip6State = ProtocolState::UNCONFIGURED;
        }
    }
}

#endif /* HAL_PLATFORM_IFAPI */