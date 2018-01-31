## sahara
`sahara` is a GUI tool for working with devices in Qualcomm DLOAD mode. This includes bricked devices in 9008 mode. It should be noted that devices that are hard bricked and in Sahara mode (Qualcomm USB PID 9008) and have secure boot enabled (pretty much EVERY production device) will only accept signed programmers. This means that it is not only device specific (i.e. MSM8x26,MSM8974,etc) but vendor specific as well (i.e. LG, HTC, etc) since OEMs generate their own private keys to sign all firmware images, including EDL images. This is generally model specific as well. For example, a LG G2 has a different private key used for signing than G3 but this may not be the case depending on the OEM (i.e an OEM uses a single key for all devices).

However, even if you have no valid programmer file(s) to send, you can still query in command mode for some helpful information such as:

	- MSM HW ID - The Hardware ID of the MSM chipset. Burned in a QFPROM fuse.
	- Serial Number - The serial number of the device. Burned in a QFPROM fuse.
	- OEM_PK_HASH - The hash of the entire root certificate which is blown into the QFPROM fuses of the device. the root certificate is included in the X509 certificate store of the SBL1 image. (Thanks Tal Aloni)
	- Log buffer - Get a raw log buffer of the failed boot process (?assumed? corred me here)
	- SBL SW Version - Returns the SBL version. 

Additionally sahara mode also handles debug mode. Debug mode allows to transfer the state of a crash. It also allows for reading memory at the state of the crash point. This includes all user and kernel mode memory. However, it does not include memory protected by the TEE (Trusted Execution Environment) implementation. So yeah.... not the good stuff :(

When a device in sahara debug mode is the case, it will respond the initial hello handshake with a memory address to read that is a table of name and memory locations to read to help the developer debug a system crash. The program will ask if and where to dump these files, and further confirm if you wish to dump the larger segments.

### Compiling

    git clone https://github.com/openpst/sahara --recursive
    make

Be sure to install the requied dependencies: boost and QT5.

On debian systems with aptitude install the following:

	sudo apt-get install build-essential qt5-default qt5-qmake libboost-dev

### Usage

#### Sending Firmware/Loader

- Connect to the Device
- You should receive a hello packet. If not restart the device.
- Once hello handshake is complete, it should be ready to accept firmaware/loaders. You should see the image it is requesting displayed in the log.
- Browse for the loader file Sahara is requesting or provide a sahara.xml file with the image definitions and paths defined in the xml document. See https://github.com/openpst/assets/blob/master/examples/sahara.xml.example as an example format.
- Send the selected file / process the selected sahara.xml
- Once no more images are requested, click the Done button to send the done command.
- If a loader was sent, transfer to the apropriate protocol. i.e. streaming dload, or firehose (not supported yet)

#### Sending Client Commands
- Uncheck Auto Hello
- Connect to the device
- It should automatically read the hello from the device, if not press the Read Hello button.
- Set Mode in the Hello Handshake section to Client Command Mode
- Click the Respond button in the Hello Handshake section
- Go to Command tab if it does not take you there automatically
- Send a client command


### Misc Notes

#### Entering EDL/QDL/DLOAD
Some devices allow for direct entry into sahara EDL mode through adb.
    
    adb reboot edl

Though not all devices are supported. For me Nexus 5 works however it seems to be on some sort of timer that ends up rebooting the device. I assume it can detect a device that isn't really in a crash state. I have some additional extras I may include at some point that attempt to deal with a user mode or kernel mode entry into EDL.

On some device you can short certain things on the board itself and enter edl mode. The idea here is that you short access to EMMC, forcing PBL to enter Sahara. Search XDA for this info, it is available for some devices.

#### Forcing Memory Debug
Memory Debug mode can be triggered on a device with root access. I have found you can enter it by zero'ing out the aboot partition (this includes abootb partitions which are backups, that may replace your zero'ed out aboot partition). I would not recommend this for the average user. You can easily brick your device with this method but it has worked for me on every device I have tested. When in memory debug mode the device will enumerate the EMMC to your machine as a mass storage device so you can just write back the aboot partition.


### Pre-Builts

Some pre-builds available https://github.com/openpst/sahara/releases
