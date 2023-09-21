# ns3-botnet-module

## Introduction
This is a botnet module based on ns-3 and BRITE topology generator. This tool can help generate simulated DDoS datasets for research purposes.

## Dependencies
This module is dependent on [ns-3.37](https://www.nsnam.org/releases/ns-3-37/) and BRITE. Refer to [BRITE integration](https://www.nsnam.org/docs/models/html/brite.html) for integration setup.

Requires CMake version > 3.10

## Config file generation
To generate config files with BRITE GUI, refer to [patched BRITE](https://github.com/nsol-nmsu/brite-patch.git).

Change file name of seedfile.default to seedfile.

Run: ```make``` to generate the GUI executable.

Run: ```./britegui```

Copy the generated file **GUI_GEN.conf** to botnet/examples/conf_files and change the conf_file variable in the botnet examples to try out different topologies.

Modify **GUI_GEN.conf** by commenting out the last three lines since they are not supported by the parser:

```
BeginOutput
	BRITE = 1 	 #1/0=enable/disable output in BRITE format
	OTTER = 0 	 #1/0=enable/disable visualization in otter
	# DML = 0 	 #1/0=enable/disable output to SSFNet's DML format
	# NS = 0	 #1/0=enable/disable output to NS-2
	# Javasim = 0	 #1/0=enable/disable output to Javasim
EndOutput
```

## Install
Place the repository in the **contrib** directory as shown in this [example](https://www.nsnam.org/docs/manual/html/new-modules.html).

## Run Example
In ns3-3.7 directory:
`./ns3 configure --with-brite=<BRITE_path> --enable-examples --enable-tests`

To run the botnet examples, do:
`./ns3 run botnet-example` for botnet-example.cc and `./ns3 run botnet-example-2` for botnet-example-2.cc

To run the botnet examples, and save the logs to a file, do:
`./ns3 run botnet-example > log.txt 2>&1`
