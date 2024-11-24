idf_component_register(SRCS "flash.c" "handle.c"
                    INCLUDE_DIRS "include"
                REQUIRES "json" "tinyusb" "communication" "keyboard" "defines" "nvs_flash" "ads1115" "mcp")
