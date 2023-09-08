# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES driver nvs_flash)
set(COMPONENT_PRIV_REQUIRES shnode)

set(COMPONENT_SRCS "main.c")
set(COMPONENT_ADD_INCLUDEDIRS "")

register_component()
