BIN = Linux-ppc440

GCC = /space/global/buildroot/CentOS6_64/ppc440/buildroot-2013.11/output/host/usr/bin/powerpc-buildroot-linux-gnu-gcc -g

all : $(BIN) $(BIN)/gtsServer

$(BIN) :
	mkdir $(BIN)

$(BIN)/gtsServer : $(BIN)/gtsServer.o $(BIN)/udpServer.o $(BIN)/udpClient.o $(BIN)/udpInterpret.o $(BIN)/xdebug_i.o $(BIN)/xmmregs.o \
                   $(BIN)/xmmregs_i.o $(BIN)/xmmregs_Reg.o $(BIN)/xmmregs_Reg_l.o $(BIN)/xmmregs_Trigger.o $(BIN)/xmmregs_TdcDebug.o \
                   $(BIN)/xmmregs_ClockPath.o $(BIN)/xmmregs_MuxInOut.o $(BIN)/xmmregs_DataPath.o $(BIN)/xmmregs_RocketIO.o \
                   $(BIN)/xmmregs_LmkPll.o $(BIN)/xmmregs_LmkPll_i.o $(BIN)/xtdc.o $(BIN)/xoutput_i.o $(BIN)/xspi.o $(BIN)/xspi_tdcgp2.o \
                   $(BIN)/align_tree.o $(BIN)/align.o $(BIN)/align_sfp.o $(BIN)/align_path.o $(BIN)/align_mgtdata.o $(BIN)/align_tdc.o \
                   $(BIN)/rocketIO.o $(BIN)/trigger.o $(BIN)/align_clk.o $(BIN)/align_delay.o $(BIN)/gtsInit.o
	$(GCC) -o $@ $(BIN)/gtsServer.o $(BIN)/udpServer.o $(BIN)/udpClient.o $(BIN)/udpInterpret.o $(BIN)/xdebug_i.o $(BIN)/xmmregs.o \
                     $(BIN)/xmmregs_i.o $(BIN)/xmmregs_Reg.o $(BIN)/xmmregs_Reg_l.o $(BIN)/xmmregs_Trigger.o $(BIN)/xmmregs_TdcDebug.o \
                     $(BIN)/xmmregs_ClockPath.o $(BIN)/xmmregs_MuxInOut.o $(BIN)/xmmregs_DataPath.o $(BIN)/xmmregs_RocketIO.o \
                     $(BIN)/xmmregs_LmkPll.o $(BIN)/xmmregs_LmkPll_i.o $(BIN)/xtdc.o $(BIN)/xoutput_i.o $(BIN)/xspi.o $(BIN)/xspi_tdcgp2.o \
                     $(BIN)/align_tree.o $(BIN)/align.o $(BIN)/align_sfp.o $(BIN)/align_path.o $(BIN)/align_mgtdata.o $(BIN)/align_tdc.o \
                     $(BIN)/rocketIO.o $(BIN)/trigger.o $(BIN)/align_clk.o $(BIN)/align_delay.o $(BIN)/gtsInit.o -lpthread -lm -lrt

$(BIN)/gtsServer.o : gtsServer.c gts.h udpStruct.h gtsCommands.h xmmregs.h xstatus.h xmmregs_l.h xmmregs_i.h xdebug_i.h xspi.h xspi_l.h xoutput_i.h
	$(GCC) -c -o $@ gtsServer.c

$(BIN)/udpServer.o : udpServer.c gts.h udpStruct.h gtsCommands.h xmmregs.h  xstatus.h xmmregs_l.h xmmregs_i.h xdebug_i.h xspi.h xspi_l.h xoutput_i.h
	$(GCC) -c -o $@ udpServer.c

$(BIN)/udpClient.o : udpClient.c gts.h udpStruct.h gtsCommands.h xmmregs.h  xstatus.h xmmregs_l.h xmmregs_i.h xdebug_i.h xspi.h xspi_l.h xoutput_i.h
	$(GCC) -c -o $@ udpClient.c

