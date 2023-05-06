# Arduino Library for the A76XX family of SIMCOM cellular modems

Native MQTT(S), HTTP(S), etc, clients for the A76XX family of SIMCOM cellular modems.

## Why another GSM library?
There are a number of great libraries out there to allow IoT devices to connect to the internet using a variety of network protocols. For cellular-enabled devices, the choice typically falls on TinyGSM as the base layer but other libraries are required for using protocols such as MQTT, HTTP and so on, with additional complications if you want to enable secure communication.

However, the latest SIMCOM firmware provides native clients for several network protocols that are popular in IoT, accessible through standard AT commands and with optional SSL/TLS security built-in! The goal of this library is to make all these protocols and features easibly accessible from a single library.

## Supported protocols and features
The latest AT command manual for the A76XX family is the 1.08 version, released on 2022/10/12. This is not easily found on SIMCOM's website (why??), but a copy can be found by searching the string "A76XX_Series_AT_Command_Manual_V1.08" in Google. 

Based on the AT command manual, the table below shows the protocols that are natively supported by the firmware described in the v1.08 version of the firmware documentation, along with their current support in this library. Native clients that have a secure option (SSL/TLS) are indicated with a trailing [S].

| Protocol/Feature | Currently supported |
| ---------------- | ------------------  |
| BlueTooth        | No                  |
| COAP             | No                  |
| FTP[S]           | No                  |
| HTTP[S]          | Yes                 |
| LwM2M            | No                  |
| MQTT[S]          | Yes                 |
| SMS              | No                  |
| SMTPS            | No                  |
| TCP/IP           | No                  |
| WebSockets[S]    | No                  |
| WIFI             | No                  |
| GNSS             | No                  |

Note that the firmware in your module might be outdated, and therefore might not support all the features listed here. You might need to update it. 


## Supported modems
It's difficult to say exactly what modules are compatibles with this library. SIMCOM sells a number of different 4G Cat 1 and Cat 4 modules, some branded as SIM76XX, some others as A76XX. There's also a SIM7500X module. 

The table below shows a list of A76XX modules and the protocols/features that have ben tested with this library. Other modules are likely compatible, but I do not have data. If you test this library with another module not listed here, please do let me know!

| Module      | MQTT[S] tested | HTTP[S] tested |
| ----------- | -------------  | -------------  |
| A7608E      | Yes[Yes]       | Yes[Yes]       |


## Documentation and examples
The documentation is a bit lacking at the moment, as I am focusing on implementing new features. There are several examples available in this repo that demonstrate what functionality is enabled with this library. Pull requests are welcome!