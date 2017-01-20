/**
* LICENSE PLACEHOLDER
*
* @file sahara_window.cpp
* @class SaharaWindow
* @package openpst/sahara
* @brief Sahara GUI interface class
*
* @author Gassan Idriss <ghassani@gmail.com>
*/

#include "sahara_window.h"

using namespace OpenPST::GUI;

#define log(m) ui->logWidget->log(m); 

SaharaWindow::SaharaWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::SaharaWindow),
	port("", 115200),
	deviceState({}),
	taskRunner(&taskShouldCancel)
{

	ui->setupUi(this);

	ui->writeHelloSwitchModeValue->addItem("", -1);
	ui->writeHelloSwitchModeValue->addItem("Image Transfer Pending", kSaharaModeImageTxPending);
	ui->writeHelloSwitchModeValue->addItem("Memory Debug",	kSaharaModeMemoryDebug);
	ui->writeHelloSwitchModeValue->addItem("Client Command Mode", kSaharaModeCommand);

	ui->clientCommandValue->addItem("", -1);
	ui->clientCommandValue->addItem("NOP", kSaharaClientCmdNop);
	ui->clientCommandValue->addItem("Read Serial Number", kSaharaClientCmdSerialNumRead);
	ui->clientCommandValue->addItem("Read MSM HW ID", kSaharaClientCmdMsmHWIDRead);
	ui->clientCommandValue->addItem("Read OEM PK Hash", kSaharaClientOemPkHashRead);
	//ui->clientCommandValue->addItem("Switch To DMSS DLOAD", kSaharaClientCmdSwitchDMSS);
	//ui->clientCommandValue->addItem("Switch To Streaming DLOAD", kSaharaClientCmdSwitchToStreamingDload);
	ui->clientCommandValue->addItem("Read Debug Data", kSaharaClientCmdReadDebugData);
	ui->clientCommandValue->addItem("Get SBL SW Version", kSaharaClientCmdGetSblVersion);

	ui->switchModeValue->addItem("", -1);
	ui->switchModeValue->addItem("Image Transfer Pending", kSaharaModeImageTxPending);
	ui->switchModeValue->addItem("Memory Debug", kSaharaModeMemoryDebug);

	updatePortList();

	QObject::connect(ui->portRefreshButton,			SIGNAL(clicked()), this, SLOT(updatePortList()));
	QObject::connect(ui->readHelloButton,			SIGNAL(clicked()), this, SLOT(readHello()));
	QObject::connect(ui->writeHelloButton,			SIGNAL(clicked()), this, SLOT(writeHello()));
	QObject::connect(ui->portDisconnectButton,		SIGNAL(clicked()), this, SLOT(disconnectPort()));
	QObject::connect(ui->portConnectButton,			SIGNAL(clicked()), this, SLOT(connectToPort()));
	QObject::connect(ui->switchModeButton,			SIGNAL(clicked()), this, SLOT(switchMode()));
	QObject::connect(ui->clientCommandButton,		SIGNAL(clicked()), this, SLOT(sendClientCommand()));
	QObject::connect(ui->resetButton,				SIGNAL(clicked()), this, SLOT(sendReset()));
	QObject::connect(ui->doneButton,				SIGNAL(clicked()), this, SLOT(sendDone()));
	QObject::connect(ui->sendImageFileBrowseButton, SIGNAL(clicked()), this, SLOT(browseForImage()));
	QObject::connect(ui->sendImageButton,			SIGNAL(clicked()), this, SLOT(sendImage()));
	QObject::connect(ui->sendImageXmlButton,		SIGNAL(clicked()), this, SLOT(sendImage()));
	QObject::connect(ui->sendImageXmlBrowseButton,  SIGNAL(clicked()), this, SLOT(browseForXml()));
	QObject::connect(ui->sendImageCheckButton,		SIGNAL(clicked()), this, SLOT(checkImage()));
	QObject::connect(ui->sendImageXmlCheckButton,	SIGNAL(clicked()), this, SLOT(checkXml()));
	QObject::connect(ui->memoryReadButton,			SIGNAL(clicked()), this, SLOT(memoryRead()));
	QObject::connect(ui->actionAbout,				SIGNAL(triggered()), this, SLOT(showAboutDialog()));
	

	QObject::connect(ui->progressGroupBox->cancelButton,    SIGNAL(clicked()), this, SLOT(cancelCurrentTask()));
	QObject::connect(ui->progressGroupBox->cancelAllButton, SIGNAL(clicked()), this, SLOT(cancelAllTasks()));	
}

