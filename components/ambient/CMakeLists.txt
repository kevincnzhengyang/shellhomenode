# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES esp_adc shnode)

set(COMPONENT_SRCS "src/sensor_ambient.c" "src/ambient_temt6000.c" "src/ambient_veml7700.c")
set(COMPONENT_ADD_INCLUDEDIRS "include")

register_component()