$(BIN)/udpInterpret.o : udpInterpret.c gts.h udpStruct.h gtsCommands.h xmmregs.h xstatus.h xmmregs_l.h xmmregs_i.h xdebug_i.h xspi.h xspi_l.h xoutput_i.h
	$(GCC) -c -o $@ udpInterpret.c

$(BIN)/xmmregs.o : xmmregs.c xmmregs.h xmmregs_i.h xdebug_l.h xdebug_i.h xstatus.h xmmregs_l.h
	$(GCC) -c -o $@ xmmregs.c

$(BIN)/xmmregs_i.o : xmmregs_i.c xmmregs.h xmmregs_i.h xdebug_l.h xdebug_i.h xstatus.h xmmregs_l.h
	$(GCC) -c -o $@ xmmregs_i.c

$(BIN)/xmmregs_Reg.o : xmmregs_Reg.c xmmregs_Reg.h xmmregs.h xstatus.h xmmregs_i.h xmmregs_l.h xdebug_l.h xdebug_i.h xmmregs_Reg_l.h
	$(GCC) -c -o $@ xmmregs_Reg.c

$(BIN)/xmmregs_Reg_l.o : xmmregs_Reg_l.c xmmregs_Reg.h xmmregs.h xstatus.h xmmregs_i.h xmmregs_l.h xdebug_l.h xdebug_i.h xmmregs_Reg_l.h
	$(GCC) -c -o $@ xmmregs_Reg_l.c

$(BIN)/xdebug_i.o : xdebug_i.c xdebug_i.h xstatus.h
	$(GCC) -c -o $@ xdebug_i.c

$(BIN)/xoutput_i.o : xoutput_i.c xoutput_i.h xstatus.h
	$(GCC) -c -o $@ xoutput_i.c

$(BIN)/xmmregs_Trigger.o : xmmregs_Trigger.c xmmregs_Trigger.h xmmregs.h xstatus.h xmmregs_i.h xmmregs_l.h xdebug_l.h xdebug_i.h xdiagnose_l.h
	$(GCC) -c -o $@ xmmregs_Trigger.c

$(BIN)/xmmregs_TdcDebug.o : xmmregs_TdcDebug.c xmmregs.h xstatus.h xmmregs_i.h xmmregs_l.h xdebug_l.h xdebug_i.h
	$(GCC) -c -o $@ xmmregs_TdcDebug.c

$(BIN)/xmmregs_ClockPath.o : xmmregs_ClockPath.c xmmregs.h xstatus.h xmmregs_i.h xmmregs_l.h xdebug_l.h xdebug_i.h
	$(GCC) -c -o $@ xmmregs_ClockPath.c

$(BIN)/xmmregs_MuxInOut.o : xmmregs_MuxInOut.c xmmregs.h xstatus.h xmmregs_i.h xmmregs_l.h xdebug_l.h xdebug_i.h
	$(GCC) -c -o $@ xmmregs_MuxInOut.c

$(BIN)/xmmregs_DataPath.o : xmmregs_DataPath.c xmmregs_DataPath.h xmmregs.h xstatus.h xmmregs_i.h xmmregs_l.h xdebug_l.h xdebug_i.h
	$(GCC) -c -o $@ xmmregs_DataPath.c

$(BIN)/xmmregs_RocketIO.o : xmmregs_RocketIO.c xmmregs_RocketIO.h xmmregs.h xstatus.h xmmregs_i.h xmmregs_l.h xdebug_l.h xdebug_i.h
	$(GCC) -c -o $@ xmmregs_RocketIO.c

$(BIN)/xmmregs_LmkPll.o : xmmregs_LmkPll.c xmmregs_LmkPll.h xmmregs_LmkPll_i.h xmmregs.h xstatus.h xmmregs_i.h xmmregs_l.h xdebug_l.h xdebug_i.h
	$(GCC) -c -o $@ xmmregs_LmkPll.c

$(BIN)/xmmregs_LmkPll_i.o : xmmregs_LmkPll_i.c xmmregs_LmkPll.h xmmregs_LmkPll_i.h
	$(GCC) -c -o $@ xmmregs_LmkPll_i.c