/**
* @brief SaharaWindow::~SaharaWindow
*/
SaharaWindow::~SaharaWindow()
{
	if (port.isOpen()) {
		port.close();
	}

	delete ui;
}

/**
* @brief SaharaWindow::UpdatePortList
*/
void SaharaWindow::updatePortList()
{
	if (port.isOpen()) {
		log("Port is currently open");
		return;
	}

	std::vector<serial::PortInfo> devices = serial::list_ports();

	ui->portList->clear();
	ui->portList->addItem("- Select a Port -");

	QString tmp;

	log(tmp.sprintf("Found %d devices", devices.size()));

    for (auto device : serial::list_ports())  {

		log(tmp.sprintf("%s %s %s",
			device.port.c_str(),
			device.hardware_id.c_str(),
			device.description.c_str()
		));

		ui->portList->addItem(tmp, device.port.c_str());
	}
}

/**
* @brief SaharaWindow::ConnectToPort
*/
void SaharaWindow::connectToPort()
{
	serial::PortInfo device;
	QString selected = ui->portList->currentData().toString();
	QString tmp;

	if (port.isOpen()) {
		log("A device is already open");
		return;
	} else if (selected.compare("0") == 0 || !selected.length()) {
		log("Select to a device first");
		return;
	}

	for (auto device : serial::list_ports())  {
		if (selected.compare(device.port.c_str(), Qt::CaseInsensitive) == 0) {
			try {
				port.setPort(device.port);

				if (!port.isOpen()){
					port.open();
				}

				ui->deviceStateText->setText("Connected");

				log(tmp.sprintf("Connected to %s", device.port.c_str()));

				if (port.available()) {
					readHello();
					if (ui->autoHelloCheckbox->isChecked()) {
						writeHello();
					}
				}

				ui->portDisconnectButton->setEnabled(true);

			} catch (SerialError& e) {
				log(tmp.sprintf("Error connecting to device on %s", device.port.c_str()));
				log(e.getCode() == 13 ? "Permission Denied" : e.what());
				return;
			}
			break;
		}
	}
		
}

/**
* @brief SaharaWindow::readHello
*/
void SaharaWindow::readHello()
{
	SaharaHelloRequest req = {};
	
	if (!port.isOpen()) {
		log("Connect to a device first");
		return;
	} else if (!port.available()) {
		log("No data waiting. Not in sahara mode, already in a session, or device requires a restart.");
		return;
	}

	log("Reading hello handshake");

	try {
		req = port.readHello();
	} catch (SaharaSerialError e) {
		log(e.what());
		return disconnectPort();
	} catch (SerialError& e) {
		log(e.what());
		return disconnectPort();
	}
	
	QString tmp;
	log(tmp.sprintf("Device In Mode: %s", port.getNamedMode(req.mode).c_str()));
	log(tmp.sprintf("Version: %i", req.version));
	log(tmp.sprintf("Minimum Version: %i", req.minVersion));
	log(tmp.sprintf("Max Command Packet Size: %i", req.maxCommandPacketSize));

	int index = ui->writeHelloSwitchModeValue->findData(req.mode);

	if (index != -1) {
		ui->writeHelloSwitchModeValue->setCurrentIndex(index);
	}

	deviceState.mode   		= req.mode;
	deviceState.version 	= req.version;
	deviceState.minVersion  = req.minVersion;

	updateDeviceState();
}

