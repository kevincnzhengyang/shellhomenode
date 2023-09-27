# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES esp_wifi wifi_provisioning driver json mbedtls)

set(COMPONENT_SRCS "src/blufi_prov.c" "src/shn_network.c" "src/node_status.c")
set(COMPONENT_ADD_INCLUDEDIRS "include")

register_component()
