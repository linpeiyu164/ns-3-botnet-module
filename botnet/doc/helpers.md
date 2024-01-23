# Helpers

## BotnetHelper

`BotnetHelper` performs the initialization of the `Botnet` type by assigning roles
such as benign, bot, and central controller to the nodes in the network. In addition,
users will mainly interact with the nodes through this helper class. Actions such as
adding and installing applications and setting attributes will be done through its APIs.

**Methods**

-   `CreateBotnet` sets up the botnet, including choosing the bots from our topology.
    This is done by randomly picking a maximum of maxBotsPerAs from each existing AS in
    our topology.
-   `AddApplication`: Adding applications to benign/bot/central controller nodes.
-   `SetAttributeBot`, `SetAttributeCC`, `SetAttributeBenign`:
    Set attributes of the added applications on the bots, central controller, and benign nodes.
    The application index is needed to specify which application we are configurating.
-   `InstallApplications`: Install the added applications onto the central controller and bots.