/**
* @brief SaharaWindow::writeHello
*/
void SaharaWindow::writeHello(uint32_t overrideMode)
{
	QString tmp;

	if (!port.isOpen()) {
		log("Connect to a device first");
		return;
	}
	
	uint32_t mode = NULL != overrideMode ? overrideMode : ui->writeHelloSwitchModeValue->currentData().toUInt();
	uint32_t version = std::stoul(ui->writeHelloVersionValue->text().toStdString().c_str(), nullptr, 10);
	uint32_t minVersion = std::stoul(ui->writeHelloMinimumVersionValue->text().toStdString().c_str(), nullptr, 10);

	bool isSwitchMode = deviceState.mode != mode;

	try {
		deviceState = port.sendHello(mode, version, minVersion, 0x00);
	} catch (SaharaSerialError& e) {
		log(e.what());
		return;
	} catch (SerialError& e) {
		log(e.what());
		disconnectPort();
		return;
	}

	updateDeviceState();

	if (isSwitchMode) {
		log(tmp.sprintf("Attempting to switch device to mode: %s", port.getNamedMode(mode).c_str()));
	}

	if (deviceState.mode == kSaharaModeImageTxPending) {
		log(tmp.sprintf("Device requesting %lu bytes of image 0x%02X - %s", 
			deviceState.imageTransfer.size, deviceState.imageTransfer.imageId, port.getNamedRequestedImage(deviceState.imageTransfer.imageId).c_str()));
		
		QMessageBox confirmation;
		confirmation.setWindowTitle("Send Image");
		confirmation.setText(tmp.append(". Would you like to browse and send this now?"));
		
		QAbstractButton* confirmationButtonFile    = confirmation.addButton("Browse for image", QMessageBox::YesRole);
		QAbstractButton* confirmationButtonXml     = confirmation.addButton("Browse for sahara.xml", QMessageBox::YesRole);
		QAbstractButton* confirmationButtonCancel  = confirmation.addButton("Cancel", QMessageBox::RejectRole);

		confirmation.exec();

		if (confirmation.clickedButton() == confirmationButtonFile) {
			browseForImage();

			if (ui->sendImageFileValue->text().length()) {
				sendImage();
			}
		} else if (confirmation.clickedButton() == confirmationButtonXml) {
			browseForXml();

			if (ui->sendImageXmlPathValue->text().length()) {
				sendImage();
			}
		}

	} else if (deviceState.mode == kSaharaModeMemoryDebug) {
		log(tmp.sprintf("Memory table located at 0x%08X with size of %lu bytes", deviceState.memoryDebug.memoryTableAddress, deviceState.memoryDebug.memoryTableLength));
		debugMemoryRead();
	} else if (deviceState.mode == kSaharaModeCommand) {
		log("Device is awaiting client commands");
	}
}

/**
* @brief SaharaWindow::browseForImage
*/
void SaharaWindow::browseForImage()
{
	QString fileName = QFileDialog::getOpenFileName(this, "Select Image To Send", "", "Image Files (*.mbn *.bin *.img)");

	if (fileName.length()) {
		ui->sendImageFileValue->setText(fileName);
	}
}