$(BIN)/xtdc.o : xtdc.c xtdc.h xdebug.h xdebug_i.h xoutput.h xoutput_i.h xspi.h xspi_tdcgp2.h xspi_l.h  xspi_i.h xstatus.h xmmregs_i.h xmmregs.h xmmregs_l.h
	$(GCC) -c -o $@ xtdc.c

$(BIN)/align_tree.o : align_tree.c align.h xmmregs_i.h xmmregs.h xstatus.h xmmregs_l.h xdebug_i.h xmmregs_RocketIO.h debug.h xspi_i.h xspi_tdcgp2.h
	$(GCC) -c -o $@ align_tree.c

$(BIN)/align.o : align.c align.h xmmregs_i.h xmmregs.h xstatus.h xmmregs_l.h xdebug_i.h xmmregs_RocketIO.h debug.h xtdc.h xspi.h xspi_i.h xspi_l.h xspi_tdcgp2.h
	$(GCC) -c -o $@ align.c

$(BIN)/align_sfp.o : align_sfp.c align.h xmmregs_i.h xmmregs.h xstatus.h xmmregs_l.h xdebug_i.h debug.h xspi.h xspi_i.h xspi_l.h xspi_tdcgp2.h
	$(GCC) -c -o $@ align_sfp.c

$(BIN)/align_path.o : align_path.c align.h xmmregs_i.h xmmregs.h xstatus.h xmmregs_l.h xdebug_i.h debug.h xspi.h xspi_i.h xspi_l.h xspi_tdcgp2.h
	$(GCC) -c -o $@ align_path.c

$(BIN)/align_mgtdata.o : align_mgtdata.c align.h xmmregs_i.h xmmregs.h xstatus.h xmmregs_l.h xdebug_i.h debug.h xspi.h xspi_i.h xspi_l.h xspi_tdcgp2.h
	$(GCC) -c -o $@ align_mgtdata.c

$(BIN)/align_tdc.o : align_tdc.c xmmregs_i.h xmmregs.h xstatus.h xmmregs_l.h xdebug_i.h debug.h xtdc.h xspi.h xspi_i.h xspi_l.h xspi_tdcgp2.h
	$(GCC) -c -o $@ align_tdc.c

$(BIN)/rocketIO.o : rocketIO.c xmmregs_RocketIO.h xmmregs_i.h xmmregs.h xmmregs_l.h xstatus.h
	$(GCC) -c -o $@ rocketIO.c

$(BIN)/trigger.o : trigger.c align.h xspi_tdcgp2.h xspi_i.h xspi.h xspi_l.h xstatus.h xmmregs_i.h xmmregs.h xmmregs_l.h xdebug_i.h debug.h
	$(GCC) -c -o $@ trigger.c

$(BIN)/align_clk.o : align_clk.c align.h xspi_tdcgp2.h xspi_i.h xspi.h xspi_l.h xstatus.h xmmregs_i.h xmmregs.h xmmregs_l.h xdebug_i.h debug.h
	$(GCC) -c -o $@ align_clk.c

$(BIN)/align_delay.o : align_delay.c align.h xspi_tdcgp2.h xspi_i.h xspi.h xspi_l.h xstatus.h xmmregs_i.h xmmregs.h xmmregs_l.h xdebug_i.h debug.h
	$(GCC) -c -o $@ align_delay.c

$(BIN)/gtsInit.o : gtsInit.c xstatus.h xmmregs_i.h xmmregs.h xmmregs_l.h xdebug_i.h debug.h
	$(GCC) -c -o $@ gtsInit.c

$(BIN)/xspi.o : xspi.c xspi.h xspi_i.h xspi_l.h xstatus.h
	$(GCC) -c -o $@ xspi.c

$(BIN)/xspi_tdcgp2.o : xspi_tdcgp2.c xspi.h xspi_l.h xstatus.h xspi_tdcgp2.h
	$(GCC) -c -o $@ xspi_tdcgp2.c
