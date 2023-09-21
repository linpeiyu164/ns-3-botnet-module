.. heading hierarchy:
   ------------- Chapter
   ************* Section (#.#)
   ============= Subsection (#.#.#)
   ############# Paragraph (no number)

Model Description
*****************

The source code for the new module lives in the directory ``contrib/botnet``.

It is dependent on ns-3.37 and BRITE.

Design
======
WIP

Scope and Limitations
=====================

The botnet module is currently able to simulate distributed pulsing attacks with a central controller in a BRITE topology.

Limitations (Future work)
=========================

Long term goal of this project is to be able to simulate realistic DDoS scenarios and generate DDoS datasets for research purposes.
Currently, this module supports the generation of pulsing DDoS attacks.
Some possible future directions are:

1. Add benign background traffic. Current research usually add self-generated data and real-world benign traffic traces to form a new dataset.
2. Imitate existing botnet family behavior. Some botnet families follow specific behaviors, ex. use specific protocols. This could make the dataset more realistic.
3. Add additional flow monitoring information. Allowing users to extract flow information for further analysis.
4. Add custom packet tags specifically designed for DDoS detection, ex. malicious packets/benign packets

References
==========
TODO

Usage
*****

This section is principally concerned with the usage of your model, using
the public API.  Focus first on most common usage patterns, then go
into more advanced topics.

Building New Module
===================

Install the BRITE library and configure ns-3 to use the library.

``./ns3 configure --with-brite=<BRITE_path> --enable-examples --enable-tests``


Helpers
=======

``BotnetHelper``: The helper class for a botnet. It is used for:

- ``CreateBotnet`` sets up the botnet, including choosing the bots from our topology. This is done by randomly picking a maximum of maxBotsPerAs from each existing AS in our topology.
- Adding applications to the central controller(CC) and the bots with ``AddApplication``.
- Setting the attributes of the added applications on the central controller and bots. The application index is needed to specify which application we are configurating.
- Installing the applications onto the central controller and bots.

Models
==========

``Botnet``: Where metainfo on the botnet is stored, including botnet name, type, size and the central controller node.

``PulsingAttackCC``: The application model used for a central controller to simulate pulsing DDoS attacks.

``PulsingAttackBot``: The application model for a bot for simulate pulsing DDoS attacks.

Model Attributes
================

PulsingAttackBot
----------------

- ``ReceivePort``: The receiving port. Default value of 8000.
- ``PacketSize``: Packet size of attack packets. Default value of 100.
- ``TargetPort``: The target port that will receive the attack packets. Default value of 8081.
- ``TargetAddress``: Target address that will receive the attack packets.
- ``CCAddress``: Central controller address.
- ``Rounds``: The number of attack rounds.
- ``AttackInterval``: The interval between each round of attack.
- ``CCPort``: Central controller port that to communicate with the bot.

PulsingAttackCC
---------------

- ``ReceivePort``: The receiving port. Default value of 8080.
- ``PacketSize``: Packet size of the command packet from central controller to bots.
- ``RemotePort``: Port of the bot that will receive the command packet.
- ``AttackTime``: The time gap between bot connection and attack. This value is needed because we have to wait for all bot connections to be finished.

Output
======

Generate packet traces from the network simulation.

Advanced Usage
==============

Users can create their own attack application models to install on the botnet and collect the resulting traces.

Examples
========

``botnet-example`` and ``botnet-example-2`` differ by the size of the target network.
For the former, we manually attach the target node to a leaf node of AS 0.
For the latter, the target network topology is shown in ``examples/botnet-example-2-target-network.png``.

Troubleshooting
===============

When debugging, do: ``./ns3 run botnet-example > logfile.txt 2>&1``
to print out the logs. This saves the logs in a file and makes it easier to debug.