/**
* @brief SaharaWindow::sendImage
*/
void SaharaWindow::sendImage()
{
	QString tmp; 

	if (!ui->sendImageFileValue->text().length() && !ui->sendImageXmlPathValue->text().length()) {
		log("Enter or browse for the requested image, or specify a sahara.xml file");
		return;
	} else if (!port.isOpen()) {
		log("Connect to a device first");
		return;
	} else if (deviceState.mode != kSaharaModeImageTxPending || (deviceState.mode == kSaharaModeImageTxPending && deviceState.imageTransfer.imageId == kMbnImageNone)) {
		log("Device is not requesting an image transfer");
		return;
	}

	bool useSaharaXml = false;

	if (ui->sendImageFileValue->text().length() && ui->sendImageXmlPathValue->text().length()) {
		QMessageBox confirmation;
		confirmation.setWindowTitle("Select Image Source");
		confirmation.setText("You have specified an image file as well as a shara.xml file. Which would you like to use?");
		
		QAbstractButton* confirmationButtonFile = confirmation.addButton("Specified image file", QMessageBox::YesRole);
		QAbstractButton* confirmationButtonXml  = confirmation.addButton("Specified sahara.xml", QMessageBox::YesRole);

		confirmation.exec();

		useSaharaXml = confirmation.clickedButton() == confirmationButtonXml;

	} else if(ui->sendImageXmlPathValue->text().length()) {
		useSaharaXml = true;
	}

	if (useSaharaXml) {
		QFile saharXml(ui->sendImageXmlPathValue->text());

		if (!saharXml.exists()) {
			log(tmp.sprintf("File %s does not exist", ui->sendImageXmlPathValue->text().toStdString().c_str()));
			return;
		} else if(!saharXml.open(QIODevice::ReadOnly | QIODevice::Text)) {
			log(tmp.sprintf("Error opening %s", ui->sendImageXmlPathValue->text().toStdString().c_str()));
			return;
		}

		QFileInfo xmlFileInfo(ui->sendImageXmlPathValue->text());
		QDir xmlFileDir 	= xmlFileInfo.dir();
		QDir applicationDir = QDir::current();

		QDomDocument dom;
		dom.setContent(&saharXml);

		QDomNodeList nodes = dom.elementsByTagName("image");

		saharXml.close();

		if (!nodes.count()) {
			log("No matching XML elements found. Check XML format and try again.");
			return;
		}

		QString imagePath;

		for (int i = 0; i < nodes.count(); i++){
			QDomElement el = nodes.at(i).toElement();
			
			if (!el.hasAttribute("image_id") || !el.hasAttribute("image_path")) {
				continue;
			}

			int id 	     = el.attribute("image_id").toInt();
			QString path = el.attribute("image_path");
			
			if (id == deviceState.imageTransfer.imageId) {
				QFileInfo absoluteInfo(path);

				if (absoluteInfo.exists()) {
					imagePath = absoluteInfo.path();
				} else if (xmlFileDir.exists(path)) {
					imagePath = xmlFileDir.filePath(path);
				} else if(applicationDir.exists(path)) {
					applicationDir = xmlFileDir.filePath(path);
				}
				break;
			}
		}

		if (!imagePath.length()) {
			log("Image not found in search paths or as an absolute path. Run check on XML to get more information.");
			return;
		}

		addTask(new SaharaImageTransferTask(imagePath.toStdString(), deviceState.imageTransfer, ui->progressGroupBox, port));
	} else {
		addTask(new SaharaImageTransferTask(ui->sendImageFileValue->text().toStdString(), deviceState.imageTransfer, ui->progressGroupBox, port));
	}
}

/**
* @brief SaharaWindow::browseForXml
*/
void SaharaWindow::browseForXml()
{
	QString fileName = QFileDialog::getOpenFileName(this, "Select Image To Send", "", "XML Files (*.xml)");

	if (fileName.length()) {
		ui->sendImageXmlPathValue->setText(fileName);
	}
}

/**
* @brief SaharaWindow::checkImage
*/
void SaharaWindow::checkImage()
{
	QString tmp;

	if (!ui->sendImageFileValue->text().length()) {
		log("Enter or browse for an image");
		return;
	}
	
	MbnParser parser;

	try {
		auto fileInfo = parser.parse(ui->sendImageFileValue->text().toStdString(), OpenPST::QC::kMbnParserFlagNone);

		log(tmp.sprintf("This image says it is image id %d (%s)", fileInfo->getImageId(), fileInfo->getImageName().c_str()));

		if (deviceState.mode == kSaharaModeImageTxPending) {
			if (fileInfo->getImageId() == deviceState.imageTransfer.imageId) {
				if (fileInfo->getSignaturePtr()) {
					log("Base on the requested image id, this appears to be a correct image. This image is signed, and may not be compatible with this device.");
				} else {
					log("Base on the requested image id, this appears to be a correct image.");
				}		
			}
		} 

	} catch (MbnParserException& e) {
		log(tmp.sprintf("Error parsing mbn file at %s.", ui->sendImageFileValue->text().toStdString().c_str()));
		log(e.what());
	}
}

