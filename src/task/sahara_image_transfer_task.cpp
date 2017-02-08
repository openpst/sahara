/**
*
* (c) Gassan Idriss <ghassani@gmail.com>
* 
* This file is part of sahara.
* 
* sahara is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* sahara is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with sahara. If not, see <http://www.gnu.org/licenses/>.
*
* @file sahara_image_transfer_task.cpp
* @class SaharaImageTransferTask
* @package openpst/sahara
* @brief Handles the image transfer threaded task operation
*
* @author Gassan Idriss <ghassani@gmail.com>
*/

#include "task/sahara_image_transfer_task.h"

using namespace OpenPST::GUI;

SaharaImageTransferTask::SaharaImageTransferTask(std::string imagePath, SaharaReadDataRequest initalReadRequest, ProgressGroupWidget* progressContainer, SaharaSerial& port) :
	imagePath(imagePath),
	initalReadRequest(initalReadRequest),
	progressContainer(progressContainer),
	port(port)
{
}

SaharaImageTransferTask::~SaharaImageTransferTask()
{
}

void SaharaImageTransferTask::run()
{
	size_t fileSize;
	size_t totalSent = 0;
	SaharaReadDataRequest nextOffset = initalReadRequest;
	QString tmp;

	QMetaObject::invokeMethod(progressContainer, "reset", Qt::QueuedConnection);

	std::ifstream file(imagePath.c_str(), std::ios::in | std::ios::binary);

	if (!file.is_open()) {
		emit error(tmp.sprintf("Could not open image file at %s", imagePath.c_str()));
		return;
	}

	file.seekg(0, file.end);
	fileSize = (size_t)file.tellg();
	file.seekg(0, file.beg);

	QMetaObject::invokeMethod(progressContainer, "setProgress", Qt::QueuedConnection, Q_ARG(int, 0), Q_ARG(int, fileSize), Q_ARG(int, 0));
	QMetaObject::invokeMethod(progressContainer, "setTextLeft", Qt::QueuedConnection, Q_ARG(QString, tmp.sprintf("Sending %lu bytes from offset 0x%08X", nextOffset.size, nextOffset.offset)));
	QMetaObject::invokeMethod(progressContainer, "setTextRight", Qt::QueuedConnection, Q_ARG(QString, tmp.sprintf("%d/%d bytes", totalSent, fileSize)));

	emit log(tmp.sprintf("Sending image %s", imagePath.c_str()));

	while (totalSent != fileSize) {
		if (cancelled()) {
			emit aborted();
			return;
		}

		uint32_t thisWriteOffset = nextOffset.offset;
		size_t thisWriteSize = nextOffset.size;

		QMetaObject::invokeMethod(progressContainer, "setTextLeft", Qt::QueuedConnection, Q_ARG(QString, tmp.sprintf("Sending %lu bytes from offset 0x%08X", thisWriteSize, thisWriteOffset)));
		QMetaObject::invokeMethod(progressContainer, "setTextRight", Qt::QueuedConnection, Q_ARG(QString, tmp.sprintf("%d/%d bytes", totalSent, fileSize)));
		QMetaObject::invokeMethod(progressContainer, "setProgress", Qt::QueuedConnection, Q_ARG(int, totalSent));

		try {
			nextOffset = port.sendImage(file, thisWriteOffset, thisWriteSize);
		} catch (SaharaSerialError& e) {
			file.close(); 
			emit error(e.what());
			return;
		} catch (SerialError& e) {
			file.close(); 
			emit error(e.what());
			return;
		} catch (...) {
			file.close();
			throw;
		}

		totalSent += thisWriteSize;

		if ((nextOffset.offset == 0x00 && nextOffset.size == 0x00) || initalReadRequest.imageId != nextOffset.imageId) {
			break;
		}
	}

	file.close();

	// reset progress to 100% based on total sent in case the image was pulled from a live device in which case there is probably extra data
	QMetaObject::invokeMethod(progressContainer, "setProgress", Qt::QueuedConnection, Q_ARG(int, 0), Q_ARG(int, totalSent), Q_ARG(int, totalSent));
	QMetaObject::invokeMethod(progressContainer, "setTextLeft", Qt::QueuedConnection, Q_ARG(QString,"Send Image Complete"));
	QMetaObject::invokeMethod(progressContainer, "setTextRight", Qt::QueuedConnection, Q_ARG(QString, tmp.sprintf("%d/%d bytes", totalSent, totalSent)));
	
	emit log(tmp.sprintf("Image %s successfully sent.", imagePath.c_str()));
	
	if (initalReadRequest.imageId != nextOffset.imageId) {
		emit log(tmp.sprintf("Device is now requesting %lu bytes from image %02X - %s",
			nextOffset.size,
			nextOffset.imageId,
			port.getNamedRequestedImage(nextOffset.imageId)
		));
	} else {
		emit log("If there are no more images requested, you should send the done command.");
	}

	emit complete();
}

