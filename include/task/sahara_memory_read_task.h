/**
* LICENSE PLACEHOLDER
*
* @file sahara_image_transfer_task.h
* @class SaharaMemoryReadTask
* @package openpst/sahara
* @brief Handles thememory reading threaded task operation
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
		class SaharaMemoryReadTask : public Task
		{
			private:
				uint32_t	address;
				size_t		size;
				std::string outFile;
				SaharaSerial& port;
				ProgressGroupWidget* progressContainer;

			public:
				SaharaMemoryReadTask(uint32_t address, size_t size, std::string outFile, ProgressGroupWidget* progressContainer, SaharaSerial& port);
				~SaharaMemoryReadTask();
				void run();
		};
	}
}