/**
* @brief SaharaWindow::checkXml
*/
void SaharaWindow::checkXml()
{
	QString tmp;

	if (!ui->sendImageXmlPathValue->text().length()) {
		log("Enter or browse for a valid sahara.xml file");
		return;
	}

	QFile saharXml(ui->sendImageXmlPathValue->text());

	if (!saharXml.exists()) {
		log(tmp.sprintf("File %s does not exist", ui->sendImageXmlPathValue->text().toStdString().c_str()));
		return;
	} else if(!saharXml.open(QIODevice::ReadOnly | QIODevice::Text)) {
		log(tmp.sprintf("Error opening %s", ui->sendImageXmlPathValue->text().toStdString().c_str()));
		return;
	}

	QFileInfo xmlFileInfo(ui->sendImageXmlPathValue->text());
	QDir xmlFileDir 	= xmlFileInfo.dir();
	QDir applicationDir = QDir::current();

	QDomDocument dom;
	dom.setContent(&saharXml);

	QDomNodeList nodes = dom.elementsByTagName("image");
	
	saharXml.close();

	if (!nodes.count()) {
		log("No matching XML elements found. Check XML format and try again.");
		return;
	}

	log("Searching the following directories for relative images:");
	log("\t- Specified absolute path to image");
	log("\t- " + xmlFileDir.path());
	log("\t- " + applicationDir.path());

	for (int i = 0; i < nodes.count(); i++){
		QDomElement el = nodes.at(i).toElement();
		
		if (!el.hasAttribute("image_id") || !el.hasAttribute("image_path")) {
			log(tmp.sprintf("Image declaration %d must have id and image_path attributes", i));
			continue;
		}

		int id 	     = el.attribute("image_id").toInt();
		QString path = el.attribute("image_path");
		
		QFileInfo absoluteInfo(path);

		tmp.sprintf("Image ID %d. Path: %s.", id, path.toStdString().c_str());

		if (absoluteInfo.exists()) {
			log(tmp + " Image file found at " + absoluteInfo.filePath());
		} else if (xmlFileDir.exists(path)) {
			log(tmp + " Image file found in " + xmlFileDir.path());
		} else if(applicationDir.exists(path)) {
			log(tmp + " Image file found in " + applicationDir.path());
		} else {
			log(tmp + " Image file not found in search paths or as an absolute path.");
		}
	}
}

/**
* @brief SaharaWindow::SwitchMode
*/
void SaharaWindow::switchMode()
{
	if (!port.isOpen()) {
		log("Connect to a device first.");
		return;
	}

	QString tmp;
	uint16_t requestMode = ui->switchModeValue->currentData().toUInt();

	log(tmp.sprintf("Requesting mode switch from %s (0x%02x) to %s (0x%02x)",
		port.getNamedMode(deviceState.mode).c_str(),
		deviceState.mode,
		port.getNamedMode(requestMode).c_str(),
		requestMode
	));

	try {
		deviceState = port.switchModeAndHello(requestMode);
	} catch (SaharaSerialError& e) {
		log(e.what());
		return;
	} catch (SerialError& e) {
		log(e.what());
		disconnectPort();
		return;
	}

	log(tmp.sprintf("Device In Mode: %s", port.getNamedMode(deviceState.mode).c_str()));

	updateDeviceState();
}

