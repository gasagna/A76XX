# Arduino Library for the A76XX family of SIMCOM cellular modems

Native MQTT(S), HTTP(S), etc, clients for the A76XX family of SIMCOM cellular modems.

## Why another GSM library?
There are a number of great libraries out there to allow IoT devices to connect to the internet using a variety of network protocols. For cellular-enabled devices, the choice typically falls on TinyGSM as the base layer but other libraries are required for using protocols such as MQTT, HTTP and so on, with additional complications if you want to enable secure communication.

However, the latest SIMCOM firmware provides native clients for several network protocols that are popular in IoT, accessible through standard AT commands and with optional SSL/TLS security built-in! The goal of this library is to make all these protocols and features easibly accessible from a single library.

## Supported protocols and features
The table below shows the protocols that are natively supported by the firmware described in the latest AT command manual for the A76XX family of modems, along with their current support in this library. Native clients that have a secure option (SSL/TLS) are indicated with a trailing [S].

| Protocol/Feature | Currently supported |
| ---------------- | ------------------  |
| BlueTooth        | No                  |
| COAP             | No                  |
| FTP[S]           | No                  |
| HTTP[S]          | Yes                 |
| LWM2M            | No                  |
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
There are no proper documentation pages at the moment, but doxygen-like documentation is available in the header files and will at some point collected into a proper auto-generated documentation page. There are several examples available in this repo that demonstrate what functionality is enabled with this library. Pull requests are welcome!

## Library structure
This section's purpose is to illustrate the code structure and to introduce some of the key classes defined in this library. More details on the usage of various components can be inferred from the documentation embedded in the source code.

### `ModemSerial` – an Arduino's `Serial` object on steroids
The foundation of the library is the class `ModemSerial`. This is a thin wrapper around the Arduino's `Serial` object used to talk to the SIMCOM module. This class adds several features that are required to send, receive and parse AT commands, in addition to capturing and queueing unsolicited result codes emitted by the module (e.g. when receiving an MQTT message of a subscription). You do not need to use this class, but it's used everywhere else in the library, so you might want to look at it first if you want to contribute to this repo.

### AT commands wrappers (low-level)
Given an instance of `ModemSerial`, AT commands can be issued to the module and the response can be read and parsed appropriately. One of the goals of this library is to mirror quite closely the AT command manual from SIMCOM. In the manual, AT commands are grouped by category in chapters, e.g. network, status control, packet domain, etc. For each of this category, the library defines a header files defining a class in the directory `src/commands`, where some of commands are implemented by member functions. These are low level wrappers to send and parse AT commands, so that other parts of the library or user code does not need to deal with tedious parsing, leading to more robust and reusable code. Most of these member functions return an `int8_t` return code, signalling a successful operation or an error code. 

### `A76XX` - the modem (high level)
This class is what you will use directly most of the time to connect to the network, etc. It is a wrapper around a `ModemSerial` object and the low-level AT commands implementations, and provides a more intuitive interface to use in sketches.

### Native clients for network protocols supported by the SIMCOM firmware
The SIMCOM firmware provides native clients for some network protocols, e.g. MQTT. The AT commands required for these protocols are quite low level and quite annoying to use directy. Hence, each protocol can be accessed through a high-level client object, wrapping the low-level interface, and providing a more intuitive interface.