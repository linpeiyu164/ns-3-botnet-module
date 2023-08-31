# ns3-botnet-module

## Introduction
This is a botnet module based on ns-3 and BRITE topology generator. This tool can help generate simulated DDoS datasets for research purposes.

## Dependencies
This module is dependent on [ns-3.37](https://www.nsnam.org/releases/ns-3-37/) and BRITE. Refer to [BRITE integration](https://www.nsnam.org/docs/models/html/brite.html) for integration setup.

## Install
Place the repository in the **contrib** directory as shown in this [example](https://www.nsnam.org/docs/manual/html/new-modules.html).

## Run Example
In ns3-3.7 directory:
./ns3 configure --with-brite=<BRITE_path> --enable-examples --enable-tests