/**
* @brief SaharaWindow::sendClientCommand
*
*/
void SaharaWindow::sendClientCommand()
{
	if (!port.isOpen()) {
		log("Connect to a device first.");
		return;
	}

	QString tmp;
	std::vector<uint8_t> data;
	uint16_t requestedCommand = ui->clientCommandValue->currentData().toUInt();
	
	try {
		data = port.sendClientCommand(requestedCommand);
	} catch (SaharaSerialError& e) {
		log(e.what());
		return;
	} catch (SerialError& e) {
		log(e.what());
		disconnectPort();
		return;
	}

	if (data.size()) {

		if (requestedCommand == kSaharaClientOemPkHashRead) {
			SaharaOemPkHashResponse* resp = (SaharaOemPkHashResponse*)&data[0];
			//TODO hexdump(resp->hash, sizeof(SaharaOemPkHashResponse), tmp, false);
			log(tmp.sprintf("OEM Public Key Hash Hex:\n %s", tmp.toStdString().c_str()));
		} else if (requestedCommand == kSaharaClientCmdGetSblVersion) {
			SaharaSblVersionResponse* resp = (SaharaSblVersionResponse*)&data[0];
			log(tmp.sprintf("SBL SW Version: %u", resp->version));
		} else if (requestedCommand == kSaharaClientCmdSerialNumRead) {
			SaharaSerialNumberResponse* resp = (SaharaSerialNumberResponse*)&data[0];
			log(tmp.sprintf("Serial Number: %u - %08X", resp->serial, resp->serial));
		} else if (requestedCommand == kSaharaClientCmdMsmHWIDRead) {
			SaharaMsmHwIdResponse* resp = (SaharaMsmHwIdResponse*)&data[0];
			log(tmp.sprintf("Unknown ID 1: %u", resp->unknown1));
			log(tmp.sprintf("Unknown ID 2: %u", resp->unknown2));
			log(tmp.sprintf("MSM HW ID: %u - %08X", resp->msmId, resp->msmId));
		} else {
			log(tmp.sprintf("========\nDumping Data For Command: 0x%02x - %s - %lu Bytes\n========\n\n",
				requestedCommand, port.getNamedClientCommand(requestedCommand).c_str(), data.size()
			));
		}
	}
}

/**
* @brief SaharaWindow::sendReset
*/
void SaharaWindow::sendReset()
{
	if (!port.isOpen()) {
		log("Connect to a device first.");
		return;
	}

	log("Sending Reset Command");

	try {
		port.sendReset();
	} catch (SaharaSerialError& e) {
		log(e.what());
	} catch (SerialError& e) {
		log(e.what());
	}

	return disconnectPort();
}


/**
* @brief SaharaWindow::sendDone
*/
void SaharaWindow::sendDone()
{
	if (!port.isOpen()) {
		log("Connect to a device first.");
		return;
	}

	log("Sending Done Command");

	try {
		port.sendDone();
	} catch (SaharaSerialError& e) {
		log(e.what());
		return;
	} catch (SerialError& e) {
		log(e.what());
		return;
	}

	log("Done Command Successfully Sent");
}

/**
* @brief SaharaWindow::disconnectPort
*/
void SaharaWindow::disconnectPort()
{
	if (port.isOpen()) {
		port.close();
		log("Port Closed");
		ui->portDisconnectButton->setEnabled(false);
		ui->deviceStateText->setText("Disconnected");
		deviceState = {};
	}
}


/**
* @brief SaharaWindow::memoryRead
*/
void SaharaWindow::memoryRead()
{
	if (!port.isOpen()) {
		log("Connect to a device first.");
		return;
	}

	if (!ui->memoryReadAddressValue->text().length()) {
		log("Enter a starting read address in hexidecimal format");
		return;
	}

	if (!ui->memoryReadSizeValue->text().length()) {
		log("Enter an amount in bytes to read");
		return;
	}

	uint32_t address = std::stoul(ui->memoryReadAddressValue->text().toStdString().c_str(), nullptr, 16);
	size_t size		 = std::stoul(ui->memoryReadSizeValue->text().toStdString().c_str(), nullptr, 10);

	if (size <= 0) {
		log("Invalid Size");
		return;
	}

	QString tmp;

	QString fileName = QFileDialog::getSaveFileName(this, tr("Save Read Data"), "", tr("Binary Files (*.bin)"));

	if (!fileName.length()) {
		log("No file set to save memory content. Operation cancelled");
		return;
	}

	log(tmp.sprintf("Reading %lu bytes from address 0x%08X", size, address));

	addTask(new SaharaMemoryReadTask(address, size, fileName.toStdString(), ui->progressGroupBox, port));

}

