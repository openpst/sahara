#include "task/sahara_memory_read_task.h"

using namespace OpenPST::GUI;

SaharaMemoryReadTask::SaharaMemoryReadTask(uint32_t address, size_t size, std::string outFile, ProgressGroupWidget* progressContainer, SaharaSerial& port) :
	address(address),
	size(size),
	outFile(outFile),
	progressContainer(progressContainer),
	port(port)
{
}

SaharaMemoryReadTask::~SaharaMemoryReadTask()
{
}

void SaharaMemoryReadTask::run()
{
	QString tmp;
	size_t readSoFar = 0;
	size_t amountToRead;
	std::vector<uint8_t> buffer;

	QMetaObject::invokeMethod(progressContainer, "setProgress",  Qt::QueuedConnection, Q_ARG(int, 0), Q_ARG(int, size), Q_ARG(int, 0));
	QMetaObject::invokeMethod(progressContainer, "setTextLeft",  Qt::QueuedConnection, Q_ARG(QString, tmp.sprintf("Reading %lu bytes from 0x%08X", size, address)));
	QMetaObject::invokeMethod(progressContainer, "setTextRight", Qt::QueuedConnection, Q_ARG(QString, tmp.sprintf("%d/%d bytes", readSoFar, size)));
	
	emit started();

	std::ofstream file(outFile.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);

	if (!file.is_open()) {
		emit error("Error opening file for writing");
		return;
	}

	int stepSize = SAHARA_MAX_MEMORY_REQUEST_SIZE;

	while (readSoFar < size) {
		if (cancelled()) {
			emit aborted();
			return;
		}

		if (size > stepSize) {
			amountToRead = stepSize;

			if ((size - readSoFar) < stepSize) {
				amountToRead = size - readSoFar;
			}

		} else {
			amountToRead = size;
		}


		try {
			readSoFar += port.readMemory(address + readSoFar, amountToRead, file);
			
		} catch (SaharaSerialError e) {
			emit error(e.what());
			return;
		} catch (serial::IOException e) {
			emit error(e.what());
			return;
		}

		QMetaObject::invokeMethod(progressContainer, "setProgress", Qt::QueuedConnection, Q_ARG(int, readSoFar));
		QMetaObject::invokeMethod(progressContainer, "setTextRight", Qt::QueuedConnection, Q_ARG(QString, tmp.sprintf("%d/%d bytes", readSoFar, size)));
	}

	emit log(tmp.sprintf("Read %lu bytes from 0x%08X. Contents saved to %s", size, address, outFile.c_str()));

	emit complete();
	
	return;
}

