/**
* LICENSE PLACEHOLDER
*
* @file sahara_image_transfer_task.h
* @class SaharaImageTransferTask
* @package openpst/sahara
* @brief Handles the image transfer threaded task operation
*
* @author Gassan Idriss <ghassani@gmail.com>
*/

#pragma once

#include "definitions.h"
#include "task/task.h"
#include "qualcomm/sahara_serial.h"
#include "widget/progress_group_widget.h"


using OpenPST::QC::SaharaSerial;
using OpenPST::QC::SaharaSerialError;

namespace OpenPST {
	namespace GUI {
		class SaharaImageTransferTask : public Task
		{
			private:
				std::string imagePath;
				SaharaSerial& port;
				ProgressGroupWidget* progressContainer;
				SaharaReadDataRequest initalReadRequest;

			public:
				SaharaImageTransferTask(std::string imagePath, SaharaReadDataRequest initalReadRequest, ProgressGroupWidget* progressContainer, SaharaSerial& port);
				~SaharaImageTransferTask();
				void run();
		};
	}
}