void SaharaWindow::debugMemoryRead()
{
	QString tmp;
	int totalRegions;
	SaharaMemoryTableEntry* entry;
	std::vector<uint8_t> debugTable;

	if (deviceState.mode != kSaharaModeMemoryDebug) {
		log("Device does not appear to be in memory debug mode");
		return; 
	} else if (deviceState.memoryDebug.memoryTableLength <= 0) {
		log("Device responded with an invalid length memory table");
		return;
	}

	try {
		port.readMemory(deviceState.memoryDebug.memoryTableAddress, deviceState.memoryDebug.memoryTableLength, debugTable);
	} catch (SaharaSerialError& e) {
		log(e.what());
		return;
	} catch (SerialError& e) {
		log(e.what());
		return;
	}

	totalRegions = debugTable.size() / sizeof(SaharaMemoryTableEntry);

	log(tmp.sprintf("Memory table references %d locations", totalRegions));
	
	ui->debugMemoryLocationsTable->setRowCount(totalRegions);

	for (int i = 0; i < totalRegions; i++) {
		entry = reinterpret_cast<SaharaMemoryTableEntry*>(&debugTable[i*sizeof(SaharaMemoryTableEntry)]);
		log(tmp.sprintf("%s (%s) - Address: 0x%08X Size: %i", entry->name, entry->filename, entry->address, entry->size));
		ui->debugMemoryLocationsTable->setItem(i, kSaharaMemoryDebugTableColumnName, new QTableWidgetItem(tmp.sprintf("%s", entry->name).trimmed()));
		ui->debugMemoryLocationsTable->setItem(i, kSaharaMemoryDebugTableColumnFileName, new QTableWidgetItem(tmp.sprintf("%s", entry->filename).trimmed()));
		ui->debugMemoryLocationsTable->setItem(i, kSaharaMemoryDebugTableColumnAddress, new QTableWidgetItem(tmp.sprintf("0x%08X", entry->address)));
		ui->debugMemoryLocationsTable->setItem(i, kSaharaMemoryDebugTableColumnSize, new QTableWidgetItem(tmp.sprintf("%lu", entry->size)));
	}

	QMessageBox::StandardButton userResponse = QMessageBox::question(this, "Memory Table", tmp.sprintf("Pull all %d files referenced in the memory table?", totalRegions));
		
	if (userResponse == QMessageBox::Yes) {

		QString dumpPath = QFileDialog::getExistingDirectory(this, tr("Select where to dump the files"), "");
		QString outFile;

		if (dumpPath.length()) {

			for (int i = 0; i < totalRegions; i++) {
				entry = reinterpret_cast<SaharaMemoryTableEntry*>(&debugTable[i*sizeof(SaharaMemoryTableEntry)]);

				if (entry->size > 1000000) { // confirm files larger than 1mb
					QMessageBox::StandardButton largeFileUserResponse = QMessageBox::question(this, "Confirm Large File", tmp.sprintf("Pull large file %s (%lu bytes) or skip it?", entry->filename, entry->size));

					if (largeFileUserResponse != QMessageBox::Yes) {
						log(tmp.sprintf("Skipping %s - %s", entry->filename, entry->name));
						continue;
					}
				}

				outFile.sprintf("%s/%s", dumpPath.toStdString().c_str(), entry->filename);

				addTask(new SaharaMemoryReadTask(entry->address, entry->size, outFile.toStdString(), ui->progressGroupBox, port));
			}
			
		} else {
			log("Dump all cancelled");
		}
	}
	
	QMessageBox::StandardButton saveMemoryTableResponse = QMessageBox::question(this, "Save Memory Table", "Save the raw memory table to a file?");

	if (saveMemoryTableResponse == QMessageBox::Yes) {
		QString memoryTableFileName = QFileDialog::getSaveFileName(this, tr("Save Raw Memory Table"), "", tr("Binary Files (*.bin)"));

		if (memoryTableFileName.length()) {

			std::ofstream file(memoryTableFileName.toStdString().c_str(), std::ios::out | std::ios::binary | std::ios::trunc);

			if (file.is_open()) {
				file.write(reinterpret_cast<char*>(&debugTable[0]), debugTable.size());
				file.close();
			} else {
				log("Error opening memory table file for writing");
			}
		} else {
			log("Save raw memory table cancelled");
		}
	}
}


