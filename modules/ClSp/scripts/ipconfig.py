import eternalhush as eh
from additional.clingyspider import api
from additional.clingyspider import const
from additional.clingyspider import structs
from additional import memrwlib
from additional.clingyspider.nt_const import *

def decode_node_type(type):
    if type == 1:
        return "Broadcast"
    if type == 2:
        return "Peer to peer"
    if type == 4:
        return "Mixed"
    if type == 8:
        return "Hybrid"

    return ""

def decode_adapter_type(type):
    if type == 6:
        return "Ethernet adapter"
    if type == 9:
        return "Token Ring adapter"
    if type == 15:
        return "FDDI adapter"
    if type == 23:
        return "PPP adapter"
    if type == 24:
        return "Loopback adapter"
    if type == 28:
        return "Slip adapter"

    return "Other adapter"

def main(args):
    msg_node_type = "Node Type . . . . . . . . . : "
    msg_hostname = "Host Name . . . . . . . . . : "
    msg_domainname = "Domain Name . . . . . . . . : "
    msg_dns_servers = "DNS . . . . . . . . . . . . : "
    msg_routing_enabled = "IP Routing Enabled. . . . . : "
    msg_dns_enabled = "DNS Enabled . . . . . . . . : "
    msg_wins_proxy_enabled = "WINS Proxy Enabled. . . . . : "

    msg_dhcp_enabled = "DHCP Enabled. . . . . . . . : "
    msg_dhcp_server = "DHCP Server . . . . . . . . : "
    msg_ipv4_address = "IPv4 Address. . . . . . . . : "
    msg_subnet_mask = "Subnet Mask . . . . . . . . : "
    msg_physical_address = "Physical Address. . . . . . : "
    msg_desc = "Description . . . . . . . . : "
    msg_gateway = "Gateway . . . . . . . . . . : "

    s = api.GetNetworkParameters()
    eh.ui.Echo(msg_hostname + s.hostname.get(str), eh.ECHO_DEFAULT)
    eh.ui.Echo(msg_domainname + s.domainname.get(str), eh.ECHO_DEFAULT)

    dns_count = s.dns_count.get(int)
    offset = 0
    for i in range(dns_count):
        dns_server = eh.data.Struct(structs.STR_IP_ADDR)
        dns_server.from_bytes(s.dns_buffer.get(bytes), global_offset=offset)
        offset += 16
        if offset > 16:
            msg_dns_servers += " "*30 + dns_server.addr.get(str) + "\n"
        else:
            msg_dns_servers += dns_server.addr.get(str) + "\n"
    
    eh.ui.Echo(msg_dns_servers, eh.ECHO_DEFAULT)
    
    eh.ui.Echo(msg_node_type + decode_node_type(s.node_type.get(int)), eh.ECHO_DEFAULT)

    if s.enable_routing.get(int):
        state = "yes"
    else:
        state = "no"
    eh.ui.Echo(msg_routing_enabled + state, eh.ECHO_DEFAULT)
    
    if s.enable_proxy.get(int):
        state = "yes"
    else:
        state = "no"
    eh.ui.Echo(msg_wins_proxy_enabled + state, eh.ECHO_DEFAULT)

    if s.enable_dns.get(int):
        state = "yes"
    else:
        state = "no"
    eh.ui.Echo(msg_dns_enabled + state, eh.ECHO_DEFAULT)
    eh.ui.Echo("", eh.ECHO_DEFAULT)
    eh.ui.Echo("", eh.ECHO_DEFAULT)

    s = api.GetNetworkAdapters()
    for adapter in s:
        eh.ui.Echo(decode_adapter_type(adapter.type.get(int)) + "\t" + adapter.name.get(str) + ":", eh.ECHO_DEFAULT)
        eh.ui.Echo(msg_desc + adapter.description.get(str), eh.ECHO_DEFAULT)
        eh.ui.Echo(msg_physical_address + adapter.physical_address.get(str), eh.ECHO_DEFAULT)
        if adapter.enable_dhcp.get(int):
            state = "yes"
        else:
            state = "no"
        eh.ui.Echo(msg_dhcp_enabled + state, eh.ECHO_DEFAULT)

        offset = 0
        for i in range(adapter.netaddr_count.get(int)):
            ip_a = eh.data.Struct(structs.STR_IP_ADDR)
            
            ip_a.from_bytes(adapter.netaddr_buffer.get(bytes), global_offset=offset)
            eh.ui.Echo(msg_ipv4_address+ip_a.addr.get(str), eh.ECHO_DEFAULT)
            
            ip_a.from_bytes(adapter.netaddr_buffer.get(bytes), global_offset=offset+16)
            eh.ui.Echo(msg_subnet_mask+ip_a.addr.get(str), eh.ECHO_DEFAULT)
            offset += 16

        offset = 0
        for i in range(adapter.gateway_count.get(int)):
            ip_a = eh.data.Struct(structs.STR_IP_ADDR)
            
            ip_a.from_bytes(adapter.gateway_buffer.get(bytes), global_offset=offset)
            eh.ui.Echo(msg_gateway+ip_a.addr.get(str), eh.ECHO_DEFAULT)
            offset += 16
       
        eh.ui.Echo(msg_dhcp_server+adapter.dhcp_server.get(str), eh.ECHO_DEFAULT)

        eh.ui.Echo("", eh.ECHO_DEFAULT)
