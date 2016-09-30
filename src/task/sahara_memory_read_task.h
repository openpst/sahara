
#ifndef _GUI_TASK_SAHARA_MEMORY_READ_TASK_H
#define _GUI_TASK_SAHARA_MEMORY_READ_TASK_H

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


#endif // _GUI_TASK_SAHARA_MEMORY_READ_TASK_H