void SaharaWindow::addTask(Task* task)
{
	connect(task, &Task::started,   this, &SaharaWindow::onTaskStarted);
	connect(task, &Task::complete,  this, &SaharaWindow::onTaskComplete);
	connect(task, &Task::aborted,   this, &SaharaWindow::onTaskAborted);
	connect(task, &Task::error,		this, &SaharaWindow::onTaskError);
	connect(task, &Task::log,		this, &SaharaWindow::onTaskLog);

	ui->progressGroupBox->setTaskCount(++taskCount);

	taskRunner.queue(task);

}

void SaharaWindow::cancelCurrentTask()
{
	QMessageBox::StandardButton answer = QMessageBox::question(
		this,
		tr("Confirmation"),
		tr("Are you sure you would like to cancel the current task?")
	);

	if (answer == QMessageBox::No) {
		return;
	}

	taskShouldCancel = true;
	taskRunner.waitForDone();
}

void SaharaWindow::cancelAllTasks()
{
	QMessageBox::StandardButton answer = QMessageBox::question(
		this,
		tr("Confirmation"),
		tr("Are you sure you would like to cancel all tasks?")
	);

	if (answer == QMessageBox::No) {
		return;
	}

	taskRunner.clearQueue();
	cancelCurrentTask();
	taskCount = 0;
	ui->progressGroupBox->disableCancel();
	ui->progressGroupBox->disableCancelAll();
	ui->progressGroupBox->setTaskCount(taskCount);
}

void SaharaWindow::onTaskStarted()
{
	ui->progressGroupBox->enableCancel();

	if (taskCount > 1) {
		ui->progressGroupBox->enableCancelAll();
	}
}

void SaharaWindow::onTaskComplete()
{
	QString tmp;

	if (taskCount > 0) {
		taskCount--;
	}

	ui->progressGroupBox->setTaskCount(taskCount);

	if (!taskCount) {
		ui->progressGroupBox->disableCancel();
		ui->progressGroupBox->disableCancelAll();
	}	
}

void SaharaWindow::onTaskAborted()
{
	QString tmp;

	if (taskCount > 0) {
		taskCount--;
	}

	ui->progressGroupBox->setTaskCount(taskCount);

	if (!taskCount) {
		ui->progressGroupBox->disableCancel();
		ui->progressGroupBox->disableCancelAll();
	}
}

void SaharaWindow::onTaskError(QString msg)
{
	QString tmp; 
	
	if (taskCount > 0) {
		taskCount--;
	}
	
	ui->progressGroupBox->setTaskCount(taskCount);

	if (!taskCount) {
		ui->progressGroupBox->disableCancel();
		ui->progressGroupBox->disableCancelAll();
	}

	log(msg);
}

void SaharaWindow::onTaskLog(QString msg)
{
	log(msg);
}

void SaharaWindow::showAboutDialog()
{
	aboutDialog.exec();
}

void SaharaWindow::closeEvent(QCloseEvent *event)
{
	
	if (taskRunner.isRunning()) {
		QMessageBox::StandardButton answer = QMessageBox::question(
			this,
			tr("Active Tasks Running"),
			tr("There are active tasks running. Are you sure?")
		);

		if (answer == QMessageBox::No) {
			event->ignore();
			return;
		}
	}

	event->accept();
}

void SaharaWindow::updateDeviceState()
{
	QString ver;
	QString minVer;

	ui->deviceStateText->setText(port.getNamedMode(deviceState.mode).c_str());
	ui->deviceMinVersionText->setText(ver.sprintf("Ver: %d", deviceState.version));
	ui->deviceMinVersionText->setText(minVer.sprintf("Min. Ver: %d", deviceState.minVersion));

	if (deviceState.mode == kSaharaModeImageTxPending) {
		ui->mainTabSet->setCurrentIndex(0);
	} else if (deviceState.mode == kSaharaModeMemoryDebug) {
		ui->mainTabSet->setCurrentIndex(1);
	} else if(deviceState.mode == kSaharaModeCommand) {
		ui->mainTabSet->setCurrentIndex(2);
	}
}