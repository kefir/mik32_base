/opt/synacore/bin/openocd -f interface/jlink.cfg -f ./scripts/mik32.cfg  -f ./scripts/include_eeprom.tcl  -c "eeprom_write_file build/mik32_base.hex" -c "resume 0x0" -c "exit"
